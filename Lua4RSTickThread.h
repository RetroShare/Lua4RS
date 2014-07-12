#ifndef LUA4RSTICKTHREAD_H
#define LUA4RSTICKTHREAD_H

#include <stdlib.h>

#include <retroshare/rsplugin.h>
#include <util/rsthreads.h>

#include "Lua4RSConfig.h"

class Lua4RSTickThread :  public RsThread
{
public:
    Lua4RSTickThread();

private:
    /*
     * from RsThread
     */
    // virtual void start() { mIsRunning = true; createThread(*this); }
    virtual void run(); /* called once the thread is started */
    // virtual	void join(); /* waits for the the mTid thread to stop */
    // virtual	void stop(); /* calls pthread_exit() */

    time_t _lastRun;
    time_t _initTime;
    bool _startUpEventTriggered;
    uint32_t _counter;

};

#endif // LUA4RSTICKTHREAD_H
