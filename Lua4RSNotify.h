#ifndef LUA4RSNOTIFY_H
#define LUA4RSNOTIFY_H

#include <retroshare/rsplugin.h>
// #include "plugins/rspqiservice.h"
#include <retroshare/rsiface.h>

class Lua4RSNotify : public NotifyClient
{
public:
    Lua4RSNotify();

    virtual void notifyListPreChange              (int /* list */, int /* type */);
    virtual void notifyListChange                 (int /* list */, int /* type */);
    // virtual void notifyErrorMsg                   (int /* list */, int /* sev  */, std::string /* msg */);
    // virtual void notifyChatStatus                 (const std::string& /* peer_id  */, const std::string& /* status_string */ ,bool /* is_private */);
    virtual void notifyChatLobbyEvent             (uint64_t           /* lobby id */, uint32_t           /* event type    */ ,const std::string& /* nickname */,const std::string& /* any string */);
    // virtual void notifyChatLobbyTimeShift         (int                /* time_shift*/);
    virtual void notifyCustomState                (const std::string& /* peer_id   */, const std::string&               /* status_string */);
    virtual void notifyHashingInfo                (uint32_t           /* type      */, const std::string&               /* fileinfo      */);
    // virtual void notifyTurtleSearchResult         (uint32_t           /* search_id */, const std::list<TurtleFileInfo>& /* files         */);
    // virtual void notifyPeerHasNewAvatar           (std::string        /* peer_id   */);
    // virtual void notifyOwnAvatarChanged           ();
    // virtual void notifyOwnStatusMessageChanged    ();
    virtual void notifyDiskFull                   (uint32_t           /* location  */, uint32_t                         /* size limit in MB */);
    virtual void notifyPeerStatusChanged          (const std::string& /* peer_id   */, uint32_t                         /* status           */);

    /* one or more peers has changed the states */
    virtual void notifyPeerStatusChangedSummary   ();
    virtual void notifyDiscInfoChanged            ();
    // virtual void notifyForumMsgReadSatusChanged   (const std::string& /* channelId */, const std::string& /* msgId */, uint32_t /* status */);
    // virtual void notifyChannelMsgReadSatusChanged (const std::string& /* channelId */, const std::string& /* msgId */, uint32_t /* status */);
    // virtual bool askForDeferredSelfSignature      (const void *       /* data      */, const uint32_t     /* len   */, unsigned char * /* sign */, unsigned int * /* signlen */,int& signature_result ) { signature_result = false ;return true; }
    virtual void notifyDownloadComplete           (const std::string& /* fileHash  */);
    // virtual void notifyDownloadCompleteCount      (uint32_t           /* count     */);
    // virtual void notifyHistoryChanged             (uint32_t           /* msgId     */, int /* type */);

    // virtual bool askForPassword                   (const std::string& /* key_details     */, bool               /* prev_is_bad */, std::string& /* password */ );
    // virtual bool askForPluginConfirmation         (const std::string& /* plugin_filename */, const std::string& /* plugin_file_hash */);

};

#endif // LUA4RSNOTIFY_H
