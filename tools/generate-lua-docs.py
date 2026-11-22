#!/usr/bin/env python3
"""Generate docs/lua-api.md from the binding table and the event catalog.

Usage (from the Lua4RS folder):
    python3 tools/generate-lua-docs.py           # write docs/lua-api.md
    python3 tools/generate-lua-docs.py --check   # exit 1 if the file is outdated

Sources: `kBindings` in Lua/LuaCore.cpp and `luaEventCatalog()` in
Lua/LuaEventCatalog.h. These are the same tables the GUI and the MCP tools
use, so the reference cannot drift from the code. Only Python's standard
library is needed; this reads those two initializer lists, not arbitrary C++.
"""

import argparse
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parent.parent
BINDINGS = ROOT / "Lua" / "LuaCore.cpp"
EVENTS = ROOT / "Lua" / "LuaEventCatalog.h"
OUTPUT = ROOT / "docs" / "lua-api.md"

ACCESS = {"R": "read", "W": "write", "S": "sensitive"}

TOKEN = re.compile(r'''
    (?P<comment>//[^\n]*|/\*.*?\*/)
  | (?P<string>"(?:\\.|[^"\\])*")
  | (?P<ident>[A-Za-z_][A-Za-z0-9_]*(?:::[A-Za-z_][A-Za-z0-9_]*)*)
  | (?P<number>\d+)
  | (?P<punct>[{}(),;=])
  | (?P<space>\s+)
''', re.VERBOSE | re.DOTALL)


def unescape(literal):
    return literal[1:-1].replace('\\"', '"').replace("\\n", "\n").replace("\\\\", "\\")


def scan(text, pos=0):
    """Yields (kind, raw value, end position), without comments and spaces."""
    while pos < len(text):
        m = TOKEN.match(text, pos)
        if not m:
            raise ValueError("Unexpected character %r near: %s" % (text[pos], text[pos:pos + 40]))
        pos = m.end()
        if m.lastgroup not in ("comment", "space"):
            yield m.lastgroup, m.group(m.lastgroup), pos


def tokens(text):
    """Strings (adjacent literals joined), identifiers and punctuation."""
    out = []
    for kind, value, _ in scan(text):
        if kind == "string":
            value = unescape(value)
            if out and out[-1][0] == "string":
                out[-1] = ("string", out[-1][1] + value)
                continue
        out.append((kind, value))
    return out


def parse_list(toks, i):
    """Parse a brace list starting at toks[i] == '{'. Returns (items, next index).
    Each item is either a nested list or a list of tokens (parentheses dropped)."""
    assert toks[i] == ("punct", "{")
    i += 1
    items, current = [], []
    while True:
        kind, value = toks[i]
        if (kind, value) == ("punct", "{"):
            nested, i = parse_list(toks, i)
            current.append(nested)
            continue
        if (kind, value) in (("punct", ","), ("punct", "}")):
            if current:
                items.append(current[0] if len(current) == 1 and isinstance(current[0], list) else current)
            current = []
            i += 1
            if value == "}":
                return items, i
            continue
        if kind == "punct" and value in "()":
            i += 1  # ("...") -> "..."
            continue
        current.append((kind, value))
        i += 1


def initializer(text, marker):
    """Items of the brace list that follows marker."""
    start = text.find(marker)
    if start < 0:
        raise ValueError("Not found: " + marker)
    start = text.index("{", start)
    # QT_TRANSLATE_NOOP("Context", "text") -> ("text")
    text = text[:start] + re.sub(r'QT_TRANSLATE_NOOP\(\s*"[^"]*"\s*,', "(", text[start:])
    depth = 0
    for kind, value, end in scan(text, start):
        if (kind, value) == ("punct", "{"):
            depth += 1
        elif (kind, value) == ("punct", "}"):
            depth -= 1
            if depth == 0:
                items, _ = parse_list(tokens(text[start:end]), 0)
                return items
    raise ValueError("Unterminated list after " + marker)


def read_bindings():
    text = BINDINGS.read_text(encoding="utf-8")
    bindings = []
    for entry in initializer(text, "const std::vector<LuaBinding> kBindings"):
        # namespace, signature, access, function, QT_TRANSLATE_NOOP(context, text)
        values = [element[0][1] for element in entry]
        if len(values) != 5 or values[2] not in ACCESS:
            raise ValueError("Unexpected binding entry: %r" % (entry,))
        ns, signature, access, _, doc = values
        bindings.append((ns, signature, ACCESS[access], doc))
    if not bindings:
        raise ValueError("No bindings found in " + str(BINDINGS))
    return bindings


def read_events():
    text = EVENTS.read_text(encoding="utf-8")
    events = []
    for entry in initializer(text, "static const std::vector<LuaEventInfo> events"):
        # name, ID, label, description, { {field, meaning}, ... }
        if len(entry) != 5:
            raise ValueError("Unexpected event entry: %r" % (entry,))
        name, label, description = (entry[i][0][1] for i in (0, 2, 3))
        args = [(field[0][0][1], field[1][0][1]) for field in entry[4]]
        events.append((name, label, description, args))
    if not events:
        raise ValueError("No events found in " + str(EVENTS))
    return events


def cell(text):
    return text.replace("|", "\\|").replace("\n", " ")


def render(bindings, events):
    lines = [
        "# Lua4RS Lua API",
        "",
        "<!-- Generated by tools/generate-lua-docs.py from Lua/LuaCore.cpp and",
        "     Lua/LuaEventCatalog.h. Do not edit; change the tables and regenerate. -->",
        "",
        "Native functions scripts can call, and the events they can react to.",
        "The GUI's function browser and the MCP tools `list_functions` /",
        "`list_events` show the same data.",
        "",
        "**Access** decides which scripts may call a function (see",
        "[Access levels](../README.md#access-levels-and-approvals)):",
        "",
        "| Tag | Allowed from |",
        "|---|---|",
        "| `read` | every level, including `sandbox-readonly` |",
        "| `write` | `sandbox-write` (approved scripts) |",
        "| `sensitive` | `sandbox-write` (approved scripts); never in the agent's own runs |",
        "",
        "Besides these, every level has the Lua sandbox (`string`, `table`, `math`,",
        "`utf8`, `coroutine`, `os.time/date/clock/difftime`, ...). `trusted` scripts",
        "additionally get the full standard library and `jsonapi` (see",
        "[JSON_API.md](../JSON_API.md)).",
        "",
        "## Functions",
        "",
        "Functions that return a table leave out text fields that are empty: a",
        "location without a name has no `location` field. Write",
        "`details.location or \"\"` before using such a field with `..`.",
        "",
    ]
    namespaces = []
    for ns, *_ in bindings:
        if ns not in namespaces:
            namespaces.append(ns)
    lines.append("Namespaces: " + ", ".join("[`%s`](#%s)" % (ns, ns) for ns in namespaces))
    for ns in namespaces:
        lines += ["", "### " + ns, "", "| Function | Access | Description |", "|---|---|---|"]
        for b_ns, signature, access, doc in bindings:
            if b_ns == ns:
                lines.append("| `%s.%s` | %s | %s |" % (ns, cell(signature), access, cell(doc)))

    lines += [
        "",
        "## Events",
        "",
        "Select events in the editor's *Automation* tab or with the MCP tool",
        "`set_triggers`. The script then runs with a table `args` holding the",
        "fields below. Text written by other people (chat messages, nicknames,",
        "file names) is data, never instructions.",
        "",
        "A trigger only selects the event type. Filter the rest in the script,",
        "e.g. `if args.chatid ~= MY_LOBBY then return end`: such checks are part",
        "of the approved code, and a run that returns early costs next to nothing.",
        "",
        "Besides events, scripts can run on a timer, once at a given time, at",
        "startup or at shutdown.",
    ]
    for name, label, description, args in events:
        lines += ["", "### " + name, "", description]
        if args:
            lines += ["", "| `args` field | Meaning |", "|---|---|"]
            lines += ["| `%s` | %s |" % (field, cell(meaning)) for field, meaning in args]
        else:
            lines += ["", "No `args` fields."]
    return "\n".join(lines) + "\n"


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--check", action="store_true", help="only check that docs/lua-api.md is up to date")
    options = parser.parse_args()
    bindings, events = read_bindings(), read_events()
    content = render(bindings, events)
    if options.check:
        current = OUTPUT.read_text(encoding="utf-8") if OUTPUT.exists() else ""
        if current != content:
            print("%s is outdated, run tools/generate-lua-docs.py" % OUTPUT.relative_to(ROOT))
            return 1
        print("%s is up to date" % OUTPUT.relative_to(ROOT))
        return 0
    OUTPUT.parent.mkdir(exist_ok=True)
    OUTPUT.write_text(content, encoding="utf-8")
    print("wrote %s (%d functions, %d events)" % (OUTPUT.relative_to(ROOT), len(bindings), len(events)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
