#ifndef LUA4RSPLUGIN_H
#define LUA4RSPLUGIN_H

#include <retroshare/rsplugin.h>
#include <retroshare-gui/mainpage.h>

class p3Lua4RS;

class Lua4RSPlugin : public RsPlugin
{
public:
    Lua4RSPlugin();

    virtual MainPage*       qt_page() const;
    virtual QIcon*          qt_icon() const;
    virtual QTranslator*    qt_translator(QApplication *app, const QString& languageCode, const QString& externalDir) const;
    virtual ConfigPage*     qt_config_page() const;
    virtual QDialog         *qt_about_page() const;


    virtual     RsPQIService*   rs_pqi_service() const;
    uint16_t    rs_service_id() const;

    virtual void    getPluginVersion(int &major, int &minor, int &build, int &svn_rev) const;
    virtual void    setPlugInHandler(RsPluginHandler *pgHandler);
    virtual void    stop();

    virtual std::string configurationFileName() const { return "Lua4RS.cfg"; }

    virtual std::string getShortPluginDescription() const;
    virtual std::string getPluginName() const;
    virtual void        setInterfaces(RsPlugInInterfaces& interfaces);

    virtual void getLibraries(std::list<RsLibraryInfo> &libraries);

protected:

private:
    mutable QIcon*              _icon ;
    mutable MainPage*           _mainpage ;
    mutable RsNotify*           _notify;
    mutable RsPluginHandler *   _pluginHandler;
    mutable RsPeers*            _peers;
    mutable p3Lua4RS*           _service;
};

#endif // LUA4RSPLUGIN_H
