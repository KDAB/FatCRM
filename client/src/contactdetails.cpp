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
    connect( mUi.description, SIGNAL( textChanged( const QString& ) ),
             this,  SLOT( slotEnableSaving() ) );
}

void ContactDetails::slotEnableSaving()
{
    mUi.saveButton->setEnabled( true );
}

void ContactDetails::slotSaveContact()
{
    qDebug() << "Sorry - ContactDetails::slotSaveContact() - NYI";
}



