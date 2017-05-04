/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>

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

#ifndef REFERENCEUPDATEJOB_H
#define REFERENCEUPDATEJOB_H

#include <KCompositeJob>
#include <AkonadiCore/Collection>
#include <AkonadiCore/Item>

typedef std::function<bool(Akonadi::Item &)> ReferenceUpdateFunction;

/**
 * @brief The reference update job goes through a collection and changes
 * all references from A to B. Example: resolving the account name to an
 * account id in all opportunities loaded before the corresponding account.
 */
class ReferenceUpdateJob : public KCompositeJob
{
    Q_OBJECT
public:
    explicit ReferenceUpdateJob(const Akonadi::Collection &collection, ReferenceUpdateFunction updateItem, QObject *parent = 0);

    void start() override;

signals:

private slots:
    void slotItemsReceived(const Akonadi::Item::List &items);
    void slotResult( KJob *job ) override;

private:
    Akonadi::Collection mCollection;
    ReferenceUpdateFunction mUpdateFunction;
};

#endif // REFERENCEUPDATEJOB_H

