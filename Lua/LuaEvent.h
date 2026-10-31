#ifndef LUAEVENT_H
#define LUAEVENT_H

#include <QDateTime>
#include <QVariant>
#include <QMap>
#include <QStringList>
#include <list>
#include <deque>
#include <memory>

// Each queued event owns its parameters; never share persistent QSettings.
class LuaEventParameters
{
public:
    void setValue(const QString& key, const QVariant& value) { values.insert(key, value); }
    QVariant value(const QString& key) const { return values.value(key); }
    QStringList allKeys() const { return values.keys(); }
    size_t estimatedBytes() const
    {
        size_t bytes = 128;
        for(auto it = values.cbegin(); it != values.cend(); ++it)
            bytes += 128 + size_t(it.key().size()) * 2 + size_t(it.value().toString().size()) * 2;
        return bytes;
    }
private:
    QMap<QString, QVariant> values;
};

//------------------------------------------------------------------------------
struct LuaEvent {
    LuaEvent() : dataParm(new LuaEventParameters()) {}
    ~LuaEvent() { delete dataParm; }
    LuaEvent(const LuaEvent&) = delete;
    LuaEvent& operator=(const LuaEvent&) = delete;

    uint        eventId;
    QDateTime   timeStamp;
    LuaEventParameters* dataParm;
};
/*
 * 3 chars to identify type
 * Types:
 * - string : str
 * - int    : int
 * - uint32 : u32
 * - uint64 : u64
 * - bool   : boo
 * - std::list<TurtleFileInfo>  : tfi
 */

typedef std::list<LuaEvent *> LuaEventList;

// Access is serialized by LuaCore's queue mutex. Rejected and pending events
// are automatically destroyed, including their parameter maps.
class LuaEventQueue
{
public:
    bool push(std::unique_ptr<LuaEvent> event)
    {
        if(!event) return false;
        const size_t bytes = event->dataParm->estimatedBytes();
        if(_events.size() >= 1024 || bytes > 8 * 1024 * 1024 - _bytes) return false;
        _events.push_back(std::move(event));
        _bytes += bytes;
        return true;
    }
    std::unique_ptr<LuaEvent> pop()
    {
        if(_events.empty()) return {};
        auto event = std::move(_events.front());
        _events.pop_front();
        _bytes -= event->dataParm->estimatedBytes();
        return event;
    }
    void clear() { _events.clear(); _bytes = 0; }
    size_t size() const { return _events.size(); }
private:
    std::deque<std::unique_ptr<LuaEvent>> _events;
    size_t _bytes = 0;
};

//------------------------------------------------------------------------------
//  Fundamental Events from the Plugin
//------------------------------------------------------------------------------
#define L4R_NOEVENT                     00000
#define L4R_TIMERTICK                   00001
#define L4R_STARTUP                     00002
#define L4R_SHUTDOWN                    99999

//------------------------------------------------------------------------------
//  Events from Infolog
//------------------------------------------------------------------------------
#define L4R_INFO_NEWENTRY               10000
#define L4R_INFO_PEERLISTCHANGE         10010
#define L4R_INFO_PEERLISTCHANGE_PRE     10011
#define L4R_INFO_DISCOVERY_UPDATE       10020
#define L4R_INFO_DISK_FULL              10030

//------------------------------------------------------------------------------
//  Events from Friends
//------------------------------------------------------------------------------
#define L4R_FRIEND_CAMEONLINE           11000
#define L4R_FRIEND_WENTOFFLINE          11010
#define L4R_FRIEND_ADDED                11020
#define L4R_FRIEND_BLOCKED              11030
#define L4R_FRIEND_CUSTOM_STATE         11040
#define L4R_FRIEND_STATUS_CHANGED       11050
#define L4R_FRIEND_STATUS_CHANGED_MANY  11060

//------------------------------------------------------------------------------
//  Events from File Downloads
//------------------------------------------------------------------------------
#define L4R_FILE_DOWNLOADSTARTED        12000
#define L4R_FILE_DOWNLOADFINISHED       12010
#define L4R_FILE_DOWNLOADCANCELLED      12020

//------------------------------------------------------------------------------
//  Events from File Uploads
//------------------------------------------------------------------------------
#define L4R_FILE_UPLOADSTARTED          13000
#define L4R_FILE_UPLOADFINISHED         13010
#define L4R_FILE_HASHING_DONE           13020

//------------------------------------------------------------------------------
//  Events from File Searches
//------------------------------------------------------------------------------
#define L4R_FILE_SEARCHSTARTED          14000
#define L4R_FILE_SEARCHRESULT           14010
#define L4R_FILE_SEARCHFINISHED         14020

//------------------------------------------------------------------------------
//  Events from Mails
//------------------------------------------------------------------------------
#define L4R_MAIL_RECEIVED               15000
#define L4R_MAIL_SENT                   15010
#define L4R_MAIL_DELETED                15020

//------------------------------------------------------------------------------
//  Events from Chatlobbys
//------------------------------------------------------------------------------
#define L4R_LOBBY_MESSAGERECEIVED       16000
#define L4R_LOBBY_MESSAGESENT           16010
#define L4R_LOBBY_PEERJOINED            16020
#define L4R_LOBBY_PEERLEFT              16030
#define L4R_LOBBY_PEERMUTED             16040
#define L4R_LOBBY_PEERUNMUTED           16050
#define L4R_LOBBY_EVENT                 16060

//------------------------------------------------------------------------------
//  Events from Private Chats
//------------------------------------------------------------------------------
#define L4R_PCHAT_CREATED               17000
#define L4R_PCHAT_INVITESENT            17010
#define L4R_PCHAT_INVITERECEIVED        17020
#define L4R_PCHAT_INVITEACCEPTED        17030
#define L4R_PCHAT_INVITEDECLINED        17040
#define L4R_PCHAT_JOINED                17050
#define L4R_PCHAT_LEFT                  17060

//------------------------------------------------------------------------------
//  Events from Forums
//------------------------------------------------------------------------------
#define L4R_FORUM_POSTRECEIVED          18000
#define L4R_FORUM_POSTSENT              18010
#define L4R_FORUM_CREATED               18020
#define L4R_FORUM_SUBSCRIBED            18030
#define L4R_FORUM_UNSUBSCRIBED          18040

//------------------------------------------------------------------------------
//  Events from Channels
//------------------------------------------------------------------------------
#define L4R_CHANNEL_POSTRECEIVED        19000
#define L4R_CHANNEL_POSTSENT            19010
#define L4R_CHANNEL_CREATED             19020
#define L4R_CHANNEL_SUBSCRIBED          19030
#define L4R_CHANNEL_UNSUBSCRIBED        19040

#endif // LUAEVENT_H
