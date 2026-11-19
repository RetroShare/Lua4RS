#include "ApprovalDialog.h"
#include "AccessText.h"
#include "LineDiff.h"

#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QFontDatabase>
#include <QFontMetricsF>
#include <QGroupBox>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QTextBlock>
#include <QTextCursor>
#include <QVBoxLayout>

using State = LuaTrustStore::State;

ApprovalDialog::ApprovalDialog(const QString& name, const QString& code, const LuaTrustStore::Status& status,
                               bool haveApprovedCode, const QString& approvedCode,
                               const QStringList& writeFunctions, const QStringList& sensitiveFunctions,
                               QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Approve %1").arg(name));
    auto layout = new QVBoxLayout(this);

    auto current = new QLabel(AccessText::details(status), this);
    current->setWordWrap(true);
    layout->addWidget(current);

    auto note = new QLabel(this);
    note->setWordWrap(true);
    layout->addWidget(note);

    _view = new QPlainTextEdit(this);
    _view->setReadOnly(true);
    _view->setLineWrapMode(QPlainTextEdit::NoWrap);
    _view->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    _view->setTabStopDistance(QFontMetricsF(_view->font()).horizontalAdvance(' ') * 4);
    layout->addWidget(_view, 1);

    if(status.state == State::Modified && haveApprovedCode)
    {
        note->setText(tr("Changes since the approval (green: added, red: removed):"));
        showDiff(approvedCode, code);
    }
    else if(status.state == State::ChangedExample && haveApprovedCode)
    {
        note->setText(tr("Changes compared with the example shipped with Lua4RS (green: added, red: removed):"));
        showDiff(approvedCode, code);
    }
    else
    {
        switch(status.state)
        {
        case State::Modified:
        case State::ChangedExample:
            note->setText("<b>" + tr("The saved copy of the approved version is missing or was changed, "
                                     "so the changes cannot be shown.") + "</b> " +
                          tr("Review the whole script."));
            break;
        case State::Unknown:
            note->setText(tr("This script was never approved. Review the whole script."));
            break;
        case State::Approved:
            note->setText(tr("No changes since the approval. You can change the access level."));
            break;
        case State::Bundled:
            note->setText(tr("This is an unchanged example shipped with Lua4RS."));
            break;
        }
        showCode(code);
    }

    // Only a reading aid: a script can reach the same functions under another name.
    QStringList used;
    for(const QString& function : writeFunctions)
        if(code.contains(function)) used << function;
    if(code.contains("jsonapi.call")) used << tr("jsonapi.call (Trusted only)");
    auto writes = new QLabel(used.isEmpty()
        ? tr("No functions that change RetroShare are mentioned in the code.")
        : tr("Functions that change RetroShare mentioned in the code: %1").arg(used.join(", ")), this);
    writes->setWordWrap(true);
    layout->addWidget(writes);

    QStringList reads;
    for(const QString& function : sensitiveFunctions)
        if(code.contains(function)) reads << function;
    if(!reads.isEmpty())
    {
        auto sensitive = new QLabel("<b>" + tr("Reads sensitive content (mail, certificate):") + "</b> " +
                                    reads.join(", ").toHtmlEscaped(), this);
        sensitive->setWordWrap(true);
        layout->addWidget(sensitive);
    }

    auto box = new QGroupBox(tr("Access"), this);
    auto boxLayout = new QVBoxLayout(box);
    _levels = new QButtonGroup(this);
    for(LuaTrust level : { LuaTrust::SandboxReadOnly, LuaTrust::SandboxWrite, LuaTrust::Trusted })
    {
        auto radio = new QRadioButton(AccessText::label(level) + " — " + AccessText::meaning(level), box);
        _levels->addButton(radio, int(level));
        boxLayout->addWidget(radio);
    }
    const LuaTrust preset = status.state == State::Unknown ? LuaTrust::SandboxReadOnly : status.approvedLevel;
    _levels->button(int(preset))->setChecked(true);
    layout->addWidget(box);

    auto buttons = new QDialogButtonBox(this);
    _approve = buttons->addButton(tr("Approve"), QDialogButtonBox::AcceptRole);
    QPushButton* cancel = buttons->addButton(QDialogButtonBox::Cancel);
    // Approving is a deliberate click, never the result of pressing Enter.
    _approve->setAutoDefault(false);
    cancel->setDefault(true);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(_levels, &QButtonGroup::buttonToggled, this, [this](QAbstractButton*, bool) { updateApproveButton(); });
    layout->addWidget(buttons);
    updateApproveButton();

    resize(820, 640);
}

LuaTrust ApprovalDialog::level() const
{
    return LuaTrust(_levels->checkedId());
}

void ApprovalDialog::updateApproveButton()
{
    _approve->setText(tr("Approve as %1").arg(AccessText::label(level())));
}

void ApprovalDialog::showCode(const QString& code)
{
    _view->setPlainText(code);
}

void ApprovalDialog::showDiff(const QString& before, const QString& after)
{
    const std::vector<DiffLine> lines = lineDiff(before.split('\n'), after.split('\n'));

    // Unchanged lines more than `context` lines away from a change are collapsed.
    const int context = 3;
    std::vector<bool> show(lines.size(), false);
    for(size_t i = 0; i < lines.size(); ++i)
        if(lines[i].kind != DiffLine::Same)
            for(size_t j = i >= size_t(context) ? i - context : 0; j < lines.size() && j <= i + context; ++j)
                show[j] = true;

    QTextBlockFormat plain, added, removed;
    added.setBackground(QColor(46, 160, 67, 70));
    removed.setBackground(QColor(248, 81, 73, 70));
    QTextCharFormat normal, collapsed;
    collapsed.setForeground(palette().color(QPalette::Disabled, QPalette::Text));
    collapsed.setFontItalic(true);

    _view->clear();
    QTextCursor cursor(_view->document());
    bool first = true;
    auto append = [&](const QTextBlockFormat& block, const QTextCharFormat& chars, const QString& text) {
        if(first) { cursor.setBlockFormat(block); first = false; }
        else cursor.insertBlock(block);
        cursor.insertText(text, chars);
    };

    for(size_t i = 0; i < lines.size();)
    {
        if(!show[i])
        {
            size_t end = i;
            while(end < lines.size() && !show[end]) ++end;
            append(plain, collapsed, tr("… %n unchanged line(s)", "", int(end - i)));
            i = end;
            continue;
        }
        const DiffLine& line = lines[i++];
        switch(line.kind)
        {
        case DiffLine::Same: append(plain, normal, "  " + line.text); break;
        case DiffLine::Removed: append(removed, normal, "- " + line.text); break;
        case DiffLine::Added: append(added, normal, "+ " + line.text); break;
        }
    }
    _view->moveCursor(QTextCursor::Start);
}
