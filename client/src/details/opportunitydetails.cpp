/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "enums.h"
#include "modelrepository.h"
#include "noteswindow.h"
#include "notesrepository.h"
#include "opportunitydataextractor.h"
#include "referenceddatamodel.h"
#include "selectitemdialog.h"

#include "kdcrmdata/kdcrmutils.h"
#include "kdcrmdata/kdcrmfields.h"
#include "kdcrmdata/sugaropportunity.h"

#include <KLocale>

#include <QCalendarWidget>

OpportunityDetails::OpportunityDetails(QWidget *parent)
    : Details(Opportunity, parent), mUi(new Ui::OpportunityDetails), mDataExtractor(new OpportunityDataExtractor(this))
{
    mUi->setupUi(this);

    mUi->urllabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    mUi->next_call_date->calendarWidget()->setVerticalHeaderFormat(QCalendarWidget::ISOWeekNumbers);
    mUi->next_call_date->setNullable(true);
    mUi->date_closed->calendarWidget()->setVerticalHeaderFormat(QCalendarWidget::ISOWeekNumbers);

    mUi->name->setObjectName(KDCRMFields::name());
    mUi->account_id->setObjectName(KDCRMFields::accountId());
    mUi->opportunity_type->setObjectName(KDCRMFields::opportunityType());
    mUi->lead_source->setObjectName(KDCRMFields::leadSource());
    mUi->date_closed->setObjectName(KDCRMFields::dateClosed());
    mUi->assigned_user_id->setObjectName(KDCRMFields::assignedUserId());
    mUi->amount->setObjectName(KDCRMFields::amount());
    mUi->next_step->setObjectName(KDCRMFields::nextStep());
    mUi->next_call_date->setObjectName(KDCRMFields::nextCallDate());
    mUi->sales_stage->setObjectName(KDCRMFields::salesStage());
    mUi->probability->setObjectName(KDCRMFields::probability());
    mUi->opportunityPriority->setObjectName(KDCRMFields::opportunityPriority());

    initialize();
}

OpportunityDetails::~OpportunityDetails()
{
    delete mUi;
}

void OpportunityDetails::initialize()
{
    ReferencedDataModel::setModelForCombo(mUi->account_id, AccountRef);
    ReferencedDataModel::setModelForCombo(mUi->assigned_user_id, AssignedToRef);
    connect(mUi->buttonSelectAccount, SIGNAL(clicked()), this, SLOT(slotSelectAccount()));
    connect(mUi->nextStepDateAutoButton, SIGNAL(clicked()), this, SLOT(slotAutoNextStepDate()));
    connect(mUi->sales_stage, SIGNAL(activated(QString)),
            this, SLOT(slotSalesStageActivated(QString)));
}

ItemDataExtractor *OpportunityDetails::itemDataExtractor() const
{
    return mDataExtractor;
}

void OpportunityDetails::slotAutoNextStepDate()
{
    mUi->next_call_date->setDate(QDate::currentDate().addDays(14));
}

void OpportunityDetails::slotSalesStageActivated(const QString &stage)
{
    int percent = 50;
    if (stage == "Prospecting")
        percent = 10;
    else if (stage == "Proposal/Price Quote")
        percent = 65;
    else if (stage == "Negotiation/Review")
        percent = 80;
    else if (stage == "Closed Won")
        percent = 100;
    else if (stage == "Closed Lost")
        percent = 0;
    mUi->probability->setValue(percent);
}

QMap<QString, QString> OpportunityDetails::data(const Akonadi::Item &item) const
{
    Q_ASSERT(item.isValid());
    Q_ASSERT(item.hasPayload<SugarOpportunity>());
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

void OpportunityDetails::setDataInternal(const QMap<QString, QString> &data)
{
    fillComboBox(mUi->opportunity_type, KDCRMFields::opportunityType());
    fillComboBox(mUi->lead_source, KDCRMFields::leadSource());
    fillComboBox(mUi->sales_stage, KDCRMFields::salesStage());
    fillComboBox(mUi->opportunityPriority, KDCRMFields::opportunityPriority());

    const QUrl url = itemDataExtractor()->itemUrl(resourceBaseUrl(), id());
    if (url.isValid())
        mUi->urllabel->setText(QString("<a href=\"%1\">Open Opportunity in Web Browser</a>").arg(url.toString()));
    else
        mUi->urllabel->clear();

    const QString oppId = id();
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
    NotesWindow *dlg = new NotesWindow(0);
    dlg->setWindowTitle(i18n("Notes for opportunity %1", name()));
    foreach(const SugarNote &note, notes) {
        dlg->addNote(note);
    }
    foreach(const SugarEmail &email, emails) {
        dlg->addEmail(email);
    }
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}

void OpportunityDetails::on_buttonOpenAccount_clicked()
{
    const QString accountId = currentAccountId();
    emit openObject(Account, accountId);
}

void OpportunityDetails::slotSelectAccount()
{
    SelectItemDialog *dlg = new SelectItemDialog(Account, this);
    dlg->setModel(ModelRepository::instance()->model(Account));
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    connect(dlg, SIGNAL(selectedItem(QString)), this, SLOT(slotAccountSelected(QString)));
    dlg->show();
}

void OpportunityDetails::slotAccountSelected(const QString &accountId)
{
    const int idx = mUi->account_id->findData(accountId);
    if (idx > -1) {
        mUi->account_id->setCurrentIndex(idx);
    }
}
