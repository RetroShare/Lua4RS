#include "ui_Lua4RSConfig.h"
#include "Lua4RSConfig.h"
#include "interface/Lua4RSInterface.h"

Lua4RSConfig::Lua4RSConfig(QWidget *parent) :
    ConfigPage(parent),
    ui(new Ui::Lua4RSConfig)
{
    ui->setupUi(this);
}

Lua4RSConfig::~Lua4RSConfig()
{
    delete ui;
}

void Lua4RSConfig::load()
{
    ui->sb_starupEvent->setValue(L4R::L4RConfig->getSecondsToStarUpEvent());
    ui->sb_tickInterval->setValue(L4R::L4RConfig->getTickIntervalInSeconds());
}

bool Lua4RSConfig::save(QString& /*errmsg*/)
{
    L4R::L4RConfig->setSecondsToStarUpEvent(ui->sb_starupEvent->value());
    L4R::L4RConfig->setTickIntervalInSeconds(ui->sb_tickInterval->value());
    return true;
}
