/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>
           Michel Boyer de la Giroday <michel.giroday@kdab.com>
           Kevin Krammer <kevin.krammer@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RESOURCECONFIGDIALOG_H
#define RESOURCECONFIGDIALOG_H
#include "fatcrmprivate_export.h"
#include <QDialog>
#include <QSortFilterProxyModel>

namespace Akonadi
{
class AgentInstance;
}

class KJob;

class FATCRMPRIVATE_EXPORT ResourceConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ResourceConfigDialog(QWidget *parent = Q_NULLPTR);

    ~ResourceConfigDialog() override;

Q_SIGNALS:
    void resourceSelected(const Akonadi::AgentInstance &resource);

public Q_SLOTS:
    void resourceSelectionChanged(const Akonadi::AgentInstance &resource);

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void updateButtonStates())
    Q_PRIVATE_SLOT(d, void addResource())
    Q_PRIVATE_SLOT(d, void configureResource())
    Q_PRIVATE_SLOT(d, void syncResources())
    Q_PRIVATE_SLOT(d, void removeResource())
    Q_PRIVATE_SLOT(d, void resourceCreateResult(KJob *))
    Q_PRIVATE_SLOT(d, void applyResourceSelection())
    Q_PRIVATE_SLOT(d, void agentInstanceChanged(const Akonadi::AgentInstance &))
};


// Workaround for AgentFilterProxyModel not being able to filter on capabilities
// when mimetypes is empty, which was fixed in 5968a044321b (kdepimlibs v4.14.7)
class WorkaroundFilterProxyModel : public QSortFilterProxyModel
{
public:
    explicit WorkaroundFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
    {
        setDynamicSortFilter(true);
    }
    bool filterAcceptsRow(int source_row, const QModelIndex &) const override;
};

#endif
