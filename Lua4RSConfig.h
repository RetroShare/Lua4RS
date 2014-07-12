#ifndef LUA4RSCONFIG_H
#define LUA4RSCONFIG_H

#include <QWidget>

#include <retroshare-gui/configpage.h>

namespace Ui {
class Lua4RSConfig;
}

class Lua4RSConfig : public ConfigPage
{
    Q_OBJECT

public:

    explicit Lua4RSConfig(QWidget *parent = 0);
    ~Lua4RSConfig();

    /** Pure virtual method. Subclassed pages load their config settings here. */
    virtual void load();

    /** Pure virtual method. Subclassed pages save their config settings here
     * and return true if everything was saved successfully. */
    virtual bool save(QString &errmsg);

    bool wasLoaded() { return loaded ; }

    virtual QPixmap iconPixmap() const { return QPixmap(":/images/lua_logo.png"); }
    virtual QString pageName() const { return "Lua4RS"; }
    virtual QString helpText() const { return tr("NOBODY WILL HELP YOU :O"); }

private:
    Ui::Lua4RSConfig *ui;
};

#endif // LUA4RSCONFIG_H
