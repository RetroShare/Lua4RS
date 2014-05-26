#include "Lua4RSMainWidget.h"
#include "ui_Lua4RSMainWidget.h"

#include "LuaCore.h"

Lua4RSMainWidget::Lua4RSMainWidget(QWidget *parent) :
    MainPage(parent),
    ui(new Ui::Lua4RSMainWidget)
{
    ui->setupUi(this);

    _lua = LuaCore::getInstance();
    _lua->setUi(this);

    connect(ui->pb_run, SIGNAL(clicked()), this, SLOT(runLua()));
}

Lua4RSMainWidget::~Lua4RSMainWidget()
{
    delete ui;
}

void Lua4RSMainWidget::runLua()
{
    std::string luaCode = ui->pte_code->toPlainText().toStdString();
    _lua->runLua(luaCode);
}

void Lua4RSMainWidget::clearOutput()
{
    ui->pte_output->clear();
}

void Lua4RSMainWidget::appendOutput(const std::string& s)
{
    appendOutput(QString::fromStdString(s));
}

void Lua4RSMainWidget::appendOutput(const QString& s)
{
    ui->pte_output->appendPlainText(s);
}
