#ifndef LUA4RSPLUGIN_H
#define LUA4RSPLUGIN_H

#include <retroshare/rsplugin.h>
#include <retroshare-gui/mainpage.h>

#include <QIcon>

class Lua4RSPlugin : public RsPlugin
{
public:
    Lua4RSPlugin();

    virtual MainPage*   qt_page() const;
    virtual QIcon       *qt_icon() const;
    virtual void        getPluginVersion(int& major,int& minor,int& svn_rev) const;
    virtual void        setPlugInHandler(RsPluginHandler *pgHandler);
    virtual RsPQIService * rs_pqi_service() const;

    virtual std::string configurationFileName() const { return std::string() ; }

    virtual std::string getShortPluginDescription() const;
    virtual std::string getPluginName() const;
    virtual void        setInterfaces(RsPlugInInterfaces& interfaces);

protected:

private:
    mutable QIcon* _icon ;
    mutable MainPage* _mainpage ;
    mutable RsNotify* _notify;
    mutable RsPluginHandler *_pluginHandler;
    mutable RsPeers* _peers;
    mutable RsPQIService* _x;
};

#endif // LUA4RSPLUGIN_H
