#include "details.h"


Details::Details( QWidget *parent )
    : QWidget( parent )

{
    initialize();
}

Details::~Details()
{
}

void Details::initialize()
{
}

QStringList Details::industryItems() const
{
    QStringList industries;
    industries << QString("") << QString( "Apparel" ) << QString( "Banking" )
               << QString( "Biotechnology" ) << QString("Chemicals" )
               << QString("Communications" ) << QString("Construction" )
               << QString("Consulting" ) << QString("Education" )
               << QString("Electronics" )<< QString("Energy" )
               << QString("Engineering" ) << QString("Entertainment" )
               << QString("Environmental" ) << QString("Finance" )
               << QString("Government" ) << QString("Healthcare" )
               << QString("Hospitality" ) << QString("Insurance" )
               << QString("Machinery" ) << QString("Manufacturing" )
               << QString("Media" ) << QString("Not For Profit" )
               << QString("Recreation" ) << QString("Retail" )
               << QString("Shipping" ) << QString("Technology" )
               << QString("Telecommunication" ) << QString("Transportation" )
               << QString("Utilities" ) << QString("Other" );
    return industries;
}


QStringList Details::sourceItems() const
{
    QStringList sources;
    sources << QString("") << QString( "Cold Call" )
            << QString( "Existing Customer" ) << QString( "Self Generated" )
            << QString( "Employee" ) << QString( "Partner" )
            << QString( "Public Relations" ) << QString( "Direct Mail" )
            << QString( "Conference" ) << QString( "Trade Show" )
            << QString( "Web Site" ) << QString( "Word of mouth" )
            << QString( "Email" ) << QString( "Campaign" )
            << QString( "Other" );
    return sources;
}

QStringList Details::currencyItems() const
{
    // we do not have the choice here
    // Should be set remotely by admin
    QStringList currencies;
    currencies << QString( "US Dollars : $" );
    return currencies;
}

QStringList Details::salutationItems() const
{
    QStringList salutations;
    salutations << QString("") << QString( "Mr." )
            << QString( "Ms." ) << QString( "Mrs." )
            << QString( "Dr." ) << QString( "Prof." );
    return salutations;
}
