#include "helper.h"
#include <QString>
#include <QTextDocument>

std::string &stripHTMLTags(std::string &s)
{
    // std::string to QString
    QString string = QString::fromUtf8(s.c_str());

    // extract plain text from HTML
    QTextDocument td;
    td.setHtml(string);
    string = td.toPlainText();

    // QString to std::string
    s = string.toStdString();

    return s;
}
