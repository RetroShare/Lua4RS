#include <QVariant>
#include <QEvent>
#include <QFontDatabase>
#include <QPainter>
#include <QPaintEvent>
#include <QTextBlock>

#include "Lua4RSTextEdit.h"

Lua4RSTextEdit::Lua4RSTextEdit(QWidget *parent) :
	QPlainTextEdit(parent)
{
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    _lineNumberArea = new QWidget(this);
    _lineNumberArea->setObjectName("lineNumberArea");
    _lineNumberArea->installEventFilter(this);
    connect(this, &QPlainTextEdit::blockCountChanged, this, [this]() {
        updateLineNumberArea();
    });
    connect(this, &QPlainTextEdit::updateRequest, this, [this](const QRect& rect, int dy) {
        if(dy) _lineNumberArea->scroll(0, dy);
        else _lineNumberArea->update(0, rect.y(), _lineNumberArea->width(), rect.height());
    });
    updateLineNumberArea();
    _highlighter = new QLuaSyntaxHighlighter(document(), palette());
}

void Lua4RSTextEdit::updateLineNumberArea()
{
    if(!_lineNumberArea) return;
    const int digits = QString::number(blockCount()).size();
    const int width = 12 + digits * fontMetrics().horizontalAdvance(QLatin1Char('9'));
    setViewportMargins(width, 0, 0, 0);
    _lineNumberArea->setGeometry(contentsRect().left(), viewport()->geometry().top(),
                                 width, viewport()->height());
    _lineNumberArea->update();
}

void Lua4RSTextEdit::resizeEvent(QResizeEvent* event)
{
    QPlainTextEdit::resizeEvent(event);
    updateLineNumberArea();
}

bool Lua4RSTextEdit::eventFilter(QObject* watched, QEvent* event)
{
    if(watched == _lineNumberArea && event->type() == QEvent::Paint)
    {
        paintLineNumbers(static_cast<QPaintEvent*>(event));
        return true;
    }
    return QPlainTextEdit::eventFilter(watched, event);
}

void Lua4RSTextEdit::paintLineNumbers(QPaintEvent* event)
{
    QPainter painter(_lineNumberArea);
    // Stylesheets may set Base/Text but leave AlternateBase at its light default.
    // Use the editor's matching background/text pair for the gutter as well.
    const QPalette editorPalette = viewport()->palette();
    painter.fillRect(event->rect(), editorPalette.color(QPalette::Base));
    painter.setFont(font());
    painter.setPen(editorPalette.color(QPalette::Text));
    QTextBlock block = firstVisibleBlock();
    while(block.isValid())
    {
        const qreal top = blockBoundingGeometry(block).translated(contentOffset()).top();
        if(top > event->rect().bottom()) break;
        if(block.isVisible() && top + blockBoundingRect(block).height() >= event->rect().top())
            painter.drawText(0, qRound(top), _lineNumberArea->width() - 6,
                             fontMetrics().height(), Qt::AlignRight,
                             QString::number(block.blockNumber() + 1));
        block = block.next();
    }
}

void Lua4RSTextEdit::changeEvent(QEvent* event)
{
    QPlainTextEdit::changeEvent(event);
    if(event->type() == QEvent::FontChange || event->type() == QEvent::StyleChange
       || event->type() == QEvent::PaletteChange)
        updateLineNumberArea();
    if(event->type() == QEvent::PaletteChange)
    {
        delete _highlighter;
        _highlighter = new QLuaSyntaxHighlighter(document(), palette());
        _highlighter->rehighlight();
    }
}

QLuaSyntaxHighlighter::QLuaSyntaxHighlighter(QTextDocument *document, const QPalette& palette) :
	QSyntaxHighlighter(document)
{
	HighlightingRule rule;
    const bool dark = palette.color(QPalette::Base).lightness() < 128;

	// special words
	QTextCharFormat keywordFormat;
	keywordFormat.setForeground(dark ? QColor("#93c5fd") : QColor("#1e40af"));
	keywordFormat.setFontWeight(QFont::Bold);
	QStringList keywordPatterns;
	keywordPatterns	<< "\\btrue\\b"		<< "\\bfalse\\b"	<< "\\bnil\\b"
					<< "\\bif\\b"		<< "\\bthen\\b"		<< "\\belse\\b"		<< "\\belseif\\b"
					<< "\\bfor\\b"		<< "\\bbreak\\b"	<< "\\bend\\b"
					<< "\\buntil\\b"	<< "\\bwhile\\b"	<< "\\bdo\\b"
					<< "\\bfunction\\b"	<< "\\bin\\b"		<< "\\blocal\\b"
					<< "\\brepeat\\b"	<< "\\breturn\\b";

	foreach (const QString &pattern, keywordPatterns) {
		rule.pattern = QRegularExpression(pattern);
		rule.format = keywordFormat;
		_highlightingRules.append(rule);
	}

	// operators
	QTextCharFormat operatorFormat;
	operatorFormat.setForeground(palette.color(QPalette::Text));
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
		rule.pattern = QRegularExpression(pattern);
		rule.format = operatorFormat;
		_highlightingRules.append(rule);
	}

	// numbers
	QTextCharFormat numberFormat;
	numberFormat.setForeground(dark ? QColor("#d8b4fe") : QColor("#7e22ce"));
	rule.pattern = QRegularExpression("[0-9]+");
	rule.format = numberFormat;
	_highlightingRules.append(rule);

	// functions
	QTextCharFormat functionFormat;
	functionFormat.setForeground(dark ? QColor("#67e8f9") : QColor("#0369a1"));
	rule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
	rule.format = functionFormat;
	_highlightingRules.append(rule);

	// namespaces
	QTextCharFormat namespaceFormat;
	namespaceFormat.setForeground(dark ? QColor("#c4b5fd") : QColor("#6d28d9"));
	rule.pattern = QRegularExpression("\\b(?=(.*[a-z]))[A-Za-z0-9_]+(?=\\.)");
	rule.format = namespaceFormat;
	_highlightingRules.append(rule);

	// quotations - add at the end! (but before comments)
	QTextCharFormat quotationFormat;
	quotationFormat.setForeground(dark ? QColor("#fcd34d") : QColor("#854d0e"));
	rule.pattern = QRegularExpression("\".*\"");
	rule.pattern.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
	rule.format = quotationFormat;
	_highlightingRules.append(rule);
	rule.pattern = QRegularExpression("'.*'");
	rule.pattern.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
	rule.format = quotationFormat;
	_highlightingRules.append(rule);

	// comments - add at the end!
	// single line
	QTextCharFormat singleLineCommentFormat;
	singleLineCommentFormat.setForeground(dark ? QColor("#86efac") : QColor("#166534"));
	rule.pattern = QRegularExpression("--[^\n]*");
	rule.format = singleLineCommentFormat;
	_highlightingRules.append(rule);

	// multi lines
	_commentStartExpression = QRegularExpression("--\\[\\[");
	_commentEndExpression = QRegularExpression("\\]\\]");
	_multiLineCommentFormat.setForeground(dark ? QColor("#86efac") : QColor("#166534"));
	_multiLineCommentFormat.setFontItalic(true);

}

void QLuaSyntaxHighlighter::highlightBlock(const QString &text)
{
	foreach (const HighlightingRule &rule, _highlightingRules) {
		auto matches = rule.pattern.globalMatch(text);
		while (matches.hasNext()) {
			const auto match = matches.next();
			setFormat(match.capturedStart(), match.capturedLength(), rule.format);
		}
	}
	setCurrentBlockState(0);

	int startIndex = 0;
	if (previousBlockState() != 1)
		startIndex = _commentStartExpression.match(text).capturedStart();

	while (startIndex >= 0) {
		const auto endMatch = _commentEndExpression.match(text, startIndex);
		int endIndex = endMatch.capturedStart();
		int commentLength;
		if (endIndex == -1) {
			setCurrentBlockState(1);
			commentLength = text.length() - startIndex;
		} else {
			commentLength = endIndex - startIndex
					+ endMatch.capturedLength();
		}
		setFormat(startIndex, commentLength, _multiLineCommentFormat);
		startIndex = _commentStartExpression.match(text, startIndex + commentLength).capturedStart();
	}
}
