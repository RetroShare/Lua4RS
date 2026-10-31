#pragma once
#include <QTreeWidget>
#include <QMap>
#include "JsonApiCatalog.h"

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
