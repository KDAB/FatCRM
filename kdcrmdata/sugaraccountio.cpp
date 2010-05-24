#include "sugaraccountio.h"
#include "sugaraccount.h"

#include <QtCore/QIODevice>
#include <QtCore/QXmlStreamWriter>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>

bool SugarAccountIO::readSugarAccount( QIODevice *device, SugarAccount &account )
{
    if ( device == 0 || !device->isReadable() )
        return false;

    account = SugarAccount();

    QTextStream reader( device );
    QString line;
    do {
        line = reader.readLine();
        if ( line.startsWith( QString( "<id>" ) ) )
            account.setId( readContent( line ) );
        if ( line.startsWith( QString( "<name>" ) ) )
            account.setName( readContent( line ) );
        if ( line.startsWith( QString( "<date_entered>" ) ) )
            account.setDateEntered( readContent( line ) );
        if ( line.startsWith( QString( "<date_modified>" ) ) )
            account.setDateModified( readContent( line ) );
        if ( line.startsWith( QString( "<modified_user_id>" ) ) )
            account.setModifiedUserId( readContent( line ) );
        if ( line.startsWith( QString( "<modified_by_name>" ) ) )
            account.setModifiedByName( readContent( line ) );
        if ( line.startsWith( QString( "<created_by>" ) ) )
            account.setCreatedBy( readContent( line ) );
        if ( line.startsWith( QString( "<created_by_name>" ) ) )
            account.setCreatedByName( readContent( line ) );
        if ( line.startsWith( QString( "<description>" ) ) )
            account.setDescription( readContent( line ) );
        if ( line.startsWith( QString( "<deleted>" ) ) )
            account.setDeleted( readContent( line ) );
        if ( line.startsWith( QString( "<assigned_user_id>" ) ) )
            account.setAssignedUserId( readContent( line ) );
        if ( line.startsWith( QString( "<assigned_user_name>" ) ) )
            account.setAssignedUserName( readContent( line ) );
        if ( line.startsWith( QString( "<account_type>" ) ) )
            account.setAccountType( readContent( line ) );
        if ( line.startsWith( QString( "<industry>" ) ) )
            account.setIndustry( readContent( line ) );
        if ( line.startsWith( QString( "<annual_revenue>" ) ) )
            account.setAnnualRevenue( readContent( line ) );
        if ( line.startsWith( QString( "<phone_fax>" ) ) )
            account.setPhoneFax( readContent( line ) );
        if ( line.startsWith( QString( "<billing_address_street>" ) ) )
            account.setBillingAddressStreet( readContent( line ) );
        if ( line.startsWith( QString( "<billing_address_city>" ) ) )
            account.setBillingAddressCity( readContent( line ) );
        if ( line.startsWith( QString( "<billing_address_state>" ) ) )
            account.setBillingAddressState( readContent( line ) );
        if ( line.startsWith( QString( "<billing_address_postalcode>" ) ) )
            account.setBillingAddressPostalcode( readContent( line ) );
        if ( line.startsWith( QString( "<billing_address_country>" ) ) )
            account.setBillingAddressCountry( readContent( line ) );
        if ( line.startsWith( QString( "<rating>" ) ) )
            account.setRating( readContent( line ) );
        if ( line.startsWith( QString( "<phone_office>" ) ) )
            account.setPhoneOffice( readContent( line ) );
        if ( line.startsWith( QString( "<phone_alternate>" ) ) )
            account.setPhoneAlternate( readContent( line ) );
        if ( line.startsWith( QString( "<website>" ) ) )
            account.setWebsite( readContent( line ) );
        if ( line.startsWith( QString( "<ownership>" ) ) )
            account.setOwnership( readContent( line ) );
        if ( line.startsWith( QString( "<employees>" ) ) )
            account.setEmployees( readContent( line ) );
        if ( line.startsWith( QString( "<tycker_symbol>" ) ) )
            account.setTyckerSymbol( readContent( line ) );
        if ( line.startsWith( QString( "<shipping_address_street>" ) ) )
            account.setShippingAddressStreet( readContent( line ) );
        if ( line.startsWith( QString( "<shipping_address_city>" ) ) )
            account.setShippingAddressCity( readContent( line ) );
        if ( line.startsWith( QString( "<shipping_address_state>" ) ) )
            account.setShippingAddressState( readContent( line ) );
        if ( line.startsWith( QString( "<shipping_address_postalcode>" ) ) )
            account.setShippingAddressPostalcode( readContent( line ) );
        if ( line.startsWith( QString( "<shipping_address_country>" ) ) )
            account.setShippingAddressCountry( readContent( line ) );
        if ( line.startsWith( QString( "<email1>" ) ) )
            account.setEmail1( readContent( line ) );
        if ( line.startsWith( QString( "<parent_id>" ) ) )
            account.setParentId( readContent( line ) );
        if ( line.startsWith( QString( "<sic_code>" ) ) )
            account.setSicCode( readContent( line ) );
        if ( line.startsWith( QString( "<parent_name>" ) ) )
            account.setParentName( readContent( line ) );
        if ( line.startsWith( QString( "<campaign_id>" ) ) )
            account.setCampaignId( readContent( line ) );
        if ( line.startsWith( QString( "<campaign_name>" ) ) )
            account.setCampaignName( readContent( line ) );
    } while ( !line.isNull() );

    return true;
}

const QString SugarAccountIO::readContent( const QString &line )
{
    // extract value <start>value<end>
    QString content,  startElement,  endElement;
    content = line;
    startElement = line;
    int index = content.indexOf( ">" ) + 1;
    startElement.truncate( index );
    content.remove( startElement );
    endElement = startElement.insert( 1, "/" );

    return content.remove( endElement );
}

bool SugarAccountIO::writeSugarAccount(  const SugarAccount &account, QIODevice *device )
{
    if ( device == 0 || !device->isWritable() )        return false;

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
    writer.writeTextElement( QString( "billing_address_street" ), account.billingAddressStreet() );
    writer.writeTextElement( QString( "billing_address_city" ), account.billingAddressCity() );
    writer.writeTextElement( QString( "billing_address_state" ), account.billingAddressState() );
    writer.writeTextElement( QString( "billing_address_postalcode" ), account.billingAddressPostalcode() );
    writer.writeTextElement( QString( "billing_address_country" ), account.billingAddressCountry() );
    writer.writeTextElement( QString( "rating" ), account.rating() );
    writer.writeTextElement( QString( "phone_office" ), account.phoneOffice() );
    writer.writeTextElement( QString( "phone_alternate" ), account.phoneAlternate() );
    writer.writeTextElement( QString( "website" ), account.website() );
    writer.writeTextElement( QString( "ownership" ), account.ownership() );
    writer.writeTextElement( QString( "employees" ), account.employees() );
    writer.writeTextElement( QString( "tycker_symbol" ), account.tyckerSymbol() );
    writer.writeTextElement( QString( "shipping_address_street" ), account.shippingAddressStreet() );
    writer.writeTextElement( QString( "shipping_address_city" ), account.shippingAddressCity() );
    writer.writeTextElement( QString( "shipping_address_state" ), account.shippingAddressState() );
    writer.writeTextElement( QString( "shipping_address_postalcode" ), account.shippingAddressPostalcode() );
    writer.writeTextElement( QString( "shipping_address_country" ), account.shippingAddressCountry() );
    writer.writeTextElement( QString( "email1" ), account.email1() );
    writer.writeTextElement( QString( "parent_id" ), account.parentId() );
    writer.writeTextElement( QString( "sic_code" ), account.sicCode() );
    writer.writeTextElement( QString( "parent_name" ), account.parentName() );
    writer.writeTextElement( QString( "campaign_id" ), account.campaignId() );
    writer.writeTextElement( QString( "campaign_name" ), account.campaignName() );
    writer.writeEndDocument();

    return true;
}

