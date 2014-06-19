#ifndef LUAEVENT_H
#define LUAEVENT_H

#include <QDateTime>


//------------------------------------------------------------------------------
struct LuaEvent {
    uint        eventId;
    QDateTime   timeStamp;
    QObject*    dataParm;
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
#define L4R_FRIENDS_STATUS_CHANGED      11050

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
