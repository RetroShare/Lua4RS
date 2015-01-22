#include <retroshare/rsstatus.h>
#include <retroshare/rsmsgs.h>

#include "Lua4RSNotify.h"
#include "Lua/LuaCore.h"
#include "Lua/LuaEvent.h"
#include "interface/L4RInterface.h"

Lua4RSNotify::Lua4RSNotify()
{
}

void Lua4RSNotify::notifyListPreChange              (int list, int type)
{
    LuaEvent e;
    e.eventId = L4R_INFO_PEERLISTCHANGE_PRE;
    e.timeStamp = QDateTime::currentDateTime();
    e.dataParm->setValue("intlist", list);
    e.dataParm->setValue("inttype", type);

    L4R::L4RConfig->getCore()->processEvent(e);
}

void Lua4RSNotify::notifyListChange                 (int list, int type)
{
    LuaEvent e;
    e.eventId = L4R_INFO_PEERLISTCHANGE;
    e.timeStamp = QDateTime::currentDateTime();
    e.dataParm->setValue("intlist", list);
    e.dataParm->setValue("inttype", type);

    L4R::L4RConfig->getCore()->processEvent(e);
}

void Lua4RSNotify::notifyChatMessage(const ChatMessage &msg)
{
    LuaEvent e;
    e.eventId = L4R_LOBBY_MESSAGERECEIVED;
    e.timeStamp = QDateTime::currentDateTime();
    // todo strip html
    e.dataParm->setValue("strchatid", QString::fromStdString(msg.chat_id.toStdString()));
    e.dataParm->setValue("strmsg", QString::fromStdString(msg.msg));

    L4R::L4RConfig->getCore()->processEvent(e);
}

void Lua4RSNotify::notifyChatLobbyEvent             (uint64_t lobby_id, uint32_t event_type, const std::string& nickname, const std::string& any_string)
{
    QVariant v = QVariant::fromValue<uint64_t>(lobby_id);
    LuaEvent e;
    e.eventId = L4R_LOBBY_EVENT;
    e.timeStamp = QDateTime::currentDateTime();
    e.dataParm->setValue("u64lobbyId", v);
    e.dataParm->setValue("u32eventType", event_type);
    e.dataParm->setValue("strnickname", QString::fromStdString(nickname));
    e.dataParm->setValue("stranyString", QString::fromStdString(any_string));

    L4R::L4RConfig->getCore()->processEvent(e);
}

void Lua4RSNotify::notifyCustomState                (const std::string& peer_id, const std::string& status_string)
{
    LuaEvent e;
    e.eventId = L4R_FRIEND_CUSTOM_STATE;
    e.timeStamp = QDateTime::currentDateTime();
    e.dataParm->setValue("strpeerId", QString::fromStdString(peer_id));
    e.dataParm->setValue("strstatusString", QString::fromStdString(status_string));

    L4R::L4RConfig->getCore()->processEvent(e);
}

void Lua4RSNotify::notifyHashingInfo                (uint32_t type, const std::string& fileinfo)
{
    LuaEvent e;
    e.eventId = L4R_FILE_HASHING_DONE;
    e.timeStamp = QDateTime::currentDateTime();
    e.dataParm->setValue("u32type", type);
    e.dataParm->setValue("strfileInfo", QString::fromStdString(fileinfo));

    L4R::L4RConfig->getCore()->processEvent(e);
}

void Lua4RSNotify::notifyDiskFull                   (uint32_t location, uint32_t size_limit /* in MB */)
{
    LuaEvent e;
    e.eventId = L4R_INFO_DISK_FULL;
    e.timeStamp = QDateTime::currentDateTime();
    e.dataParm->setValue("u32location", location);
    e.dataParm->setValue("u32sizeLimit", size_limit);

    L4R::L4RConfig->getCore()->processEvent(e);
}

void Lua4RSNotify::notifyPeerStatusChanged          (const std::string& peer_id, uint32_t status)
{
    LuaEvent e;

    if(status == RS_STATUS_ONLINE)
        e.eventId = L4R_FRIEND_CAMEONLINE;
    else if(status == RS_STATUS_OFFLINE)
        e.eventId = L4R_FRIEND_WENTOFFLINE;
    else
        e.eventId = L4R_FRIEND_STATUS_CHANGED;

    e.timeStamp = QDateTime::currentDateTime();
    e.dataParm->setValue("strpeerId", QString::fromStdString(peer_id));
    e.dataParm->setValue("u32status", status);

    L4R::L4RConfig->getCore()->processEvent(e);
}

void Lua4RSNotify::notifyPeerStatusChangedSummary   ()
{
    LuaEvent e;
    e.eventId = L4R_FRIEND_STATUS_CHANGED_MANY;
    e.timeStamp = QDateTime::currentDateTime();

    L4R::L4RConfig->getCore()->processEvent(e);
}

void Lua4RSNotify::notifyDiscInfoChanged            ()
{
    LuaEvent e;
    e.eventId = L4R_INFO_DISCOVERY_UPDATE;
    e.timeStamp = QDateTime::currentDateTime();

    L4R::L4RConfig->getCore()->processEvent(e);
}

void Lua4RSNotify::notifyDownloadComplete           (const std::string& fileHash)
{
    LuaEvent e;
    e.eventId = L4R_FILE_DOWNLOADFINISHED;
    e.timeStamp = QDateTime::currentDateTime();
    e.dataParm->setValue("strfileHash", QString::fromStdString(fileHash));

    L4R::L4RConfig->getCore()->processEvent(e);
}
