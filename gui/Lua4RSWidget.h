#ifndef LUA4RSWIDGET_H
#define LUA4RSWIDGET_H

#include <retroshare-gui/mainpage.h>

namespace Ui { class Lua4RSWidget; }
class LuaCore;
class LuaContainer;
class QTreeWidgetItem;
class QNetworkAccessManager;

class Lua4RSWidget : public MainPage
{
    Q_OBJECT
public:
    explicit Lua4RSWidget(QWidget* parent = nullptr);
    ~Lua4RSWidget();
    void disableOutput();

public slots:
    void clearOutput();
    void appendOutput(const std::string& text);
    void appendOutput(const QString& text);
    void appendLog(const std::string& text);
    void appendLog(const QString& text);

private:
    void refreshScripts();
    void selectActiveScript();
    void loadScript(LuaContainer* container);
    void createScript(const QString& code, const QString& description);
    bool saveScript();
    bool maybeSave();
    void markDirty();
    void updateEditorState();
    void filterScripts(const QString& query);
    void filterFunctions(const QString& query);
    void testApiConnection();
    void newApiExample();

    Ui::Lua4RSWidget* ui;
    LuaCore* _lua;
    LuaContainer* _activeContainer = nullptr;
    QNetworkAccessManager* _network;
    bool _disableOutput = false;
    bool _loading = false;
    bool _dirty = false;
    bool _draft = false;
    bool _running = false;

private slots:
    void on_pb_run_clicked();
    void on_pb_newscript_clicked();
    void on_pb_deletescript_clicked();
    void on_pb_load_clicked();
    void on_pb_save_clicked();
    void openScript(int row, int column);
    void on_pb_pastehint_released();
    void on_tw_hints_itemDoubleClicked(QTreeWidgetItem* item, int column);
};
#endif
