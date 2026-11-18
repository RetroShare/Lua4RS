#ifndef ACCESS_TEXT_H
#define ACCESS_TEXT_H

#include <QCoreApplication>
#include <QString>

#include "Lua/LuaTrust.h"
#include "Lua/LuaTrustStore.h"

// User-facing texts for trust levels and approval states.
class AccessText
{
    Q_DECLARE_TR_FUNCTIONS(AccessText)
public:
    static QString label(LuaTrust level);                       // "Trusted", "Write", "Read-only"
    static QString meaning(LuaTrust level);                     // what the level allows
    static QString details(const LuaTrustStore::Status& status); // tooltip, several lines
    static QString summary(const LuaTrustStore::Status& status); // one line
};

#endif // ACCESS_TEXT_H
