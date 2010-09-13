#include "campaignshandler.h"

#include "campaignshandler.h"

#include "sugarsoap.h"

#include <akonadi/abstractdifferencesreporter.h>
#include <akonadi/collection.h>

#include <kdcrmdata/sugarcampaign.h>

#include <KLocale>

#include <QHash>

typedef QString (*valueGetter)( const SugarCampaign& );
typedef void (*valueSetter)( const QString&, SugarCampaign&);

static void setId( const QString &value, SugarCampaign &campaign )
{
    campaign.setId( value );
}

static QString getName( const SugarCampaign &campaign )
{
    return campaign.name();
}

static void setName( const QString &value, SugarCampaign &campaign )
{
    campaign.setName( value );
}

static QString getDateEntered( const SugarCampaign &campaign )
{
    return campaign.dateEntered();
}

static void setDateEntered( const QString &value, SugarCampaign &campaign )
{
    campaign.setDateEntered( value );
}

static QString getDateModified( const SugarCampaign &campaign )
{
    return campaign.dateModified();
}

static void setDateModified( const QString &value, SugarCampaign &campaign )
{
    campaign.setDateModified( value );
}

static QString getModifiedUserId( const SugarCampaign &campaign )
{
    return campaign.modifiedUserId();
}

static void setModifiedUserId( const QString &value, SugarCampaign &campaign )
{
    campaign.setModifiedUserId( value );
}

static QString getModifiedByName( const SugarCampaign &campaign )
{
    return campaign.modifiedByName();
}

static void setModifiedByName( const QString &value, SugarCampaign &campaign )
{
    campaign.setModifiedByName( value );
}

static QString getCreatedBy( const SugarCampaign &campaign )
{
    return campaign.createdBy();
}

static void setCreatedBy( const QString &value, SugarCampaign &campaign )
{
    campaign.setCreatedBy( value );
}

static QString getCreatedByName( const SugarCampaign &campaign )
{
    return campaign.createdByName();
}

static void setCreatedByName( const QString &value, SugarCampaign &campaign )
{
    campaign.setCreatedByName( value );
}

static QString getDeleted( const SugarCampaign &campaign )
{
    return campaign.deleted();
}

static void setDeleted (const QString &value, SugarCampaign &campaign )
{
    campaign.setDeleted( value );
}

static QString getAssignedUserId( const SugarCampaign &campaign )
{
    return campaign.assignedUserId();
}

static void setAssignedUserId(const QString &value, SugarCampaign &campaign)
{
    campaign.setAssignedUserId( value );
}

static QString getAssignedUserName( const SugarCampaign &campaign )
{
    return campaign.assignedUserName();
}

static void setAssignedUserName(const QString &value, SugarCampaign &campaign)
{
    campaign.setAssignedUserName( value );
}

static QString getTrackerKey( const SugarCampaign &campaign )
{
    return campaign.trackerKey();
}

static void setTrackerKey(const QString &value, SugarCampaign &campaign)
{
    campaign.setTrackerKey( value );
}

static QString getTrackerCount( const SugarCampaign &campaign )
{
    return campaign.trackerCount();
}

static void setTrackerCount(const QString &value, SugarCampaign &campaign)
{
    campaign.setTrackerCount( value );
}

static QString getReferUrl( const SugarCampaign &campaign )
{
    return campaign.referUrl();
}

static void setReferUrl(const QString &value, SugarCampaign &campaign)
{
    campaign.setReferUrl( value );
}

static QString getTrackerText( const SugarCampaign &campaign )
{
    return campaign.trackerText();
}

static void setTrackerText(const QString &value, SugarCampaign &campaign)
{
    campaign.setTrackerText( value );
}

static QString getStartDate( const SugarCampaign &campaign )
{
    return campaign.startDate();
}

static void setStartDate(const QString &value, SugarCampaign &campaign)
{
    campaign.setStartDate( value );
}

static QString getEndDate( const SugarCampaign &campaign )
{
    return campaign.endDate();
}

static void setEndDate(const QString &value, SugarCampaign &campaign)
{
    campaign.setEndDate( value );
}

static QString getStatus( const SugarCampaign &campaign )
{
    return campaign.status();
}

static void setStatus(const QString &value, SugarCampaign &campaign)
{
    campaign.setStatus( value );
}

static QString getImpressions( const SugarCampaign &campaign )
{
    return campaign.impressions();
}

static void setImpressions(const QString &value, SugarCampaign &campaign)
{
    campaign.setImpressions( value );
}

static QString getCurrencyId( const SugarCampaign &campaign )
{
    return campaign.currencyId();
}

static void setCurrencyId(const QString &value, SugarCampaign &campaign)
{
    campaign.setCurrencyId( value );
}

static QString getBudget( const SugarCampaign &campaign )
{
    return campaign.budget();
}

static void setBudget(const QString &value, SugarCampaign &campaign)
{
    campaign.setBudget( value );
}

static QString getExpectedCost( const SugarCampaign &campaign )
{
    return campaign.expectedCost();
}

static void setExpectedCost(const QString &value, SugarCampaign &campaign)
{
    campaign.setExpectedCost( value );
}

static QString getActualCost( const SugarCampaign &campaign )
{
    return campaign.actualCost();
}

static void setActualCost(const QString &value, SugarCampaign &campaign)
{
    campaign.setActualCost( value );
}

static QString getExpectedRevenue( const SugarCampaign &campaign )
{
    return campaign.expectedRevenue();
}

static void setExpectedRevenue(const QString &value, SugarCampaign &campaign)
{
    campaign.setExpectedRevenue( value );
}

static QString getCampaignType( const SugarCampaign &campaign )
{
    return campaign.campaignType();
}

static void setCampaignType(const QString &value, SugarCampaign &campaign)
{
    campaign.setCampaignType( value );
}

static QString getObjective( const SugarCampaign &campaign )
{
    return campaign.objective();
}

static void setObjective(const QString &value, SugarCampaign &campaign)
{
    campaign.setObjective( value );
}

static QString getContent( const SugarCampaign &campaign )
{
    return campaign.content();
}

static void setContent(const QString &value, SugarCampaign &campaign)
{
    campaign.setContent( value );
}

static QString getFrequency( const SugarCampaign &campaign )
{
    return campaign.frequency();
}

static void setFrequency(const QString &value, SugarCampaign &campaign)
{
    campaign.setFrequency( value );
}


class AccessorPair
{
public:
    AccessorPair( valueGetter get, valueSetter set, const QString &name )
        : getter( get ), setter( set ), diffName( name )
    {}

public:
    valueGetter getter;
    valueSetter setter;
    const QString diffName;
};



CampaignsHandler::CampaignsHandler()
    : ModuleHandler( QLatin1String( "Campaigns" ) ),
      mAccessors( new AccessorHash )
{
    mAccessors->insert( QLatin1String( "id" ),
                        new AccessorPair( 0, setId, QString() ) );
    mAccessors->insert( QLatin1String( "name" ),
                        new AccessorPair( getName, setName,
                                          i18nc( "@item:intable campaign name", "Name" ) ) );
    mAccessors->insert( QLatin1String( "date_entered" ),
                        new AccessorPair( getDateEntered, setDateEntered, QString() ) );
    mAccessors->insert( QLatin1String( "date_modified" ),
                        new AccessorPair( getDateModified, setDateModified, QString() ) );
    mAccessors->insert( QLatin1String( "modified_user_id" ),
                        new AccessorPair( getModifiedUserId, setModifiedUserId, QString() ) );
    mAccessors->insert( QLatin1String( "modified_by_name" ),
                        new AccessorPair( getModifiedByName, setModifiedByName, QString() ) );
    mAccessors->insert( QLatin1String( "created_by" ),
                        new AccessorPair( getCreatedBy, setCreatedBy, QString() ) );
    mAccessors->insert( QLatin1String( "created_by_name" ),
                        new AccessorPair( getCreatedByName, setCreatedByName, QString() ) );
    mAccessors->insert( QLatin1String( "deleted" ),
                        new AccessorPair( getDeleted, setDeleted, QString() ) );
    mAccessors->insert( QLatin1String( "assigned_user_id" ),
                        new AccessorPair( getAssignedUserId, setAssignedUserId, QString() ) );
    mAccessors->insert( QLatin1String( "assigned_user_name" ),
                        new AccessorPair( getAssignedUserName, setAssignedUserName,
                                          i18nc( "@item:intable", "Assigned To" ) ) );
    mAccessors->insert( QLatin1String( "tracker_key" ),
                        new AccessorPair( getTrackerKey, setTrackerKey, QString() ) );
    mAccessors->insert( QLatin1String( "tracker_count" ),
                        new AccessorPair( getTrackerCount, setTrackerCount, QString() ) );
    mAccessors->insert( QLatin1String( "refer_url" ),
                        new AccessorPair( getReferUrl, setReferUrl,
                                          i18nc( "@item:intable", "Referer URL" ) ) );
    mAccessors->insert( QLatin1String( "tracker_text" ),
                        new AccessorPair( getTrackerText, setTrackerText,
                                          i18nc( "@item:intable", "Tracker" ) ) );
    mAccessors->insert( QLatin1String( "start_date" ),
                        new AccessorPair( getStartDate, setStartDate,
                                          i18nc( "@item:intable", "Start Date" ) ) );
    mAccessors->insert( QLatin1String( "end_date" ),
                        new AccessorPair( getEndDate, setEndDate,
                                          i18nc( "@item:intable", "End Date" ) ) );
    mAccessors->insert( QLatin1String( "status" ),
                        new AccessorPair( getStatus, setStatus,
                                          i18nc( "@item:intable", "Status" ) ) );
    mAccessors->insert( QLatin1String( "impressions" ),
                        new AccessorPair( getImpressions, setImpressions,
                                          i18nc( "@item:intable", "Impressions" ) ) );
    mAccessors->insert( QLatin1String( "currency_id" ),
                        new AccessorPair( getCurrencyId, setCurrencyId,
                                          i18nc( "@item:intable", "Currency" ) ) );
    mAccessors->insert( QLatin1String( "budget" ),
                        new AccessorPair( getBudget, setBudget,
                                          i18nc( "@item:intable", "Budget" ) ) );
    mAccessors->insert( QLatin1String( "expected_cost" ),
                        new AccessorPair( getExpectedCost, setExpectedCost,
                                          i18nc( "@item:intable", "Expected Costs" ) ) );
    mAccessors->insert( QLatin1String( "actual_cost" ),
                        new AccessorPair( getActualCost, setActualCost,
                                          i18nc( "@item:intable", "Actual Costs" ) ) );
    mAccessors->insert( QLatin1String( "expected_revenue" ),
                        new AccessorPair( getExpectedRevenue, setExpectedRevenue,
                                          i18nc( "@item:intable", "Expected Revenue" ) ) );
    mAccessors->insert( QLatin1String( "campaign_type" ),
                        new AccessorPair( getCampaignType, setCampaignType,
                                          i18nc( "@item:intable", "Type" ) ) );
    mAccessors->insert( QLatin1String( "objective" ),
                        new AccessorPair( getObjective, setObjective,
                                          i18nc( "@item:intable", "Objective" ) ) );
    mAccessors->insert( QLatin1String( "content" ),
                        new AccessorPair( getContent, setContent,
                                          i18nc( "@item:intable", "Content" ) ) );
    mAccessors->insert( QLatin1String( "frequency" ),
                        new AccessorPair( getFrequency, setFrequency,
                                          i18nc( "@item:intable", "Frequency" ) ) );
}

CampaignsHandler::~CampaignsHandler()
{
    qDeleteAll( *mAccessors );
    delete mAccessors;
}

QStringList CampaignsHandler::supportedFields() const
{
    return mAccessors->keys();
}

Akonadi::Collection CampaignsHandler::collection() const
{
    Akonadi::Collection campaignCollection;
    campaignCollection.setRemoteId( moduleName() );
    campaignCollection.setContentMimeTypes( QStringList() << SugarCampaign::mimeType() );
    campaignCollection.setName( i18nc( "@item folder name", "Campaigns" ) );
    campaignCollection.setRights( Akonadi::Collection::CanChangeItem |
                                  Akonadi::Collection::CanCreateItem |
                                  Akonadi::Collection::CanDeleteItem );

    return campaignCollection;
}

void CampaignsHandler::listEntries( int offset, Sugarsoap* soap, const QString &sessionId )
{
    const QString query = QLatin1String( "" );
    const QString orderBy = QLatin1String( "campaigns.name" );
    const int maxResults = 100;
    const int fetchDeleted = 0; // do not fetch deleted items

    TNS__Select_fields selectedFields;
    selectedFields.setItems( mAccessors->keys() );

    soap->asyncGet_entry_list( sessionId, moduleName(), query, orderBy, offset, selectedFields, maxResults, fetchDeleted );
}

bool CampaignsHandler::setEntry( const Akonadi::Item &item, Sugarsoap *soap, const QString &sessionId )
{
    if ( !item.hasPayload<SugarCampaign>() ) {
        kError() << "item (id=" << item.id() << ", remoteId=" << item.remoteId()
                 << ", mime=" << item.mimeType() << ") is missing Campaign payload";
        return false;
    }

    QList<TNS__Name_value> itemList;

    // if there is an id add it, otherwise skip this field
    // no id will result in the campaign being added
    if ( !item.remoteId().isEmpty() ) {
        TNS__Name_value field;
        field.setName( QLatin1String( "id" ) );
        field.setValue( item.remoteId() );

        itemList << field;
    }

    const SugarCampaign campaign = item.payload<SugarCampaign>();
    AccessorHash::const_iterator it    = mAccessors->constBegin();
    AccessorHash::const_iterator endIt = mAccessors->constEnd();
    for ( ; it != endIt; ++it ) {
        // check if this is a read-only field
        if ( (*it)->getter == 0 ) {
            continue;
        }
        TNS__Name_value field;
        field.setName( it.key() );
        field.setValue( (*it)->getter( campaign ) );
        if ( field.name() == "date_modified"  ||
             field.name() == "date_entered" )
            field.setValue( adjustedTime( field.value() ) );
        itemList << field;
    }

    TNS__Name_value_list valueList;
    valueList.setItems( itemList );
    soap->asyncSet_entry( sessionId, moduleName(), valueList );

    return true;
}

Akonadi::Item CampaignsHandler::itemFromEntry( const TNS__Entry_value &entry, const Akonadi::Collection &parentCollection )
{
    Akonadi::Item item;

    const QList<TNS__Name_value> valueList = entry.name_value_list().items();
    if ( valueList.isEmpty() ) {
        kWarning() << "Campaigns entry for id=" << entry.id() << "has no values";
        return item;
    }

    item.setRemoteId( entry.id() );
    item.setParentCollection( parentCollection );
    item.setMimeType( SugarCampaign::mimeType() );

    SugarCampaign campaign;
    campaign.setId( entry.id() );
    Q_FOREACH( const TNS__Name_value &namedValue, valueList ) {
        const AccessorHash::const_iterator accessIt = mAccessors->constFind( namedValue.name() );
        if ( accessIt == mAccessors->constEnd() ) {
            // no accessor for field
            continue;
        }

        // adjust time to local system
        if ( namedValue.name() == "date_modified" ||
                namedValue.name() == "date_entered" ) {
            (*accessIt)->setter( adjustedTime(namedValue.value()), campaign );
            continue;
        }
        (*accessIt)->setter( namedValue.value(), campaign );
    }
    item.setPayload<SugarCampaign>( campaign );
    item.setRemoteRevision( getDateModified( campaign ) );

    return item;
}

void CampaignsHandler::compare( Akonadi::AbstractDifferencesReporter *reporter,
                                const Akonadi::Item &leftItem, const Akonadi::Item &rightItem )
{
    Q_ASSERT( leftItem.hasPayload<SugarCampaign>() );
    Q_ASSERT( rightItem.hasPayload<SugarCampaign>() );

    const SugarCampaign leftCampaign = leftItem.payload<SugarCampaign>();
    const SugarCampaign rightCampaign = rightItem.payload<SugarCampaign>();

    const QString modifiedBy = getModifiedByName( rightCampaign );
    // TODO should get date and format it using KLocale
    const QString modifiedOn = getDateModified( rightCampaign );

    reporter->setLeftPropertyValueTitle( i18nc( "@title:column", "Local Campaign" ) );
    reporter->setRightPropertyValueTitle(
        i18nc( "@title:column", "Serverside Campaign: modified by %1 on %2",
               modifiedBy, modifiedOn ) );

    AccessorHash::const_iterator it    = mAccessors->constBegin();
    AccessorHash::const_iterator endIt = mAccessors->constEnd();
    for ( ; it != endIt; ++it ) {
        // check if this is a read-only field
        if ( (*it)->getter == 0 ) {
            continue;
        }

        const QString diffName = (*it)->diffName;
        if ( diffName.isEmpty() ) {
            // TODO some fields like currency_id should be handled as special fields instead
            // i.e. currency string, dates formatted with KLocale
            continue;
        }

        const QString leftValue = (*it)->getter( leftCampaign );
        const QString rightValue = (*it)->getter( rightCampaign );

        if ( leftValue.isEmpty() && rightValue.isEmpty() ) {
            continue;
        }

        if ( leftValue.isEmpty() ) {
            reporter->addProperty( Akonadi::AbstractDifferencesReporter::AdditionalRightMode,
                                   diffName, leftValue, rightValue );
        } else if ( rightValue.isEmpty() ) {
            reporter->addProperty( Akonadi::AbstractDifferencesReporter::AdditionalLeftMode,
                                   diffName, leftValue, rightValue );
        } else if ( leftValue != rightValue ) {
            reporter->addProperty( Akonadi::AbstractDifferencesReporter::ConflictMode,
                                   diffName, leftValue, rightValue );
        }
    }
}

QString CampaignsHandler::adjustedTime( const QString dateTime ) const
{
    QVariant var = QVariant( dateTime );
    QDateTime dt = var.toDateTime();
    QDateTime system =  QDateTime::currentDateTime();
    QDateTime utctime( system );
    utctime.setTimeSpec( Qt::OffsetFromUTC );
    return dt.addSecs( system.secsTo( utctime ) ).toString("yyyy-MM-dd hh:mm");
}
