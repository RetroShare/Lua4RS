#ifndef LUA4RSNOTIFY_H
#define LUA4RSNOTIFY_H

#include <retroshare/rsplugin.h>
#include <retroshare/rsiface.h>

class Lua4RSNotify : public NotifyClient
{
public:
    Lua4RSNotify();

    void notifyListPreChange              (int /* list */, int /* type */);
    void notifyListChange                 (int /* list */, int /* type */);
    void notifyChatMessage                (const ChatMessage& /* msg      */);
    void notifyChatLobbyEvent             (uint64_t           /* lobby id */, uint32_t           /* event type    */, const RsGxsId& /* nickname */, const std::string& /* any string */);
    void notifyCustomState                (const std::string& /* peer_id   */, const std::string&               /* status_string */);
    void notifyHashingInfo                (uint32_t           /* type      */, const std::string&               /* fileinfo      */);
    void notifyDiskFull                   (uint32_t           /* location  */, uint32_t                         /* size limit in MB */);
    void notifyPeerStatusChanged          (const std::string& /* peer_id   */, uint32_t                         /* status           */);

    /* one or more peers has changed the states */
    void notifyPeerStatusChangedSummary   ();
    void notifyDiscInfoChanged            ();
    void notifyDownloadComplete           (const std::string& /* fileHash  */);

    void notifyTurtleSearchResult         (uint32_t           /* search_id */, const std::list<TurtleFileInfo>& /* files         */);
};

#endif // LUA4RSNOTIFY_H
