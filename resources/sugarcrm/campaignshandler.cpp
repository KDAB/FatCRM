#include "campaignshandler.h"

#include "campaignshandler.h"

#include "sugarsoap.h"

#include <akonadi/collection.h>

#include <kdcrmdata/sugarcampaign.h>

#include <KLocale>

#include <QHash>

typedef QString (*valueGetter)( const SugarCampaign& );
typedef void (*valueSetter)( const QString&, SugarCampaign&);

static QString getId( const SugarCampaign &campaign )
{

    return campaign.id();
}

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
    AccessorPair( valueGetter get, valueSetter set ) : getter( get ), setter( set ){}

public:
    valueGetter getter;
    valueSetter setter;
};



CampaignsHandler::CampaignsHandler()
    : ModuleHandler( QLatin1String( "Campaigns" ) ),
      mAccessors( new AccessorHash )
{
    mAccessors->insert( QLatin1String( "id" ), AccessorPair( getId, setId ) );
    mAccessors->insert( QLatin1String( "name" ), AccessorPair( getName, setName ) );
    mAccessors->insert( QLatin1String( "date_entered" ), AccessorPair( getDateEntered, setDateEntered ) );
    mAccessors->insert( QLatin1String( "date_modified" ), AccessorPair( getDateModified, setDateModified ) );
    mAccessors->insert( QLatin1String( "modified_user_id" ), AccessorPair( getModifiedUserId, setModifiedUserId ) );
    mAccessors->insert( QLatin1String( "modified_by_name" ), AccessorPair( getModifiedByName, setModifiedByName ) );
    mAccessors->insert( QLatin1String( "created_by" ), AccessorPair( getCreatedBy, setCreatedBy ) );
    mAccessors->insert( QLatin1String( "created_by_name" ), AccessorPair( getCreatedByName, setCreatedByName ) );
    mAccessors->insert( QLatin1String( "deleted" ), AccessorPair( getDeleted, setDeleted ) );
    mAccessors->insert( QLatin1String( "assigned_user_id" ), AccessorPair( getAssignedUserId, setAssignedUserId ) );
    mAccessors->insert( QLatin1String( "assigned_user_name" ), AccessorPair( getAssignedUserName, setAssignedUserName ) );
    mAccessors->insert( QLatin1String( "tracker_key" ), AccessorPair( getTrackerKey, setTrackerKey ) );
    mAccessors->insert( QLatin1String( "tracker_count" ), AccessorPair( getTrackerCount, setTrackerCount ) );
    mAccessors->insert( QLatin1String( "refer_url" ), AccessorPair( getReferUrl, setReferUrl ) );
    mAccessors->insert( QLatin1String( "tracker_text" ), AccessorPair( getTrackerText, setTrackerText ) );
    mAccessors->insert( QLatin1String( "start_date" ), AccessorPair( getStartDate, setStartDate ) );
    mAccessors->insert( QLatin1String( "end_date" ), AccessorPair( getEndDate, setEndDate ) );
    mAccessors->insert( QLatin1String( "status" ), AccessorPair( getStatus, setStatus ) );
    mAccessors->insert( QLatin1String( "impressions" ), AccessorPair( getImpressions, setImpressions ) );
    mAccessors->insert( QLatin1String( "currency_id" ), AccessorPair( getCurrencyId, setCurrencyId ) );
    mAccessors->insert( QLatin1String( "budget" ), AccessorPair( getBudget, setBudget ) );
    mAccessors->insert( QLatin1String( "expected_cost" ), AccessorPair( getExpectedCost, setExpectedCost ) );
    mAccessors->insert( QLatin1String( "actual_cost" ), AccessorPair( getActualCost, setActualCost ) );
    mAccessors->insert( QLatin1String( "expected_revenue" ), AccessorPair( getExpectedRevenue, setExpectedRevenue ) );
    mAccessors->insert( QLatin1String( "campaign_type" ), AccessorPair( getCampaignType, setCampaignType ) );
    mAccessors->insert( QLatin1String( "objective" ), AccessorPair( getObjective, setObjective ) );
    mAccessors->insert( QLatin1String( "content" ), AccessorPair( getContent, setContent ) );
    mAccessors->insert( QLatin1String( "frequency" ), AccessorPair( getFrequency, setFrequency ) );
}

CampaignsHandler::~CampaignsHandler()
{
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
        TNS__Name_value field;
        field.setName( it.key() );
        field.setValue( it->getter( campaign ) );
        itemList << field;
    }

    TNS__Name_value_list valueList;
    valueList.setItems( itemList );
    soap->asyncSet_entry( sessionId, moduleName(), valueList );

    return true;
}

Akonadi::Item::List CampaignsHandler::itemsFromListEntriesResponse( const TNS__Entry_list &entryList,
                                                                   const Akonadi::Collection &parentCollection )
{
    Akonadi::Item::List items;

    Q_FOREACH( const TNS__Entry_value &entry, entryList.items() ) {
        const QList<TNS__Name_value> valueList = entry.name_value_list().items();
        if ( valueList.isEmpty() ) {
            kWarning() << "Campaigns entry for id=" << entry.id() << "has no values";
            continue;
        }

        Akonadi::Item item;
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
            accessIt->setter( namedValue.value(), campaign );
        }
        item.setPayload<SugarCampaign>( campaign );
        item.setRemoteRevision( getDateModified( campaign ) );
        items << item;
    }

    return items;
}


