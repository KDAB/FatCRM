/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef OPPORTUNITYDETAILS_H
#define OPPORTUNITYDETAILS_H

#include "details.h"

namespace Ui
{
class OpportunityDetails;
}
class NotesRepository;

class OpportunityDetails : public Details
{
    Q_OBJECT
public:
    explicit OpportunityDetails(QWidget *parent = 0);

    ~OpportunityDetails();

    void setNotesRepository(NotesRepository *notesRepo) Q_DECL_OVERRIDE { mNotesRepository = notesRepo; }

private Q_SLOTS:
    void slotAutoNextStepDate();

    void on_viewNotesButton_clicked();

private:
    /*reimp*/ void initialize();
    /*reimp*/ QMap<QString, QString> data(const Akonadi::Item &item) const;
    /*reimp*/ void updateItem(Akonadi::Item &item, const QMap<QString, QString> &data) const;
    /*reimp*/ void setDataInternal(const QMap<QString, QString> &data) const;

    QStringList typeItems() const;
    QStringList stageItems() const;

private:
    Ui::OpportunityDetails *mUi;
    NotesRepository *mNotesRepository;
};

#endif /* OPPORTUNITYDETAILS_H */

