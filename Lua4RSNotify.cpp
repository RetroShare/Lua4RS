#include "Lua4RSNotify.h"
#include <iostream>

Lua4RSNotify::Lua4RSNotify()
{
}

void Lua4RSNotify::notifyListPreChange              (int /* list */, int /* type */)
{
    std::cout << "notifyListPreChange" << std::endl;
}

void Lua4RSNotify::notifyListChange                 (int /* list */, int /* type */)
{
    std::cout << "notifyListChange" << std::endl;
}

void Lua4RSNotify::notifyChatStatus                 (const std::string& /* peer_id  */, const std::string& /* status_string */ ,bool /* is_private */)
{
    std::cout << "notifyChatStatus" << std::endl;
}

void Lua4RSNotify::notifyChatLobbyEvent             (uint64_t           /* lobby id */, uint32_t           /* event type    */ ,const std::string& /* nickname */,const std::string& /* any string */)
{
    std::cout << "notifyChatLobbyEvent" << std::endl;
}

void Lua4RSNotify::notifyHashingInfo                (uint32_t           /* type      */, const std::string&               /* fileinfo      */)
{
    std::cout << "notifyHashingInfo" << std::endl;
}

void Lua4RSNotify::notifyDiskFull                   (uint32_t           /* location  */, uint32_t                         /* size limit in MB */)
{
    std::cout << "notifyDiskFull" << std::endl;
}

void Lua4RSNotify::notifyPeerStatusChanged          (const std::string& /* peer_id   */, uint32_t                         /* status           */)
{
    std::cout << "notifyPeerStatusChanged" << std::endl;
}

void Lua4RSNotify::notifyPeerStatusChangedSummary   ()
{
    std::cout << "notifyPeerStatusChangedSummary" << std::endl;
}

void Lua4RSNotify::notifyDiscInfoChanged            ()
{
    std::cout << "notifyDiscInfoChanged" << std::endl;
}

void Lua4RSNotify::notifyDownloadComplete           (const std::string& /* fileHash  */)
{
    std::cout << "notifyDownloadComplete" << std::endl;
}
