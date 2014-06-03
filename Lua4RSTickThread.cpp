#include "Lua4RSTickThread.h"
#include <unistd.h>
#include <ctime>

uint    tickIntervalInSeconds       = 1;
uint    sleepPeriodInMilliseconds   = 50;

Lua4RSTickThread::Lua4RSTickThread() :
    RsThread(), _lastRun( time(0 ))
{
}

void Lua4RSTickThread::run()
{
    while(isRunning())
    {
        // tick each X second
        if(_lastRun + tickIntervalInSeconds <= time(0))
        {
            ///TODO tick something
            std::cout << "[Lua] tick" << std::endl;
            _lastRun = time(0);
        }

        // sleep X ms
        usleep(1000 * sleepPeriodInMilliseconds);
    }
}
