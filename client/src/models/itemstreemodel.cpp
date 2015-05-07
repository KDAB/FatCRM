#include "itemstreemodel.h"

#include "referenceddata.h"

#include <kdcrmdata/sugaraccount.h>
#include <kdcrmdata/sugarcampaign.h>
#include <kdcrmdata/sugarlead.h>
#include <kdcrmdata/sugaropportunity.h>
#include <kdcrmdata/kdcrmutils.h>

#include <kabc/addressee.h>
#include <kabc/phonenumber.h>

#include <QtGui>
#include <kglobal.h>
#include <kicon.h>
#include <kiconloader.h>
#include <klocale.h>

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
    // avoid string comparisons for all other roles
    if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::DecorationRole) {

        if (item.mimeType() == SugarAccount::mimeType()) {
            return accountData(item, column, role);
        } else if (item.mimeType() == SugarCampaign::mimeType()) {
            return campaignData(item, column, role);
        } else if (item.mimeType() == KABC::Addressee::mimeType()) {
            return contactData(item, column, role);
        } else if (item.mimeType() == SugarLead::mimeType()) {
            return leadData(item, column, role);
        } else if (item.mimeType() == SugarOpportunity::mimeType()) {
            return opportunityData(item, column, role);
        }
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
            } else if (headerGroup == EntityTreeModel::ItemListHeaders) {
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
            return account.billingAddressCity();
        case Country:
            return account.billingAddressCountry();
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
 * Return the data. KABC::Addressee type - ref: Contacts
 */
QVariant ItemsTreeModel::contactData(const Item &item, int column, int role) const
{
    if (!item.hasPayload<KABC::Addressee>()) {

        // Pass modeltest
        if (role == Qt::DisplayRole) {
            return item.remoteId();
        }

        return QVariant();
    }

    const KABC::Addressee addressee = item.payload<KABC::Addressee>();

    if ((role == Qt::DisplayRole) || (role == Qt::EditRole)) {
        switch (columnTypes().at(column)) {
        case FullName:
            return addressee.assembledName();
        case Role:
            return addressee.role();
        case Organization:
            return addressee.organization();
        case PreferredEmail:
            return addressee.preferredEmail();
        case PhoneNumber:
            return addressee.phoneNumber(KABC::PhoneNumber::Work).number();
        case GivenName:
            return addressee.givenName();
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
            return opportunity.accountName();
        case SalesStage:
            return opportunity.salesStage();
        case Amount:
            return QLocale().toCurrencyString(QLocale::c().toDouble(opportunity.amount()), opportunity.currencySymbol());
        case CreationDate: {
            QDateTime dt = KDCRMUtils::dateTimeFromString(opportunity.dateEntered());
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
            QDateTime dt = KDCRMUtils::dateTimeFromString(opportunity.dateModified());
            if (role == Qt::DisplayRole)
                return KDCRMUtils::formatDate(dt.date());
            return dt; // for sorting
        }
        case AssignedTo:
            return opportunity.assignedUserName();
        case Country:
            // I wish I could use accountId() as key, but SuiteCRM doesn't give me account_id for opportunities...
            return ReferencedData::instance(AccountCountryRef)->referencedData(opportunity.accountName());
        default:
            return QVariant();
        }
    }
    return QVariant();
}

ItemsTreeModel::ColumnTypes ItemsTreeModel::columnTypes(DetailsType type)
{
    ItemsTreeModel::ColumnTypes columns;

    switch (type) {
    case Account:
        columns << ItemsTreeModel::Name
                << ItemsTreeModel::City
                << ItemsTreeModel::Country
                << ItemsTreeModel::Phone
                << ItemsTreeModel::Email
                << ItemsTreeModel::CreatedBy;
        break;
    case Contact:
        columns << ItemsTreeModel::FullName
                << ItemsTreeModel::Role
                << ItemsTreeModel::Organization
                << ItemsTreeModel::PreferredEmail
                << ItemsTreeModel::PhoneNumber
                << ItemsTreeModel::GivenName;
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
                << ItemsTreeModel::Country
                << ItemsTreeModel::SalesStage
                << ItemsTreeModel::Amount
                << ItemsTreeModel::CreationDate
                << ItemsTreeModel::NextStep
                << ItemsTreeModel::NextStepDate
                << ItemsTreeModel::LastModifiedDate
                << ItemsTreeModel::AssignedTo;
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
    case Role:
        return i18nc("@title:column role - role", "Role");
    case Organization:
        return i18nc("@title:column company", "Organization");
    case PreferredEmail:
        return i18nc("@title:column email", "Preferred Email");
    case PhoneNumber:
        return i18nc("@title:column phone work", "Phone Work");
    case GivenName:
        return i18nc("@title:column given name", "Given Name");
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
    case SalesStage:
        return i18nc("@title:column sales stage", "Sales Stage");
    case Amount:
        return i18nc("@title:column amount", "Amount");
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
        break;
    case Contact:
        break;
    case Lead:
        break;
    case Opportunity:
        columns.removeAll(ItemsTreeModel::Amount);
        columns.removeAll(ItemsTreeModel::NextStep);
        columns.removeAll(ItemsTreeModel::LastModifiedDate);
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

#include "itemstreemodel.moc"
