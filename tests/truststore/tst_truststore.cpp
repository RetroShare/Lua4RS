// Tests for the approval rules in LuaTrustStore (no RetroShare needed).
// Build: qmake truststore.pro && make && ./tst_truststore
#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <cstdio>
#include <string>

#include "LineDiff.h"
#include "LuaTrustStore.h"

namespace {
int failures = 0;
int checks = 0;

void check(bool condition, const char* what, int line)
{
    ++checks;
    if(!condition) { ++failures; std::printf("FAIL  line %d: %s\n", line, what); }
}
#define CHECK(cond) check((cond), #cond, __LINE__)

using State = LuaTrustStore::State;

QByteArray readFile(const QString& path)
{
    QFile f(path);
    return f.open(QIODevice::ReadOnly) ? f.readAll() : QByteArray("<missing>");
}

void unknownScriptsAreSandboxed()
{
    LuaTrustStore s;
    CHECK(s.status("a.lua", "print(1)").state == State::Unknown);
    CHECK(s.effective("a.lua", "print(1)") == LuaTrust::SandboxReadOnly);
}

void hashRule()
{
    LuaTrustStore s;
    s.approve("a.lua", "print(1)", LuaTrust::Trusted);
    CHECK(s.status("a.lua", "print(1)").state == State::Approved);
    CHECK(s.effective("a.lua", "print(1)") == LuaTrust::Trusted);
    // any change -> sandbox-readonly, approved level is still known
    const auto changed = s.status("a.lua", "print(2)");
    CHECK(changed.state == State::Modified);
    CHECK(changed.effective == LuaTrust::SandboxReadOnly);
    CHECK(changed.approvedLevel == LuaTrust::Trusted);
    CHECK(changed.approvedAt.isValid());
    CHECK(s.status("a.lua", "print(1)").approvedAt == changed.approvedAt);
    // approval is per name
    CHECK(s.status("b.lua", "print(1)").state == State::Unknown);
    CHECK(!s.status("b.lua", "print(1)").approvedAt.isValid());
}

void guiCreateIsTrusted()
{
    LuaTrustStore s;
    s.guiSaved("new0.lua", "", "mine.lua", "print('mine')", true);
    CHECK(s.effective("mine.lua", "print('mine')") == LuaTrust::Trusted);
}

void guiEditOfApprovedKeepsLevel()
{
    LuaTrustStore s;
    s.approve("a.lua", "v1", LuaTrust::SandboxWrite);
    s.guiSaved("a.lua", "v1", "a.lua", "v2", false);
    CHECK(s.status("a.lua", "v2").state == State::Approved);
    CHECK(s.effective("a.lua", "v2") == LuaTrust::SandboxWrite); // never raised
}

void guiSaveDoesNotApproveForeignChanges()
{
    LuaTrustStore s;
    s.approve("a.lua", "v1", LuaTrust::Trusted);
    // someone else changed the file to "evil"; the user opens and saves it
    s.guiSaved("a.lua", "evil", "a.lua", "evil", false);
    CHECK(s.effective("a.lua", "evil") == LuaTrust::SandboxReadOnly);
    CHECK(s.status("a.lua", "v1").state == State::Approved); // old approval kept
    // same for a file that never had an approval
    s.guiSaved("dropped.lua", "x", "dropped.lua", "x", false);
    CHECK(s.status("dropped.lua", "x").state == State::Unknown);
}

void renameMovesApproval()
{
    QTemporaryDir dir;
    LuaTrustStore s;
    s.setScriptDirectory(dir.path());
    s.approve("a.lua", "v1", LuaTrust::Trusted);
    s.guiSaved("a.lua", "v1", "b.lua", "v1", false);
    CHECK(s.status("a.lua", "v1").state == State::Unknown);
    CHECK(s.effective("b.lua", "v1") == LuaTrust::Trusted);
    CHECK(!QFile::exists(dir.filePath("a.lua.approved")));
    CHECK(readFile(dir.filePath("b.lua.approved")) == "v1");

    // renaming a modified script keeps it modified (and keeps the old copy)
    s.approve("c.lua", "v1", LuaTrust::Trusted);
    s.guiSaved("c.lua", "changed", "d.lua", "changed", false);
    CHECK(s.status("d.lua", "changed").state == State::Modified);
    CHECK(s.status("d.lua", "v1").state == State::Approved);
    CHECK(readFile(dir.filePath("d.lua.approved")) == "v1");
}

void approvedCopyFollowsApproval()
{
    QTemporaryDir dir;
    LuaTrustStore s;
    s.setScriptDirectory(dir.path());
    s.approve("a.lua", "v1", LuaTrust::Trusted);
    CHECK(readFile(dir.filePath("a.lua.approved")) == "v1");
    s.guiSaved("a.lua", "v1", "a.lua", "v2", false);
    CHECK(readFile(dir.filePath("a.lua.approved")) == "v2");
    s.guiSaved("a.lua", "foreign", "a.lua", "foreign", false); // not approved
    CHECK(readFile(dir.filePath("a.lua.approved")) == "v2");
    s.remove("a.lua");
    CHECK(!QFile::exists(dir.filePath("a.lua.approved")));
    CHECK(s.status("a.lua", "v2").state == State::Unknown);
}

void bundledScripts()
{
    LuaTrustStore s;
    auto shipped = [](const QString& name, QByteArray& code) {
        if(name != "example.lua" && name != "example2.lua") return false;
        code = "shipped";
        return true;
    };
    s.setBundledSource(shipped);
    CHECK(s.status("example.lua", "shipped").state == State::Bundled);
    CHECK(s.effective("example.lua", "shipped") == LuaTrust::Trusted);
    CHECK(s.effective("other.lua", "shipped") == LuaTrust::SandboxReadOnly);
    CHECK(s.status("other.lua", "shipped").state == State::Unknown);

    // changed outside the editor: sandboxed, but known as a changed example
    const auto changed = s.status("example.lua", "changed");
    CHECK(changed.state == State::ChangedExample);
    CHECK(changed.effective == LuaTrust::SandboxReadOnly);
    CHECK(changed.approvedLevel == LuaTrust::Trusted);
    CHECK(!changed.approvedAt.isValid());
    // the shipped code serves as the approved version for the diff
    QString code;
    CHECK(s.approvedCode("example.lua", code) && code == "shipped");
    CHECK(!s.approvedCode("other.lua", code));
    // saving a changed example in the GUI does not approve it
    s.guiSaved("example.lua", "changed", "example.lua", "changed", false);
    CHECK(s.status("example.lua", "changed").state == State::ChangedExample);

    // user edits the unchanged example in the GUI -> stays trusted
    s.guiSaved("example.lua", "shipped", "example.lua", "mine", false);
    CHECK(s.effective("example.lua", "mine") == LuaTrust::Trusted);
    CHECK(s.status("example.lua", "mine").state == State::Approved);
    // an explicit entry wins over the shipped script
    s.approve("example2.lua", "x", LuaTrust::SandboxReadOnly);
    CHECK(s.status("example2.lua", "shipped").state == State::Modified);
    CHECK(s.effective("example2.lua", "shipped") == LuaTrust::SandboxReadOnly);
    CHECK(s.effective("example2.lua", "x") == LuaTrust::SandboxReadOnly);
}

void editorRun()
{
    LuaTrustStore s;
    s.approve("a.lua", "v1", LuaTrust::Trusted);
    CHECK(s.forEditor("a.lua", "v1", false) == LuaTrust::Trusted);
    CHECK(s.forEditor("a.lua", "foreign", false) == LuaTrust::SandboxReadOnly);
    CHECK(s.forEditor("new0.lua", "", true) == LuaTrust::Trusted);
}

void serialisationRoundTrip()
{
    LuaTrustStore a;
    a.approve("a.lua", "v1", LuaTrust::Trusted);
    a.approve("b;c.lua", "v2", LuaTrust::SandboxWrite); // ';' in the name lives in the key
    a.approve("ü.lua", "v3", LuaTrust::SandboxReadOnly);
    a.setMigrated();

    LuaTrustStore b;
    for(const auto& kv : a.toKeyValues()) CHECK(b.fromKeyValue(kv.first, kv.second));
    CHECK(b.migrated());
    CHECK(b.effective("a.lua", "v1") == LuaTrust::Trusted);
    CHECK(b.effective("b;c.lua", "v2") == LuaTrust::SandboxWrite);
    CHECK(b.status("ü.lua", "v3").state == State::Approved);
    CHECK(b.effective("ü.lua", "v3") == LuaTrust::SandboxReadOnly);

    CHECK(!b.fromKeyValue("L4RCONFIG_TIMER_TICK", "1")); // not ours
}

void malformedEntriesDoNotGrantTrust()
{
    LuaTrustStore s;
    const std::string hash = LuaTrustStore::hash("x").toStdString();
    CHECK(s.fromKeyValue("L4R_TRUST:a.lua", "root;" + hash + ";2026-01-01T00:00:00Z"));
    CHECK(s.fromKeyValue("L4R_TRUST:b.lua", "trusted;abc;2026-01-01T00:00:00Z"));
    CHECK(s.fromKeyValue("L4R_TRUST:c.lua", "trusted;" + hash));
    CHECK(s.fromKeyValue("L4R_TRUST:", "trusted;" + hash + ";2026-01-01T00:00:00Z"));
    CHECK(s.effective("a.lua", "x") == LuaTrust::SandboxReadOnly);
    CHECK(s.effective("b.lua", "x") == LuaTrust::SandboxReadOnly);
    CHECK(s.effective("c.lua", "x") == LuaTrust::SandboxReadOnly);
    CHECK(!s.migrated());
    CHECK(s.fromKeyValue("L4R_TRUST_MIGRATED", "0"));
    CHECK(!s.migrated());
}

void approvedCodeIsVerified()
{
    QTemporaryDir dir;
    LuaTrustStore s;
    QString code;
    s.approve("a.lua", "v1", LuaTrust::Trusted);
    CHECK(!s.approvedCode("a.lua", code)); // no directory -> no copy

    s.setScriptDirectory(dir.path());
    s.approve("a.lua", "v1", LuaTrust::Trusted);
    CHECK(s.approvedCode("a.lua", code) && code == "v1");
    CHECK(!s.approvedCode("b.lua", code)); // no entry

    // someone swaps the copy to hide a change from the diff
    {
        QFile f(s.approvedCopyPath("a.lua"));
        CHECK(f.open(QIODevice::WriteOnly));
        f.write("evil");
    }
    code.clear();
    CHECK(!s.approvedCode("a.lua", code) && code.isEmpty());

    QFile::remove(s.approvedCopyPath("a.lua"));
    CHECK(!s.approvedCode("a.lua", code));

    // editing an approved version in the GUI refreshes the copy
    s.approve("c.lua", "v1", LuaTrust::SandboxWrite);
    s.guiSaved("c.lua", "v1", "c.lua", "v2 ü", false);
    CHECK(s.approvedCode("c.lua", code) && code == "v2 ü");
}

QString render(const std::vector<DiffLine>& diff)
{
    QString out;
    for(const auto& line : diff)
        out += QString(line.kind == DiffLine::Same ? " " : line.kind == DiffLine::Added ? "+" : "-") + line.text + "|";
    return out;
}

void lineDiffs()
{
    const QStringList abc = { "a", "b", "c" };
    CHECK(render(lineDiff(abc, abc)) == " a| b| c|");
    CHECK(render(lineDiff(abc, { "a", "x", "c" })) == " a|-b|+x| c|");
    CHECK(render(lineDiff(abc, { "a", "b", "c", "d" })) == " a| b| c|+d|");
    CHECK(render(lineDiff(abc, { "b", "c" })) == "-a| b| c|");
    CHECK(render(lineDiff({}, { "a" })) == "+a|");
    CHECK(render(lineDiff({ "a" }, {})) == "-a|");
    // an inserted line in the middle of otherwise equal text
    CHECK(render(lineDiff({ "1", "2", "3", "4" }, { "1", "2", "new", "3", "4" })) == " 1| 2|+new| 3| 4|");
    // moved line: one removal + one addition, rest kept
    CHECK(render(lineDiff({ "a", "b", "c", "d" }, { "b", "c", "d", "a" })) == "-a| b| c| d|+a|");
    // over the size limit: plain replace of the middle part
    CHECK(render(lineDiff({ "p", "a", "b", "s" }, { "p", "b", "a", "s" }, 4)) == " p|-a|-b|+b|+a| s|");
    // every line of both sides appears exactly once in the result
    QStringList before, after;
    for(int i = 0; i < 300; ++i) { before << QString::number(i % 17); after << QString::number((i * 7) % 13); }
    int removed = 0, added = 0, same = 0;
    for(const auto& line : lineDiff(before, after))
        (line.kind == DiffLine::Same ? same : line.kind == DiffLine::Added ? added : removed)++;
    CHECK(same + removed == before.size());
    CHECK(same + added == after.size());
}

void changeCallback()
{
    LuaTrustStore s;
    int calls = 0;
    s.setChangedCallback([&calls]() { ++calls; });
    s.approve("a.lua", "v1", LuaTrust::Trusted);
    s.guiSaved("a.lua", "v1", "a.lua", "v2", false);
    s.remove("a.lua");
    s.setMigrated();
    CHECK(calls == 4);
    s.fromKeyValue("L4R_TRUST_MIGRATED", "1"); // loading must not mark the config dirty
    CHECK(calls == 4);
}
} // namespace

int main()
{
    unknownScriptsAreSandboxed();
    hashRule();
    guiCreateIsTrusted();
    guiEditOfApprovedKeepsLevel();
    guiSaveDoesNotApproveForeignChanges();
    renameMovesApproval();
    approvedCopyFollowsApproval();
    bundledScripts();
    editorRun();
    serialisationRoundTrip();
    malformedEntriesDoNotGrantTrust();
    approvedCodeIsVerified();
    lineDiffs();
    changeCallback();
    std::printf("%d checks, %d failed\n", checks, failures);
    return failures ? 1 : 0;
}
