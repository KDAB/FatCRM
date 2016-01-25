/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Tobias Koenig <tobias.koenig@kdab.com>

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

#include "contactsimportwizard.h"

#include "accountimportpage.h"
#include "contactsimportpage.h"

#include <Akonadi/ItemCreateJob>
#include <Akonadi/ItemModifyJob>
#include <KLocale>
#include <KMessageBox>

#include <QProgressDialog>

ContactsImportWizard::ContactsImportWizard(QWidget *parent)
    : QWizard(parent)
    , mAccountImportPage(new AccountImportPage)
    , mContactsImportPage(new ContactsImportPage)
{
    setAttribute(Qt::WA_DeleteOnClose);

    addPage(mAccountImportPage);
    addPage(mContactsImportPage);

    connect(mAccountImportPage, SIGNAL(chosenContactsAvailable(QVector<ContactsSet>)),
            mContactsImportPage, SLOT(setChosenContacts(QVector<ContactsSet>)));
    connect(mContactsImportPage, SIGNAL(importedItems(QVector<Akonadi::Item>)),
            this, SLOT(importItems(QVector<Akonadi::Item>)));

    setWindowTitle(i18n("Import Contacts"));
}

void ContactsImportWizard::setAccountCollection(const Akonadi::Collection &collection)
{
    mAccountImportPage->setAccountCollection(collection);
}

void ContactsImportWizard::setContactsCollection(const Akonadi::Collection &collection)
{
    mContactsCollection = collection;
}

void ContactsImportWizard::setImportedContacts(const QVector<ContactsSet> &contacts)
{
    mAccountImportPage->setImportedContacts(contacts);
}

void ContactsImportWizard::setContactsModel(ItemsTreeModel *model)
{
    mContactsImportPage->setContactsModel(model);
}

void ContactsImportWizard::accept()
{
    QWizard::accept();
}

void ContactsImportWizard::reject()
{
    mAccountImportPage->cleanup();
    mContactsImportPage->cleanup();
    QWizard::reject();
}

void ContactsImportWizard::importItems(const QVector<Akonadi::Item> &items)
{
    setAttribute(Qt::WA_DeleteOnClose, false);

    foreach (const Akonadi::Item &item, items) {
        if (item.isValid()) {
            Akonadi::ItemModifyJob *job = new Akonadi::ItemModifyJob(item, this);
            connect(job, SIGNAL(result(KJob*)), SLOT(jobFinished(KJob*)));
            mPendingJobs << job;
        } else {
            Akonadi::ItemCreateJob *job = new Akonadi::ItemCreateJob(item, mContactsCollection, this);
            connect(job, SIGNAL(result(KJob*)), SLOT(jobFinished(KJob*)));
            mPendingJobs << job;
        }
    }

    mProgressDialog = new QProgressDialog(this);
    mProgressDialog->setLabelText(i18n("Importing contacts..."));
    mProgressDialog->setRange(0, items.count());
    mProgressDialog->setValue(0);
}

void ContactsImportWizard::jobFinished(KJob *job)
{
    mPendingJobs.remove(mPendingJobs.indexOf(job));
    mProgressDialog->setValue(mProgressDialog->value() + 1);

    if (job->error()) {
        QString errorMessage;

        const Akonadi::ItemModifyJob *modifyJob = qobject_cast<Akonadi::ItemModifyJob*>(job);
        if (modifyJob) {
            const KABC::Addressee contact = modifyJob->item().payload<KABC::Addressee>();
            errorMessage = i18n("Unable to update contact %1: %2", contact.realName(), job->errorString());
        } else {
            const Akonadi::ItemCreateJob *createJob = qobject_cast<Akonadi::ItemCreateJob*>(job);
            if (createJob) {
                const KABC::Addressee contact = createJob->item().payload<KABC::Addressee>();
                errorMessage = i18n("Unable to create contact %1: %2", contact.realName(), job->errorString());
            }
        }

        KMessageBox::error(0, errorMessage);
    }

    if (mPendingJobs.isEmpty())
        deleteLater();
}
