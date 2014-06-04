#include "Lua4RSWidget.h"
#include "ui_Lua4RSWidget.h"

#include "Lua/LuaCore.h"

Lua4RSWidget::Lua4RSWidget(QWidget *parent) : 
    MainPage(parent), 
    ui(new Ui::Lua4RSWidget)
{
    ui->setupUi(this);

    _lua = LuaCore::getInstance();
    _lua->setUi(this);

//    connect(ui->pb_run, SIGNAL(clicked()), this, SLOT(runLua()));
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
    ui->tb_output->append(s);
}

void Lua4RSWidget::appendLog(const std::string& s)
{
    appendLog(QString::fromStdString(s));
}

void Lua4RSWidget::appendLog(const QString& s)
{
    ui->tb_log->append(s);
}

void Lua4RSWidget::on_pb_run_clicked()
{
    appendLog(QString("running Lua script: ") + ui->le_scriptname->text());

    std::string luaCode = ui->pte_luacode->toPlainText().toStdString();
    _lua->runLuaByString(luaCode);
}
