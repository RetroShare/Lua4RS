#include <retroshare/rsidentity.h>
#include <retroshare/rsstatus.h>
#include <retroshare/rschats.h>
#include <retroshare/rspeers.h>
#include <retroshare/rsgossipdiscovery.h>
#include <stdexcept>

#include "Lua4RSNotify.h"
#include "Lua/LuaCore.h"
#include "Lua/LuaEvent.h"
#include "interface/L4RInterface.h"

#include "helper.h"

namespace {
// Values formerly defined in rsnotify.h, retained for Lua event parameters.
constexpr int LegacyNeighbours = 1;
constexpr int LegacyLocalDirectories = 9;
constexpr int LegacyFriendDirectories = 10;
constexpr int LegacyModified = 2;
constexpr int LegacyAdded = 4;
constexpr int LegacyDeleted = 8;
}

Lua4RSNotify::Lua4RSNotify()
{
}

Lua4RSNotify::~Lua4RSNotify()
{
    stop();
}

void Lua4RSNotify::start()
{
    if(_handlerId) return;
    if(!rsEvents) throw std::runtime_error("Lua4RS: event service unavailable");
    const auto error = rsEvents->registerEventsHandler(
        [this](std::shared_ptr<const RsEvent> event) { handleEvent(event); },
        _handlerId);
    if(error) throw std::runtime_error("Lua4RS: " + error.message());
}

void Lua4RSNotify::stop()
{
    if(_handlerId && rsEvents)
        rsEvents->unregisterEventsHandler(_handlerId);
    _handlerId = 0;
}

void Lua4RSNotify::handleEvent(std::shared_ptr<const RsEvent> event)
{
    if(auto chat = std::dynamic_pointer_cast<const RsChatServiceEvent>(event))
    {
        if(chat->mEventCode == RsChatServiceEventCode::CHAT_MESSAGE_RECEIVED)
            notifyChatMessage(chat->mMsg);
    }
    else if(auto lobby = std::dynamic_pointer_cast<const RsChatLobbyEvent>(event))
    {
        // Preserve the legacy event numbers exposed to Lua scripts.
        uint32_t code = 0;
        switch(lobby->mEventCode)
        {
        case RsChatLobbyEventCode::CHAT_LOBBY_EVENT_PEER_LEFT: code = RS_CHAT_LOBBY_EVENT_PEER_LEFT; break;
        case RsChatLobbyEventCode::CHAT_LOBBY_EVENT_PEER_STATUS: code = RS_CHAT_LOBBY_EVENT_PEER_STATUS; break;
        case RsChatLobbyEventCode::CHAT_LOBBY_EVENT_PEER_JOINED: code = RS_CHAT_LOBBY_EVENT_PEER_JOINED; break;
        case RsChatLobbyEventCode::CHAT_LOBBY_EVENT_PEER_CHANGE_NICKNAME: code = RS_CHAT_LOBBY_EVENT_PEER_CHANGE_NICKNAME; break;
        case RsChatLobbyEventCode::CHAT_LOBBY_EVENT_KEEP_ALIVE: code = RS_CHAT_LOBBY_EVENT_KEEP_ALIVE; break;
        default: break;
        }
        if(code) notifyChatLobbyEvent(lobby->mLobbyId, code, lobby->mGxsId, lobby->mStr);
    }
    else if(auto peer = std::dynamic_pointer_cast<const RsFriendListEvent>(event))
    {
        switch(peer->mEventCode)
        {
        case RsFriendListEventCode::NODE_STATUS_CHANGED:
            notifyPeerStatusChanged(peer->mSslId.toStdString(), static_cast<uint32_t>(peer->mStatus));
            notifyPeerStatusChangedSummary();
            break;
        case RsFriendListEventCode::NODE_STATE_STRING_CHANGED:
            notifyCustomState(peer->mSslId.toStdString(), peer->mStateString);
            break;
        case RsFriendListEventCode::NODE_ADDED:
        case RsFriendListEventCode::PROFILE_ADDED:
            notifyListChange(LegacyNeighbours, LegacyAdded);
            break;
        case RsFriendListEventCode::NODE_REMOVED:
        case RsFriendListEventCode::PROFILE_REMOVED:
            notifyListChange(LegacyNeighbours, LegacyDeleted);
            break;
        default: break;
        }
    }
    else if(auto shared = std::dynamic_pointer_cast<const RsSharedDirectoriesEvent>(event))
    {
        switch(shared->mEventCode)
        {
        case RsSharedDirectoriesEventCode::HASHING_PROCESS_STARTED:
            notifyHashingInfo(1, shared->mFilePath); break;
        case RsSharedDirectoriesEventCode::HASHING_PROCESS_FINISHED:
            notifyHashingInfo(2, shared->mFilePath); break;
        case RsSharedDirectoriesEventCode::HASHING_FILE:
            notifyHashingInfo(3, shared->mFilePath); break;
        case RsSharedDirectoriesEventCode::SAVING_FILE_INDEX:
            notifyHashingInfo(4, shared->mFilePath); break;
        case RsSharedDirectoriesEventCode::OWN_DIR_LIST_PROCESSING:
            notifyListPreChange(LegacyLocalDirectories, LegacyModified); break;
        case RsSharedDirectoriesEventCode::OWN_DIR_LIST_UPDATED:
            notifyListChange(LegacyLocalDirectories, LegacyModified); break;
        case RsSharedDirectoriesEventCode::FRIEND_DIR_LIST_UPDATED:
            notifyListChange(LegacyFriendDirectories, LegacyModified); break;
        default: break;
        }
    }
    else if(auto file = std::dynamic_pointer_cast<const RsFileTransferEvent>(event))
    {
        if(file->mFileTransferEventCode == RsFileTransferEventCode::DOWNLOAD_COMPLETE)
            notifyDownloadComplete(file->mHash.toStdString());
        else if(file->mFileTransferEventCode == RsFileTransferEventCode::NEW_DISTANT_SEARCH_RESULTS)
            notifyTurtleSearchResult(file->mRequestId, file->mResults);
    }
    else if(auto system = std::dynamic_pointer_cast<const RsSystemEvent>(event))
    {
        if(system->mEventCode == RsSystemEventCode::DISK_SPACE_ERROR)
            notifyDiskFull(system->mDiskErrorLocation, system->mDiskErrorSizeLimit);
    }
    else if(auto discovery = std::dynamic_pointer_cast<const RsGossipDiscoveryEvent>(event))
    {
        if(discovery->mGossipDiscoveryEventType == RsGossipDiscoveryEventType::DISCOVERY_INFO_RECEIVED)
            notifyDiscInfoChanged();
    }
}

void Lua4RSNotify::notifyListPreChange              (int list, int type)
{
    struct LuaEvent *e = new LuaEvent();
    e->eventId = L4R_INFO_PEERLISTCHANGE_PRE;
    e->timeStamp = QDateTime::currentDateTime();
    e->dataParm->setValue("intlist", list);
    e->dataParm->setValue("inttype", type);

    L4R::L4RConfig->getCore()->processEvent(e);
}

void Lua4RSNotify::notifyListChange                 (int list, int type)
{
    struct LuaEvent *e = new LuaEvent();
    e->eventId = L4R_INFO_PEERLISTCHANGE;
    e->timeStamp = QDateTime::currentDateTime();
    e->dataParm->setValue("intlist", list);
    e->dataParm->setValue("inttype", type);

    L4R::L4RConfig->getCore()->processEvent(e);
}

void Lua4RSNotify::notifyChatMessage(const ChatMessage &msg)
{
    struct LuaEvent *e = new LuaEvent();
    e->eventId = L4R_LOBBY_MESSAGERECEIVED;
    e->timeStamp = QDateTime::currentDateTime();
    e->dataParm->setValue("strchatid", QString::fromUtf8(msg.chat_id.toStdString().c_str()));
    std::string msg2 = msg.msg;
    msg2 = stripHTMLTags(msg2);
    e->dataParm->setValue("strmsg", QString::fromUtf8(msg2.c_str()));
    RsGxsId gxsid = msg.lobby_peer_gxs_id;
    RsIdentityDetails gxsIdDetails;
    rsIdentity->getIdDetails(gxsid, gxsIdDetails);
    e->dataParm->setValue("strgxsid", QString::fromUtf8(gxsid.toStdString().c_str()));
    e->dataParm->setValue("strnick", QString::fromUtf8(gxsIdDetails.mNickname.c_str()));

    L4R::L4RConfig->getCore()->processEvent(e);
}

void Lua4RSNotify::notifyChatLobbyEvent             (uint64_t lobby_id, uint32_t event_type, const RsGxsId& nickname, const std::string& any_string)
{
    QVariant v = QVariant::fromValue<uint64_t>(lobby_id);
    struct LuaEvent *e = new LuaEvent();
    e->eventId = L4R_LOBBY_EVENT;
    e->timeStamp = QDateTime::currentDateTime();
    e->dataParm->setValue("u64lobbyId", v);
    e->dataParm->setValue("u32eventType", event_type);
    e->dataParm->setValue("strnickname", QString::fromUtf8(nickname.toStdString().c_str()));
    e->dataParm->setValue("stranyString", QString::fromUtf8(any_string.c_str()));

    L4R::L4RConfig->getCore()->processEvent(e);
}

void Lua4RSNotify::notifyCustomState                (const std::string& peer_id, const std::string& status_string)
{
    struct LuaEvent *e = new LuaEvent();
    e->eventId = L4R_FRIEND_CUSTOM_STATE;
    e->timeStamp = QDateTime::currentDateTime();
    e->dataParm->setValue("strpeerId", QString::fromUtf8(peer_id.c_str()));
    e->dataParm->setValue("strstatusString", QString::fromUtf8(status_string.c_str()));

    L4R::L4RConfig->getCore()->processEvent(e);
}

void Lua4RSNotify::notifyHashingInfo                (uint32_t type, const std::string& fileinfo)
{
    struct LuaEvent *e = new LuaEvent();
    e->eventId = L4R_FILE_HASHING_DONE;
    e->timeStamp = QDateTime::currentDateTime();
    e->dataParm->setValue("u32type", type);
    e->dataParm->setValue("strfileInfo", QString::fromUtf8(fileinfo.c_str()));

    L4R::L4RConfig->getCore()->processEvent(e);
}

void Lua4RSNotify::notifyDiskFull                   (uint32_t location, uint32_t size_limit /* in MB */)
{
    struct LuaEvent *e = new LuaEvent();
    e->eventId = L4R_INFO_DISK_FULL;
    e->timeStamp = QDateTime::currentDateTime();
    e->dataParm->setValue("u32location", location);
    e->dataParm->setValue("u32sizeLimit", size_limit);

    L4R::L4RConfig->getCore()->processEvent(e);
}

void Lua4RSNotify::notifyPeerStatusChanged          (const std::string& peer_id, uint32_t status)
{
    struct LuaEvent *e = new LuaEvent();

    if(status == static_cast<uint32_t>(RsStatusValue::RS_STATUS_ONLINE))
        e->eventId = L4R_FRIEND_CAMEONLINE;
    else if(status == static_cast<uint32_t>(RsStatusValue::RS_STATUS_OFFLINE))
        e->eventId = L4R_FRIEND_WENTOFFLINE;
    else
        e->eventId = L4R_FRIEND_STATUS_CHANGED;

    e->timeStamp = QDateTime::currentDateTime();
    e->dataParm->setValue("strpeerId", QString::fromUtf8(peer_id.c_str()));
    e->dataParm->setValue("u32status", status);

    L4R::L4RConfig->getCore()->processEvent(e);
}

void Lua4RSNotify::notifyPeerStatusChangedSummary   ()
{
    struct LuaEvent *e = new LuaEvent();
    e->eventId = L4R_FRIEND_STATUS_CHANGED_MANY;
    e->timeStamp = QDateTime::currentDateTime();

    L4R::L4RConfig->getCore()->processEvent(e);
}

void Lua4RSNotify::notifyDiscInfoChanged            ()
{
    struct LuaEvent *e = new LuaEvent();
    e->eventId = L4R_INFO_DISCOVERY_UPDATE;
    e->timeStamp = QDateTime::currentDateTime();

    L4R::L4RConfig->getCore()->processEvent(e);
}

void Lua4RSNotify::notifyDownloadComplete           (const std::string& fileHash)
{
    struct LuaEvent *e = new LuaEvent();
    e->eventId = L4R_FILE_DOWNLOADFINISHED;
    e->timeStamp = QDateTime::currentDateTime();
    e->dataParm->setValue("strfileHash", QString::fromUtf8(fileHash.c_str()));

    L4R::L4RConfig->getCore()->processEvent(e);
}

void Lua4RSNotify::notifyTurtleSearchResult         (uint32_t search_id, const std::vector<TurtleFileInfoV2>& files)
{
    for (const TurtleFileInfoV2& file : files) {
        struct LuaEvent *e = new LuaEvent();

        e->eventId = L4R_FILE_SEARCHRESULT;
        e->timeStamp = QDateTime::currentDateTime();
        e->dataParm->setValue("u64searchId", search_id);
        e->dataParm->setValue("strhash", QString::fromStdString(file.fHash.toStdString()));
        e->dataParm->setValue("strname", QString::fromUtf8(file.fName.c_str()));
        e->dataParm->setValue("u64size", quint64(file.fSize));

        L4R::L4RConfig->getCore()->processEvent(e);
    }

}
