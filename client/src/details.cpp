#include "details.h"

using namespace Akonadi;

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

/*
 *  Reset all widgets
 *
 */
void Details::clear()
{
    QList<QLineEdit*> lineEdits =  findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits )
        le->setText( QString() );
    QList<QComboBox*> comboBoxes =  findChildren<QComboBox*>();
    Q_FOREACH( QComboBox* cb, comboBoxes )
        cb->setCurrentIndex( 0 );
    QList<QCheckBox*> checkBoxes =  findChildren<QCheckBox*>();
    Q_FOREACH( QCheckBox* cb, checkBoxes )
        cb->setChecked( false );
    QList<QTextEdit*> textEdits = findChildren<QTextEdit*>();
    Q_FOREACH( QTextEdit* te, textEdits )
        te->setPlainText( QString() );
}

/*
 * Return the list of items for the industry combo boxes
 *
 */
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

/*
 * Return the list of items for the source combo boxes
 *
 */
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

/*
 * Return the list of items for the currency combo boxes
 *
 */
QStringList Details::currencyItems() const
{
    // we do not have the choice here
    // Should be set remotely by admin
    QStringList currencies;
    currencies << QString( "US Dollars : $" );
    return currencies;
}

/*
 * Return the list of items for the salutation combo boxes
 *
 */
QStringList Details::salutationItems() const
{
    QStringList salutations;
    salutations << QString("") << QString( "Mr." )
            << QString( "Ms." ) << QString( "Mrs." )
            << QString( "Dr." ) << QString( "Prof." );
    return salutations;
}
