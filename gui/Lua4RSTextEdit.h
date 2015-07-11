#ifndef LUA4RSTEXTEDIT_H
#define LUA4RSTEXTEDIT_H

#include <QPlainTextEdit>

class Lua4RSTextEdit : public QPlainTextEdit
{
public:
	explicit Lua4RSTextEdit(QWidget *parent = 0);
	~Lua4RSTextEdit() {}
};

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegExp>
#include <QVector>

class QLuaSyntaxHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT
public:
	explicit QLuaSyntaxHighlighter(QTextDocument *document = 0);
	~QLuaSyntaxHighlighter() {}

protected:
	void highlightBlock(const QString &text);

	struct HighlightingRule
	{
		QRegExp pattern;
		QTextCharFormat format;
	};
	QVector<HighlightingRule> highlightingRules;

	QRegExp commentStartExpression;
	QRegExp commentEndExpression;
	QTextCharFormat multiLineCommentFormat;
};

#endif // LUA4RSTEXTEDIT_H
