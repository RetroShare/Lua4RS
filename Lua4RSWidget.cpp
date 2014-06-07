#include "Lua4RSWidget.h"
#include "ui_Lua4RSWidget.h"

#include "Lua/LuaCore.h"

Lua4RSWidget::Lua4RSWidget(QWidget *parent) : MainPage(parent), ui(new Ui::Lua4RSWidget)
{
    ui->setupUi(this);

    _lua = LuaCore::getInstance();
    _lua->setUi(this);


    // Fill Hints TreeWidget with main items
    ui->tw_hints->setColumnCount(2);

    // Info
    QTreeWidgetItem *item_1 = new QTreeWidgetItem(ui->tw_hints);
    item_1->setText(0, tr("Info"));
        QTreeWidgetItem *item_11 = new QTreeWidgetItem(item_1);
        item_11->setText(0, tr("rs_log_lastentry()"));
        item_11->setText(1, tr("rs_log_lastentry()"));

    // Friends
    QTreeWidgetItem *item_2 = new QTreeWidgetItem(ui->tw_hints);
    item_2->setText(0, tr("Friends"));
        QTreeWidgetItem *item_21 = new QTreeWidgetItem(item_2);
        item_21->setText(0, tr("rs_friend_online()"));
        item_21->setText(1, tr("rs_friend_online()"));
        QTreeWidgetItem *item_22 = new QTreeWidgetItem(item_2);
        item_22->setText(0, tr("rs_friend_offline()"));
        item_22->setText(1, tr("rs_friend_offline()"));

    // Files
    QTreeWidgetItem *item_3 = new QTreeWidgetItem(ui->tw_hints);
    item_3->setText(0, tr("Files"));
        QTreeWidgetItem *item_31 = new QTreeWidgetItem(item_3);
        item_31->setText(0, tr("rs_files_search()"));
        item_31->setText(1, tr("rs_files_search()"));

    //...
}

Lua4RSWidget::~Lua4RSWidget()
{
    delete ui;
}

/*
void Lua4RSWidget::runLua()
{
    std::string luaCode = ui->pte_code->toPlainText().toStdString();
    _lua->runLua(luaCode);
}
*/

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
