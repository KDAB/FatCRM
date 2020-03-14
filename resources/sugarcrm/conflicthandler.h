/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

// temporary "copy" from kdepimlibs/akonadi/conflicthandling

#ifndef CONFLICTHANDLER_H
#define CONFLICTHANDLER_H

#include <QObject>
#include <qwindowdefs.h>

class KJob;

namespace Akonadi
{
class DifferencesAlgorithmInterface;
class Item;
}

class ConflictHandler : public QObject
{
    Q_OBJECT

public:
    /**
     * Describes the type of conflict that should be resolved by
     * the conflict handler.
     */
    enum ConflictType {
        LocalLocalConflict,   ///< Changes of two Akonadi client applications conflict.
        LocalRemoteConflict,  ///< Changes of an Akonadi client application and a resource conflict.
        BackendConflict       ///< Changes of a resource and the backend data conflict.
    };

    /**
     * Describes the strategy that should be used for resolving the conflict.
     */
    enum ResolveStrategy {
        UseLocalItem, ///< The local item overwrites the other item inside the Akonadi storage.
        UseOtherItem, ///< The local item is dropped and the other item from the Akonadi storage is used.
        UseBothItems  ///< Both items are kept in the Akonadi storage.
    };

    /**
     * Creates a new conflict handler.
     *
     * @param type The type of the conflict that should be resolved.
     * @param parent The parent object.
     */
    explicit ConflictHandler(ConflictType type, QObject *parent = nullptr);

    ~ConflictHandler() override;

    /**
     * Sets the items that causes the conflict.
     *
     * @param changedItem The item that has been changed, it needs the complete payload set.
     * @param conflictingItem The item from the Akonadi storage that is conflicting.
     *                        This needs only the id set, the payload will be refetched automatically.
     */
    void setConflictingItems(const Akonadi::Item &changedItem, const Akonadi::Item &conflictingItem);

    void setDifferencesInterface(Akonadi::DifferencesAlgorithmInterface *interface);

    void setParentName(const QString &name);
    void setParentWindowId(WId windowId);

public Q_SLOTS:
    /**
     * Starts the conflict handling.
     */
    void start();

Q_SIGNALS:
    /**
     * This signal is emitted whenever the conflict has been resolved
     * automatically or by the user.
     */
    void conflictResolved();

    /**
     * This signal is emitted whenever an error occurred during the conflict
     * handling.
     *
     * @param message A user visible string that describes the error.
     */
    void error(const QString &message);

    void commitChange(const Akonadi::Item &item);

    void updateOnBackend(const Akonadi::Item &item);

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void duplicateLocalItemResult(KJob *))
};

#endif
