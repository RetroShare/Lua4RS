#ifndef LUA4RSWIDGET_H
#define LUA4RSWIDGET_H

#include <retroshare-gui/mainpage.h>

#include <QWidget>

namespace Ui {
class Lua4RSWidget;
}

class LuaCore;

class Lua4RSWidget : public MainPage
{
  Q_OBJECT

  public:
    explicit Lua4RSWidget(QWidget *parent = 0);
    ~Lua4RSWidget();

    void clearOutput();
    void appendOutput(const std::string &s);
    void appendOutput(const QString &s);

    void appendLog(const std::string &s);
    void appendLog(const QString &s);

  private:
    Ui::Lua4RSWidget *ui;
    LuaCore* _lua;

  private slots:
    void on_pb_run_clicked();
    void on_pb_newscript_clicked();
    void on_pb_editscript_clicked();
    void on_pb_deletescript_clicked();
    void on_pb_load_clicked();
    void on_pb_save_clicked();
    void on_pb_undock_clicked();
    void on_pb_pastehint_clicked();
    void on_cbx_enable_toggled(bool checked);
    void on_cbx_timeconstraint_toggled(bool checked);
    void on_tied_timefrom_editingFinished();
    void on_tied_timeto_editingFinished();
};

#endif // LUA4RSMAINWIDGET_H
