#ifndef LUA4RSWIDGET_H
#define LUA4RSWIDGET_H

#include <retroshare-gui/mainpage.h>

#include "Lua/LuaCore.h"

namespace Ui {
class Lua4RSWidget;
}

class LuaCore;
class QTableWidgetItem;
class QModelIndex;


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
    void setLuaCodes(LuaList* list);
    void switchContainer(LuaContainer* container);

    // checks whether things entered in GUI are sane (like name is not empry)
    bool saneValues();

    // all scripts helper
    LuaContainer* allScriptsGetLuaContainerFromSelectedRow();
    LuaContainer* allScriptsGetLuaContainerFromRow(const int row);
    void allScriptsAddRow(LuaContainer* container);

    // this function will fill every form with it's corresponding values
    void luaContainerToUi(LuaContainer* container);
    // or the other way round
    bool uiToLuaContainer(LuaContainer* container);

    Ui::Lua4RSWidget *ui;
    LuaCore* _lua;

    LuaContainer* _activeContainer;

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
    void on_lw_allscripts_itemChanged(QTableWidgetItem *item);
    void on_tw_allscripts_cellDoubleClicked(int row, int column);
    void on_dd_everyunits_currentIndexChanged(int index);
    void on_spb_everycount_editingFinished();
    void on_rb_runonevent_toggled(bool checked);
    void on_pb_pastehint_released();
    void on_tw_hints_itemDoubleClicked(QTreeWidgetItem *item, int column);
};

#endif // LUA4RSMAINWIDGET_H
