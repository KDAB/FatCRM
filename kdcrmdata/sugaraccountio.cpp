#include "sugaraccountio.h"
#include "sugaraccount.h"

#include <QtCore/QIODevice>
#include <QtCore/QXmlStreamWriter>
#include <QtCore/QXmlStreamReader>
#include <QtCore/QDebug>

bool SugarAccountIO::readSugarAccount( QIODevice *device, SugarAccount &account )
{
    if ( device == 0 || !device->isReadable() )
        return false;

    account = SugarAccount();

    QXmlStreamReader reader( device );
    while ( !reader.atEnd() ) {
        if ( reader.name() == QString( "id" ) )
            account.setId( reader.readElementText() );
        if ( reader.name() == QString( "name" ) )
            account.setName( reader.readElementText() );
        if ( reader.name() == QString( "date_entered" ) )
            account.setDateEntered( reader.readElementText() );
        if ( reader.name() == QString( "date_modified" ) )
            account.setDateModified( reader.readElementText() );
        if ( reader.name() == QString( "modified_user_id" ) )
            account.setModifiedUserId( reader.readElementText() );
        if ( reader.name() == QString( "modified_by_name" ) )
            account.setModifiedByName( reader.readElementText() );
        if ( reader.name() == QString( "created_by" ) )
            account.setCreatedBy( reader.readElementText() );
        if ( reader.name() == QString( "created_by_name" ) )
            account.setCreatedByName( reader.readElementText() );
        if ( reader.name() == QString( "description" ) )
            account.setDescription( reader.readElementText() );
        if ( reader.name() == QString( "deleted" ) )
            account.setDeleted( reader.readElementText() );
        if ( reader.name() == QString( "assigned_user_id" ) )
            account.setAssignedUserId( reader.readElementText() );
        if ( reader.name() == QString( "assigned_user_name" ) )
            account.setAssignedUserName( reader.readElementText() );
        if ( reader.name() == QString( "account_type" ) )
            account.setAccountType( reader.readElementText() );
        if ( reader.name() == QString( "industry" ) )
            account.setIndustry( reader.readElementText() );
        if ( reader.name() == QString( "annual_revenue" ) )
            account.setAnnualRevenue( reader.readElementText() );
        if ( reader.name() == QString( "phone_fax" ) )
            account.setPhoneFax( reader.readElementText() );
        if ( reader.name() == QString( "billing_address_street" ) )
            account.setBillingAddressStreet( reader.readElementText() );
        if ( reader.name() == QString( "billing_address_city" ) )
            account.setBillingAddressCity( reader.readElementText() );
        if ( reader.name() == QString( "billing_address_state" ) )
            account.setBillingAddressState( reader.readElementText() );
        if ( reader.name() == QString( "billing_address_postalcode" ) )
            account.setBillingAddressPostalcode( reader.readElementText() );
        if ( reader.name() == QString( "billing_address_country" ) )
            account.setBillingAddressCountry( reader.readElementText() );
        if ( reader.name() == QString( "rating" ) )
            account.setRating( reader.readElementText() );
        if ( reader.name() == QString( "phone_office" ) )
            account.setPhoneOffice( reader.readElementText() );
        if ( reader.name() == QString( "phone_alternate" ) )
            account.setPhoneAlternate( reader.readElementText() );
        if ( reader.name() == QString( "website" ) )
            account.setWebsite( reader.readElementText() );
        if ( reader.name() == QString( "ownership" ) )
            account.setOwnership( reader.readElementText() );
        if ( reader.name() == QString( "employees" ) )
            account.setEmployees( reader.readElementText() );
        if ( reader.name() == QString( "tycker_symbol" ) )
            account.setTyckerSymbol( reader.readElementText() );
        if ( reader.name() == QString( "shipping_address_street" ) )
            account.setShippingAddressStreet( reader.readElementText() );
        if ( reader.name() == QString( "shipping_address_city" ) )
            account.setShippingAddressCity( reader.readElementText() );
        if ( reader.name() == QString( "shipping_address_state" ) )
            account.setShippingAddressState( reader.readElementText() );
        if ( reader.name() == QString( "shipping_address_postalcode" ) )
            account.setShippingAddressPostalcode( reader.readElementText() );
        if ( reader.name() == QString( "shipping_address_country" ) )
            account.setShippingAddressCountry( reader.readElementText() );
        if ( reader.name() == QString( "email1" ) )
            account.setEmail1( reader.readElementText() );
        if ( reader.name() == QString( "parent_id" ) )
            account.setParentId( reader.readElementText() );
        if ( reader.name() == QString( "sic_code" ) )
            account.setSicCode( reader.readElementText() );
        if ( reader.name() == QString( "parent_name" ) )
            account.setParentName( reader.readElementText() );
        if ( reader.name() == QString( "campaign_id" ) )
            account.setCampaignId( reader.readElementText() );
        if ( reader.name() == QString( "campaign_name" ) )
            account.setCampaignName( reader.readElementText() );
    }

    return true;
}

bool SugarAccountIO::writeSugarAccount(  const SugarAccount &account, QIODevice *device )
{
    if ( device == 0 || !device->isWritable() )
        return false;

    QXmlStreamWriter writer( device );
    writer.setAutoFormatting( true );
    writer.writeStartDocument();
    writer.writeTextElement( QString( "id" ), account.id() );
    writer.writeTextElement( QString( "name" ), account.name() );
    writer.writeTextElement( QString( "date_entered" ), account.dateEntered() );
    writer.writeTextElement( QString( "date_modified" ), account.dateModified() );
    writer.writeTextElement( QString( "modified_user_id" ), account.modifiedUserId() );
    writer.writeTextElement( QString( "modified_by_name" ), account.modifiedByName() );
    writer.writeTextElement( QString( "created_by" ), account.createdBy() );
    writer.writeTextElement( QString( "created_by_name" ), account.createdByName() );
    writer.writeTextElement( QString( "description" ), account.description() );
    writer.writeTextElement( QString( "deleted" ), account.deleted() );
    writer.writeTextElement( QString( "assigned_user_id" ), account.assignedUserId() );
    writer.writeTextElement( QString( "assigned_user_name" ), account.assignedUserName() );
    writer.writeTextElement( QString( "account_type" ), account.accountType() );
    writer.writeTextElement( QString( "industry" ), account.industry() );
    writer.writeTextElement( QString( "annual_revenue" ), account.annualRevenue() );
    writer.writeTextElement( QString( "phone_fax" ), account.phoneFax() );
    writer.writeTextElement( QString( "billing_address_sQStringeet" ), account.billingAddressStreet() );
    writer.writeTextElement( QString( "billing_address_city" ), account.billingAddressCity() );
    writer.writeTextElement( QString( "billing_address_state" ), account.billingAddressState() );
    writer.writeTextElement( QString( "billing_address_postalcode" ), account.billingAddressPostalcode() );
    writer.writeTextElement( QString( "billing_address_counQStringy" ), account.billingAddressCountry() );
    writer.writeTextElement( QString( "rating" ), account.rating() );
    writer.writeTextElement( QString( "phone_office" ), account.phoneOffice() );
    writer.writeTextElement( QString( "phone_alternate" ), account.phoneAlternate() );
    writer.writeTextElement( QString( "website" ), account.website() );
    writer.writeTextElement( QString( "ownership" ), account.ownership() );
    writer.writeTextElement( QString( "employees" ), account.employees() );
    writer.writeTextElement( QString( "tycker_symbol" ), account.tyckerSymbol() );
    writer.writeTextElement( QString( "shipping_address_sQStringeet" ), account.shippingAddressStreet() );
    writer.writeTextElement( QString( "shipping_address_city" ), account.shippingAddressCity() );
    writer.writeTextElement( QString( "shipping_address_state" ), account.shippingAddressState() );
    writer.writeTextElement( QString( "shipping_address_postalcode" ), account.shippingAddressPostalcode() );
    writer.writeTextElement( QString( "shipping_address_counQStringy" ), account.shippingAddressCountry() );
    writer.writeTextElement( QString( "email1" ), account.email1() );
    writer.writeTextElement( QString( "parent_id" ), account.parentId() );
    writer.writeTextElement( QString( "sic_code" ), account.sicCode() );
    writer.writeTextElement( QString( "parent_name" ), account.parentName() );
    writer.writeTextElement( QString( "campaign_id" ), account.campaignId() );
    writer.writeTextElement( QString( "campaign_name" ), account.campaignName() );
    writer.writeEndDocument();

    return true;
}

