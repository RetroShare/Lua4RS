#include <ctime>
#include <unistd.h>

#include "Lua4RSTickThread.h"
#include "Lua/LuaCore.h"
#include "Lua/LuaEvent.h"

uint    tickIntervalInSeconds       = 1;
uint    sleepPeriodInMilliseconds   = 50;

uint    secondsToStarUpEvent        = 5;

Lua4RSTickThread::Lua4RSTickThread() :
    RsThread(),
    _lastRun( time(0) ),
    _initTime( time(0) ),
    _startUpEventTriggered( false ),
    _counter( 0 )
{
}

void Lua4RSTickThread::run()
{
    while(isRunning())
    {
        // tick each X second
        if(_lastRun + tickIntervalInSeconds <= (uint)time(0) && _startUpEventTriggered)
        {
            LuaEvent e;
            e.eventId = L4R_TIMERTICK;
            e.timeStamp = QDateTime::currentDateTime();
            // remove ms
            e.timeStamp.setTime(QTime(e.timeStamp.time().hour(), e.timeStamp.time().minute(), e.timeStamp.time().second()));
            e.dataParm->setValue("u32counter", _counter);

            LuaCore::getInstance()->processEvent(e);

            _lastRun = time(0);
            _counter++;
        }

        // start up event
        if(!_startUpEventTriggered && (_initTime + secondsToStarUpEvent) <= (uint)time(0))
        {
            LuaEvent e;
            e.eventId = L4R_STARTUP;
            e.timeStamp = QDateTime::currentDateTime();

            LuaCore::getInstance()->processEvent(e);

            _startUpEventTriggered = true;
        }

        // sleep X ms
        usleep(1000 * sleepPeriodInMilliseconds);
    }
}
