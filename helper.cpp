#include "helper.h"
#include <QString>
#include <QTextDocument>

void replaceAll(std::string &str, const std::string &from, const std::string &to)
{
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

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
