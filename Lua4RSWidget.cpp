#include <assert.h>

#include <QMessageBox>
#include <QModelIndex>
#include <QTreeWidgetItem>
#include <QWidget>

#include "ui_Lua4RSWidget.h"
#include "Lua4RSWidget.h"
#include "Lua/LuaCore.h"
#include "Lua/LuaList.h"

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

    // disable sorting (better performance)
    ui->tw_allscripts->setSortingEnabled(false);
    LuaContainerList::const_iterator it;
    for(it = list->begin(); it != list->end(); ++it)
        allScriptsAddRow(*it);

    ui->tw_allscripts->setSortingEnabled(true);
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
    int rows = ui->tw_allscripts->rowCount();
    ui->tw_allscripts->setRowCount(rows + 1);

    QTableWidgetItem* name = new QTableWidgetItem();
    QTableWidgetItem* desc = new QTableWidgetItem();
    QTableWidgetItem* lastRun = new QTableWidgetItem();
    QTableWidgetItem* trigger = new QTableWidgetItem();
    QTableWidgetItem* enabled = new QTableWidgetItem();

    name->setText(container->getName());
    desc->setText(container->getDesc());
    lastRun->setText(container->getLastTriggered().toString());
    trigger->setText("TODO");
    enabled->setCheckState(container->getEnabled() ? Qt::Checked : Qt::Unchecked);

    ui->tw_allscripts->setItem(rows, 0, name);
    ui->tw_allscripts->setItem(rows, 1, desc);
    ui->tw_allscripts->setItem(rows, 2, lastRun);
    ui->tw_allscripts->setItem(rows, 3, trigger);
    ui->tw_allscripts->setItem(rows, 4, enabled);
}

void Lua4RSWidget::luaContainerToUi(LuaContainer* container)
{
    // for settings things to default / resetting
    if(container == NULL)
    {
        // name, desc, code
        ui->le_scriptname->clear();
        ui->le_scriptdesc->clear();
        ui->pte_luacode->clear();

        // enable, constraint
        ui->cbx_enable->setChecked(false);
        ui->cbx_timeconstraint->setChecked(false);
        {
            QTime n;
            n.setHMS(0, 0, 0);
            ui->tied_timefrom->setTime(n);
            ui->tied_timeto->setTime(n);
        }
        ///TODO rest

        ///TODO there might be better ways that this - good enough for the moment
        ui->pte_luacode->setEnabled(false);
    } else
    {
        // name, desc, code
        ui->le_scriptname->setText(container->getName());
        ui->le_scriptdesc->setText(container->getDesc());
        ui->pte_luacode->setPlainText(container->getCode());


        ui->cbx_enable->setChecked(container->getEnabled());
        ui->cbx_timeconstraint->setChecked(container->getConstraintEnabled());
        {
            QTime from, to;
            container->getConstraintFromTo(from, to);
            ui->tied_timefrom->setTime(from);
            ui->tied_timeto->setTime(to);
        }
        ///TODO rest

        ui->pte_luacode->setEnabled(true);
    }
}

bool Lua4RSWidget::uiToLuaContainer(LuaContainer* container)
{
    if(!saneValues())
    {
        std::cerr << "[Lua] Lua4RSWidget::uiToLuaContainer : wrong values detected - aborting" << std::endl;
        return false;
    }

    // name, desc, code
    container->setName(ui->le_scriptname->text());
    container->setDesc(ui->le_scriptdesc->text());
    container->setCode(ui->pte_luacode->toPlainText());

    // enable, constraint
    container->setEnabled(ui->cbx_enable->isChecked());
    container->setConstraintEnabled(ui->cbx_timeconstraint->isChecked());
    {
        QTime from, to;
        from = ui->tied_timefrom->time();
        to = ui->tied_timeto->time();
        container->setConstraintFromTo(from, to);
    }
    ///TODO rest

    return true;
}

void Lua4RSWidget::switchContainer(LuaContainer* container)
{
    // remember conatiner
    _activeContainer = container;

    // update UI
    luaContainerToUi(_activeContainer);

    if(_activeContainer != NULL)
        std::cout << "[Lua] Lua4RSWidget::switchContainer : switched to " << _activeContainer->getName().toStdString() << std::endl;
    else
        std::cout << "[Lua] Lua4RSWidget::switchContainer : switched to NULL "<< std::endl;
}

void saneValuesHelper(const QString& msg, QString& allMsgs)
{
    std::cerr << "[Lua] Lua4RSWidget::saneValues : " << msg.toStdString() << std::endl;
    allMsgs += "- " + msg + '\n';
}

bool Lua4RSWidget::saneValues()
{
    QString msg = "The following problem(s) was/were found:\n";
    bool ret = true;
    if(ui->le_scriptname->text().isEmpty())
    {
        saneValuesHelper("script name is empty", msg);
        ret = false;
    }

    if(ui->dte_runonce->dateTime() < QDateTime::currentDateTime())
    {
        saneValuesHelper("runOnce value lies in the past", msg);
        ret = false;
    }

    if(ui->cbx_timeconstraint->isChecked() && ui->rb_once->isChecked() && ((    // contraint enabled + run once
            ui->tied_timefrom->time() < ui->tied_timeto->time() &&              // from < to e.g. from 09:00 to 15:00
                (ui->dte_runonce->time() < ui->tied_timefrom->time() || ui->dte_runonce->time() > ui->tied_timeto->time())      // run once is outside of time window
            ) || (
            ui->tied_timefrom->time() > ui->tied_timeto->time() &&              // from > to e.g. from 23:00 to 06:00
                (ui->dte_runonce->time() <= ui->tied_timefrom->time() && ui->dte_runonce->time() >= ui->tied_timeto->time())    // run once is outside of time window
            // !(ui->dte_runonce->time() >  ui->tied_timefrom->time() || ui->dte_runonce->time() <  ui->tied_timeto->time())    equivalent - maybe easier to understand
            )))
    {
        saneValuesHelper("runOnce value lies outside of constraint", msg);
        ret = false;
    }

    ///TODO check rest

    if(!ret)
    {
        // show errors to user
        QMessageBox mbox;
        mbox.setIcon(QMessageBox::Warning);
        mbox.setText("Error(s) while checking");
        mbox.setInformativeText(msg);
        mbox.setStandardButtons(QMessageBox::Ok);
        mbox.exec();
    }

    return ret;
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
    QString name = "";
    if(_activeContainer != NULL)
    {
        // a file was opened -> save its name
        name = _activeContainer->getName();

        // ask for confirmation
        QMessageBox mbox;
        mbox.setIcon(QMessageBox::Information);
        mbox.setText("Continue?");
        mbox.setInformativeText("You have a Lua script opened. Save it before closing it?");
        mbox.setStandardButtons( QMessageBox::Save | QMessageBox::Discard | QMessageBox::Abort);

        int ret = mbox.exec();
        if(ret == QMessageBox::Abort)
            return;

        if(ret == QMessageBox::Save)
            on_pb_save_clicked();
    }

    LuaList* list = _lua->codeList();

    list->loadAll();
    // _activeContainer is invalid from now on!
    _activeContainer = NULL;

    setLuaCodes(list);

    if(name == "")
        // no file was opened - were are done
        return;

    LuaContainer* lc;
    if(list->itemByName(name, lc))
        switchContainer(lc);
    else
        // couldn't find the file one was working one ...
        switchContainer(NULL);
}

// "Save" clicked : save the contents of the editor control to a file on disk
void Lua4RSWidget::on_pb_save_clicked()
{
    if(_activeContainer == NULL)
        return;

    // check for rename
    {
        QString oldName = _activeContainer->getName();
        // get values from ui
        if(!uiToLuaContainer(_activeContainer))
            return;

        if(_activeContainer->getName() != oldName)
        {
            std::cout << "[Lua] Lua4RSWidget::on_pb_save_clicked() : renaming " << oldName.toStdString() << " to " << _activeContainer->getName().toStdString() << std::endl;
            _lua->codeList()->rename(oldName, _activeContainer->getName());
        }
    }

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
    if(_activeContainer == NULL)
    {
        std::cerr << "[Lua] Lua4RSWidget::on_cbx_timeconstraint_toggled : got no activeContainer" << std::endl;
        return;
    }
    _activeContainer->setConstraintEnabled(checked);
}

// from : Constraint "from"-time has changed
// note: dont forget to check if from < to!
void Lua4RSWidget::on_tied_timefrom_editingFinished()
{
/*
    if(_activeContainer == NULL)
    {
        std::cerr << "[Lua] Lua4RSWidget::on_tied_timefrom_editingFinished : got no activeContainer" << std::endl;
        return;
    }
    _activeContainer->_luaConfig->setConstraintFrom(ui->tied_timefrom->time());
*/
}

// to : Constraint "to"-time has changed
// note: dont forget to check if from < to!
void Lua4RSWidget::on_tied_timeto_editingFinished()
{
/*
    if(_activeContainer == NULL)
    {
        std::cerr << "[Lua] Lua4RSWidget::on_tied_timeto_editingFinished : got no activeContainer" << std::endl;
        return;
    }
    _activeContainer->_luaConfig->setConstraintTo(ui->tied_timeto->time());
*/
}

//------------------------------------------------------------------------------
// All Scripts
//------------------------------------------------------------------------------

void Lua4RSWidget::on_lw_allscripts_itemChanged(QTableWidgetItem *item)
{
    if (item)
    {
        return;
    }
}

void Lua4RSWidget::on_tw_allscripts_cellClicked(int row, int column)
{
    if(column == 4) // 4 = enabled
    {
        LuaContainer* container = allScriptsGetLuaContainerFromRow(row);
        QTableWidgetItem* cell = ui->tw_allscripts->item(row, column);

        container->setEnabled(cell->checkState() == Qt::Checked ? true : false);
    }
}

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
    uint TIME_UNITS[5] = {1, 60, 3600, 86400, 604800};
    uint unit,amount,interval;

    amount = ui->spb_everycount->value();
    unit = TIME_UNITS[index];
    interval = amount * unit;

    ui->l_runeveryhelper->setText( QString::number(interval) + " secs" );
/*
    tbd:
    1. hole ref auf akt. container
    2. hole ref auf config aus container
    3. wandle wert aus dropdown (secs, mins, days ...) in sec um
    4. schreibe diesen sec-wert nach config._timerunit
*/
}

void Lua4RSWidget::on_spb_everycount_editingFinished()
{
}

// "RunEvery" : unit of timer units has changed
// note: if changed, rb_runevery should be selected
void Lua4RSWidget::on_spb_everycount_valueChanged(int arg1)
{
    uint TIME_UNITS[5] = {1, 60, 3600, 86400, 604800};
    uint unit,amount,interval;

    amount = arg1;
    unit = TIME_UNITS[ui->dd_everyunits->currentIndex()];
    interval = amount * unit;

    ui->l_runeveryhelper->setText( QString::number(interval) + " secs" );
}


void Lua4RSWidget::on_rb_runonevent_toggled(bool /*checked*/)
{
}

//------------------------------------------------------------------------------
// hints
//------------------------------------------------------------------------------
void Lua4RSWidget::on_pb_pastehint_released()
{
    QList<QTreeWidgetItem*> items = ui->tw_hints->selectedItems();
    if(items.empty() || items.size() != 1 || !ui->pte_luacode->isEnabled())
        return;

    ui->pte_luacode->insertPlainText(items.at(0)->text(1));
}

void Lua4RSWidget::on_tw_hints_itemDoubleClicked(QTreeWidgetItem *item, int /*column*/)
{
    QString hint = item->text(1);

    // when you want to expant a namespace, you double click it --> don't append namespaces
    if(hint.endsWith('.') || !ui->pte_luacode->isEnabled())
        return;

    ui->pte_luacode->insertPlainText(hint);
}























