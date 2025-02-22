/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore BVBA and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MU_DOCK_DOCKFRAMEMODEL_H
#define MU_DOCK_DOCKFRAMEMODEL_H

#include <QQuickItem>

#include "modularity/ioc.h"
#include "actions/iactionsdispatcher.h"

namespace mu::dock {
class DockFrameModel : public QObject
{
    Q_OBJECT

    INJECT(dock, actions::IActionsDispatcher, dispatcher)

    Q_PROPERTY(QQuickItem * frame READ frame WRITE setFrame NOTIFY frameChanged)
    Q_PROPERTY(QVariantList tabs READ tabs NOTIFY tabsChanged)

    Q_PROPERTY(bool titleBarVisible READ titleBarVisible NOTIFY titleBarVisibleChanged)
    Q_PROPERTY(QObject * navigationSection READ navigationSection NOTIFY navigationSectionChanged)
    Q_PROPERTY(QString currentDockUniqueName READ currentDockUniqueName NOTIFY currentDockChanged)
    Q_PROPERTY(QVariant currentDockContextMenuModel READ currentDockContextMenuModel NOTIFY currentDockChanged)

public:
    explicit DockFrameModel(QObject* parent = nullptr);

    QQuickItem* frame() const;
    QVariantList tabs() const;

    bool titleBarVisible() const;
    QObject* navigationSection() const;
    QString currentDockUniqueName() const;
    QVariant currentDockContextMenuModel() const;

    Q_INVOKABLE void handleMenuItem(const QVariant& item);

public slots:
    void setFrame(QQuickItem* item);

signals:
    void frameChanged(QQuickItem* frame);
    void tabsChanged();
    void titleBarVisibleChanged(bool visible);
    void navigationSectionChanged();
    void currentDockChanged();

private:
    bool eventFilter(QObject* watched, QEvent* event);

    void listenChangesInFrame();
    void setTitleBarVisible(bool visible);

    const QObject* currentDockObject() const;
    QVariant currentDockProperty(const char* propertyName) const;

    QObject* currentNavigationSection() const;
    void updateNavigationSection();

    QQuickItem* m_frame = nullptr;
    bool m_titleBarVisible = false;
    QObject* m_navigationSection = nullptr;
};
}

#endif // MU_DOCK_DOCKFRAMEMODEL_H
