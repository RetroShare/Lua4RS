#include <ctime>
#include <unistd.h>

#include "Lua4RSConfig.h"
#include "Lua4RSTickThread.h"
#include "interface/Lua4RSInterface.h"
#include "Lua/LuaCore.h"
#include "Lua/LuaEvent.h"

//uint    tickIntervalInSeconds       = 1;
//uint    sleepPeriodInMilliseconds   = 50;

//uint    secondsToStarUpEvent        = 5;

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
        if(L4R::L4RConfig == NULL)
        {
            usleep(1000 * 5); // 500ms
            continue;
        }
        if(L4R::L4RConfig->getSleepPeriodInMilliseconds() > 1000)
            L4R::L4RConfig->setSleepPeriodInMilliseconds(50);

        // tick each X second
        if(_lastRun + L4R::L4RConfig->getTickIntervalInSeconds() <= (uint)time(0) && _startUpEventTriggered)
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
        if(!_startUpEventTriggered && (_initTime + L4R::L4RConfig->getSecondsToStarUpEvent()) <= (uint)time(0))
        {
            LuaEvent e;
            e.eventId = L4R_STARTUP;
            e.timeStamp = QDateTime::currentDateTime();

            if(LuaCore::getInstance()->processEvent(e))
                // startup event wasn't blocked by core
                _startUpEventTriggered = true;
        }

        // sleep X ms
        usleep(1000 * L4R::L4RConfig->getSleepPeriodInMilliseconds());
    }
}
