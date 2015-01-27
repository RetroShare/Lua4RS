#include <retroshare/rsstatus.h>
#include <retroshare/rsmsgs.h>

#include "Lua4RSNotify.h"
#include "Lua/LuaCore.h"
#include "Lua/LuaEvent.h"
#include "interface/L4RInterface.h"

Lua4RSNotify::Lua4RSNotify()
{
}

///TODO find a better place
static void replaceAll(std::string& str, const std::string& from, const std::string& to)
{
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

static std::string& stripHTMLTags(std::string& s)
{
    // Remove all special HTML characters
    bool done = false;
    while(!done)
    {
        // Look for start of tag:
        size_t leftPos = s.find('<');
        if(leftPos != std::string::npos)
        {
            // See if tag close is in this line:
            size_t rightPos = s.find('>', leftPos);
            if(rightPos == std::string::npos)
            {
                done = true;
                s.erase(leftPos);
            }
            else
                s.erase(leftPos, rightPos - leftPos + 1);
        }
        else
            done = true;
    }

    replaceAll(s, "&lt;", "<");
    replaceAll(s, "&gt;", ">");
    replaceAll(s, "&amp;", "&");
    replaceAll(s, "&nbsp;", " ");

    return s;
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
    e.dataParm->setValue("strchatid", QString::fromUtf8(msg.chat_id.toStdString().c_str()));
    std::string msg2 = msg.msg;
    msg2 = stripHTMLTags(msg2);

    // PANIC: unprotected error in call to Lua API (invalid option '%1' to 'lua_pushfstring')
    ///TODO proper fix
    if(     msg2.find('%0') != std::string::npos ||
            msg2.find('%1') != std::string::npos ||
            msg2.find('%2') != std::string::npos ||
            msg2.find('%3') != std::string::npos ||
            msg2.find('%4') != std::string::npos ||
            msg2.find('%5') != std::string::npos ||
            msg2.find('%6') != std::string::npos ||
            msg2.find('%7') != std::string::npos ||
            msg2.find('%8') != std::string::npos ||
            msg2.find('%9') != std::string::npos) {
        std::cerr << "[Lua] Lua4RSNotify::notifyChatMessage : found harmful string aborting" << std::endl;
        return;
    }
    e.dataParm->setValue("strmsg", QString::fromUtf8(msg2.c_str()));
    e.dataParm->setValue("strnick", QString::fromUtf8(msg.lobby_peer_nickname.c_str()));

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
    e.dataParm->setValue("strnickname", QString::fromUtf8(nickname.c_str()));
    e.dataParm->setValue("stranyString", QString::fromUtf8(any_string.c_str()));

    L4R::L4RConfig->getCore()->processEvent(e);
}

void Lua4RSNotify::notifyCustomState                (const std::string& peer_id, const std::string& status_string)
{
    LuaEvent e;
    e.eventId = L4R_FRIEND_CUSTOM_STATE;
    e.timeStamp = QDateTime::currentDateTime();
    e.dataParm->setValue("strpeerId", QString::fromUtf8(peer_id.c_str()));
    e.dataParm->setValue("strstatusString", QString::fromUtf8(status_string.c_str()));

    L4R::L4RConfig->getCore()->processEvent(e);
}

void Lua4RSNotify::notifyHashingInfo                (uint32_t type, const std::string& fileinfo)
{
    LuaEvent e;
    e.eventId = L4R_FILE_HASHING_DONE;
    e.timeStamp = QDateTime::currentDateTime();
    e.dataParm->setValue("u32type", type);
    e.dataParm->setValue("strfileInfo", QString::fromUtf8(fileinfo.c_str()));

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
    e.dataParm->setValue("strpeerId", QString::fromUtf8(peer_id.c_str()));
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
    e.dataParm->setValue("strfileHash", QString::fromUtf8(fileHash.c_str()));

    L4R::L4RConfig->getCore()->processEvent(e);
}
