#ifndef LUA4RSNOTIFY_H
#define LUA4RSNOTIFY_H

#include <retroshare/rschats.h>
#include <retroshare/rsfiles.h>
#include <retroshare/rsevents.h>

class Lua4RSNotify
{
public:
    Lua4RSNotify();
    ~Lua4RSNotify();
    void start();
    void stop();

	// Legacy Lua event adapters
public:
    void notifyListPreChange              (int /* list */, int /* type */);
    void notifyListChange                 (int /* list */, int /* type */);
    void notifyChatMessage                (const ChatMessage& /* msg       */);
    void notifyChatLobbyEvent             (uint64_t           /* lobby id  */, uint32_t							/* event type    */, const RsGxsId& /* nickname */, const std::string& /* any string */);
    void notifyCustomState                (const std::string& /* peer_id   */, const std::string&               /* status_string */);
    void notifyHashingInfo                (uint32_t           /* type      */, const std::string&               /* fileinfo      */);
    void notifyTurtleSearchResult         (uint32_t search_id, const std::vector<TurtleFileInfoV2>& files);
    void notifyDiskFull                   (uint32_t           /* location  */, uint32_t                         /* size limit in MB */);
    void notifyPeerStatusChanged          (const std::string& /* peer_id   */, uint32_t                         /* status           */);

    /* one or more peers has changed the states */
    void notifyPeerStatusChangedSummary   ();
    void notifyDiscInfoChanged            ();
    void notifyDownloadComplete           (const std::string& /* fileHash  */);

private:
    void handleEvent(std::shared_ptr<const RsEvent> event);
    RsEventsHandlerId_t _handlerId = 0;
};

#endif // LUA4RSNOTIFY_H
