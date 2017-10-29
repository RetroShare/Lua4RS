#include <util/misc.h>

#include "ui_Lua4RSConfig.h"
#include "Lua4RSConfig.h"
#include "interface/L4RInterface.h"

Lua4RSConfig::Lua4RSConfig(QWidget *parent) :
    ConfigPage(parent),
    ui(new Ui::Lua4RSConfig)
{
    ui->setupUi(this);

    connect(ui->sb_starupEvent, SIGNAL(valueChanged(int)), this, SLOT(startUpChanged(int)));
    connect(ui->sb_tickInterval, SIGNAL(valueChanged(int)), this, SLOT(tickIntervalChanged(int)));
}

Lua4RSConfig::~Lua4RSConfig()
{
    delete ui;
}

void Lua4RSConfig::load()
{
    whileBlocking(ui->sb_starupEvent)->setValue(L4R::L4RConfig->getSecondsToStarUpEvent());
    whileBlocking(ui->sb_tickInterval)->setValue(L4R::L4RConfig->getTickIntervalInSeconds());
}

bool Lua4RSConfig::save(QString& /*errmsg*/)
{
    L4R::L4RConfig->setSecondsToStarUpEvent(ui->sb_starupEvent->value());
    L4R::L4RConfig->setTickIntervalInSeconds(ui->sb_tickInterval->value());
    return true;
}

void Lua4RSConfig::startUpChanged(int)
{
    L4R::L4RConfig->setSecondsToStarUpEvent(ui->sb_starupEvent->value());
}

void Lua4RSConfig::tickIntervalChanged(int)
{
    L4R::L4RConfig->setTickIntervalInSeconds(ui->sb_tickInterval->value());
}
