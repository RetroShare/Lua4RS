#include <assert.h>

#include <QWidget>
#include <QModelIndex>

#include "Lua4RSWidget.h"
#include "ui_Lua4RSWidget.h"

Lua4RSWidget::Lua4RSWidget(QWidget *parent) :
    MainPage(parent),
    ui(new Ui::Lua4RSWidget),
    _activeContainer(NULL)
{
    ui->setupUi(this);

    _lua = LuaCore::getInstance();
    _lua->setUi(this);

    setLuaCodes(_lua->codeList());
    luaContainerToUi(_activeContainer);

    // Fill Hints TreeWidget with main items
    _lua->setupRsFunctionsAndTw(ui->tw_hints);

    // f*c: Set header resize mode of tw_allscripts to content dependant
    ui->tw_allscripts->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

    // Help Button
    QString help_str = tr(
    "<h1><img width=\"32\" src=\":/images/64px_help.png\">&nbsp;&nbsp;Lua4RS</h1> \
      <p>With Lua4RS you get three things with one Plugin: </p> \
      <ul> \
        <li>You can write, save, load and run Lua programs within RetroShare.</li> \
        <li>You can use Lua programs like macros (think of macros in LibreOffice) \
            to control and automate many features of RetroShare. </li> \
        <li>You can execute your Lua programs either by timer control (think of \
            cron or at) or by certain RetroShare events (e.g. <i>a friend comes \
            online</i> or <i>a chat message is received</i> and many more).</li> \
      </ul> \
    ");

    registerHelpButton(ui->helpButton, help_str);
}

Lua4RSWidget::~Lua4RSWidget()
{
    delete ui;
}

void Lua4RSWidget::setLuaCodes(LuaList* list)
{
    ui->tw_allscripts->setRowCount(0);

    LuaContainerList::const_iterator it;
    for(it = list->begin(); it != list->end(); ++it)
        allScriptsAddRow(*it);
}

void Lua4RSWidget::clearOutput()
{
    ui->tb_output->clear();
}

void Lua4RSWidget::appendOutput(const std::string& s)
{
    appendOutput(QString::fromStdString(s));
}

void Lua4RSWidget::appendOutput(const QString& s)
{
    ui->tb_output->appendPlainText(s);
}

void Lua4RSWidget::appendLog(const std::string& s)
{
    appendLog(QString::fromStdString(s));
}

void Lua4RSWidget::appendLog(const QString& s)
{
    ui->tb_log->appendPlainText(QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss") + QString(" > ") + s);
}

/* #############################################################
 * # helper
 * #############################################################
 */

LuaContainer* Lua4RSWidget::allScriptsGetLuaContainerFromSelectedRow()
{
    // get corresponding LuaContainer
    QModelIndexList rows = ui->tw_allscripts->selectionModel()->selectedRows();
    if(rows.count() != 1)
        return NULL;

    return allScriptsGetLuaContainerFromRow(rows[0].row());
}

LuaContainer* Lua4RSWidget::allScriptsGetLuaContainerFromRow(const int row)
{
    if(row < 0)
        return NULL;

    // get script name
    QTableWidgetItem* name = ui->tw_allscripts->item(row, 0);

    std::cout << "[Lua] Lua4RSWidget::allScriptsGetLuaContainerFromRow : trying to load LuaContaienr for " << name->text().toStdString() << " ...";

    // get container by name
    LuaContainer* container;
    if(LuaCore::getInstance()->codeList()->itemByName(name->text(), container))
    {
        std::cout << " got it!" << std::endl;
        return container;
    }
    else
    {
        std::cout << " failed!" << std::endl;
        return NULL;
    }
}

void Lua4RSWidget::allScriptsAddRow(LuaContainer* container)
{
    // disable sorting (better performance)
    ui->tw_allscripts->setSortingEnabled(false);
    int rows = ui->tw_allscripts->rowCount();
    ui->tw_allscripts->setRowCount(rows + 1);

    QTableWidgetItem* name = new QTableWidgetItem();
    QTableWidgetItem* desc = new QTableWidgetItem();
    QTableWidgetItem* lastRun = new QTableWidgetItem();
    QTableWidgetItem* trigger = new QTableWidgetItem();

    name->setText(container->getName());
    desc->setText(container->getDesc());
    ///TODO rest

    ui->tw_allscripts->setItem(rows, 0, name);
    ui->tw_allscripts->setItem(rows, 1, desc);
    ui->tw_allscripts->setItem(rows, 2, lastRun);
    ui->tw_allscripts->setItem(rows, 3, trigger);

    ui->tw_allscripts->setSortingEnabled(true);
}

void Lua4RSWidget::luaContainerToUi(LuaContainer* container)
{
    if(container == NULL)
    {
        ui->le_scriptname->clear();
        ui->le_scriptdesc->clear();
        ui->pte_luacode->clear();

        ///TODO there might be better ways that this - good enough for the moment
        ui->pte_luacode->setEnabled(false);
    } else
    {
        ui->le_scriptname->setText(container->getName());
        ui->le_scriptdesc->setText(container->getDesc());
        ui->pte_luacode->setPlainText(container->getCode());
        ///TODO rest

        ui->pte_luacode->setEnabled(true);
    }
}

void Lua4RSWidget::uiToLuaContainer(LuaContainer* container)
{
    container->setName(ui->le_scriptname->text());
    container->setDesc(ui->le_scriptdesc->text());
    container->setCode(ui->pte_luacode->toPlainText());
    ///TODO rest
}

void Lua4RSWidget::switchContainer(LuaContainer* container)
{
    // remember conatiner
    _activeContainer = container;

    // update UI
    luaContainerToUi(_activeContainer);

    std::cout << "[Lua] Lua4RSWidget::switchContainer : switched to " << _activeContainer->getName().toStdString() << std::endl;
}

/* #############################################################
 * # slots
 * #############################################################
 */
// "Run" clicked : execute the script in the editor control
void Lua4RSWidget::on_pb_run_clicked()
{
    appendLog(QString("running: ") + ui->le_scriptname->text());

    QString code = ui->pte_luacode->toPlainText();
    // not sure if this is actually needed - better safe than sorry
    std::string luaCode;
#ifdef _WIN32
    luaCode = code.toLocal8Bit().constData();
#else
    luaCode = code.toUtf8().constData();
#endif
    _lua->runLuaByString(luaCode);

}

// "New" clicked : create a new empty script
void Lua4RSWidget::on_pb_newscript_clicked()
{
    ///TODO make function for creating new container
    _activeContainer = new LuaContainer();
    _activeContainer->setName(QString("new.lua"));

    // add new container to list
    _lua->codeList()->addItem(_activeContainer);

    // update all scripts
    setLuaCodes(_lua->codeList());

    // update ui
    luaContainerToUi(_activeContainer);
}

// "Edit" clicked : edit the script selected in AllMyScripts
void Lua4RSWidget::on_pb_editscript_clicked()
{
    // get corresponding LuaContainer
    LuaContainer* container = allScriptsGetLuaContainerFromSelectedRow();

    if(container == NULL)
    {
        std::cerr << "[Lua] Lua4RSWidget::on_pb_editscript_clicked : got NULL" << std::endl;
        return;
    }
    switchContainer(container);
}

// "Delete" clicked : delete the script selected in AllMyScripts
void Lua4RSWidget::on_pb_deletescript_clicked()
{
    LuaContainer* container = allScriptsGetLuaContainerFromSelectedRow();
    if(container == NULL)
        return;

    // update UI when necessary
    if(_activeContainer == container)
    {
        _activeContainer = NULL;
        luaContainerToUi(_activeContainer);
    }

    _lua->codeList()->removeItemAndDelete(container);
    delete container;
    container = NULL;

    // update all scripts
    setLuaCodes(_lua->codeList());
}

// "Load" clicked : load a scriptfile from disk into the editor control
void Lua4RSWidget::on_pb_load_clicked()
{

}

// "Save" clicked : save the contents of the editor control to a file on disk
void Lua4RSWidget::on_pb_save_clicked()
{
    if(_activeContainer == NULL)
        return;

    // get values from ui
    uiToLuaContainer(_activeContainer);

    if(!_lua->codeList()->saveAll())
        std::cerr << "[Lua] Lua4RSWidget::on_pb_save_clicked() : failed to save " << _activeContainer->getName().toStdString() << std::endl;
    else
        std::cout << "[Lua] Lua4RSWidget::on_pb_save_clicked() : saved " << _activeContainer->getName().toStdString() << std::endl;

    // update all scripts
    setLuaCodes(_lua->codeList());
}

// "Undock" clicked : detach the editor control from the plugin into an own window
void Lua4RSWidget::on_pb_undock_clicked()
{

}

// "Paste" clicked : paste the selected rs hint into the editor at cursor pos
void Lua4RSWidget::on_pb_pastehint_clicked()
{

}

// "Enabled Script" toggled :
void Lua4RSWidget::on_cbx_enable_toggled(bool checked)
{
    if (checked)
    {
        ;
    }
}


//------------------------------------------------------------------------------
// Execution Constraint
//------------------------------------------------------------------------------

// "...between" toggled : Constraint enabled/disabled has changed
// note: think about disabling constraint from and to timeedits if unchecked
void Lua4RSWidget::on_cbx_timeconstraint_toggled(bool checked)
{
    if (checked)
    {
        ;
    }
}

// from : Constraint "from"-time has changed
// note: dont forget to check if from < to!
void Lua4RSWidget::on_tied_timefrom_editingFinished()
{

}

// to : Constraint "to"-time has changed
// note: dont forget to check if from < to!
void Lua4RSWidget::on_tied_timeto_editingFinished()
{

}

//------------------------------------------------------------------------------
// All Scripts
//------------------------------------------------------------------------------

// AllMyScripts : selected row changed (by click or cursor key)
void Lua4RSWidget::on_lw_allscripts_itemChanged(QTableWidgetItem *item)
{
    if (item)
    {
        return;
    }
}

// AllMyScripts : cell double clicked
void Lua4RSWidget::on_tw_allscripts_cellDoubleClicked(int row, int /*column*/)
{
    if(row < 0)
        return;

    // save then load
    on_pb_save_clicked();

    // get container
    LuaContainer* container = allScriptsGetLuaContainerFromRow(row);
    if(container == NULL)
    {
        std::cerr << "[Lua] Lua4RSWidget::on_tw_allscripts_doubleClicked : got NULL" << std::endl;
        return;
    }
    switchContainer(container);
}



//------------------------------------------------------------------------------
// Tabpage "By Event"
//------------------------------------------------------------------------------

// "Run Every" : amount of timer units has changed
// note: if changed, rb_runevery should be selected
void Lua4RSWidget::on_dd_everyunits_currentIndexChanged(int index)
{
/*
    tbd:
    1. hole ref auf akt. container
    2. hole ref auf config aus container
    3. wandle wert aus dropdown (secs, mins, days ...) in sec um
    4. schreibe diesen sec-wert nach config._timerunit
*/
}

// "RunEvery" : unit of timer units has changed
// note: if changed, rb_runevery should be selected
void Lua4RSWidget::on_spb_everycount_editingFinished()
{
/*
    tbd:
    1. hole ref auf akt. container
    2. hole ref auf config aus container
    3. schreibe wert aus spinbutton nach config._timeramount
*/
}

void Lua4RSWidget::on_rb_runonevent_toggled(bool checked)
{
    if(checked==true){
        ui->rb_every->setChecked(false);

        ui->rb_once->setChecked(false);

        ui->rb_startup->setChecked(false);

        ui->rb_shutdown->setChecked(false);


    }
}
