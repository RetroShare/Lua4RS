#include <retroshare/rsplugin.h>
#include <util/rsversion.h>

#include "Lua4RSPlugin.h"
#include "Lua4RSMainWidget.h"
#include "Lua/LuaCore.h"

#define LUA_ICON_LINK ":/images/lua_logo.png"


extern "C" {
    void *RETROSHARE_PLUGIN_provide()
    {
        static Lua4RSPlugin *p = new Lua4RSPlugin() ;

        return (void*)p ;
    }

    // This symbol contains the svn revision number grabbed from the executable.
    // It will be tested by RS to load the plugin automatically, since it is safe to load plugins
    // with same revision numbers, assuming that the revision numbers are up-to-date.
    //
    uint32_t RETROSHARE_PLUGIN_revision = SVN_REVISION_NUMBER ;

    // This symbol contains the svn revision number grabbed from the executable.
    // It will be tested by RS to load the plugin automatically, since it is safe to load plugins
    // with same revision numbers, assuming that the revision numbers are up-to-date.
    //
    uint32_t RETROSHARE_PLUGIN_api = RS_PLUGIN_API_VERSION ;
}

Lua4RSPlugin::Lua4RSPlugin()
{
    _icon = NULL;
    _mainpage = NULL;
    _pluginHandler = NULL;
}

void Lua4RSPlugin::getPluginVersion(int& major, int& minor, int& svn_rev) const
{
    major = 5;
    minor = 5;
    svn_rev = SVN_REVISION_NUMBER;
}

void Lua4RSPlugin::setInterfaces(RsPlugInInterfaces &interfaces)
{
    // get stuff
    _peers = interfaces.mPeers;
    _notify = interfaces.mNotify;

    // setup other stuff
    LuaCore* lc = LuaCore::getInstance();
    lc->setPeers(_peers);
    _notify->registerNotifyClient(lc->notify());
}

MainPage* Lua4RSPlugin::qt_page() const
{
    if(_mainpage == NULL)
        _mainpage = new Lua4RSMainWidget();

    return _mainpage ;
}

QIcon* Lua4RSPlugin::qt_icon() const
{
    if(_icon == NULL)
    {
        Q_INIT_RESOURCE(Lua4RS_images);
        _icon = new QIcon(LUA_ICON_LINK);
    }

    return _icon ;
}

#include "p3Lua4RS.h"

RsPQIService* Lua4RSPlugin::rs_pqi_service() const
{
    if(_x == NULL)
        _x = new p3Lua4RS(_pluginHandler);

    return _x;
}

void Lua4RSPlugin::setPlugInHandler(RsPluginHandler *pgHandler)
{
    _pluginHandler = pgHandler;
}

std::string Lua4RSPlugin::getShortPluginDescription() const
{
    return "This plugin let you script RS with Lua.";
}

std::string Lua4RSPlugin::getPluginName() const
{
    return "Lua4RS";
}
