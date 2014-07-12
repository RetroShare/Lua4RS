#include <QIcon>
#include <QTranslator>

#include <retroshare/rsplugin.h>
#include <util/rsversion.h>

#include "Lua4RSConfig.h"
#include "Lua4RSNotify.h"
#include "Lua4RSPlugin.h"
#include "Lua4RSWidget.h"
#include "interface/Lua4RSInterface.h"
#include "Lua/LuaCore.h"
#include "service/p3Lua4RS.h"

#define LUA_ICON_LINK ":/images/lua_logo.png"

extern "C" {
#ifdef WIN32
    __declspec(dllexport)
#endif
    void *RETROSHARE_PLUGIN_provide()
    {
        static Lua4RSPlugin *p = new Lua4RSPlugin() ;

        return (void*)p ;
    }

    // This symbol contains the svn revision number grabbed from the executable.
    // It will be tested by RS to load the plugin automatically, since it is safe to load plugins
    // with same revision numbers, assuming that the revision numbers are up-to-date.
    //
#ifdef WIN32
    __declspec(dllexport)
#endif
    uint32_t RETROSHARE_PLUGIN_revision = SVN_REVISION_NUMBER ;

    // This symbol contains the svn revision number grabbed from the executable.
    // It will be tested by RS to load the plugin automatically, since it is safe to load plugins
    // with same revision numbers, assuming that the revision numbers are up-to-date.
    //
#ifdef WIN32
    __declspec(dllexport)
#endif
    uint32_t RETROSHARE_PLUGIN_api = RS_PLUGIN_API_VERSION ;
}

Lua4RSPlugin::Lua4RSPlugin()
{
    _icon = NULL;
    _mainpage = NULL;
    _notify = NULL;
    _peers = NULL;
    _pluginHandler = NULL;
    _service = NULL;
}

void Lua4RSPlugin::stop()
{
    LuaCore::shutDown();

    // this code causes Segmentation fault on shutdown ....
    /*
    delete _icon;
    _icon = NULL;
    delete _mainpage;
    _mainpage = NULL;

    // from RS
    _notify = NULL;
    _peers = NULL;
    _pluginHandler = NULL;
    */
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
    lc->setUi(dynamic_cast<Lua4RSWidget*>(qt_page()));
    _notify->registerNotifyClient(lc->notify());
}

MainPage* Lua4RSPlugin::qt_page() const
{
    if(_mainpage == NULL)
        _mainpage = new Lua4RSWidget();

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

QTranslator* Lua4RSPlugin::qt_translator(QApplication* /*app*/, const QString& languageCode, const QString& externalDir) const
{
    if (languageCode == "en") {
        return NULL;
    }

    QTranslator* translator = new QTranslator();

    if (translator->load(externalDir + "/Lua4RS_" + languageCode + ".qm")) {
        return translator;
    } else if (translator->load(":/lang/Lua4RS_" + languageCode + ".qm")) {
        return translator;
    }

    delete(translator);
    return NULL;
}

ConfigPage *Lua4RSPlugin::qt_config_page() const
{
    return new Lua4RSConfig();
}

RsPQIService *Lua4RSPlugin::rs_pqi_service() const
{
    if(_service == NULL)
    {
        _service = new p3Lua4RS(_pluginHandler);
        L4R::L4RConfig = _service;
    }

    return _service;
}

uint16_t Lua4RSPlugin::rs_service_id() const
{
    return RS_SERVICE_TYPE_L4R_PLUGIN;
}

void Lua4RSPlugin::setPlugInHandler(RsPluginHandler *pgHandler)
{
    _pluginHandler = pgHandler;
}

std::string Lua4RSPlugin::getShortPluginDescription() const
{
    return QObject::tr("This plugin let you script RS with Lua.").toStdString();
}

std::string Lua4RSPlugin::getPluginName() const
{
    return "Lua4RS";
}
