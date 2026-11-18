#include "McpPanel.h"

#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

#include "mcp/McpSettings.h"
#ifdef LUA4RS_MCP
#include "mcp/McpServer.h"
#endif

McpPanel::McpPanel(McpSettings& settings, McpServer* server, QWidget* parent)
    : QWidget(parent), _settings(settings), _server(server)
{
    auto layout = new QVBoxLayout(this);

    auto intro = new QLabel(tr("Lets a coding agent such as Claude Code read, write and test your scripts. "
                               "Scripts the agent writes or changes run read-only until you approve them."), this);
    intro->setWordWrap(true);
    layout->addWidget(intro);

    _enable = new QCheckBox(tr("Enable MCP endpoint"), this);
    layout->addWidget(_enable);

    _status = new QLabel(this);
    _status->setWordWrap(true);
    _status->setTextInteractionFlags(Qt::TextSelectableByMouse);
    layout->addWidget(_status);

    layout->addWidget(new QLabel(tr("Token (only valid for this endpoint, not for the JSON API):"), this));
    auto tokenRow = new QHBoxLayout;
    _token = new QLineEdit(this);
    _token->setReadOnly(true);
    _token->setEchoMode(QLineEdit::Password);
    _token->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    tokenRow->addWidget(_token, 1);
    _show = new QPushButton(tr("Show"), this);
    _show->setCheckable(true);
    tokenRow->addWidget(_show);
    auto renew = new QPushButton(tr("New token"), this);
    tokenRow->addWidget(renew);
    layout->addLayout(tokenRow);

    layout->addWidget(new QLabel(tr("Add to Claude Code:"), this));
    _command = new QPlainTextEdit(this);
    _command->setReadOnly(true);
    _command->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    _command->setMaximumHeight(90);
    layout->addWidget(_command);
    auto copy = new QPushButton(tr("Copy command"), this);
    layout->addWidget(copy, 0, Qt::AlignLeft);
    layout->addStretch(1);

    _enable->setChecked(_settings.enabled());
    connect(_enable, &QCheckBox::toggled, this, [this](bool on) {
        _settings.setEnabled(on);
#ifdef LUA4RS_MCP
        if(_server) _server->apply();
#endif
        refresh();
    });
    connect(_show, &QPushButton::toggled, this, &McpPanel::refresh);
    connect(renew, &QPushButton::clicked, this, [this]() {
        if(QMessageBox::question(this, tr("New MCP token"),
               tr("Agents using the current token lose access until you give them the new one. Continue?"),
               QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel) != QMessageBox::Yes) return;
        _settings.regenerateToken();
        refresh();
    });
    connect(copy, &QPushButton::clicked, this, [this]() {
        QApplication::clipboard()->setText(command(true));
    });

    // The JSON API can be switched on and off in RetroShare's settings.
    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &McpPanel::refresh);
    timer->start(3000);
    refresh();
}

QString McpPanel::command(bool revealToken) const
{
#ifdef LUA4RS_MCP
    const QString url = McpServer::endpointUrl();
#else
    const QString url;
#endif
    const QString token = revealToken ? _settings.token() : QString(8, QChar(0x2022));
    return QString("claude mcp add --transport http lua4rs %1 --header \"Authorization: Bearer %2\"").arg(url, token);
}

void McpPanel::refresh()
{
#ifndef LUA4RS_MCP
    _enable->setEnabled(false);
    _status->setText(tr("This build of Lua4RS has no JSON API support, so there is no MCP endpoint."));
    return;
#else
    const bool on = _settings.enabled();
    const bool hasToken = !_settings.token().isEmpty();
    _token->setText(_settings.token());
    _token->setEchoMode(_show->isChecked() ? QLineEdit::Normal : QLineEdit::Password);
    _command->setPlainText(on && hasToken ? command(_show->isChecked()) : QString());

    if(!_server) _status->setText(tr("MCP endpoint unavailable."));
    else if(!on) _status->setText(tr("Off."));
    else if(!hasToken) _status->setText(tr("No valid token — click New token."));
    else if(!McpServer::jsonApiRunning())
        _status->setText(tr("Waiting for the JSON API. Enable it in RetroShare: Settings → JSON API."));
    else if(!_server->registered()) _status->setText(tr("Starting…"));
    else _status->setText(tr("Listening on %1").arg(McpServer::endpointUrl()));
#endif
}
