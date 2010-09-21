#include "opportunitieshandler.h"

#include "sugarsession.h"
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
    AccessorPair( valueGetter get, valueSetter set, const QString &name )
        : getter( get ), setter( set ), diffName( name )
    {}

public:
    valueGetter getter;
    valueSetter setter;
    const QString diffName;
};

OpportunitiesHandler::OpportunitiesHandler( SugarSession *session )
    : ModuleHandler( QLatin1String( "Opportunities" ), session ),
      mAccessors( new AccessorHash )
{
    mAccessors->insert( QLatin1String( "id" ),
                        new AccessorPair( 0, setId, QString() ) );
    mAccessors->insert( QLatin1String( "name" ),
                        new AccessorPair( getName, setName,
                                          i18nc( "@item:intable", "Name" ) ) );
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
                        new AccessorPair( getCreatedByName, setCreatedByName,
                                          i18nc( "@item:intable", "Created By" ) ) );
    mAccessors->insert( QLatin1String( "description" ),
                        new AccessorPair( getDescription, setDescription,
                                          i18nc( "@item:intable", "Description" ) ) );
    mAccessors->insert( QLatin1String( "deleted" ),
                        new AccessorPair( getDeleted, setDeleted, QString() ) );
    mAccessors->insert( QLatin1String( "assigned_user_id" ),
                        new AccessorPair( getAssignedUserId, setAssignedUserId, QString() ) );
    mAccessors->insert( QLatin1String( "assigned_user_name" ),
                        new AccessorPair( getAssignedUserName, setAssignedUserName,
                                          i18nc( "@item:intable", "Assigned To" ) ) );
    mAccessors->insert( QLatin1String( "opportunity_type" ),
                        new AccessorPair( getOpportunityType, setOpportunityType,
                                          i18nc( "@item:intable", "Type" ) ) );
    mAccessors->insert( QLatin1String( "account_name" ),
                        new AccessorPair( getAccountName, setAccountName,
                                          i18nc( "@item:intable", "Account" ) ) );
    mAccessors->insert( QLatin1String( "account_id" ),
                        new AccessorPair( getAccountId, setAccountId, QString() ) );
    mAccessors->insert( QLatin1String( "campaign_id" ),
                        new AccessorPair( getCampaignId, setCampaignId, QString() ) );
    mAccessors->insert( QLatin1String( "campaign_name" ),
                        new AccessorPair( getCampaignName, setCampaignName,
                                          i18nc( "@item:intable", "Campaign" ) ) );
    mAccessors->insert( QLatin1String( "lead_source" ),
                        new AccessorPair( getLeadSource, setLeadSource,
                                          i18nc( "@item:intable", "Lead Source" ) ) );
    mAccessors->insert( QLatin1String( "amount" ),
                        new AccessorPair( getAmount, setAmount,
                                          i18nc( "@item:intable", "Amount" ) ) );
    mAccessors->insert( QLatin1String( "amount_usdollar" ),
                        new AccessorPair( getAmountUsDollar, setAmountUsDollar,
                                          i18nc( "@item:intable", "Account in USD" ) ) );
    mAccessors->insert( QLatin1String( "currency_id" ),
                        new AccessorPair( getCurrencyId, setCurrencyId, QString() ) );
    mAccessors->insert( QLatin1String( "currency_name" ),
                        new AccessorPair( getCurrencyName, setCurrencyName,
                                          i18nc( "@item:intable", "Currency" ) ) );
    mAccessors->insert( QLatin1String( "currency_symbol" ),
                        new AccessorPair( getCurrencySymbol, setCurrencySymbol, QString() ) );
    mAccessors->insert( QLatin1String( "date_closed" ),
                        new AccessorPair( getDateClosed, setDateClosed, QString() ) );
    mAccessors->insert( QLatin1String( "next_step" ),
                        new AccessorPair( getNextStep, setNextStep,
                                          i18nc( "@item:intable", "Next Step" ) ) );
    mAccessors->insert( QLatin1String( "sales_stage" ),
                        new AccessorPair( getSalesStage, setSalesStage,
                                          i18nc( "@item:intable", "Sales Stage" ) ) );
    mAccessors->insert( QLatin1String( "probability" ),
                        new AccessorPair( getProbability, setProbability,
                                          i18nc( "@item:intable", "Probability (percent)" ) ) );
}

OpportunitiesHandler::~OpportunitiesHandler()
{
    qDeleteAll( *mAccessors );
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

void OpportunitiesHandler::listEntries( const ListEntriesScope &scope )
{
    const QString query = scope.query( QLatin1String( "opportunities" ) );
    const QString orderBy = QLatin1String( "opportunities.name" );
    const int offset = scope.offset();
    const int maxResults = 100;
    const int fetchDeleted = scope.deleted();

    TNS__Select_fields selectedFields;
    selectedFields.setItems( mAccessors->keys() );

    soap()->asyncGet_entry_list( sessionId(), moduleName(), query, orderBy, offset, selectedFields, maxResults, fetchDeleted );
}

bool OpportunitiesHandler::setEntry( const Akonadi::Item &item )
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
        if ( (*it)->getter == 0 ) {
            continue;
        }
        TNS__Name_value field;
        field.setName( it.key() );
        field.setValue( (*it)->getter( account ) );

        itemList << field;
    }

    TNS__Name_value_list valueList;
    valueList.setItems( itemList );
    soap()->asyncSet_entry( sessionId(), moduleName(), valueList );

    return true;
}

Akonadi::Item OpportunitiesHandler::itemFromEntry( const TNS__Entry_value &entry, const Akonadi::Collection &parentCollection )
{
    Akonadi::Item item;

    const QList<TNS__Name_value> valueList = entry.name_value_list().items();
    if ( valueList.isEmpty() ) {
        kWarning() << "Opportunities entry for id=" << entry.id() << "has no values";
        return item;
    }

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

        (*accessIt)->setter( namedValue.value(), account );
    }
    item.setPayload<SugarOpportunity>( account );
    item.setRemoteRevision( getDateModified( account ) );

    return item;
}

void OpportunitiesHandler::compare( Akonadi::AbstractDifferencesReporter *reporter,
                                    const Akonadi::Item &leftItem, const Akonadi::Item &rightItem )
{
    Q_ASSERT( leftItem.hasPayload<SugarOpportunity>() );
    Q_ASSERT( rightItem.hasPayload<SugarOpportunity>() );

    const SugarOpportunity leftOpportunity = leftItem.payload<SugarOpportunity>();
    const SugarOpportunity rightOpportunity = rightItem.payload<SugarOpportunity>();

    const QString modifiedBy = mSession->userName();
    const QString modifiedOn = formatDate( getDateModified( rightOpportunity ) );

    reporter->setLeftPropertyValueTitle( i18nc( "@title:column", "Local Opportunity" ) );
    reporter->setRightPropertyValueTitle(
        i18nc( "@title:column", "Serverside Opportunity: modified by %1 on %2",
               modifiedBy, modifiedOn ) );

    AccessorHash::const_iterator it    = mAccessors->constBegin();
    AccessorHash::const_iterator endIt = mAccessors->constEnd();
    for ( ; it != endIt; ++it ) {
        // check if this is a read-only field
        if ( (*it)->getter == 0 ) {
            continue;
        }

        QString leftValue = (*it)->getter( leftOpportunity );
        QString rightValue = (*it)->getter( rightOpportunity );

        QString diffName = (*it)->diffName;
        if ( diffName.isEmpty() ) {
            if ( it.key() == "date_closed" ) {
                diffName = i18nc( "@item:intable", "Expected Close Date" );
                leftValue = formatDate( getDateClosed( leftOpportunity ) );
                rightValue = formatDate( getDateClosed( rightOpportunity ) );
            } else {
                // internal field, skip
                continue;
            }
        }

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
