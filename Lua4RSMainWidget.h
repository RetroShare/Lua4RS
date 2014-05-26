#ifndef LUA4RSMAINWIDGET_H
#define LUA4RSMAINWIDGET_H

#include <retroshare-gui/mainpage.h>

#include <QWidget>

namespace Ui {
class Lua4RSMainWidget;
}

class LuaCore;

class Lua4RSMainWidget : public MainPage
{
    Q_OBJECT

public:
    explicit Lua4RSMainWidget(QWidget *parent = 0);
    ~Lua4RSMainWidget();

    void clearOutput();
    void appendOutput(const std::string &s);
    void appendOutput(const QString &s);

private:
    Ui::Lua4RSMainWidget *ui;
    LuaCore* _lua;

private slots:
    void runLua();
};

#endif // LUA4RSMAINWIDGET_H
