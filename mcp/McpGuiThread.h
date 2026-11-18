#ifndef MCP_GUI_THREAD_H
#define MCP_GUI_THREAD_H

#include <atomic>
#include <chrono>
#include <exception>
#include <future>
#include <memory>

#include <QMetaObject>
#include <QObject>
#include <QThread>

#include "McpProtocol.h"

// Runs f on the thread of `context` (the GUI thread for LuaCore) and waits
// for the result. If the GUI does not pick it up in time, f is abandoned and
// never runs, so the caller can report "not applied" truthfully. Modal
// dialogs keep the event loop running, so they do not block this.
template<class F>
McpResult mcpRunOnThreadOf(QObject* context, F f, std::chrono::milliseconds timeout = std::chrono::seconds(5))
{
    if(QThread::currentThread() == context->thread()) return f();

    enum Phase { Pending, Running, Abandoned };
    struct State
    {
        std::atomic<int> phase{ Pending };
        std::promise<McpResult> result;
    };
    auto state = std::make_shared<State>();
    std::future<McpResult> future = state->result.get_future();

    const bool queued = QMetaObject::invokeMethod(context, [state, f]() mutable {
        int expected = Pending;
        if(!state->phase.compare_exchange_strong(expected, Running)) return; // caller gave up
        try { state->result.set_value(f()); }
        catch(...) { state->result.set_exception(std::current_exception()); }
    }, Qt::QueuedConnection);
    if(!queued) return McpResult::fail("Internal error: cannot reach the RetroShare GUI.");

    if(future.wait_for(timeout) != std::future_status::ready)
    {
        int expected = Pending;
        if(state->phase.compare_exchange_strong(expected, Abandoned))
            return McpResult::fail("RetroShare's GUI is busy; nothing was changed. Try again.");
        future.wait(); // already running: it finishes, report its result
    }
    return future.get();
}

#endif // MCP_GUI_THREAD_H
