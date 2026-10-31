#include "Lua4RSWidget.h"
#include "FunctionBrowser.h"
#include "ui_Lua4RSWidget.h"
#include "Lua/LuaCore.h"
#include "Lua/LuaList.h"
#include "Lua/BundledScripts.h"
#include "interface/L4RInterface.h"

#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QFontDatabase>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>
#include <QPainter>
#include <QPixmap>
#include <QIcon>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QShortcut>
#include <QSignalBlocker>
#include <QTimer>
#include <QUrl>

namespace {
class ScriptTableItem : public QTableWidgetItem
{
public:
    ScriptTableItem(const QString& text, bool enabled, const QString& name)
        : QTableWidgetItem(text), _enabled(enabled), _name(name) {}

    bool operator<(const QTableWidgetItem& other) const override
    {
        const auto& script = static_cast<const ScriptTableItem&>(other);
        if(_enabled != script._enabled)
        {
            // Keep enabled scripts first even when the user reverses the sort.
            const bool descending = tableWidget() &&
                tableWidget()->horizontalHeader()->sortIndicatorOrder() == Qt::DescendingOrder;
            return descending ? !_enabled : _enabled;
        }
        return QString::localeAwareCompare(_name, script._name) < 0;
    }

private:
    bool _enabled;
    QString _name;
};
}

Lua4RSWidget::Lua4RSWidget(QWidget* parent) :
    MainPage(parent), ui(new Ui::Lua4RSWidget),
    _lua(L4R::L4RConfig->getCore()), _network(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    connect(_lua, &LuaCore::manualRunFinished, this, [this](bool success) {
        _running = false;
        ui->runStatus->setText(success ? tr("Finished. Results appear in Output.") : tr("Script failed - see Output."));
        updateEditorState();
    }, Qt::QueuedConnection);
    ui->workspaceSplitter->setSizes({210, 720, 290});
    ui->editorSplitter->setSizes({500, 190});
    ui->workspaceSplitter->setStretchFactor(1, 1);
    ui->editorSplitter->setStretchFactor(0, 1);
    ui->tw_allscripts->verticalHeader()->hide();
    ui->tw_allscripts->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tw_allscripts->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tw_allscripts->setSortingEnabled(true);
    ui->tw_allscripts->sortItems(0);
    ui->pte_luacode->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->tb_output->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->tb_output->setMaximumBlockCount(5000);
    ui->tb_log->setMaximumBlockCount(2000);
    _lua->setupRsFunctionsAndTw(ui->tw_hints);
    ui->tw_hints->hideColumn(1);
    ui->tw_hints->header()->hide();
    if(ui->tw_hints->topLevelItemCount()) ui->tw_hints->topLevelItem(0)->setExpanded(true);

    ui->le_apiurl->setText(qEnvironmentVariable("RETROSHARE_API_URL", "http://127.0.0.1:9092"));
    ui->le_apiuser->setText(qEnvironmentVariable("RETROSHARE_API_USER"));
    ui->le_apipassword->setText(qEnvironmentVariable("RETROSHARE_API_PASSWORD"));
    connect(ui->pb_apitest, &QPushButton::clicked, this, &Lua4RSWidget::testApiConnection);
    connect(ui->pb_apiexample, &QPushButton::clicked, this, &Lua4RSWidget::newApiExample);
    for(auto field : {ui->le_apiurl, ui->le_apiuser, ui->le_apipassword})
        connect(field, &QLineEdit::textChanged, this, [this]() { ui->apiStatus->setText(tr("Settings changed — click Apply & test connection.")); });
    connect(ui->pb_folder, &QPushButton::clicked, this, [this]() {
        QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdString(_lua->getPath())));
    });
    connect(ui->pb_clearoutput, &QPushButton::clicked, this, &Lua4RSWidget::clearOutput);
    connect(ui->le_scriptfilter, &QLineEdit::textChanged, this, &Lua4RSWidget::filterScripts);
    connect(ui->tw_allscripts, &QTableWidget::currentCellChanged, this,
            [this](int row, int column, int, int) { openScript(row, column); });
    connect(ui->le_functionfilter, &QLineEdit::textChanged, this, &Lua4RSWidget::filterFunctions);
    auto examples = new QMenu(ui->pb_example);
    examples->addAction(tr("Hello world — no setup needed"), this, [this]() {
        createScript("-- Click Run or press F5.\nprint(\"Hello from Lua4RS!\")\n", tr("My first Lua script"));
    });
    examples->addAction(tr("List friends with the JSON API"), this, &Lua4RSWidget::newApiExample);
    auto bundled = examples->addMenu(tr("Bundled scripts"));
    for(const QString& name : BundledScripts::names())
        bundled->addAction(name, this, [this, name]() {
            QFile source(":/lua4rs/examples/" + name);
            if(!source.open(QIODevice::ReadOnly))
            {
                QMessageBox::warning(this, tr("Example unavailable"), source.errorString());
                return;
            }
            createScript(QString::fromUtf8(source.readAll()), tr("Example: %1").arg(name));
        });
    ui->pb_example->setMenu(examples);

    connect(ui->le_scriptname, &QLineEdit::textChanged, this, &Lua4RSWidget::markDirty);
    connect(ui->le_scriptdesc, &QLineEdit::textChanged, this, &Lua4RSWidget::markDirty);
    connect(ui->pte_luacode, &QPlainTextEdit::textChanged, this, &Lua4RSWidget::markDirty);
    for(auto box : ui->automationTab->findChildren<QCheckBox*>())
        connect(box, &QCheckBox::toggled, this, [this]() { markDirty(); updateEditorState(); });
    connect(ui->spb_everycount, QOverload<int>::of(&QSpinBox::valueChanged), this, &Lua4RSWidget::markDirty);
    connect(ui->dd_everyunits, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Lua4RSWidget::markDirty);
    for(auto field : ui->automationTab->findChildren<QDateTimeEdit*>())
        connect(field, &QDateTimeEdit::dateTimeChanged, this, &Lua4RSWidget::markDirty);

    auto save = new QShortcut(QKeySequence::Save, this);
    connect(save, &QShortcut::activated, this, &Lua4RSWidget::on_pb_save_clicked);
    auto run = new QShortcut(QKeySequence(Qt::Key_F5), this);
    connect(run, &QShortcut::activated, this, &Lua4RSWidget::on_pb_run_clicked);
    auto fresh = new QShortcut(QKeySequence::New, this);
    connect(fresh, &QShortcut::activated, this, &Lua4RSWidget::on_pb_newscript_clicked);
    registerHelpButton(ui->helpButton, tr(
        "<h2>Start with a simple script</h2><ol>"
        "<li>Choose <b>Example → Hello world</b>.</li>"
        "<li>Click <b>Run</b> (F5). Read the result in <b>Output</b>.</li>"
        "<li>Give your script a name and click <b>Save</b> (Ctrl+S).</li></ol>"
        "<p>Use <b>Automation</b> only if you want scheduled or event-driven runs. Save to apply them.</p>"
        "<p>For JSON API scripts, enable the API in RetroShare, enter its authorized credentials in the right panel, "
        "and click <b>Apply &amp; test connection</b>. Then choose <b>New API example</b>.</p>"
        "<p>Use <code>print(...)</code> or <code>rs.print(...)</code> for output. "
        "Continuous JSON API subscriptions run in the separate command-line runner.</p>"), "Lua4RSWidget");
    refreshScripts();
    loadScript(nullptr);
}

Lua4RSWidget::~Lua4RSWidget()
{
    _lua->setUi(nullptr);
    if(_draft) delete _activeContainer;
    delete ui;
}

void Lua4RSWidget::disableOutput() { _disableOutput = true; }
void Lua4RSWidget::clearOutput() { if(!_disableOutput) ui->tb_output->clear(); }
void Lua4RSWidget::appendOutput(const std::string& s) { appendOutput(QString::fromUtf8(s.c_str())); }
void Lua4RSWidget::appendOutput(const QString& s)
{
    if(!_disableOutput) { ui->tb_output->appendPlainText(s); ui->outputTabs->setCurrentWidget(ui->outputTab); }
}
void Lua4RSWidget::appendLog(const std::string& s) { appendLog(QString::fromUtf8(s.c_str())); }
void Lua4RSWidget::appendLog(const QString& s)
{
    if(_disableOutput) return;
    ui->tb_log->appendPlainText(QDateTime::currentDateTime().toString("HH:mm:ss") + "  " + s);
    if(s.startsWith("Lua error:")) { appendOutput(s); ui->runStatus->setText(tr("Script failed — see Output.")); }
}

void Lua4RSWidget::refreshScripts()
{
    std::lock_guard<std::recursive_mutex> scriptsLock(_lua->scriptsMutex());
    const QSignalBlocker blocker(ui->tw_allscripts);
    ui->tw_allscripts->setSortingEnabled(false);
    ui->tw_allscripts->setRowCount(0);
    for(auto container : *_lua->codeList())
    {
        const int row = ui->tw_allscripts->rowCount();
        ui->tw_allscripts->insertRow(row);
        const bool enabled = container->getEnabled();
        auto name = new ScriptTableItem(container->getName(), enabled, container->getName());
        name->setToolTip(container->getDesc() + "\n" + container->getTriggerNames() + "\n" +
                         tr("Last run: %1").arg(container->getLastTriggered().isValid() ? container->getLastTriggered().toString() : tr("Never")));
        ui->tw_allscripts->setItem(row, 0, name);
        auto status = new ScriptTableItem(enabled ? tr("On") : tr("Off"), enabled, container->getName());
        const qreal ratio = ui->tw_allscripts->devicePixelRatioF();
        QPixmap dot(qRound(16 * ratio), qRound(16 * ratio));
        dot.setDevicePixelRatio(ratio);
        dot.fill(Qt::transparent);
        {
            QPainter painter(&dot);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setPen(Qt::NoPen);
            painter.setBrush(enabled ? QColor("#218b46") : QColor("#808080"));
            painter.drawEllipse(QRectF(3, 3, 10, 10));
        }
        QIcon icon;
        // Keep the status colour when the row is selected.
        icon.addPixmap(dot, QIcon::Normal);
        icon.addPixmap(dot, QIcon::Selected);
        status->setIcon(icon);
        status->setToolTip(enabled ? tr("Automation enabled") : tr("Automation disabled"));
        ui->tw_allscripts->setItem(row, 1, status);
    }
    ui->tw_allscripts->setSortingEnabled(true);
    filterScripts(ui->le_scriptfilter->text());
    selectActiveScript();
}

void Lua4RSWidget::selectActiveScript()
{
    const QSignalBlocker blocker(ui->tw_allscripts);
    ui->tw_allscripts->clearSelection();
    ui->tw_allscripts->setCurrentCell(-1, -1);
    if(!_activeContainer || _draft) return;
    for(int row = 0; row < ui->tw_allscripts->rowCount(); ++row)
        if(ui->tw_allscripts->item(row, 0)->text() == _activeContainer->getName())
        { ui->tw_allscripts->selectRow(row); break; }
}

void Lua4RSWidget::loadScript(LuaContainer* container)
{
    std::lock_guard<std::recursive_mutex> scriptsLock(_lua->scriptsMutex());
    _loading = true;
    _activeContainer = container;
    ui->le_scriptname->setText(container ? container->getName() : QString());
    ui->le_scriptdesc->setText(container ? container->getDesc() : QString());
    ui->pte_luacode->setPlainText(container ? container->getCode() : QString());
    ui->cbx_enable->setChecked(container && container->getEnabled());
    ui->cbx_timeconstraint->setChecked(container && container->getConstraintEnabled());
    QTime from(0,0), to(0,0);
    if(container) container->getConstraintFromTo(from, to);
    ui->tied_timefrom->setTime(from); ui->tied_timeto->setTime(to);
    uint amount = 5, unit = 1;
    const bool every = container && container->getRunEveryChecked(amount, unit);
    ui->cb_every->setChecked(every);
    ui->spb_everycount->setValue(amount);
    ui->dd_everyunits->setCurrentIndex(unit < 5 ? unit : 1);
    QDateTime when = QDateTime::currentDateTime().addSecs(3600);
    ui->cb_once->setChecked(container && container->getRunOnceChecked(when));
    ui->dte_runonce->setDateTime(when);
    ui->cb_startup->setChecked(container && container->getRunStartupChecked());
    ui->cb_shutdown->setChecked(container && container->getRunShutdownChecked());
    ui->cb_chatmessage->setChecked(container && container->getEventTriggerChecked(L4R_LOBBY_MESSAGERECEIVED));
    ui->cb_turtleSearchResult->setChecked(container && container->getEventTriggerChecked(L4R_FILE_SEARCHRESULT));
    _dirty = false;
    _loading = false;
    ui->editorTabs->setCurrentWidget(ui->codeTab);
    ui->runStatus->setText(container ? tr("Ready — F5 runs the code shown above.") : tr("Choose a script. Write Lua. Run and see the result."));
    updateEditorState();
    selectActiveScript();
}

void Lua4RSWidget::updateEditorState()
{
    const bool active = _activeContainer && !_running;
    ui->le_scriptname->setEnabled(active);
    ui->le_scriptdesc->setEnabled(active);
    ui->editorTabs->setEnabled(active);
    ui->pb_run->setEnabled(active);
    ui->pb_save->setEnabled(active && _dirty);
    ui->pb_save->setText(_dirty ? tr("Save *") : tr("Save"));
    ui->pb_deletescript->setEnabled(active && !_draft);
    const bool automatic = active && ui->cbx_enable->isChecked();
    for(auto box : ui->automationTab->findChildren<QCheckBox*>())
        if(box != ui->cbx_enable) box->setEnabled(automatic);
    ui->spb_everycount->setEnabled(automatic && ui->cb_every->isChecked());
    ui->dd_everyunits->setEnabled(automatic && ui->cb_every->isChecked());
    ui->dte_runonce->setEnabled(automatic && ui->cb_once->isChecked());
    ui->tied_timefrom->setEnabled(automatic && ui->cbx_timeconstraint->isChecked());
    ui->tied_timeto->setEnabled(automatic && ui->cbx_timeconstraint->isChecked());
}

void Lua4RSWidget::markDirty()
{
    if(_loading || !_activeContainer) return;
    _dirty = true;
    ui->runStatus->setText(tr("Unsaved changes — Run uses the editor; automatic runs use the saved script."));
    updateEditorState();
}

bool Lua4RSWidget::maybeSave()
{
    if(!_dirty) return true;
    const auto choice = QMessageBox::question(this, tr("Unsaved script"),
        tr("Save changes to %1?").arg(ui->le_scriptname->text()),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);
    if(choice == QMessageBox::Cancel) return false;
    if(choice == QMessageBox::Save) return saveScript();
    if(_draft) { delete _activeContainer; _activeContainer = nullptr; _draft = false; }
    loadScript(_activeContainer);
    return true;
}

void Lua4RSWidget::createScript(const QString& code, const QString& description)
{
    if(!maybeSave()) return;
    auto draft = _lua->codeList()->createItem();
    draft->setCode(code); draft->setDesc(description); draft->setEnabled(false);
    _draft = true;
    loadScript(draft);
    markDirty();
    ui->le_scriptname->setFocus(); ui->le_scriptname->selectAll();
}

bool Lua4RSWidget::saveScript()
{
    if(!_activeContainer) return true;
    QString name = ui->le_scriptname->text().trimmed();
    if(!name.endsWith(".lua")) name += ".lua";
    LuaContainer* duplicate = nullptr;
    if(name == ".lua" || name.contains('/') || name.contains('\\') || name.contains(':') ||
       name.contains("..") || name.contains(QRegularExpression("[<>\"|?*]")) ||
       (_lua->codeList()->itemByName(name, duplicate) && duplicate != _activeContainer))
    {
        QMessageBox::warning(this, tr("Choose a script name"), tr("Use a unique filename, such as my-script.lua, without folders or special filename characters."));
        return false;
    }
    if(ui->cbx_enable->isChecked() && ui->cb_once->isChecked() && ui->dte_runonce->dateTime() <= QDateTime::currentDateTime())
    { QMessageBox::warning(this, tr("Check the schedule"), tr("Choose a future time for the one-time run.")); return false; }
    if(ui->cbx_enable->isChecked() && ui->cbx_timeconstraint->isChecked())
    {
        const QTime from = ui->tied_timefrom->time(), to = ui->tied_timeto->time();
        const QTime once = ui->dte_runonce->time();
        if(from >= to || (ui->cb_once->isChecked() && (once < from || once > to)))
        { QMessageBox::warning(this, tr("Check the time window"), tr("Choose an end time after the start time. A one-time run must fall inside that window.")); return false; }
    }
    const quint64 units[] = {1, 60, 3600, 86400, 604800};
    if(quint64(ui->spb_everycount->value()) * units[ui->dd_everyunits->currentIndex()] > 0xffffffffULL)
    { QMessageBox::warning(this, tr("Check the interval"), tr("Choose a shorter repeat interval.")); return false; }
    std::unique_lock<std::recursive_mutex> scriptsLock(_lua->scriptsMutex());
    const QString oldName = _activeContainer->getName();
    _activeContainer->setName(name);
    _activeContainer->setDesc(ui->le_scriptdesc->text());
    _activeContainer->setCode(ui->pte_luacode->toPlainText());
    _activeContainer->setEnabled(ui->cbx_enable->isChecked());
    _activeContainer->setConstraintEnabled(ui->cbx_timeconstraint->isChecked());
    _activeContainer->setConstraintFromTo(ui->tied_timefrom->time(), ui->tied_timeto->time());
    _activeContainer->removeAllTrigger();
    if(ui->cb_every->isChecked()) _activeContainer->addRunEveryTrigger(ui->spb_everycount->value(), ui->dd_everyunits->currentIndex());
    if(ui->cb_once->isChecked()) _activeContainer->addRunOnceTrigger(ui->dte_runonce->dateTime());
    if(ui->cb_startup->isChecked()) _activeContainer->addRunStratupTrigger();
    if(ui->cb_shutdown->isChecked()) _activeContainer->addRunShutdownTrigger();
    if(ui->cb_chatmessage->isChecked()) _activeContainer->addEventTrigger(L4R_LOBBY_MESSAGERECEIVED);
    if(ui->cb_turtleSearchResult->isChecked()) _activeContainer->addEventTrigger(L4R_FILE_SEARCHRESULT);
    if(_draft) { _lua->codeList()->addItem(_activeContainer); _draft = false; }
    else if(oldName != name) _lua->codeList()->rename(oldName, name);
    QDir().mkpath(QString::fromStdString(_lua->getPath()));
    const bool saved = _lua->codeList()->saveAll();
    scriptsLock.unlock();
    if(!saved)
    { QMessageBox::warning(this, tr("Could not save"), tr("The script could not be fully saved. Check write access to the scripts folder and try again.")); return false; }
    _dirty = false;
    { const QSignalBlocker blocker(ui->le_scriptname); ui->le_scriptname->setText(name); }
    refreshScripts(); updateEditorState();
    ui->runStatus->setText(tr("Saved %1").arg(name));
    return true;
}

void Lua4RSWidget::on_pb_newscript_clicked() { createScript("-- Write your Lua code here.\n", QString()); }
void Lua4RSWidget::on_pb_save_clicked() { saveScript(); }
void Lua4RSWidget::on_pb_load_clicked()
{
    if(!maybeSave()) return;
    std::lock_guard<std::recursive_mutex> scriptsLock(_lua->scriptsMutex());
    const QString name = _activeContainer ? _activeContainer->getName() : QString();
    _activeContainer = nullptr;
    _lua->codeList()->loadAll();
    LuaContainer* selected = nullptr;
    _lua->codeList()->itemByName(name, selected);
    loadScript(selected); refreshScripts();
}
void Lua4RSWidget::openScript(int row, int)
{
    if(row < 0 || !ui->tw_allscripts->item(row, 0)) return;
    const QString name = ui->tw_allscripts->item(row, 0)->text();
    if(_activeContainer && !_draft && _activeContainer->getName() == name) return;
    if(!maybeSave()) { selectActiveScript(); return; }
    LuaContainer* selected = nullptr;
    if(_lua->codeList()->itemByName(name, selected)) { _draft = false; loadScript(selected); }
}
void Lua4RSWidget::on_pb_deletescript_clicked()
{
    if(!_activeContainer || _draft) return;
    if(QMessageBox::question(this, tr("Delete script"), tr("Delete %1 from disk? This also discards any editor changes.").arg(_activeContainer->getName()),
       QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel) != QMessageBox::Yes) return;
    std::unique_lock<std::recursive_mutex> scriptsLock(_lua->scriptsMutex());
    auto removed = _activeContainer;
    loadScript(nullptr);
    const bool deleted = _lua->codeList()->removeItemAndDelete(removed);
    scriptsLock.unlock();
    if(!deleted)
        QMessageBox::warning(this, tr("Could not delete file"), tr("The script was removed from this list, but its file could not be deleted. Check the scripts folder."));
    refreshScripts();
}
void Lua4RSWidget::on_pb_run_clicked()
{
    if(!_activeContainer || _running) return;
    _running = true; updateEditorState();
    ui->outputTabs->setCurrentWidget(ui->outputTab);
    ui->runStatus->setText(tr("Running…"));
    appendLog(tr("Run: %1").arg(ui->le_scriptname->text()));
    if(!_lua->queueManualRun(ui->pte_luacode->toPlainText()))
    {
        _running = false;
        ui->runStatus->setText(tr("Cannot run: Lua is busy or shutting down."));
        updateEditorState();
    }
}
void Lua4RSWidget::newApiExample()
{
    createScript("-- Apply your connection settings in the JSON API panel first.\n"
                 "local result, err = jsonapi.call(\"/rsPeers/getFriendList\")\n"
                 "assert(result, err)\n"
                 "assert(result.retval, \"RetroShare could not list friends\")\n"
                 "print(\"Friends: \" .. #result.sslIds)\n"
                 "for _, peer in ipairs(result.sslIds) do\n    print(peer)\nend\n", tr("List friends through the JSON API"));
}
void Lua4RSWidget::filterScripts(const QString& query)
{
    for(int row = 0; row < ui->tw_allscripts->rowCount(); ++row)
        ui->tw_allscripts->setRowHidden(row, !ui->tw_allscripts->item(row, 0)->text().contains(query, Qt::CaseInsensitive));
}
void Lua4RSWidget::filterFunctions(const QString& query)
{
    for(int i = 0; i < ui->tw_hints->topLevelItemCount(); ++i)
    {
        FunctionBrowser::filter(ui->tw_hints->topLevelItem(i), query);
    }
}
void Lua4RSWidget::on_pb_pastehint_released()
{
    auto selected = ui->tw_hints->currentItem();
    if(selected) on_tw_hints_itemDoubleClicked(selected, 0);
}
void Lua4RSWidget::on_tw_hints_itemDoubleClicked(QTreeWidgetItem* item, int)
{
    if(!_activeContainer || !item || item->childCount() || item->text(1).isEmpty()) return;
    ui->editorTabs->setCurrentWidget(ui->codeTab);
    ui->pte_luacode->insertPlainText(item->text(1)); ui->pte_luacode->setFocus();
}

void Lua4RSWidget::testApiConnection()
{
    const QUrl origin(ui->le_apiurl->text().trimmed());
    if(!origin.isValid() || (origin.scheme() != "http" && origin.scheme() != "https") || origin.host().isEmpty() ||
       !origin.userInfo().isEmpty() || origin.hasQuery() || origin.hasFragment() || (!origin.path().isEmpty() && origin.path() != "/") || ui->le_apiuser->text().contains(':'))
    { ui->apiStatus->setText(tr("Enter an HTTP(S) server address without a path, and a username without a colon.")); return; }
    QString error;
    if(!_lua->configureJsonApi(origin.toString(), ui->le_apiuser->text(), ui->le_apipassword->text(), error))
    { ui->apiStatus->setText(error); return; }
    QUrl endpoint(origin); endpoint.setPath("/rsPeers/getFriendList");
    QNetworkRequest request(endpoint);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);
    request.setRawHeader("Authorization", "Basic " + (ui->le_apiuser->text() + ':' + ui->le_apipassword->text()).toUtf8().toBase64());
    auto reply = _network->post(request, "{}");
    auto timer = new QTimer(reply); timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, reply, [reply]() { reply->setProperty("timedOut", true); reply->abort(); });
    timer->start(5000);
    ui->pb_apitest->setEnabled(false);
    for(auto field : {ui->le_apiurl, ui->le_apiuser, ui->le_apipassword}) field->setEnabled(false);
    ui->apiStatus->setText(tr("Settings applied. Testing connection…"));
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        const QJsonDocument result = QJsonDocument::fromJson(reply->readAll());
        if(reply->property("timedOut").toBool()) ui->apiStatus->setText(tr("Connection timed out. Check that the JSON API is enabled and the address is correct."));
        else if(status == 401 || status == 403) ui->apiStatus->setText(tr("Credentials rejected. Authorize this API user in RetroShare, then try again."));
        else if(reply->error() != QNetworkReply::NoError) ui->apiStatus->setText(tr("Could not connect: %1").arg(reply->errorString()));
        else if(status != 200 || !result.isObject() || !result.object().value("retval").toBool()) ui->apiStatus->setText(tr("Server replied, but the friend-list request failed (HTTP %1).").arg(status));
        else ui->apiStatus->setText(tr("Connected. Choose New API example, then Run."));
        ui->pb_apitest->setEnabled(true);
        for(auto field : {ui->le_apiurl, ui->le_apiuser, ui->le_apipassword}) field->setEnabled(true);
        reply->deleteLater();
    });
}
