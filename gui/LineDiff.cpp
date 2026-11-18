#include "LineDiff.h"

#include <algorithm>
#include <cstdint>

std::vector<DiffLine> lineDiff(const QStringList& before, const QStringList& after, long long maxCells)
{
    std::vector<DiffLine> result;
    const int na = before.size(), nb = after.size();
    int prefix = 0;
    while(prefix < na && prefix < nb && before[prefix] == after[prefix]) ++prefix;
    int suffix = 0;
    while(suffix < na - prefix && suffix < nb - prefix &&
          before[na - 1 - suffix] == after[nb - 1 - suffix]) ++suffix;

    for(int i = 0; i < prefix; ++i) result.push_back({ DiffLine::Same, before[i] });

    const int n = na - prefix - suffix, m = nb - prefix - suffix;
    auto a = [&](int i) -> const QString& { return before[prefix + i]; };
    auto b = [&](int j) -> const QString& { return after[prefix + j]; };

    if(n > 0 && m > 0 && (long long)(n + 1) * (m + 1) <= maxCells)
    {
        // lcs[i][j] = length of the LCS of a[i..] and b[j..]; bounded by min(n, m)
        // which is at most sqrt(maxCells), so 32 bit is plenty.
        std::vector<uint32_t> lcs((size_t)(n + 1) * (m + 1), 0);
        auto at = [&](int i, int j) -> uint32_t& { return lcs[(size_t)i * (m + 1) + j]; };
        for(int i = n - 1; i >= 0; --i)
            for(int j = m - 1; j >= 0; --j)
                at(i, j) = a(i) == b(j) ? at(i + 1, j + 1) + 1 : std::max(at(i + 1, j), at(i, j + 1));
        int i = 0, j = 0;
        while(i < n && j < m)
        {
            if(a(i) == b(j)) { result.push_back({ DiffLine::Same, a(i) }); ++i; ++j; }
            else if(at(i + 1, j) >= at(i, j + 1)) result.push_back({ DiffLine::Removed, a(i++) });
            else result.push_back({ DiffLine::Added, b(j++) });
        }
        for(; i < n; ++i) result.push_back({ DiffLine::Removed, a(i) });
        for(; j < m; ++j) result.push_back({ DiffLine::Added, b(j) });
    }
    else
    {
        for(int i = 0; i < n; ++i) result.push_back({ DiffLine::Removed, a(i) });
        for(int j = 0; j < m; ++j) result.push_back({ DiffLine::Added, b(j) });
    }

    for(int i = na - suffix; i < na; ++i) result.push_back({ DiffLine::Same, before[i] });
    return result;
}
