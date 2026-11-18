#ifndef LINE_DIFF_H
#define LINE_DIFF_H

#include <vector>

#include <QString>
#include <QStringList>

// Line-based diff (longest common subsequence) for the approval dialog.
struct DiffLine
{
    enum Kind { Same, Removed, Added };
    Kind kind;
    QString text;
};

// Above maxCells (lines before x lines after, without common prefix/suffix)
// the middle part is reported as removed + added instead of diffed.
std::vector<DiffLine> lineDiff(const QStringList& before, const QStringList& after,
                               long long maxCells = 4000000);

#endif // LINE_DIFF_H
