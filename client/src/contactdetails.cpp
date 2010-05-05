#include "contactdetails.h"

#include <akonadi/item.h>

#include <kabc/addressee.h>

using namespace Akonadi;

ContactDetails::ContactDetails( QWidget *parent )
    : QWidget( parent )

{
    mUi.setupUi( this );
    initialize();
}

ContactDetails::~ContactDetails()
{
}

void ContactDetails::initialize()
{
    QList<QLineEdit*> lineEdits =
        mUi.contactInformationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits )
        le->setReadOnly( true );
    mUi.description->setReadOnly( true );
    mModifyFlag = false;
    connect( mUi.saveButton, SIGNAL( clicked() ),
             this, SLOT( slotSaveContact() ) );
}

void ContactDetails::setItem (const Item &item )
{
    const KABC::Addressee addressee = item.payload<KABC::Addressee>();
    mUi.firstName->setText( addressee.givenName() );
    mUi.lastName->setText( addressee.familyName() );
    // Pending Michel ( complete )
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
    // Pending (michel)
    // to be completed
    KABC::Addressee addressee;
    addressee.setGivenName( mUi.firstName->text() );
    addressee.setFamilyName( mUi.lastName->text() );
    Item item;
    item.setMimeType( KABC::Addressee::mimeType() );
    item.setPayload<KABC::Addressee>( addressee );
    if ( !mModifyFlag )
        emit saveContact( item );
    else
        emit modifyContact( item );
}


