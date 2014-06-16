#include <retroshare/rsstatus.h>

#include "Lua4RSNotify.h"
#include "Lua/LuaCore.h"
#include "Lua/LuaEvent.h"

Lua4RSNotify::Lua4RSNotify()
{
}

void Lua4RSNotify::notifyListPreChange              (int /* list */, int /* type */)
{
    LuaEvent e;
    e.eventId = L4R_INFO_PEERLISTCHANGE_PRE;
    e.timeStamp = QDateTime::currentDateTime();

    LuaCore::getInstance()->processEvent(e);
}

void Lua4RSNotify::notifyListChange                 (int /* list */, int /* type */)
{
    LuaEvent e;
    e.eventId = L4R_INFO_PEERLISTCHANGE;
    e.timeStamp = QDateTime::currentDateTime();

    LuaCore::getInstance()->processEvent(e);
}

void Lua4RSNotify::notifyChatLobbyEvent             (uint64_t           /* lobby id */, uint32_t           /* event type    */ ,const std::string& /* nickname */,const std::string& /* any string */)
{
    LuaEvent e;
    e.eventId = L4R_LOBBY_EVENT;
    e.timeStamp = QDateTime::currentDateTime();

    LuaCore::getInstance()->processEvent(e);
}

void Lua4RSNotify::notifyCustomState                (const std::string& /* peer_id   */, const std::string&               /* status_string */)
{
    LuaEvent e;
    e.eventId = L4R_FRIEND_CUSTOM_STATE;
    e.timeStamp = QDateTime::currentDateTime();

    LuaCore::getInstance()->processEvent(e);
}

void Lua4RSNotify::notifyHashingInfo                (uint32_t           /* type      */, const std::string&               /* fileinfo      */)
{
    LuaEvent e;
    e.eventId = L4R_FILE_HASHING_DONE;
    e.timeStamp = QDateTime::currentDateTime();

    LuaCore::getInstance()->processEvent(e);
}

void Lua4RSNotify::notifyDiskFull                   (uint32_t           /* location  */, uint32_t                         /* size limit in MB */)
{
    LuaEvent e;
    e.eventId = L4R_INFO_DISK_FULL;
    e.timeStamp = QDateTime::currentDateTime();

    LuaCore::getInstance()->processEvent(e);
}

void Lua4RSNotify::notifyPeerStatusChanged          (const std::string& /* peer_id   */, uint32_t status)
{
    if(status == RS_STATUS_ONLINE)
    {
        LuaEvent e;
        e.eventId = L4R_FRIEND_CAMEONLINE;
        e.timeStamp = QDateTime::currentDateTime();

        LuaCore::getInstance()->processEvent(e);
    }

    if(status == RS_STATUS_OFFLINE)
    {
        LuaEvent e;
        e.eventId = L4R_FRIEND_WENTOFFLINE;
        e.timeStamp = QDateTime::currentDateTime();

        LuaCore::getInstance()->processEvent(e);
    }
}

void Lua4RSNotify::notifyPeerStatusChangedSummary   ()
{
    LuaEvent e;
    e.eventId = L4R_FRIENDS_STATUS_CHANGED;
    e.timeStamp = QDateTime::currentDateTime();

    LuaCore::getInstance()->processEvent(e);
}

void Lua4RSNotify::notifyDiscInfoChanged            ()
{
    LuaEvent e;
    e.eventId = L4R_INFO_DISCOVERY_UPDATE;
    e.timeStamp = QDateTime::currentDateTime();

    LuaCore::getInstance()->processEvent(e);
}

void Lua4RSNotify::notifyDownloadComplete           (const std::string& /* fileHash  */)
{
    LuaEvent e;
    e.eventId = L4R_FILE_DOWNLOADFINISHED;
    e.timeStamp = QDateTime::currentDateTime();

    LuaCore::getInstance()->processEvent(e);
}
