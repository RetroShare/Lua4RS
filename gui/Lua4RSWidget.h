#ifndef LUA4RSWIDGET_H
#define LUA4RSWIDGET_H

#include <map>

#include <retroshare-gui/mainpage.h>

namespace Ui { class Lua4RSWidget; }
class LuaCore;
class LuaContainer;
class QTreeWidgetItem;
class QLabel;
class QPushButton;
class QListWidget;
class QListWidgetItem;
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
    void updateOtherEvents();
    void filterScripts(const QString& query);
    void filterFunctions(const QString& query);
    void testApiConnection();
    void newApiExample();
    // Once per profile: ask whether scripts from before approvals existed are trusted.
    void checkTrustMigration();
    // Approval dialog for the saved version of the open script.
    void approveActiveScript();
    void updateAccessStatus();
    void onScriptRemovedExternally(const QString& name);
    void onScriptChangedExternally(const QString& name);

    Ui::Lua4RSWidget* ui;
    LuaCore* _lua;
    LuaContainer* _activeContainer = nullptr;
    QNetworkAccessManager* _network;
    bool _disableOutput = false;
    bool _loading = false;
    bool _dirty = false;
    bool _draft = false;
    bool _draftUntrusted = false; // draft kept from a script the agent deleted
    bool _running = false;
    QLabel* _otherEvents = nullptr;          // event triggers without a checkbox
    QPushButton* _removeOtherEvents = nullptr;
    bool _dropOtherEvents = false;
    QListWidget* _eventList = nullptr;             // events of the catalog, checkable
    std::map<unsigned, QListWidgetItem*> _eventItems; // event id -> entry

private slots:
    void on_pb_run_clicked();
    void on_pb_newscript_clicked();
    void on_pb_deletescript_clicked();
    void on_pb_load_clicked();
    void on_pb_save_clicked();
    void on_pb_approve_clicked();
    void openScript(int row, int column);
    void on_pb_pastehint_released();
    void on_tw_hints_itemDoubleClicked(QTreeWidgetItem* item, int column);
};
#endif
