#pragma once
#include <vector>
#include <QCoreApplication>
#include <QTreeWidget>
#include <QMap>
#include "JsonApiCatalog.h"
#include "Lua/LuaBindings.h"

namespace FunctionBrowser {
inline void addJsonEndpoints(QTreeWidget* tree)
{
    auto root = new QTreeWidgetItem(tree, {QObject::tr("JSON API endpoints")});
    root->setToolTip(0, QObject::tr("Catalogue generated from the RetroShare source build, not live server discovery. Hover for input types and response fields. Define input variables before running an inserted call."));
    QMap<QString, QTreeWidgetItem*> services;
    for(const auto& entry : jsonApiCatalog)
    {
        const QString path = QString::fromUtf8(entry.path);
        const QString service = path.section('/', 1, 1);
        if(!services.contains(service)) services[service] = new QTreeWidgetItem(root, {service});
        auto item = new QTreeWidgetItem(services[service], {path.section('/', 2)});
        item->setText(1, QString::fromUtf8(entry.snippet));
        item->setToolTip(0, path + "\n" + QString::fromUtf8(entry.hint));
        if(item->text(1).isEmpty()) item->setText(0, item->text(0) + QObject::tr(" [streaming — unavailable in GUI]"));
    }
    root->setExpanded(true);
}

// Column 0: label, column 1 (hidden): text pasted into the editor.
inline void addJsonHelpers(QTreeWidget* tree)
{
    QTreeWidgetItem* json = new QTreeWidgetItem(tree);
    json->setText(0, "JSON API client helpers");
    json->setToolTip(0, QObject::tr("Connection and JSON utilities. subscribe is standalone-only; use Automation triggers in the GUI."));
    const QStringList jsonFunctions = {
        "configure({url = \"http://127.0.0.1:9092\", username = \"YOUR_USER\", password = \"YOUR_PASSWORD\"})", "call(\"/rsPeers/getFriendList\", {})",
        "encode({})", "decode(\"{}\")", "array()", "null"
    };
    for(const QString& function : jsonFunctions)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(json);
        item->setText(0, function);
        item->setText(1, "jsonapi." + function);
        item->setToolTip(0, QObject::tr("RetroShare JSON API client. See JSON_API.md for connection settings and examples."));
    }
}

inline void addNativeBindings(QTreeWidget* tree, const std::vector<LuaBinding>& bindings)
{
    auto native = new QTreeWidgetItem(tree, {QObject::tr("Native Lua helpers (GUI only)")});
    native->setToolTip(0, QObject::tr("Direct plugin bindings; no JSON API connection required. Replace placeholder arguments before running."));
    QMap<QString, QTreeWidgetItem*> namespaces;
    for(const LuaBinding& binding : bindings)
    {
        const QString ns = QString::fromUtf8(binding.ns) + ".";
        if(!namespaces.contains(ns))
        {
            namespaces[ns] = new QTreeWidgetItem(native);
            namespaces[ns]->setText(0, ns);
            namespaces[ns]->setText(1, ns);
        }
        auto item = new QTreeWidgetItem(namespaces[ns]);
        const QString signature = QString::fromUtf8(binding.signature);
        item->setText(0, signature);
        item->setText(1, ns + signature);
        item->setToolTip(0, (binding.access == LuaAccess::Write ? QObject::tr("[write] ") : QObject::tr("[read] ")) +
                            QCoreApplication::translate("LuaBindings", binding.doc));
    }
}

inline void setup(QTreeWidget* tree, const std::vector<LuaBinding>& bindings)
{
    tree->setColumnCount(2);
    // after setColumnCount: no headers, wide label column, paste column needs no width
    tree->setHeaderHidden(true);
    tree->setColumnWidth(0, 250);
    tree->setColumnWidth(1, 0);
    addJsonEndpoints(tree);
    addJsonHelpers(tree);
    addNativeBindings(tree, bindings);
}

inline bool filter(QTreeWidgetItem* item, const QString& query, bool ancestorMatches = false)
{
    const bool match = ancestorMatches || item->text(0).contains(query, Qt::CaseInsensitive)
            || (item->childCount() == 0 && (item->text(1).contains(query, Qt::CaseInsensitive)
                || item->toolTip(0).contains(query, Qt::CaseInsensitive)));
    bool visible = match;
    for(int i = 0; i < item->childCount(); ++i)
        visible = filter(item->child(i), query, match) || visible;
    item->setHidden(!visible);
    if(!query.isEmpty() && visible && item->childCount()) item->setExpanded(true);
    return visible;
}
}
