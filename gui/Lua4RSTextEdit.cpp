#include <QVariant>

#include "Lua4RSTextEdit.h"

Lua4RSTextEdit::Lua4RSTextEdit(QWidget *parent) :
	QPlainTextEdit(parent)
{
	new QLuaSyntaxHighlighter(document());
}

QLuaSyntaxHighlighter::QLuaSyntaxHighlighter(QTextDocument *document) :
	QSyntaxHighlighter(document)
{
	HighlightingRule rule;

	// special words
	QTextCharFormat keywordFormat;
	keywordFormat.setForeground(Qt::darkBlue);
	keywordFormat.setFontWeight(QFont::Bold);
	QStringList keywordPatterns;
	keywordPatterns	<< "\\btrue\\b"		<< "\\bfalse\\b"	<< "\\bnil\\b"
					<< "\\bif\\b"		<< "\\bthen\\b"		<< "\\belse\\b"		<< "\\belseif\\b"
					<< "\\bfor\\b"		<< "\\bbreak\\b"	<< "\\bend\\b"
					<< "\\buntil\\b"	<< "\\bwhile\\b"	<< "\\bdo\\b"
					<< "\\bfunction\\b"	<< "\\bin\\b"		<< "\\blocal\\b"
					<< "\\brepeat\\b"	<< "\\breturn\\b";

	foreach (const QString &pattern, keywordPatterns) {
		rule.pattern = QRegExp(pattern);
		rule.format = keywordFormat;
		_highlightingRules.append(rule);
	}

	// operators
	QTextCharFormat operatorFormat;
	operatorFormat.setForeground(Qt::darkGray);
	operatorFormat.setFontWeight(QFont::Bold);
	QStringList operatorPatterns;
	operatorPatterns << "\\band\\b"	<< "\\bnot\\b" << "\\bor\\b"
					 << "\\+" << "\\-"
					 << "\\*" << "\\/"
					 << "\\<" << "\\<\\="
					 << "\\>" << "\\>\\="
					 << "\\=\\=" << "\\~\\=" << "\\="
					 << "\\^" << "\\.\\."
					 << "\\#";

	foreach (const QString &pattern, operatorPatterns) {
		rule.pattern = QRegExp(pattern);
		rule.format = operatorFormat;
		_highlightingRules.append(rule);
	}

	// numbers
	QTextCharFormat numberFormat;
	numberFormat.setForeground(Qt::magenta);
	rule.pattern = QRegExp("[0-9]+");
	rule.format = numberFormat;
	_highlightingRules.append(rule);

	// functions
	QTextCharFormat functionFormat;
	functionFormat.setForeground(Qt::blue);
	rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
	rule.format = functionFormat;
	_highlightingRules.append(rule);

	// namespaces
	QTextCharFormat namespaceFormat;
	namespaceFormat.setForeground(Qt::darkMagenta);
	rule.pattern = QRegExp("\\b(?=(.*[a-z]))[A-Za-z0-9_]+(?=\\.)");
	rule.format = namespaceFormat;
	_highlightingRules.append(rule);

	// quotations - add at the end! (but before comments)
	QTextCharFormat quotationFormat;
	quotationFormat.setForeground(Qt::darkYellow);
	rule.pattern = QRegExp("\".*\"");
	rule.pattern.setMinimal(true);
	rule.format = quotationFormat;
	_highlightingRules.append(rule);
	rule.pattern = QRegExp("'.*'");
	rule.pattern.setMinimal(true);
	rule.format = quotationFormat;
	_highlightingRules.append(rule);

	// comments - add at the end!
	// single line
	QTextCharFormat singleLineCommentFormat;
	singleLineCommentFormat.setForeground(Qt::darkGreen);
	rule.pattern = QRegExp("--[^\n]*");
	rule.format = singleLineCommentFormat;
	_highlightingRules.append(rule);

	// multi lines
	_commentStartExpression = QRegExp("--\\[\\[");
	_commentEndExpression = QRegExp("\\]\\]");
	_multiLineCommentFormat.setForeground(Qt::darkGreen);
	_multiLineCommentFormat.setFontItalic(true);

}

void QLuaSyntaxHighlighter::highlightBlock(const QString &text)
{
	foreach (const HighlightingRule &rule, _highlightingRules) {
		QRegExp expression(rule.pattern);
		int index = expression.indexIn(text);
		while (index >= 0) {
			int length = expression.matchedLength();
			setFormat(index, length, rule.format);
			index = expression.indexIn(text, index + length);
		}
	}
	setCurrentBlockState(0);

	int startIndex = 0;
	if (previousBlockState() != 1)
		startIndex = _commentStartExpression.indexIn(text);

	while (startIndex >= 0) {
		int endIndex = _commentEndExpression.indexIn(text, startIndex);
		int commentLength;
		if (endIndex == -1) {
			setCurrentBlockState(1);
			commentLength = text.length() - startIndex;
		} else {
			commentLength = endIndex - startIndex
					+ _commentEndExpression.matchedLength();
		}
		setFormat(startIndex, commentLength, _multiLineCommentFormat);
		startIndex = _commentStartExpression.indexIn(text, startIndex + commentLength);
	}
}
