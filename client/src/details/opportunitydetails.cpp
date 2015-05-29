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

#include "opportunitydetails.h"

#include "ui_opportunitydetails.h"
#include "referenceddatamodel.h"
#include "kdcrmutils.h"
#include "notesrepository.h"

#include <kdcrmdata/sugaropportunity.h>
#include <notesdialog.h>
#include <KLocale>

OpportunityDetails::OpportunityDetails(QWidget *parent)
    : Details(Opportunity, parent), mUi(new Ui::OpportunityDetails)
{
    mUi->setupUi(this);
    mUi->urllabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    mUi->next_call_date->calendarWidget()->setVerticalHeaderFormat(QCalendarWidget::ISOWeekNumbers);
    mUi->next_call_date->setNullable(true);
    mUi->date_closed->calendarWidget()->setVerticalHeaderFormat(QCalendarWidget::ISOWeekNumbers);
    mUi->date_closed->setNullable(true);
    initialize();
}

OpportunityDetails::~OpportunityDetails()
{
    delete mUi;
}

void OpportunityDetails::initialize()
{
    ReferencedDataModel::setModelForCombo(mUi->account_name, AccountRef);
    mUi->opportunity_type->addItems(typeItems());
    mUi->lead_source->addItems(sourceItems());
    mUi->sales_stage->addItems(stageItems());
    ReferencedDataModel::setModelForCombo(mUi->assigned_user_name, AssignedToRef);
    connect(mUi->nextStepDateAutoButton, SIGNAL(clicked()), this, SLOT(slotAutoNextStepDate()));
}

void OpportunityDetails::slotAutoNextStepDate()
{
    mUi->next_call_date->setDate(QDate::currentDate().addDays(14));
}

QStringList OpportunityDetails::typeItems() const
{
    QStringList types;
    types << QString("") << QString("Existing Business")
          << QString("New Business");
    return types;
}

QStringList OpportunityDetails::stageItems() const
{
    QStringList stages;
    stages << QString("")
           << QString("Prospecting")
           << QString("Qualification")
           << QString("Needs Analysis")
           << QString("Value Proposition")
           << QString("Id.Decision Makers")
           << QString("Perception Analysis")
           << QString("Proposal/Price Quote")
           << QString("Negociation/Review")
           << QString("Closed Won")
           << QString("Closed Lost");
    return stages;
}

QMap<QString, QString> OpportunityDetails::data(const Akonadi::Item &item) const
{
    SugarOpportunity opportunity = item.payload<SugarOpportunity>();
    return opportunity.data();
}

void OpportunityDetails::updateItem(Akonadi::Item &item, const QMap<QString, QString> &data) const
{
    SugarOpportunity opportunity;
    if (item.hasPayload<SugarOpportunity>()) {
        opportunity = item.payload<SugarOpportunity>();
    }
    opportunity.setData(data);

    item.setMimeType(SugarOpportunity::mimeType());
    item.setPayload<SugarOpportunity>(opportunity);
}

void OpportunityDetails::setDataInternal(const QMap<QString, QString> &) const
{
    const QString baseUrl = resourceBaseUrl();
    const QString oppId = id();
    if (!baseUrl.isEmpty() && !oppId.isEmpty()) {
        const QString url = baseUrl + "?action=DetailView&module=Opportunities&record=" + oppId;
        mUi->urllabel->setText(QString("<a href=\"%1\">Open Opportunity in Web Browser</a>").arg(url));
    } else {
        mUi->urllabel->clear();
    }

    const int notes = oppId.isEmpty() ? 0 : mNotesRepository->notesForOpportunity(oppId).count() + mNotesRepository->emailsForOpportunity(oppId).count();
    mUi->viewNotesButton->setEnabled(notes > 0);
    const QString buttonText = (notes == 0) ? i18n("View Notes") : i18np("View 1 Note", "View %1 Notes", notes);
    mUi->viewNotesButton->setText(buttonText);
}

void OpportunityDetails::on_viewNotesButton_clicked()
{
    const QString oppId = id();
    const QVector<SugarNote> notes = mNotesRepository->notesForOpportunity(oppId);
    kDebug() << notes.count() << "notes found for opp" << oppId;
    const QVector<SugarEmail> emails = mNotesRepository->emailsForOpportunity(oppId);
    kDebug() << emails.count() << "emails found for opp" << oppId;
    NotesDialog *dlg = new NotesDialog(this);
    dlg->setWindowTitle(i18n("Notes for opportunity %1", property("name").toString()));
    foreach(const SugarNote &note, notes) {
        dlg->addNote(note);
    }
    foreach(const SugarEmail &email, emails) {
        dlg->addEmail(email);
    }
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}
