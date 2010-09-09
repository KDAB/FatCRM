#include "opportunitieshandler.h"

#include "sugarsoap.h"

#include <akonadi/collection.h>

#include <akonadi/abstractdifferencesreporter.h>
#include <kdcrmdata/sugaropportunity.h>

#include <KLocale>

#include <QHash>

typedef QString (*valueGetter)( const SugarOpportunity& );
typedef void (*valueSetter)( const QString&, SugarOpportunity&);

static void setId( const QString &value, SugarOpportunity &opportunity )
{
    opportunity.setId( value );
}

static QString getName( const SugarOpportunity &opportunity )
{
    return opportunity.name();
}

static void setName( const QString &value, SugarOpportunity &opportunity )
{
    opportunity.setName( value );
}

static QString getDateEntered( const SugarOpportunity &opportunity )
{
    return opportunity.dateEntered();
}

static void setDateEntered( const QString &value, SugarOpportunity &opportunity )
{
    opportunity.setDateEntered( value );
}

static QString getDateModified( const SugarOpportunity &opportunity )
{
    return opportunity.dateModified();
}

static void setDateModified( const QString &value, SugarOpportunity &opportunity )
{
    opportunity.setDateModified( value );
}

static QString getModifiedUserId( const SugarOpportunity &opportunity )
{
    return opportunity.modifiedUserId();
}

static void setModifiedUserId( const QString &value, SugarOpportunity &opportunity )
{
    opportunity.setModifiedUserId( value );
}

static QString getModifiedByName( const SugarOpportunity &opportunity )
{
    return opportunity.modifiedByName();
}

static void setModifiedByName( const QString &value, SugarOpportunity &opportunity )
{
    opportunity.setModifiedByName( value );
}

static QString getCreatedBy( const SugarOpportunity &opportunity )
{
    return opportunity.createdBy();
}

static void setCreatedBy( const QString &value, SugarOpportunity &opportunity )
{
    opportunity.setCreatedBy( value );
}

static QString getCreatedByName( const SugarOpportunity &opportunity )
{
    return opportunity.createdByName();
}

static void setCreatedByName( const QString &value, SugarOpportunity &opportunity )
{
    opportunity.setCreatedByName( value );
}

static QString getDescription( const SugarOpportunity &opportunity )
{
    return opportunity.description();
}

static void setDescription( const QString &value, SugarOpportunity &opportunity )
{
    opportunity.setDescription( value );
}

static QString getDeleted( const SugarOpportunity &opportunity )
{
    return opportunity.deleted();
}

static void setDeleted (const QString &value, SugarOpportunity &opportunity )
{
    opportunity.setDeleted( value );
}

static QString getAssignedUserId( const SugarOpportunity &opportunity )
{
    return opportunity.assignedUserId();
}

static void setAssignedUserId(const QString &value, SugarOpportunity &opportunity)
{
    opportunity.setAssignedUserId( value );
}

static QString getAssignedUserName( const SugarOpportunity &opportunity )
{
    return opportunity.assignedUserName();
}

static void setAssignedUserName(const QString &value, SugarOpportunity &opportunity)
{
    opportunity.setAssignedUserName( value );
}

static QString getOpportunityType( const SugarOpportunity &opportunity )
{
    return opportunity.opportunityType();
}

static void setOpportunityType(const QString &value, SugarOpportunity &opportunity)
{
    opportunity.setOpportunityType( value );
}

static QString getAccountName( const SugarOpportunity &opportunity )
{
    return opportunity.accountName();
}

static void setAccountName(const QString &value, SugarOpportunity &opportunity)
{
    opportunity.setAccountName( value );
}

static QString getAccountId( const SugarOpportunity &opportunity )
{
    return opportunity.accountId();
}

static void setAccountId(const QString &value, SugarOpportunity &opportunity)
{
    opportunity.setAccountId( value );
}

static QString getCampaignId( const SugarOpportunity &opportunity )
{
    return opportunity.campaignId();
}

static void setCampaignId(const QString &value, SugarOpportunity &opportunity)
{
    opportunity.setCampaignId( value );
}

static QString getCampaignName( const SugarOpportunity &opportunity )
{
    return opportunity.campaignName();
}

static void setCampaignName(const QString &value, SugarOpportunity &opportunity)
{
    opportunity.setCampaignName( value );
}

static QString getLeadSource( const SugarOpportunity &opportunity )
{
    return opportunity.leadSource();
}

static void setLeadSource(const QString &value, SugarOpportunity &opportunity)
{
    opportunity.setLeadSource( value );
}

static QString getAmount( const SugarOpportunity &opportunity )
{
    return opportunity.amount();
}

static void setAmount(const QString &value, SugarOpportunity &opportunity)
{
    opportunity.setAmount( value );
}

static QString getAmountUsDollar( const SugarOpportunity &opportunity )
{
    return opportunity.amountUsDollar();
}

static void setAmountUsDollar(const QString &value, SugarOpportunity &opportunity)
{
    opportunity.setAmountUsDollar( value );
}

static QString getCurrencyId( const SugarOpportunity &opportunity )
{
    return opportunity.currencyId();
}

static void setCurrencyId(const QString &value, SugarOpportunity &opportunity)
{
    opportunity.setCurrencyId( value );
}

static QString getCurrencyName( const SugarOpportunity &opportunity )
{
    return opportunity.currencyName();
}

static void setCurrencyName(const QString &value, SugarOpportunity &opportunity)
{
    opportunity.setCurrencyName( value );
}

static QString getCurrencySymbol( const SugarOpportunity &opportunity )
{
    return opportunity.currencySymbol();
}

static void setCurrencySymbol(const QString &value, SugarOpportunity &opportunity)
{
    opportunity.setCurrencySymbol( value );
}

static QString getDateClosed( const SugarOpportunity &opportunity )
{
    return opportunity.dateClosed();
}

static void setDateClosed(const QString &value, SugarOpportunity &opportunity)
{
    opportunity.setDateClosed( value );
}

static QString getNextStep( const SugarOpportunity &opportunity )
{
    return opportunity.nextStep();
}

static void setNextStep(const QString &value, SugarOpportunity &opportunity)
{
    opportunity.setNextStep( value );
}

static QString getSalesStage( const SugarOpportunity &opportunity )
{
    return opportunity.salesStage();
}

static void setSalesStage(const QString &value, SugarOpportunity &opportunity)
{
    opportunity.setSalesStage( value );
}

static QString getProbability( const SugarOpportunity &opportunity )
{
    return opportunity.probability();
}

static void setProbability(const QString &value, SugarOpportunity &opportunity)
{
    opportunity.setProbability( value );
}


class AccessorPair
{
public:
    AccessorPair( valueGetter get, valueSetter set ) : getter( get ), setter( set ){}

public:
    valueGetter getter;
    valueSetter setter;
};



OpportunitiesHandler::OpportunitiesHandler()
    : ModuleHandler( QLatin1String( "Opportunities" ) ),
      mAccessors( new AccessorHash )
{
    mAccessors->insert( QLatin1String( "id" ), AccessorPair( 0, setId ) );
    mAccessors->insert( QLatin1String( "name" ), AccessorPair( getName, setName ) );
    mAccessors->insert( QLatin1String( "date_entered" ), AccessorPair( getDateEntered, setDateEntered ) );
    mAccessors->insert( QLatin1String( "date_modified" ), AccessorPair( getDateModified, setDateModified ) );
    mAccessors->insert( QLatin1String( "modified_user_id" ), AccessorPair( getModifiedUserId, setModifiedUserId ) );
    mAccessors->insert( QLatin1String( "modified_by_name" ), AccessorPair( getModifiedByName, setModifiedByName ) );
    mAccessors->insert( QLatin1String( "created_by" ), AccessorPair( getCreatedBy, setCreatedBy ) );
    mAccessors->insert( QLatin1String( "created_by_name" ), AccessorPair( getCreatedByName, setCreatedByName ) );
    mAccessors->insert( QLatin1String( "description" ), AccessorPair( getDescription, setDescription ) );
    mAccessors->insert( QLatin1String( "deleted" ), AccessorPair( getDeleted, setDeleted ) );
    mAccessors->insert( QLatin1String( "assigned_user_id" ), AccessorPair( getAssignedUserId, setAssignedUserId ) );
    mAccessors->insert( QLatin1String( "assigned_user_name" ), AccessorPair( getAssignedUserName, setAssignedUserName ) );
    mAccessors->insert( QLatin1String( "opportunity_type" ), AccessorPair( getOpportunityType, setOpportunityType ) );
    mAccessors->insert( QLatin1String( "account_name" ), AccessorPair( getAccountName, setAccountName ) );
    mAccessors->insert( QLatin1String( "account_id" ), AccessorPair( getAccountId, setAccountId ) );
    mAccessors->insert( QLatin1String( "campaign_id" ), AccessorPair( getCampaignId, setCampaignId ) );
    mAccessors->insert( QLatin1String( "campaign_name" ), AccessorPair( getCampaignName, setCampaignName ) );
    mAccessors->insert( QLatin1String( "lead_source" ), AccessorPair( getLeadSource, setLeadSource ) );
    mAccessors->insert( QLatin1String( "amount" ), AccessorPair( getAmount, setAmount ) );
    mAccessors->insert( QLatin1String( "amount_usdollar" ), AccessorPair( getAmountUsDollar, setAmountUsDollar ) );
    mAccessors->insert( QLatin1String( "currency_id" ), AccessorPair( getCurrencyId, setCurrencyId ) );
    mAccessors->insert( QLatin1String( "currency_name" ), AccessorPair( getCurrencyName, setCurrencyName ) );
    mAccessors->insert( QLatin1String( "currency_symbol" ), AccessorPair( getCurrencySymbol, setCurrencySymbol ) );
    mAccessors->insert( QLatin1String( "date_closed" ), AccessorPair( getDateClosed, setDateClosed ) );
    mAccessors->insert( QLatin1String( "next_step" ), AccessorPair( getNextStep, setNextStep ) );
    mAccessors->insert( QLatin1String( "sales_stage" ), AccessorPair( getSalesStage, setSalesStage ) );
    mAccessors->insert( QLatin1String( "probability" ), AccessorPair( getProbability, setProbability ) );
}

OpportunitiesHandler::~OpportunitiesHandler()
{
    delete mAccessors;
}

QStringList OpportunitiesHandler::supportedFields() const
{
    return mAccessors->keys();
}

Akonadi::Collection OpportunitiesHandler::collection() const
{
    Akonadi::Collection accountCollection;
    accountCollection.setRemoteId( moduleName() );
    accountCollection.setContentMimeTypes( QStringList() << SugarOpportunity::mimeType() );
    accountCollection.setName( i18nc( "@item folder name", "Opportunities" ) );
    accountCollection.setRights( Akonadi::Collection::CanChangeItem |
                                 Akonadi::Collection::CanCreateItem |
                                 Akonadi::Collection::CanDeleteItem );

    return accountCollection;
}

void OpportunitiesHandler::listEntries( int offset, Sugarsoap* soap, const QString &sessionId )
{
    const QString query = QLatin1String( "" );
    const QString orderBy = QLatin1String( "opportunities.name" );
    const int maxResults = 100;
    const int fetchDeleted = 0; // do not fetch deleted items

    TNS__Select_fields selectedFields;
    selectedFields.setItems( mAccessors->keys() );

    soap->asyncGet_entry_list( sessionId, moduleName(), query, orderBy, offset, selectedFields, maxResults, fetchDeleted );
}

bool OpportunitiesHandler::setEntry( const Akonadi::Item &item, Sugarsoap *soap, const QString &sessionId )
{
    if ( !item.hasPayload<SugarOpportunity>() ) {
        kError() << "item (id=" << item.id() << ", remoteId=" << item.remoteId()
                 << ", mime=" << item.mimeType() << ") is missing Opportunity payload";
        return false;
    }

    QList<TNS__Name_value> itemList;

    // if there is an id add it, otherwise skip this field
    // no id will result in the account being added
    if ( !item.remoteId().isEmpty() ) {
        TNS__Name_value field;
        field.setName( QLatin1String( "id" ) );
        field.setValue( item.remoteId() );

        itemList << field;
    }

    const SugarOpportunity account = item.payload<SugarOpportunity>();
    AccessorHash::const_iterator it    = mAccessors->constBegin();
    AccessorHash::const_iterator endIt = mAccessors->constEnd();
    for ( ; it != endIt; ++it ) {
        // check if this is a read-only field
        if ( it->getter == 0 ) {
            continue;
        }
        TNS__Name_value field;
        field.setName( it.key() );
        field.setValue( it->getter( account ) );
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

Akonadi::Item::List OpportunitiesHandler::itemsFromListEntriesResponse( const TNS__Entry_list &entryList,
                                                                   const Akonadi::Collection &parentCollection )
{
    Akonadi::Item::List items;

    Q_FOREACH( const TNS__Entry_value &entry, entryList.items() ) {
        const QList<TNS__Name_value> valueList = entry.name_value_list().items();
        if ( valueList.isEmpty() ) {
            kWarning() << "Opportunities entry for id=" << entry.id() << "has no values";
            continue;
        }

        Akonadi::Item item;
        item.setRemoteId( entry.id() );
        item.setParentCollection( parentCollection );
        item.setMimeType( SugarOpportunity::mimeType() );

        SugarOpportunity account;
        account.setId( entry.id() );
        Q_FOREACH( const TNS__Name_value &namedValue, valueList ) {
            const AccessorHash::const_iterator accessIt = mAccessors->constFind( namedValue.name() );
            if ( accessIt == mAccessors->constEnd() ) {
                // no accessor for field
                continue;
            }

            // adjust time to local system
            if ( namedValue.name() == "date_modified" ||
                 namedValue.name() == "date_entered" ) {
                accessIt->setter( adjustedTime(namedValue.value()), account );
                continue;
            }
            accessIt->setter( namedValue.value(), account );
        }
        item.setPayload<SugarOpportunity>( account );
        item.setRemoteRevision( getDateModified( account ) );
        items << item;
    }

    return items;
}

void OpportunitiesHandler::compare( Akonadi::AbstractDifferencesReporter *reporter,
                                    const Akonadi::Item &leftItem, const Akonadi::Item &rightItem )
{
    Q_ASSERT( leftItem.hasPayload<SugarOpportunity>() );
    Q_ASSERT( rightItem.hasPayload<SugarOpportunity>() );

    reporter->setLeftPropertyValueTitle( i18nc( "@title:column", "Local Opportunity" ) );
    reporter->setRightPropertyValueTitle( i18nc( "@title:column", "Serverside Opportunity" ) );

    const SugarOpportunity leftOpportunity = leftItem.payload<SugarOpportunity>();
    const SugarOpportunity rightOpportunity = rightItem.payload<SugarOpportunity>();

    AccessorHash::const_iterator it    = mAccessors->constBegin();
    AccessorHash::const_iterator endIt = mAccessors->constEnd();
    for ( ; it != endIt; ++it ) {
        // check if this is a read-only field
        if ( it->getter == 0 ) {
            continue;
        }

        const QString leftValue = it->getter( leftOpportunity );
        const QString rightValue = it->getter( rightOpportunity );

        if ( leftValue.isEmpty() && rightValue.isEmpty() ) {
            continue;
        }

        if ( leftValue.isEmpty() ) {
            reporter->addProperty( Akonadi::AbstractDifferencesReporter::AdditionalRightMode,
                                   it.key(), leftValue, rightValue );
        } else if ( rightValue.isEmpty() ) {
            reporter->addProperty( Akonadi::AbstractDifferencesReporter::AdditionalLeftMode,
                                   it.key(), leftValue, rightValue );
        } else if ( leftValue == rightValue ) {
            reporter->addProperty( Akonadi::AbstractDifferencesReporter::NormalMode,
                                   it.key(), leftValue, rightValue );
        } else if ( leftValue != rightValue ) {
            reporter->addProperty( Akonadi::AbstractDifferencesReporter::ConflictMode,
                                   it.key(), leftValue, rightValue );
        }
    }
}

QString OpportunitiesHandler::adjustedTime( const QString dateTime ) const
{
    QVariant var = QVariant( dateTime );
    QDateTime dt = var.toDateTime();
    QDateTime system =  QDateTime::currentDateTime();
    QDateTime utctime( system );
    utctime.setTimeSpec( Qt::OffsetFromUTC );
    return dt.addSecs( system.secsTo( utctime ) ).toString("yyyy-MM-dd hh:mm");
}
