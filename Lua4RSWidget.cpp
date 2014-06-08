#include "Lua4RSWidget.h"
#include "ui_Lua4RSWidget.h"

#include <QWidget>

Lua4RSWidget::Lua4RSWidget(QWidget *parent) : MainPage(parent), ui(new Ui::Lua4RSWidget)
{
    ui->setupUi(this);

    _lua = LuaCore::getInstance();
    _lua->setUi(this);

    setLuaCodes(_lua->codeList());

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

void Lua4RSWidget::setLuaCodes(LuaList* /*list*/)
{
    ///TODO needs rewrite
    /*
    LuaContainer* lc = NULL;
    for(size_t i = 0; i < list->size(); i++)
    {
        if(list->itemAt(i, lc))
            ui->lw_allscripts->addItem(QString::fromStdString(lc->getLuaCode()->name()));
        else
            break;
    }
    */
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

}

// "Edit" clicked : edit the script selected in AllMyScripts
void Lua4RSWidget::on_pb_editscript_clicked()
{

}

// "Delete" clicked : delete the script selected in AllMyScripts
void Lua4RSWidget::on_pb_deletescript_clicked()
{

}

// "Load" clicked : load a scriptfile from disk into the editor control
void Lua4RSWidget::on_pb_load_clicked()
{

}

// "Save" clicked : save the contents of the editor control to a file on disk
void Lua4RSWidget::on_pb_save_clicked()
{

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
