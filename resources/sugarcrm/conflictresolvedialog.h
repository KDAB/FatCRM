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

#ifndef CONFLICTRESOLVEDIALOG_H
#define CONFLICTRESOLVEDIALOG_H

#include "conflicthandler.h"

#include <QDialog>

class ConflictResolveDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * Creates a new conflict resolve dialog.
     *
     * @param parent The parent widget.
     */
    explicit ConflictResolveDialog(QWidget *parent = nullptr);

    ~ConflictResolveDialog() override;

    /**
     * Sets the items that causes the conflict.
     *
     * @param localItem The local item which causes the conflict.
     * @param otherItem The conflicting item from the Akonadi storage.
     *
     * @note Both items need the full payload set.
     */
    void setConflictingItems(const Akonadi::Item &localItem, const Akonadi::Item &otherItem);

    void setDifferencesInterface(Akonadi::DifferencesAlgorithmInterface *interface);

    /**
     * Returns the resolve strategy the user choose.
     */
    ConflictHandler::ResolveStrategy resolveStrategy() const;

    void reject() override;

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void useLocalItem())
    Q_PRIVATE_SLOT(d, void useOtherItem())
    Q_PRIVATE_SLOT(d, void useBothItems())
    Q_PRIVATE_SLOT(d, void openEditor())
    Q_PRIVATE_SLOT(d, void createReport())
};

#endif
