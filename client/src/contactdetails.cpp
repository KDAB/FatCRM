#include "contactdetails.h"

#include <akonadi/item.h>

#include <kabc/addressee.h>
#include <kabc/address.h>

#include <QMouseEvent>

using namespace Akonadi;


EditCalendarButton::EditCalendarButton( QWidget *parent )
    : QToolButton( parent ), calendar(new QCalendarWidget())
{
    setText( tr( "&Edit" ) );
    setEnabled( false );
}

EditCalendarButton::~EditCalendarButton()
{
    delete calendar;
}


void EditCalendarButton::mousePressEvent( QMouseEvent* e )
{
    if ( calendar->isVisible() )
        calendar->hide();
    else {
        calendar->move( e->globalPos() );
        calendar->show();
        calendar->raise();
    }
}

ContactDetails::ContactDetails( QWidget *parent )
    : QWidget( parent )

{
    mUi.setupUi( this );
    initialize();
}

ContactDetails::~ContactDetails()
{
    delete calendarButton;
}

void ContactDetails::initialize()
{
    QList<QLineEdit*> lineEdits =
        mUi.contactInformationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits )
        le->setReadOnly( true );
    mUi.description->setReadOnly( true );

    calendarButton = new EditCalendarButton(this);
    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget( calendarButton );
    mUi.calendarWidget->setLayout( buttonLayout );
    connect( calendarButton->calendarWidget(), SIGNAL( selectionChanged() ),
             this, SLOT( slotSetBirthday() ) );

    mModifyFlag = false;
    connect( mUi.saveButton, SIGNAL( clicked() ),
             this, SLOT( slotSaveContact() ) );
}

void ContactDetails::setItem (const Item &item )
{
    // contact info
    const KABC::Addressee addressee = item.payload<KABC::Addressee>();
    mUi.firstName->setText( addressee.givenName() );
    mUi.lastName->setText( addressee.familyName() );
    mUi.title->setText( addressee.title() );
    mUi.department->setText( addressee.department() );
    mUi.accountName->setText( addressee.organization() );
    mUi.primaryEmail->setText( addressee.preferredEmail() );
    mUi.homePhone->setText(addressee.phoneNumber( KABC::PhoneNumber::Home ).number() );
    mUi.mobilePhone->setText( addressee.phoneNumber( KABC::PhoneNumber::Cell ).number() );
    mUi.officePhone->setText( addressee.phoneNumber( KABC::PhoneNumber::Work ).number() );
    mUi.otherPhone->setText( addressee.phoneNumber( KABC::PhoneNumber::Car ).number() );
    mUi.fax->setText( addressee.phoneNumber( KABC::PhoneNumber::Fax ).number() );

    const KABC::Address address = addressee.address( KABC::Address::Work|KABC::Address::Pref);
    mUi.primaryAddress->setText( address.street() );
    mUi.city->setText( address.locality() );
    mUi.state->setText( address.region() );
    mUi.postalCode->setText( address.postalCode() );
    mUi.country->setText( address.country() );

    const KABC::Address other = addressee.address( KABC::Address::Home );
    mUi.otherAddress->setText( other.street() );
    mUi.otherCity->setText( other.locality() );
    mUi.otherState->setText( other.region() );
    mUi.otherPostalCode->setText( other.postalCode() );
    mUi.otherCountry->setText( other.country() );
    mUi.birthDate->setText( QDateTime( addressee.birthday() ).date().toString( QString("yyyy-MM-dd" ) ) );
}

void ContactDetails::clearFields ()
{
    QList<QLineEdit*> lineEdits =
        mUi.contactInformationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits )
        if ( !le->text().isEmpty() ) le->clear();
    mUi.description->clear();
    mUi.firstName->setFocus();
}

void ContactDetails::disableFields()
{
    QList<QLineEdit*> lineEdits =
        mUi.contactInformationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits ) {
        le->setReadOnly(true);
    }
    mUi.description->setReadOnly( true );
    mUi.saveButton->setEnabled( false );
    calendarButton->setEnabled( false );
    mModifyFlag = false;
}


void ContactDetails::enableFields()
{
    QList<QLineEdit*> lineEdits =
        mUi.contactInformationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits ) {
        le->setReadOnly(false);
        connect( le, SIGNAL( textChanged( const QString& ) ),
                 this, SLOT( slotEnableSaving() ) );
    }
    mUi.description->setReadOnly( false );
    calendarButton->setEnabled( true );
    connect( mUi.description, SIGNAL( textChanged() ),
             this,  SLOT( slotEnableSaving() ) );
}

void ContactDetails::setModifyFlag()
{
    mModifyFlag = true;
}

void ContactDetails::slotEnableSaving()
{
    mUi.saveButton->setEnabled( true );
}

void ContactDetails::slotSaveContact()
{
   if ( !mContactData.empty() )
       mContactData.clear();

    QList<QLineEdit*> lineEdits =
        mUi.contactInformationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits )
        mContactData[le->objectName()] = le->text();

    mContactData["description"] = mUi.description->toPlainText();

    if ( !mModifyFlag )
        emit saveContact();
    else
        emit modifyContact();
}

void ContactDetails::slotSetBirthday()
{
    mUi.birthDate->setText( calendarButton->calendarWidget()->selectedDate().toString( QString("yyyy-MM-dd" ) ) );

}


