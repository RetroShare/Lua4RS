#include <assert.h>

#include <QWidget>

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
    ui->tw_hints->setColumnCount(2);


    QTreeWidgetItem *item_0 = new QTreeWidgetItem(ui->tw_hints);
    item_0->setText(0, tr("RS."));

        QTreeWidgetItem *item_01 = new QTreeWidgetItem(item_0);
        item_01->setText(0, "Print()");
        item_01->setToolTip(0,tr("..."));

        QTreeWidgetItem *item_02 = new QTreeWidgetItem(item_0);
        item_02->setText(0, "ClearLog()");
        item_02->setToolTip(0,tr("..."));

    // Info
    QTreeWidgetItem *item_1 = new QTreeWidgetItem(ui->tw_hints);
    item_1->setText(0, tr("RS.Info."));

        QTreeWidgetItem *item_11 = new QTreeWidgetItem(item_1);
        item_11->setText(0, "LastEntry()");
        item_11->setToolTip(0,tr("Liefert den letzten Eintrag der Infoliste"));

        QTreeWidgetItem *item_12 = new QTreeWidgetItem(item_1);
        item_12->setText(0, "AllEntries()");
        item_12->setToolTip(0,tr("Liefert alle Einträge der Infoliste"));

    // Friends
    QTreeWidgetItem *item_2 = new QTreeWidgetItem(ui->tw_hints);
    item_2->setText(0, tr("RS.Friends."));

        QTreeWidgetItem *item_21 = new QTreeWidgetItem(item_2);
        item_21->setText(0, "Online()");

        QTreeWidgetItem *item_22 = new QTreeWidgetItem(item_2);
        item_22->setText(0, "Offline()");

    // Files
    QTreeWidgetItem *item_3 = new QTreeWidgetItem(ui->tw_hints);
    item_3->setText(0, tr("RS.Files."));

        QTreeWidgetItem *item_31 = new QTreeWidgetItem(item_3);
        item_31->setText(0, "Search()");

        QTreeWidgetItem *item_32 = new QTreeWidgetItem(item_3);
        item_32->setText(0, "Download()");

    //...
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
    ui->tb_log->appendPlainText(s);
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
    // get script name
    QTableWidgetItem* name = ui->tw_allscripts->item(row, 0);

    // get container by name
    LuaContainer* container;
    if(LuaCore::getInstance()->codeList()->itemByName(name->text().toStdString(), container))
        return container;
    else
        return NULL;
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

/* #############################################################
 * # slots
 * #############################################################
 */
// "Run" clicked : execute the script in the editor control
void Lua4RSWidget::on_pb_run_clicked()
{
    appendLog(QString("running Lua script: ") + ui->le_scriptname->text());

    std::string luaCode = ui->pte_luacode->toPlainText().toStdString();
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
        return;

    // remember conatiner
    _activeContainer = container;

    // update UI
    luaContainerToUi(_activeContainer);
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
    // get values from ui
    uiToLuaContainer(_activeContainer);

    if(!_lua->codeList()->saveAll())
        std::verr << "[Lua] saving failed" << std::endl;

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

// "Enabled" toggled :
void Lua4RSWidget::on_cbx_enable_toggled(bool checked)
{
    if (checked)
    {
        ;
    }
}

// "...between" toggled :
void Lua4RSWidget::on_cbx_timeconstraint_toggled(bool checked)
{
    if (checked)
    {
        ;
    }
}

// from time changed :
void Lua4RSWidget::on_tied_timefrom_editingFinished()
{

}

// to time changed :
void Lua4RSWidget::on_tied_timeto_editingFinished()
{

}

// AllMyScripts : selected row changed (by click or cursor key)
void Lua4RSWidget::on_lw_allscripts_itemChanged(QTableWidgetItem *item)
{
    if (item)
    {
        return;
    }
}
