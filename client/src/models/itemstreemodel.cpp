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

#include "itemstreemodel.h"
#include "referenceddata.h"
#include "clientsettings.h"

#include "kdcrmdata/sugaraccount.h"
#include "kdcrmdata/sugarcampaign.h"
#include "kdcrmdata/sugarlead.h"
#include "kdcrmdata/sugaropportunity.h"
#include "kdcrmdata/kdcrmutils.h"

#include <KContacts/Addressee>
#include <KContacts/PhoneNumber>

#include <AkonadiCore/ChangeRecorder>

#include <QIcon>
#include <KIconLoader>
#include <KLocalizedString>
#include <QMetaEnum>
#include <QFont>

using namespace Akonadi;

class ItemsTreeModel::Private
{
public:
    Private()
        : mColumns(),
          mIconSize(KIconLoader::global()->currentSize(KIconLoader::Small))
    {
    }

    ItemsTreeModel::ColumnTypes mColumns;
    const int mIconSize;
};

ItemsTreeModel::ItemsTreeModel(DetailsType type, ChangeRecorder *monitor, QObject *parent)
    : EntityTreeModel(monitor, parent), d(new Private), mType(type)
{
    d->mColumns = columnTypes(mType);

    if (mType == Opportunity) {
        // Update accountName and country columns once all accounts are loaded
        connect(AccountRepository::instance(), SIGNAL(initialLoadingDone()),
                this, SLOT(slotAccountsLoaded()));

        // and update it again later in case of single changes (by the user or when updating from server)
        connect(AccountRepository::instance(), SIGNAL(accountModified(QString,QVector<AccountRepository::Field>)),
                this, SLOT(slotAccountModified(QString,QVector<AccountRepository::Field>)));
    }
}

ItemsTreeModel::~ItemsTreeModel()
{
    delete d;
}

/**
 * Returns the columns that the model currently shows.
 */
ItemsTreeModel::ColumnTypes ItemsTreeModel::columnTypes() const
{
    return d->mColumns;
}

/**
 * Reimp: Returns the data displayed by the model
 */
QVariant ItemsTreeModel::entityData(const Item &item, int column, int role) const
{
    // avoid calling item.payload() for all other roles
    if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::DecorationRole) {

        if (mType == Account) {
            return accountData(item, column, role);
        } else if (mType == Campaign) {
            return campaignData(item, column, role);
        } else if (mType == Contact) {
            return contactData(item, column, role);
        } else if (mType == Lead) {
            return leadData(item, column, role);
        } else if (mType == Opportunity) {
            return opportunityData(item, column, role);
        }
    } else if (role == Qt::ToolTipRole) {
        if (ClientSettings::self()->showToolTips()) {
            if (mType == Account) {
                return accountToolTip(item);
            } else if (mType == Opportunity) {
                return opportunityToolTip(item);
            }
        }
        return QVariant();
    } else if (mType == Opportunity && role == Qt::FontRole) {
        return opportunityData(item, column, role);
    }

    return EntityTreeModel::entityData(item, column, role);
}

/**
 * Reimp
 */
QVariant ItemsTreeModel::entityData(const Collection &collection, int column, int role) const
{
    if (role == Qt::DisplayRole) {
        switch (column) {
        case 0:
            return EntityTreeModel::entityData(collection, column, role);
        default:
            return QString(); // pass model test
        }
    }

    return EntityTreeModel::entityData(collection, column, role);
}

/**
 * Reimp
 */
int ItemsTreeModel::entityColumnCount(HeaderGroup headerGroup) const
{
    if (headerGroup == EntityTreeModel::CollectionTreeHeaders) {
        return 1;
    } else if (headerGroup == EntityTreeModel::ItemListHeaders) {
        return d->mColumns.count();
    } else {
        return EntityTreeModel::entityColumnCount(headerGroup);
    }
}

QString ItemsTreeModel::countryForContact(const KContacts::Addressee &addressee)
{
    // Get the country from the contact, if it has an address.
    const QString cc = addressee.address(KContacts::Address::Work | KContacts::Address::Pref).country();
    if (!cc.isEmpty())
        return cc;
    // Otherwise get the country via the account
    const QString country = AccountRepository::instance()->accountById(addressee.organization()).countryForGui();
    return country;
}

void ItemsTreeModel::slotAccountModified(const QString &accountId, const QVector<AccountRepository::Field> &changedFields)
{
    Q_UNUSED(accountId);
    if (mType == Opportunity) {
        // We could iterate over all opps to find those which use that account.... but maybe this is just faster:
        const int rows = rowCount();
        if (rows == 0)
            return;
        QVector<int> columns;
        if (changedFields.contains(AccountRepository::Country)) {
            columns.append(d->mColumns.indexOf(Country));
        }
        if (changedFields.contains(AccountRepository::Name)) {
            columns.append(d->mColumns.indexOf(OpportunityAccountName));
        }
        if (columns.isEmpty())
            return;
        const int firstColumn = *std::min_element(columns.constBegin(), columns.constEnd());
        const int lastColumn = *std::max_element(columns.constBegin(), columns.constEnd());
        qDebug() << "emit dataChanged" << 0 << firstColumn << rows-1 << lastColumn;
        emit dataChanged(index(0, firstColumn), index(rows - 1, lastColumn));
    }
}

// Called when the accounts have just been loaded
// Normally we have no opps yet, but it can happen if akonadi syncs the folders in a different order than we expected
// (i.e. due to queued jobs in the resource)
void ItemsTreeModel::slotAccountsLoaded()
{
    const int rows = rowCount();
    if (rows > 0) {
        const int columnCountry = d->mColumns.indexOf(Country);
        const int columnAccountName = d->mColumns.indexOf(OpportunityAccountName);
        const int firstColumn = qMin(columnCountry, columnAccountName);
        const int lastColumn = qMax(columnCountry, columnAccountName);
        qDebug() << "emit dataChanged" << 0 << firstColumn << rows-1 << lastColumn;
        emit dataChanged(index(0, firstColumn), index(rows - 1, lastColumn));
    }
}

/**
 * Reimp: Return the Header data to display
 */
QVariant ItemsTreeModel::entityHeaderData(int section, Qt::Orientation orientation, int role, HeaderGroup headerGroup) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            if (headerGroup == EntityTreeModel::CollectionTreeHeaders) {

                if (section >= 1) {
                    return QVariant();
                }

                switch (section) {
                case 0:
                    return i18nc("@title:Sugar items overview", "Items");
                    break;
                }
            } else { // e.g EntityTreeModel::ItemListHeaders
                if (section < 0 || section >= d->mColumns.count()) {
                    return QVariant();
                }
                return columnTitle(d->mColumns.at(section));
            }
        }
    }

    return EntityTreeModel::entityHeaderData(section, orientation, role, headerGroup);
}

/**
 * Return the data. SugarAccount type
 */
QVariant ItemsTreeModel::accountData(const Item &item, int column, int role) const
{
    if (!item.hasPayload<SugarAccount>()) {
        // Pass modeltest
        if (role == Qt::DisplayRole) {
            return item.remoteId();
        }

        return QVariant();
    }
    const SugarAccount account = item.payload<SugarAccount>();

    if ((role == Qt::DisplayRole) || (role == Qt::EditRole)) {
        switch (columnTypes().at(column)) {
        case Name:
            return account.name();
        case City:
            return account.shippingAddressCity().isEmpty() ? account.billingAddressCity() : account.shippingAddressCity();
        case Country:
            return account.shippingAddressCountry().isEmpty() ? account.billingAddressCountry() : account.shippingAddressCountry();
        case Street:
            return account.shippingAddressStreet().isEmpty() ? account.billingAddressStreet() : account.shippingAddressStreet();
        case Phone:
            return account.phoneOffice();
        case Email:
            return account.email1();
        case CreatedBy:
            return account.createdByName();
        default:
            return QVariant();
        }
    }
    return QVariant();
}

/**
 * Return the data. SugarCampaign type
 */
QVariant ItemsTreeModel::campaignData(const Item &item, int column, int role) const
{
    if (!item.hasPayload<SugarCampaign>()) {

        // Pass modeltest
        if (role == Qt::DisplayRole) {
            return item.remoteId();
        }

        return QVariant();
    }

    const SugarCampaign campaign = item.payload<SugarCampaign>();

    if ((role == Qt::DisplayRole) || (role == Qt::EditRole)) {
        switch (columnTypes().at(column)) {
        case CampaignName:
            return campaign.name();
        case Status:
            return campaign.status();
        case Type:
            return campaign.campaignType();
        case EndDate:
            return campaign.endDate();
        case User:
            return campaign.assignedUserName();
        default:
            return QVariant();
        }
    }
    return QVariant();
}

/**
 * Return the data. KContacts::Addressee type - ref: Contacts
 */
QVariant ItemsTreeModel::contactData(const Item &item, int column, int role) const
{
    if (!item.hasPayload<KContacts::Addressee>()) {

        // Pass modeltest
        if (role == Qt::DisplayRole) {
            return item.remoteId();
        }

        return QVariant();
    }

    const KContacts::Addressee addressee = item.payload<KContacts::Addressee>();

    if ((role == Qt::DisplayRole) || (role == Qt::EditRole)) {
        switch (columnTypes().at(column)) {
        case FullName:
            return addressee.assembledName();
        case Title:
            return addressee.title();
        case Organization: {
            // not using addressee.organization() since that doesn't follow account renames/deletions
            const QString accountId = addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-AccountId"));
            return ReferencedData::instance(AccountRef)->referencedData(accountId);
        }
        case PreferredEmail:
            return addressee.preferredEmail();
        case PhoneWork:
            return addressee.phoneNumber(KContacts::PhoneNumber::Work).number();
        case PhoneMobile:
            return addressee.phoneNumber(KContacts::PhoneNumber::Cell).number();
        case Country:
            return countryForContact(addressee);
        case LastModifiedDate:
        {
            const QDateTime dt = KDCRMUtils::dateTimeFromString(addressee.custom(QStringLiteral("FATCRM"), QStringLiteral("X-DateModified")));
            if (role == Qt::DisplayRole)
                return KDCRMUtils::formatDate(dt.date());
            return dt; // for sorting
        }
        default:
            return QVariant();
        }
    }
    return QVariant();
}

/**
 * Return the data. SugarLead type
 */
QVariant ItemsTreeModel::leadData(const Item &item, int column, int role) const
{
    if (!item.hasPayload<SugarLead>()) {

        // Pass modeltest
        if (role == Qt::DisplayRole) {
            return item.remoteId();
        }

        return QVariant();
    }

    const SugarLead lead = item.payload<SugarLead>();

    if ((role == Qt::DisplayRole) || (role == Qt::EditRole)) {
        switch (columnTypes().at(column)) {
        case LeadName:
            return lead.lastName();
        case LeadStatus:
            return lead.status();
        case LeadAccountName:
            return lead.accountName();
        case LeadEmail:
            return lead.email1();
        case LeadUser:
            return lead.assignedUserName();
        default:
            return QVariant();
        }
    }
    return QVariant();
}

/**
 * Return the data. SugarOpportunity type
 */
QVariant ItemsTreeModel::opportunityData(const Item &item, int column, int role) const
{
    if (!item.hasPayload<SugarOpportunity>()) {

        // Pass modeltest
        if (role == Qt::DisplayRole) {
            return item.remoteId();
        }

        return QVariant();
    }

    const SugarOpportunity opportunity = item.payload<SugarOpportunity>();

    if ((role == Qt::DisplayRole) || (role == Qt::EditRole)) {
        switch (columnTypes().at(column)) {
        case OpportunityName:
            return opportunity.name();
        case OpportunityAccountName:
            return ReferencedData::instance(AccountRef)->referencedData(opportunity.accountId());
        case SalesStage:
            return opportunity.salesStage();
        case Amount: {
            const double amount = QLocale::c().toDouble(opportunity.amount());
            if (role == Qt::DisplayRole)
                return QLocale().toCurrencyString(amount, opportunity.currencySymbol());
            return amount; // for sorting
        }
        case OpportunitySize:
            return opportunity.opportunitySize();
        case OpportunityPriority: {
            const QString priority = opportunity.opportunityPriority().toUpper();
            if (role == Qt::DisplayRole)
                return priority;
            return priority.isEmpty() ? QString::fromLatin1("Z") : priority; // for better ordering: A, B, C, <none>
        }
        case Description:
            return opportunity.limitedDescription(2);
        case CreationDate: {
            const QDateTime dt = KDCRMUtils::dateTimeFromString(opportunity.dateEntered());
            if (role == Qt::DisplayRole)
                return KDCRMUtils::formatDate(dt.date());
            return dt; // for sorting
        }
        case NextStepDate:
            if (role == Qt::DisplayRole)
                return KDCRMUtils::formatDate(opportunity.nextCallDate());
            return opportunity.nextCallDate(); // for sorting
        case NextStep:
            return opportunity.nextStep();
        case LastModifiedDate: {
            const QDateTime dt = opportunity.dateModified();
            if (role == Qt::DisplayRole)
                return KDCRMUtils::formatDate(dt.date());
            return dt; // for sorting
        }
        case AssignedTo:
            return opportunity.assignedUserName();
        case PostalCode:
            return AccountRepository::instance()->accountById(opportunity.accountId()).postalCodeForGui();
        case City:
            return AccountRepository::instance()->accountById(opportunity.accountId()).cityForGui();
        case Country:
            return AccountRepository::instance()->accountById(opportunity.accountId()).countryForGui();

        default:
            return QVariant();
        }
    }
    if (role == Qt::FontRole) {
        if (opportunity.customFields().value(QStringLiteral("urgent")) == QLatin1String("1")) {
            QFont boldFont;
            boldFont.setBold(true);
            return boldFont;
        }
    }
    return QVariant();
}

QVariant ItemsTreeModel::accountToolTip(const Item &item) const
{
    if (!item.hasPayload<SugarAccount>()) {
        return QVariant();
    }

    const SugarAccount account = item.payload<SugarAccount>();
    QString accountDescription = account.limitedDescription(5);
    accountDescription.replace('\n', QLatin1String("<br>"));

    QString toolTipOutput;
    QString accountCountry = account.countryForGui();

    if (!account.name().isEmpty()) {
        toolTipOutput.append(i18n("<p><b>Name</b><br>%1</p>", account.name()));

        if (!accountCountry.isEmpty()) {
            toolTipOutput.insert(toolTipOutput.length()-4, i18n(" (%1)", accountCountry));
        }

    } else if (!accountCountry.isEmpty()) {
        toolTipOutput.append(i18n("<p><b>Country</b><br>%1</p>", accountCountry));
    }

    QDateTime dateModified = KDCRMUtils::dateTimeFromString(account.dateModified());
    if (dateModified.isValid()) {
        toolTipOutput.append(i18n("<p><b>Last Modified</b><br>%1 (%2)</p>", dateModified.toString(Qt::SystemLocaleShortDate), account.modifiedByName()));
    }

    if (!accountDescription.isEmpty()) {
        toolTipOutput.append(i18n("<p><b>Description</b><br>%1</p>", accountDescription));
    }

    return toolTipOutput;
}

QVariant ItemsTreeModel::opportunityToolTip(const Item &item) const
{
    if (!item.hasPayload<SugarOpportunity>()) {
        return QVariant();
    }

    const SugarOpportunity opportunity = item.payload<SugarOpportunity>();

    QString toolTipOutput;

    if (!opportunity.name().isEmpty()) {
        toolTipOutput.append(i18n("<p><b>Name</b><br>%1</p>", opportunity.name()));
    }

    QDateTime dateModified = opportunity.dateModified();
    if (dateModified.isValid()) {
        toolTipOutput.append(i18n("<p><b>Last Modified</b><br>%1 (%2)</p>", dateModified.toString(Qt::SystemLocaleShortDate), opportunity.modifiedByName()));
    }

    if (!opportunity.nextStep().isEmpty()) {
        toolTipOutput.append(i18n("<p><b>Next Step</b><br>%1 (%2)</p>", opportunity.nextStep(), opportunity.nextCallDate().toString(Qt::SystemLocaleShortDate)));
    }

    QString opportunityShortDescription = opportunity.limitedDescription(5);
    opportunityShortDescription.replace('\n', QLatin1String("<br>"));
    if (!opportunityShortDescription.isEmpty()) {
        toolTipOutput.append(i18n("<p><b>Description</b><br>%1</p>", opportunityShortDescription));
    }

    return toolTipOutput;
}

ItemsTreeModel::ColumnTypes ItemsTreeModel::columnTypes(DetailsType type)
{
    ItemsTreeModel::ColumnTypes columns;

    switch (type) {
    case Account:
        columns << ItemsTreeModel::Name
                << ItemsTreeModel::Street
                << ItemsTreeModel::City
                << ItemsTreeModel::Country
                << ItemsTreeModel::Phone
                << ItemsTreeModel::Email
                << ItemsTreeModel::CreatedBy;
        break;
    case Contact:
        columns << ItemsTreeModel::FullName
                << ItemsTreeModel::Title
                << ItemsTreeModel::Organization
                << ItemsTreeModel::Country
                << ItemsTreeModel::PreferredEmail
                << ItemsTreeModel::PhoneWork
                << ItemsTreeModel::PhoneMobile
                << ItemsTreeModel::LastModifiedDate;
        break;
    case Lead:
        columns << ItemsTreeModel::LeadName
                << ItemsTreeModel::LeadStatus
                << ItemsTreeModel::LeadAccountName
                << ItemsTreeModel::LeadEmail
                << ItemsTreeModel::LeadUser;
        break;
    case Opportunity:
        columns << ItemsTreeModel::OpportunityAccountName
                << ItemsTreeModel::OpportunityName
                << ItemsTreeModel::PostalCode
                << ItemsTreeModel::City
                << ItemsTreeModel::Country
                << ItemsTreeModel::SalesStage
                << ItemsTreeModel::Amount
                << ItemsTreeModel::OpportunitySize
                << ItemsTreeModel::Description
                << ItemsTreeModel::CreationDate
                << ItemsTreeModel::NextStep
                << ItemsTreeModel::NextStepDate
                << ItemsTreeModel::LastModifiedDate
                << ItemsTreeModel::AssignedTo
                << ItemsTreeModel::OpportunityPriority;
        break;
    case Campaign:
        columns << ItemsTreeModel::CampaignName
                << ItemsTreeModel::Status
                << ItemsTreeModel::Type
                << ItemsTreeModel::EndDate
                << ItemsTreeModel::User;
    default:
        break;
    }

    return columns;
}

QString ItemsTreeModel::columnTitle(ItemsTreeModel::ColumnType col) const
{
    switch (col) {
    case Name:
        return i18nc("@title:column name", "Name");
    case Street:
        return i18nc("@title:column street", "Street");
    case PostalCode:
        return i18nc("@title:column postalcode", "Postal Code");
    case City:
        return i18nc("@title:column city", "City");
    case Country:
        return i18nc("@title:column country ", "Country");
    case Phone:
        return i18nc("@title:column phone", "Phone");
    case Email:
        return i18nc("@title:column email", "Email");
    case CreationDate:
        return i18nc("@title:column date created", "Creation Date");
    case CreatedBy:
        return i18nc("@title:column created by user", "Created By");
    case CampaignName:
        return i18nc("@title:column name of a campaign ", "Campaign");
    case Status:
        return i18nc("@title:column status - status", "Status");
    case Type:
        return i18nc("@title:column type - Campaign type", "Type");
    case EndDate:
        return i18nc("@title:column end date - End Date", "End Date");
    case User:
        return i18nc("@title:column Assigned User Name", "User");
    case FullName:
        return i18nc("@title:column full name of a contact ", "Name");
    case Title:
        return i18nc("@title:column contact title", "Title");
    case Organization:
        return i18nc("@title:column company", "Organization");
    case PreferredEmail:
        return i18nc("@title:column email", "Preferred Email");
    case PhoneWork:
        return i18nc("@title:column phone (work)", "Office Phone");
    case PhoneMobile:
        return i18nc("@title:column phone (mobile)", "Mobile");
    case LeadName:
        return i18nc("@title:column Lead's Full Name", "Name");
    case LeadStatus:
        return i18nc("@title:column Lead's Status", "Status");
    case LeadAccountName:
        return i18nc("@title:column Account Name", "Account Name");
    case LeadEmail:
        return i18nc("@title:column Lead's Primary email", "Email");
    case LeadUser:
        return i18nc("@title:column Lead's Assigny name", "User");
    case OpportunityName:
        return i18nc("@title:column name for the Opportunity", "Opportunity");
    case OpportunityAccountName:
        return i18nc("@title:column account name", "Account Name");
    case OpportunitySize:
        return i18nc("@title:column opportunity size", "Size");
    case OpportunityPriority:
        return i18nc("@title:column opportunity priority", "Priority");
    case SalesStage:
        return i18nc("@title:column sales stage", "Sales Stage");
    case Amount:
        return i18nc("@title:column amount", "Amount");
    case Description:
        return i18nc("@title:column description", "Description");
    case NextStep:
        return i18nc("@title:column next step", "Next Step");
    case NextStepDate:
        return i18nc("@title:column next step date", "Next Step Date");
    case LastModifiedDate:
        return i18nc("@title:column next step date", "Last Modified Date");
    case AssignedTo:
        return i18nc("@title:column assigned to name", "Assigned To");
    }
    return QString();
}

QString ItemsTreeModel::columnNameFromType(ItemsTreeModel::ColumnType col)
{
    const QMetaObject &mo = staticMetaObject;
    int index = mo.indexOfEnumerator("ColumnType");
    Q_ASSERT(index >= 0);
    QMetaEnum metaEnum = mo.enumerator(index);
    return metaEnum.valueToKey(col);
}

ItemsTreeModel::ColumnType ItemsTreeModel::columnTypeFromName(const QString &name)
{
    const QMetaObject &mo = staticMetaObject;
    int index = mo.indexOfEnumerator("ColumnType");
    Q_ASSERT(index >= 0);
    QMetaEnum metaEnum = mo.enumerator(index);
    return static_cast<ColumnType>(metaEnum.keyToValue(name.toLatin1().constData()));
}

ItemsTreeModel::ColumnTypes ItemsTreeModel::defaultVisibleColumns() const
{
    ItemsTreeModel::ColumnTypes columns = columnTypes();
    switch (mType) {
    case Account:
        columns.removeAll(ItemsTreeModel::Street);
        columns.removeAll(ItemsTreeModel::CreatedBy);
        break;
    case Contact:
        // too wide and too seldom filled in
        columns.removeAll(ItemsTreeModel::Title);
        columns.removeAll(ItemsTreeModel::LastModifiedDate);
        break;
    case Lead:
        break;
    case Opportunity:
        columns.removeAll(ItemsTreeModel::PostalCode);
        columns.removeAll(ItemsTreeModel::City);
        columns.removeAll(ItemsTreeModel::Amount);
        columns.removeAll(ItemsTreeModel::NextStep);
        columns.removeAll(ItemsTreeModel::LastModifiedDate);
        columns.removeAll(ItemsTreeModel::OpportunitySize);
        break;
    case Campaign:
        break;
    default:
        break;
    }
    return columns;
}

QString ItemsTreeModel::columnName(int column) const
{
    return columnNameFromType(d->mColumns.at(column));
}

