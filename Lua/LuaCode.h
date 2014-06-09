#ifndef LUACODE_H
#define LUACODE_H

class QString;

class LuaCode
{
public:
    LuaCode();
    LuaCode(QString name, QString desc);
    LuaCode(QString name, QString desc, QString code);

    // bool save();

    // getter/setter
    QString code() const;
    void setCode(const QString &code);

    QString desc() const;
    void setDesc(const QString &desc);

    QString name() const;
    void setName(const QString &name);

private:
    QString _code;
    QString _desc;
    QString _name;
};

#endif // LUACODE_H
