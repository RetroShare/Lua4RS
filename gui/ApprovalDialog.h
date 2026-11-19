#ifndef APPROVAL_DIALOG_H
#define APPROVAL_DIALOG_H

#include <QDialog>
#include <QStringList>

#include "Lua/LuaTrust.h"
#include "Lua/LuaTrustStore.h"

class QButtonGroup;
class QPlainTextEdit;
class QPushButton;

// Shows what changed since the last approval and lets the user pick the
// access level for exactly the code shown.
class ApprovalDialog : public QDialog
{
    Q_OBJECT
public:
    // approvedCode: verified copy of the approved version (see
    // LuaTrustStore::approvedCode), only used if haveApprovedCode is true.
    ApprovalDialog(const QString& name, const QString& code, const LuaTrustStore::Status& status,
                   bool haveApprovedCode, const QString& approvedCode,
                   const QStringList& writeFunctions, const QStringList& sensitiveFunctions,
                   QWidget* parent = nullptr);

    LuaTrust level() const;

private:
    void showDiff(const QString& before, const QString& after);
    void showCode(const QString& code);
    void updateApproveButton();

    QPlainTextEdit* _view;
    QButtonGroup* _levels;
    QPushButton* _approve;
};

#endif // APPROVAL_DIALOG_H
