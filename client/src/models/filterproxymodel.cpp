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

#include "filterproxymodel.h"
#include "itemstreemodel.h"

#include <kdcrmdata/sugaraccount.h>
#include <kdcrmdata/sugarcampaign.h>
#include <kdcrmdata/sugarlead.h>
#include <kdcrmdata/sugaropportunity.h>

#include <kabc/addressee.h>
#include <kabc/phonenumber.h>

#include <klocalizedstring.h>

#include <akonadi/entitytreemodel.h>

static bool accountMatchesFilter(const SugarAccount &account,
                                 const QString &filterString);
static bool campaignMatchesFilter(const SugarCampaign &campaign,
                                  const QString &filterString);
static bool contactMatchesFilter(const KABC::Addressee &addressee,
                                 const QString &filterString);
static bool leadMatchesFilter(const SugarLead &lead,
                              const QString &filterString);
static bool opportunityMatchesFilter(const SugarOpportunity &opportunity,
                                     const QString &filterString);

using namespace Akonadi;

class FilterProxyModel::Private
{
public:
    Private(DetailsType type)
        : mType(type)
    {}
    DetailsType mType;
    QString mFilter;
};

FilterProxyModel::FilterProxyModel(DetailsType type, QObject *parent)
    : QSortFilterProxyModel(parent), d(new Private(type))
{
    // account names should be sorted correctly
    setSortLocaleAware(true);
    setDynamicSortFilter(true); // for sorting during insertion, too
}

FilterProxyModel::~FilterProxyModel()
{
    delete d;
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
    if (d->mFilter.isEmpty()) {
        return true;
    }
    const QModelIndex index = sourceModel()->index(row, 0, parent);
    const Akonadi::Item item =
        index.data(Akonadi::EntityTreeModel::ItemRole).value<Akonadi::Item>();

    switch (d->mType) {
    case Account: {
        Q_ASSERT(item.hasPayload<SugarAccount>());
        const SugarAccount account = item.payload<SugarAccount>();
        return accountMatchesFilter(account, d->mFilter);
    }
    case Campaign: {
        Q_ASSERT(item.hasPayload<SugarCampaign>());
        const SugarCampaign campaign = item.payload<SugarCampaign>();
        return campaignMatchesFilter(campaign, d->mFilter);
    }
    case Contact: {
        Q_ASSERT(item.hasPayload<KABC::Addressee>());
        const KABC::Addressee contact = item.payload<KABC::Addressee>();
        return contactMatchesFilter(contact, d->mFilter);
    }
    case Lead: {
        Q_ASSERT(item.hasPayload<SugarLead>());
        const SugarLead lead = item.payload<SugarLead>();
        return leadMatchesFilter(lead, d->mFilter);
    }
    case Opportunity: {
        Q_ASSERT(item.hasPayload<SugarOpportunity>());
        const SugarOpportunity opportunity = item.payload<SugarOpportunity>();
        return opportunityMatchesFilter(opportunity, d->mFilter);
    }
    }
    return true;
}

static bool accountMatchesFilter(const SugarAccount &account, const QString &filter)
{
    if (account.name().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (account.billingAddressCity().contains(filter, Qt::CaseInsensitive)) {
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

static bool contactMatchesFilter(const KABC::Addressee& contact, const QString &filter)
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
    if (contact.phoneNumber(KABC::PhoneNumber::Work).number().contains(filter, Qt::CaseInsensitive)) {
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

static bool opportunityMatchesFilter(const SugarOpportunity &opportunity, const QString &filter)
{
    if (opportunity.name().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (opportunity.accountName().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (opportunity.salesStage().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (opportunity.amount().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }
    if (opportunity.assignedUserName().contains(filter, Qt::CaseInsensitive)) {
        return true;
    }

    return false;
}

#include "filterproxymodel.moc"

