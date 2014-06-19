#ifndef LUACODE_H
#define LUACODE_H

class QString;

class LuaCode
{
public:
    LuaCode();
    LuaCode(QString name);
    LuaCode(QString name, QString code);

    // bool save();

    // getter/setter
    QString code() const;
    void setCode(const QString &code);

    QString name() const;
    void setName(const QString &name);

private:
    QString _code;
    QString _name;
};

#endif // LUACODE_H
