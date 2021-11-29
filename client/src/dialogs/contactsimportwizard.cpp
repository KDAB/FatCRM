/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "kjobprogresstracker.h"

#include <Akonadi/ItemCreateJob>
#include <Akonadi/ItemModifyJob>
#include <KLocalizedString>

ContactsImportWizard::ContactsImportWizard(QWidget *parent)
    : QWizard(parent)
    , mAccountImportPage(new AccountImportPage)
    , mContactsImportPage(new ContactsImportPage)
{
    setAttribute(Qt::WA_DeleteOnClose);

    addPage(mAccountImportPage);
    addPage(mContactsImportPage);

    connect(mAccountImportPage, &AccountImportPage::chosenContactsAvailable,
            mContactsImportPage, &ContactsImportPage::setChosenContacts);
    connect(mAccountImportPage, &AccountImportPage::layoutChanged,
            this, &ContactsImportWizard::adaptWindowSize);
    connect(mContactsImportPage, &ContactsImportPage::importedItems,
            this, &ContactsImportWizard::importItems);
    connect(mContactsImportPage, &ContactsImportPage::layoutChanged,
            this, &ContactsImportWizard::adaptWindowSize);

    setWindowTitle(i18n("Import Contacts"));
    resize(800, 600); // use a sensible size which fits all sub pages
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

void ContactsImportWizard::adaptWindowSize()
{
    resize(qMax(mAccountImportPage->width(), mContactsImportPage->width()) + 40, height());
}

void ContactsImportWizard::importItems(const QVector<Akonadi::Item> &items)
{
    if (items.isEmpty())
        return;

    setAttribute(Qt::WA_DeleteOnClose, false);

    auto *tracker = new KJobProgressTracker(this, this);
    tracker->setCaption(i18n("Import Contacts"));
    tracker->setLabel(i18n("Importing contacts, please wait..."));
    connect(tracker, &KJobProgressTracker::finished, this, &QObject::deleteLater);

    foreach (const Akonadi::Item &item, items) {
        Q_ASSERT(item.hasPayload<KContacts::Addressee>());
        const KContacts::Addressee contact = item.payload<KContacts::Addressee>();
        if (item.isValid()) {
            auto *job = new Akonadi::ItemModifyJob(item, this);

            const QString errorMessage = i18n("Unable to update contact %1:", contact.realName());
            tracker->addJob(job, errorMessage);
        } else {
            auto *job = new Akonadi::ItemCreateJob(item, mContactsCollection, this);

            const QString errorMessage = i18n("Unable to create contact %1:", contact.realName());
            if (mContactsImportPage->openContactsAfterImport()) {
                connect(job, &KJob::result, this, &ContactsImportWizard::slotContactCreated);
            }
            tracker->addJob(job, errorMessage);
        }
    }

    tracker->start();
}

void ContactsImportWizard::slotContactCreated(KJob *job)
{
    auto *createJob = static_cast<Akonadi::ItemCreateJob *>(job);
    if (!createJob->error()) {
        const Akonadi::Item item = createJob->item();
        // no remote id yet, can't use ItemDataExtractor
        emit openFutureContact(item.id());
    }
}
