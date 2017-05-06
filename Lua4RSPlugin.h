#ifndef LUA4RSPLUGIN_H
#define LUA4RSPLUGIN_H

#include <retroshare/rsplugin.h>
#include <retroshare-gui/mainpage.h>

class p3Lua4RS;

class Lua4RSPlugin : public RsPlugin
{
public:
    Lua4RSPlugin();

    MainPage        *qt_page() const;
    QIcon           *qt_icon() const;
    QTranslator     *qt_translator(QApplication *app, const QString &languageCode, const QString &externalDir) const;
    ConfigPage      *qt_config_page() const;
    QDialog         *qt_about_page() const;

    p3Service   *p3_service() const;
    uint16_t    rs_service_id() const;

    void    getPluginVersion(int &major, int &minor, int &build, int &svn_rev) const;
    void    setPlugInHandler(RsPluginHandler *pgHandler);
    void    stop();

    std::string configurationFileName() const { return "Lua4RS.cfg"; }

    std::string getShortPluginDescription() const;
    std::string getPluginName() const;
    void        setInterfaces(RsPlugInInterfaces& interfaces);

    void getLibraries(std::list<RsLibraryInfo> &libraries);

protected:

private:
    mutable QIcon           *_icon ;
    mutable MainPage        *_mainpage ;
    mutable RsNotify        *_notify;
    mutable RsPluginHandler *_pluginHandler;
    mutable RsPeers         *_peers;
    mutable p3Lua4RS        *_service;
};

#endif // LUA4RSPLUGIN_H
