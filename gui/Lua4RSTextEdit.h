#ifndef LUA4RSTEXTEDIT_H
#define LUA4RSTEXTEDIT_H

#include <QPlainTextEdit>
#include <QPalette>

class QLuaSyntaxHighlighter;

class Lua4RSTextEdit : public QPlainTextEdit
{
public:
	explicit Lua4RSTextEdit(QWidget *parent = 0);
	~Lua4RSTextEdit() {}
protected:
    void changeEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
private:
    void updateLineNumberArea();
    void paintLineNumbers(QPaintEvent* event);
    QWidget* _lineNumberArea = nullptr;
    QLuaSyntaxHighlighter* _highlighter = nullptr;
};

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QVector>

class QLuaSyntaxHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT
public:
	explicit QLuaSyntaxHighlighter(QTextDocument *document, const QPalette& palette);
	~QLuaSyntaxHighlighter() {}

protected:
	void highlightBlock(const QString &text);

	struct HighlightingRule
	{
		QRegularExpression pattern;
		QTextCharFormat format;
	};
	QVector<HighlightingRule> _highlightingRules;

	QRegularExpression _commentStartExpression;
	QRegularExpression _commentEndExpression;
	QTextCharFormat _multiLineCommentFormat;
	HighlightingRule _singleLineComment;
};

#endif // LUA4RSTEXTEDIT_H
