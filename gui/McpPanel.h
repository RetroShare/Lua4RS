#ifndef MCP_PANEL_H
#define MCP_PANEL_H

#include <QWidget>

class QCheckBox;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class McpServer;
class McpSettings;

// Tools tab "MCP": enable the endpoint, show its URL and token, and a
// ready-to-paste command for Claude Code.
class McpPanel : public QWidget
{
    Q_OBJECT
public:
    McpPanel(McpSettings& settings, McpServer* server, QWidget* parent = nullptr);

private:
    void refresh();
    QString command(bool revealToken) const;

    McpSettings& _settings;
    McpServer* _server;
    QCheckBox* _enable;
    QLabel* _status;
    QLineEdit* _token;
    QPushButton* _show;
    QPlainTextEdit* _command;
};

#endif // MCP_PANEL_H
