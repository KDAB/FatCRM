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

#include "filterproxymodel.h"
#include "itemstreemodel.h"
#include "linkeditemsrepository.h"

#include "kdcrmdata/sugaraccount.h"
#include "kdcrmdata/sugarcampaign.h"
#include "kdcrmdata/sugarlead.h"
#include "kdcrmutils.h"

#include <KContacts/Addressee>
#include <KContacts/PhoneNumber>

#include <AkonadiCore/EntityTreeModel>

#include <KLocalizedString>
#include <QCoreApplication>
#include <QFile>

static bool accountMatchesFilter(const SugarAccount &account,
                                 const QString &filterString);
static bool campaignMatchesFilter(const SugarCampaign &campaign,
                                  const QString &filterString);
static bool contactMatchesFilter(const KContacts::Addressee &addressee,
                                 const QString &filterString);
static bool leadMatchesFilter(const SugarLead &lead,
                              const QString &filterString);

using namespace Akonadi;

class FilterProxyModel::Private
{
public:
    explicit Private(DetailsType type)
        : mType(type)
    {}
    DetailsType mType;
    QString mFilter;
    LinkedItemsRepository *mLinkedItemsRepository = nullptr;
    FilterProxyModel::Action mGDPRFilterAction = FilterProxyModel::NoAction;
    QStringList mProtectedEmails; // never touch those
};

FilterProxyModel::FilterProxyModel(DetailsType type, QObject *parent)
    : QSortFilterProxyModel(parent), d(new Private(type))
{
    // account names should be sorted correctly
    setSortLocaleAware(true);
    setDynamicSortFilter(true); // for sorting during insertion, too

    if (type == DetailsType::Contact) {
         // mailchimp export
        const QString filePath = QCoreApplication::applicationDirPath() + QLatin1String("/newsletter.txt");
        QFile f(filePath);
        if (f.open(QIODevice::ReadOnly)) {
            d->mProtectedEmails.reserve(918);
            while (!f.atEnd()) {
                QByteArray line = f.readLine();
                Q_ASSERT(line.endsWith('\n'));
                line.chop(1);
                d->mProtectedEmails << QString::fromLatin1(line);
            }
            qDebug() << "Read" << d->mProtectedEmails.count() << "protected emails from" << filePath;
        }
    }
}

FilterProxyModel::~FilterProxyModel()
{
    delete d;
}

void FilterProxyModel::setLinkedItemsRepository(LinkedItemsRepository *repo)
{
    d->mLinkedItemsRepository = repo;
}

void FilterProxyModel::setGDPRFilter(Action action)
{
    d->mGDPRFilterAction = action;
    invalidateFilter();
}

bool FilterProxyModel::hasGDPRProtectedEmails() const
{
    return !d->mProtectedEmails.isEmpty();
}

QString FilterProxyModel::filterString() const
{
    return d->mFilter;
}

QString FilterProxyModel::filterDescription() const
{
    if (!d->mFilter.isEmpty())
        return i18n("containing \"%1\"", d->mFilter);
    return QString();
}

void FilterProxyModel::setFilterString(const QString &filter)
{
    d->mFilter = filter;
    invalidateFilter();
}

bool FilterProxyModel::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    if (d->mFilter.isEmpty() && d->mGDPRFilterAction == NoAction) {
        return true;
    }
    const QModelIndex index = sourceModel()->index(row, 0, parent);
    const Akonadi::Item item =
        index.data(Akonadi::EntityTreeModel::ItemRole).value<Akonadi::Item>();

    switch (d->mType) {
    case DetailsType::Account: {
        Q_ASSERT(item.hasPayload<SugarAccount>());
        return d->mFilter.isEmpty() || accountMatchesFilter(item.payload<SugarAccount>(), d->mFilter);
    }
    case DetailsType::Campaign: {
        Q_ASSERT(item.hasPayload<SugarCampaign>());
        return d->mFilter.isEmpty() || campaignMatchesFilter(item.payload<SugarCampaign>(), d->mFilter);
    }
    case DetailsType::Contact: {
        Q_ASSERT(item.hasPayload<KContacts::Addressee>());
        const KContacts::Addressee contact = item.payload<KContacts::Addressee>();
        if (d->mGDPRFilterAction != NoAction) {
            const QString contactId = contact.custom(QStringLiteral("FATCRM"), QStringLiteral("X-ContactId"));
            const QString accountId = contact.custom(QStringLiteral("FATCRM"), QStringLiteral("X-AccountId"));
            Q_ASSERT(!contactId.isEmpty());
            const QString accountType = AccountRepository::instance()->accountById(accountId).accountType();
            if (accountType == "Partner" || accountType == "Competitor") {
                // Don't delete partners, competitors or (TODO) providers (we don't create opportunities to model our collaboration)
                return false;
            }
            static QDate today = QDate::currentDate();
            if ((accountId.isEmpty() ||
                    (d->mLinkedItemsRepository->opportunitiesForAccount(accountId).isEmpty() &&
                     d->mLinkedItemsRepository->documentsForAccount(accountId).isEmpty() &&
                     d->mLinkedItemsRepository->notesForAccount(accountId).isEmpty() &&
                     d->mLinkedItemsRepository->emailsForAccount(accountId).isEmpty())
                 ) &&
                    contact.note().isEmpty() && // description
                    d->mLinkedItemsRepository->notesForContact(contactId).isEmpty() &&
                    d->mLinkedItemsRepository->emailsForContact(contactId).isEmpty() &&
                    KDCRMUtils::dateTimeFromString(contact.custom(QStringLiteral("FATCRM"), QStringLiteral("X-DateCreated"))).date().daysTo(today) > 5*365) {
                // No account -> delete
                // Otherwise -> anonymize
                const bool shouldDelete = accountId.isEmpty();
                const bool matchesFilter = (d->mGDPRFilterAction == FullyDelete) ? shouldDelete : !shouldDelete;
                if (!matchesFilter)
                    return false;
                if (d->mProtectedEmails.contains(contact.preferredEmail())) {
                    qDebug() << "PROTECTED BY NEWSLETTER:" << contact.preferredEmail() << "against" << (shouldDelete?"deletion":"anonymization");
                    return false;
                }
                return d->mFilter.isEmpty() || contactMatchesFilter(contact, d->mFilter);
            } else {
                return false;
            }
        } else {
            return d->mFilter.isEmpty() || contactMatchesFilter(contact, d->mFilter);
        }
    }
    case DetailsType::Lead: {
        Q_ASSERT(item.hasPayload<SugarLead>());
        return d->mFilter.isEmpty() || leadMatchesFilter(item.payload<SugarLead>(), d->mFilter);
    }
    case DetailsType::Opportunity: // notreached, handled by subclass
        return false;
    }
    return true;
}

static bool accountMatchesFilter(const SugarAccount &account, const QString &filter)
{
    if (account.name().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (account.billingAddressCity().contains(filter, Qt::CaseInsensitive) ||
            account.shippingAddressCity().contains(filter, Qt::CaseInsensitive) ||
            account.billingAddressStreet().contains(filter, Qt::CaseInsensitive) ||
            account.shippingAddressStreet().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (account.email1().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (account.billingAddressCountry().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (account.phoneOffice().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (account.postalCodeForGui().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    return false;
}

static bool campaignMatchesFilter(const SugarCampaign &campaign, const QString &filter)
{
    if (campaign.name().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (campaign.status().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (campaign.campaignType().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (campaign.endDate().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (campaign.assignedUserName().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }

    return false;
}

static bool contactMatchesFilter(const KContacts::Addressee& contact, const QString &filter)
{
    if (contact.assembledName().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (contact.organization().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (contact.preferredEmail().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (contact.phoneNumber(KContacts::PhoneNumber::Work).number().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (contact.phoneNumber(KContacts::PhoneNumber::Cell).number().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (contact.givenName().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (ItemsTreeModel::countryForContact(contact).contains(filter, Qt::CaseInsensitive)) {
        return true;
    }

    return false;
}

static bool leadMatchesFilter(const SugarLead &lead, const QString &filter)
{
    if (lead.firstName().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (lead.lastName().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (lead.status().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (lead.accountName().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (lead.email1().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (lead.assignedUserName().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }

    return false;
}

