#include "AccessText.h"

#include <QDateTime>

#include "util/DateTime.h"

using State = LuaTrustStore::State;

QString AccessText::label(LuaTrust level)
{
    switch(level)
    {
    case LuaTrust::Trusted: return tr("Trusted");
    case LuaTrust::SandboxWrite: return tr("Write");
    case LuaTrust::SandboxReadOnly: break;
    }
    return tr("Read-only");
}

QString AccessText::meaning(LuaTrust level)
{
    switch(level)
    {
    case LuaTrust::Trusted: return tr("full access, like a program you installed");
    case LuaTrust::SandboxWrite: return tr("sandbox, may use functions that change RetroShare");
    case LuaTrust::SandboxReadOnly: break;
    }
    return tr("sandbox, read-only: no file or shell access, no JSON API, nothing that changes RetroShare");
}

QString AccessText::details(const LuaTrustStore::Status& status)
{
    const QString when = DateTime::formatDateTime(status.approvedAt.toLocalTime()); // RS "Date Format"
    switch(status.state)
    {
    case State::Approved:
        return tr("Approved as %1 on %2.\nRuns with %3.")
            .arg(label(status.approvedLevel), when, meaning(status.effective));
    case State::Bundled:
        return tr("Unchanged example shipped with Lua4RS, not approved.\nRuns with %1.").arg(meaning(status.effective));
    case State::Modified:
        return tr("Changed outside the editor since it was approved as %1 on %2.\n"
                  "Runs with %3 until it is approved again.")
            .arg(label(status.approvedLevel), when, meaning(status.effective));
    case State::ChangedExample:
        return tr("Example shipped with Lua4RS, changed since, not approved.\n"
                  "Runs with %1.").arg(meaning(status.effective));
    case State::Unknown: break;
    }
    return tr("Not approved.\nRuns with %1.").arg(meaning(status.effective));
}

QString AccessText::summary(const LuaTrustStore::Status& status)
{
    switch(status.state)
    {
    case State::Approved: return tr("Access: %1 (approved).").arg(label(status.effective));
    case State::Bundled: return tr("Access: %1 — unchanged example, not approved.").arg(label(status.effective));
    case State::Modified: return tr("Access: %1 — changed since approval.").arg(label(status.effective));
    case State::ChangedExample: return tr("Access: %1 — changed example, not approved.").arg(label(status.effective));
    case State::Unknown: break;
    }
    return tr("Access: %1 — not approved.").arg(label(status.effective));
}
