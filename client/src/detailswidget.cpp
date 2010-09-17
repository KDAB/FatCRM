#include "detailswidget.h"
#include "sugarclient.h"
#include "accountdetails.h"
#include "leaddetails.h"
#include "contactdetails.h"
#include "opportunitydetails.h"
#include "campaigndetails.h"

#include <akonadi/item.h>

using namespace Akonadi;

DetailsWidget::DetailsWidget( DetailsType type, QWidget* parent )
    : QWidget( parent ), mType( type ), mEditing( false )
{
    mUi.setupUi( this );
    initialize();
}

DetailsWidget::~DetailsWidget()
{

}

void DetailsWidget::initialize()
{
    if ( mUi.detailsContainer->layout() )
        delete mUi.detailsContainer->layout();

    switch( mType ) {
    case Account:
    {
        mDetails = new AccountDetails;
        break;
    }
    case Opportunity:
    {
        mDetails = new OpportunityDetails;
        break;
    }
    case Contact:
    {
        mDetails = new ContactDetails;
        break;
    }
    case Lead:
    {
        mDetails = new LeadDetails;
        break;
    }
    case Campaign:
    {
        mDetails = new CampaignDetails;
        break;
    }
    default:
        return;
    }

    setConnections();

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget( mDetails );
    mUi.detailsContainer->setLayout( hlayout );

}

void DetailsWidget::setConnections()
{
    // Pending(michel ) - ToDo CalendarWidget - checkBoxes
    QList<QLineEdit*> lineEdits =  mDetails->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits )
        connect( le, SIGNAL( textChanged( const QString& ) ),
                 this, SLOT( slotEnableSaving() ) );
    QList<QComboBox*> comboBoxes =  mDetails->findChildren<QComboBox*>();
    Q_FOREACH( QComboBox* cb, comboBoxes )
        connect( cb, SIGNAL( currentIndexChanged( int ) ),
                    this, SLOT( slotEnableSaving() ) );
    QList<QCheckBox*> checkBoxes =  mDetails->findChildren<QCheckBox*>();
    Q_FOREACH( QCheckBox* cb, checkBoxes )
        connect( cb, SIGNAL( toggled( bool ) ),
                    this, SLOT( slotEnableSaving() ) );
    QList<QTextEdit*> textEdits = mDetails->findChildren<QTextEdit*>();
    Q_FOREACH( QTextEdit* te, textEdits )
        connect( te, SIGNAL( textChanged() ),
                    this, SLOT( slotEnableSaving() ) );
    connect( mUi.description, SIGNAL( textChanged() ),
             this,  SLOT( slotEnableSaving() ) );
    connect( mUi.saveButton, SIGNAL( clicked() ),
             this, SLOT( slotSaveData() ) );
    mUi.saveButton->setEnabled( false );

}

void DetailsWidget::reset()
{
    // Pending(michel ) - ToDo CalendarWidget - checkBoxes
    QList<QLineEdit*> lineEdits =  mDetails->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits )
        disconnect( le, SIGNAL( textChanged( const QString& ) ),
                 this, SLOT( slotEnableSaving() ) );
    QList<QComboBox*> comboBoxes =  mDetails->findChildren<QComboBox*>();
    Q_FOREACH( QComboBox* cb, comboBoxes )
        disconnect( cb, SIGNAL( currentIndexChanged( int ) ),
                    this, SLOT( slotEnableSaving() ) );
    QList<QCheckBox*> checkBoxes =  mDetails->findChildren<QCheckBox*>();
    Q_FOREACH( QCheckBox* cb, checkBoxes )
        disconnect( cb, SIGNAL( toggled( bool ) ),
                    this, SLOT( slotEnableSaving() ) );
    QList<QTextEdit*> textEdits = mDetails->findChildren<QTextEdit*>();
    Q_FOREACH( QTextEdit* te, textEdits )
        disconnect( te, SIGNAL( textChanged() ),
                    this, SLOT( slotEnableSaving() ) );
    disconnect( mUi.description, SIGNAL( textChanged() ),
                this,  SLOT( slotEnableSaving() ) );
    mUi.saveButton->setEnabled( false );
}

void DetailsWidget::clearFields ()
{
    mDetails->clear();

    // reset this - label and properties
    QList<QLabel*> labels = mUi.informationGB->findChildren<QLabel*>();
    Q_FOREACH( QLabel* lab, labels ) {
        QString value = lab->objectName();
        if ( value == "modifiedBy" ) {
            lab->clear();
            lab->setProperty( "modifiedUserId", qVariantFromValue<QString>( QString() ) );
            lab->setProperty( "modifiedUserName", qVariantFromValue<QString>( QString() ) );
        }
        else if ( value == "dateEntered" ) {
            lab->clear();
            lab->setProperty( "id", qVariantFromValue<QString>( QString() ) );
            lab->setProperty( "deleted", qVariantFromValue<QString>( QString() ) );

            lab->setProperty( "contactId", qVariantFromValue<QString>( QString() ) );
            lab->setProperty( "opportunityRoleFields", qVariantFromValue<QString>( QString() ) );
            lab->setProperty( "cAcceptStatusFields",  qVariantFromValue<QString>( QString() ) );
            lab->setProperty( "mAcceptStatusFields",  qVariantFromValue<QString>( QString() ) );
        }
        else if ( value == "createdBy" ) {
            lab->clear();
            lab->setProperty( "createdBy", qVariantFromValue<QString>( QString() ) );
        }
    }

    // initialize other fields
    mUi.description->setPlainText( QString() );

    // we are creating a new account
    slotSetModifyFlag( false );
}

void DetailsWidget::setItem (const Item &item )
{
    // new item selected reset flag and saving
    mModifyFlag = true;
    reset();
    QMap<QString, QString> data;
    data = mDetails->data( item );
    setData( data );
    setConnections();
}

void DetailsWidget::setData( QMap<QString, QString> data )
{
    QString key;

    QList<QLineEdit*> lineEdits =  mUi.informationGB->findChildren<QLineEdit*>();

    Q_FOREACH( QLineEdit* le, lineEdits ) {
        key = le->objectName();
        le->setText( data.value( key ) );
    }
    QList<QComboBox*> comboBoxes =  mUi.informationGB->findChildren<QComboBox*>();
    Q_FOREACH( QComboBox* cb, comboBoxes ) {
        key = cb->objectName();
        cb->setCurrentIndex( cb->findText( data.value( key ) ) );
        // currency is unique an cannot be changed from the client atm
        if ( key == "currency" ) {
            cb->setCurrentIndex( 0 ); // default
            cb->setProperty( "currencyId",
                               qVariantFromValue<QString>( data.value( "currencyId" ) ) );
            cb->setProperty( "currencyName",
                               qVariantFromValue<QString>( data.value( "currencyName" ) ) );
            cb->setProperty( "currencySymbol",
                               qVariantFromValue<QString>( data.value( "currencySymbol" ) ) );
        }
    }
    QList<QTextEdit*> textEdits = mUi.informationGB->findChildren<QTextEdit*>();
    Q_FOREACH( QTextEdit* te, textEdits ) {
        key = te->objectName();
        te->setPlainText( data.value( key ) );
    }
    QList<QLabel*> labels = mUi.informationGB->findChildren<QLabel*>();
    Q_FOREACH( QLabel* lb, labels ) {
        key = lb->objectName();
        if ( key == "dateEntered" )
            lb->setText( data.value( "dateEntered" ) );
        if ( key == "modifiedBy" ) {
            if ( !data.value( "modifiedByName" ).isEmpty() )
                lb->setText( data.value( "modifiedByName" ) );
            else if ( !data.value( "modifiedBy" ).isEmpty() )
                lb->setText( data.value( "modifiedBy" ) );
            else
                lb->setText( data.value( "modifiedUserName" ) );

            lb->setProperty( "modifiedUserId",
                             qVariantFromValue<QString>( data.value( "modifiedUserId" ) ) );
            lb->setProperty( "modifiedUserName",
                             qVariantFromValue<QString>( data.value( "modifiedUserName" ) ) );
        }
        if ( key == "dateEntered" ) {
            lb->setText( data.value( "dateEntered" ) );
            lb->setProperty( "deleted",
                             qVariantFromValue<QString>( data.value( "deleted" ) ) );
            lb->setProperty( "id",
                             qVariantFromValue<QString>( data.value("id" ) ) );
            lb->setProperty( "contactId",
                             qVariantFromValue<QString>( data.value( "contactId" ) ) );
            lb->setProperty( "opportunityRoleFields",
                             qVariantFromValue<QString>( data.value( "opportunityRoleFields" ) ) );
            lb->setProperty( "cAcceptStatusFields",
                             qVariantFromValue<QString>( data.value( "cAcceptStatusFields" ) ) );
            lb->setProperty( "mAcceptStatusFields",
                             qVariantFromValue<QString>( data.value( "mAcceptStatusFields" ) ) );
        }
        if ( key == "createdBy" ) {
            if ( !data.value( "createdByName" ).isEmpty() )
                lb->setText( data.value( "createdByName" ) );
            else
                lb->setText( data.value( "createdBy" ) );
            lb->setProperty( "createdBy",
                             qVariantFromValue<QString>( data.value("createdBy" ) ) );
            lb->setProperty( "createdById",
                             qVariantFromValue<QString>( data.value( "createdById" ) ) );
        }
    }
    mUi.description->setPlainText( ( mType != Campaign )?
                                   data.value( "description" ):
                                   data.value( "content") );
}

QMap<QString, QString> DetailsWidget::data()
{
    QMap<QString, QString> currentData;
    QString key;

    QList<QLineEdit*> lineEdits =  mUi.informationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits ) {
        key = le->objectName();
        currentData[key] = le->text();
    }
    QList<QComboBox*> comboBoxes =  mUi.informationGB->findChildren<QComboBox*>();
    Q_FOREACH( QComboBox* cb, comboBoxes ) {
        key = cb->objectName();
        currentData[key] = cb->currentText();
        if ( key == "currency" ) {
            currentData["currencyId"] = cb->property( "currencyId" ).toString();
            currentData["currencyName"] = cb->property( "currencyName" ).toString();
            currentData["currencySymbol"] = cb->property( "currencySymbol" ).toString();
        }
    }
    QList<QTextEdit*> textEdits = mUi.informationGB->findChildren<QTextEdit*>();
    Q_FOREACH( QTextEdit* te, textEdits ) {
        key = te->objectName();
        currentData[key] = te->toPlainText();
    }
    QList<QLabel*> labels = mUi.informationGB->findChildren<QLabel*>();
    Q_FOREACH( QLabel* lb, labels ) {
        key = lb->objectName();
        currentData[key] = lb->text();
        if ( key == "modifiedBy" ) {
            currentData["modifiedUserId"] = lb->property( "modifiedUserId" ).toString();
            currentData["modifiedUserName"] = lb->property( "modifiedUserName" ).toString();
        }
        if ( key == "dateEntered" ) {
            currentData["deleted"] = lb->property( "deleted" ).toString();
            currentData["id"] = lb->property( "id" ).toString();
            currentData["contactId"] = lb->property( "contactId" ).toString();
            currentData["opportunityRoleFields"] =
                lb->property( "opportunityRoleFields" ).toString();
            currentData["cAcceptStatusFields"] =
                lb->property( "opportunityRoleFields" ).toString();
            currentData["mAcceptStatusFields"] =
                lb->property( "mAcceptStatusFields" ).toString();
        }
        if ( key == "createdBy" ) {
            currentData["createdBy"] = lb->property( "createdBy" ).toString();
              currentData["createdById"] = lb->property( "createdById" ).toString();
        }
    }

    currentData["description"] = mUi.description->toPlainText();
    currentData["content"] = mUi.description->toPlainText();

    currentData["parentId"] =  accountsData().value( currentAccountName() );
    currentData["accountId"] = accountsData().value(  currentAccountName() );
    currentData["campaignId"] = campaignsData().value( currentCampaignName() );
    currentData["assignedUserId"] = assignedToData().value( currentAssignedUserName() );
    currentData["assignedToId"] = assignedToData().value( currentAssignedUserName() );
    currentData["reportsToId"] = reportsToData().value( currentReportsToName() );
    return currentData;
}


void DetailsWidget::slotSetModifyFlag( bool value )
{
    mModifyFlag = value;
}

void DetailsWidget::slotEnableSaving()
{
    mUi.saveButton->setEnabled( true );
}

void DetailsWidget::slotSaveData()
{
    if ( !mData.empty() )
        mData.clear();

    mData["remoteRevision"] = mUi.dateModified->text();

    QMap<QString, QString> detailsMap = data();
    QMap<QString, QString>::const_iterator i = detailsMap.constBegin();
    while ( i != detailsMap.constEnd() ) {
        mData[i.key()] = mData[i.value()];
        ++i;
    }
    connect( mUi.dateModified, SIGNAL( textChanged( const QString& ) ),
             this, SLOT( slotResetCursor( const QString& ) ) );

    if ( !mModifyFlag )
        emit saveItem();
    else
        emit modifyItem();
}

void DetailsWidget::addAccountData( const QString &name,  const QString &id )
{
    QString dataKey;
    dataKey = mAccountsData.key( id );
    removeAccountData( dataKey );
    mAccountsData.insert( name, id );
    if ( mType != Campaign && mType != Lead ) {
        QString key;
        QList<QComboBox*> comboBoxes =  mUi.informationGB->findChildren<QComboBox*>();
        Q_FOREACH( QComboBox* cb, comboBoxes ) {
            key = cb->objectName();
            if ( key == "parentName" || key == "accountName" ) {
                if ( cb->findText( name ) < 0 )
                    cb->addItem( name );
            }
        }
    }
}

QString DetailsWidget::currentAccountName() const
{
    if ( mType != Campaign && mType != Lead ) {
        QString key;
        QList<QComboBox*> comboBoxes =  mUi.informationGB->findChildren<QComboBox*>();
        Q_FOREACH( QComboBox* cb, comboBoxes ) {
            key = cb->objectName();
            if ( key == "parentName" || key == "accountName" )
                return cb->currentText();
        }
    }
    return QString();
}

void DetailsWidget::removeAccountData( const QString &name )
{
    mAccountsData.remove( name );
    if ( mType != Campaign && mType != Lead ) {
        QString key;
        QList<QComboBox*> comboBoxes =  mUi.informationGB->findChildren<QComboBox*>();
        Q_FOREACH( QComboBox* cb, comboBoxes ) {
            key = cb->objectName();
            if ( key == "parentName" || key == "accountName" ) {
                int index = cb->findText( name );
                if ( index > 0 ) // always leave the first blank field
                    cb->removeItem( index );
            }
        }
    }
}

void DetailsWidget::addCampaignData( const QString &name,  const QString &id )
{
    QString dataKey;
    dataKey = mCampaignsData.key( id );
    removeCampaignData( dataKey );
    mCampaignsData.insert( name, id );
    if ( mType != Campaign ) {
        QString key;
        QList<QComboBox*> comboBoxes =  mUi.informationGB->findChildren<QComboBox*>();
        Q_FOREACH( QComboBox* cb, comboBoxes ) {
            key = cb->objectName();
            if ( key == "campaignName" || key == "campaign" ) {
                if ( cb->findText( name ) < 0 )
                    cb->addItem( name );
            }
        }
    }
}

QString DetailsWidget::currentCampaignName() const
{
    if ( mType != Campaign ) {
        QString key;
        QList<QComboBox*> comboBoxes =  mUi.informationGB->findChildren<QComboBox*>();
        Q_FOREACH( QComboBox* cb, comboBoxes ) {
            key = cb->objectName();
            if ( key == "campaignName" || key == "campaign" )
                return cb->currentText();
        }
    }
    return QString();
}

void DetailsWidget::removeCampaignData( const QString &name )
{
    mCampaignsData.remove( name );
    if ( mType != Campaign ) {
        QString key;
        QList<QComboBox*> comboBoxes =  mUi.informationGB->findChildren<QComboBox*>();
        Q_FOREACH( QComboBox* cb, comboBoxes ) {
            key = cb->objectName();
            if ( key == "campaignName" || key == "campaign" ) {
                int index = cb->findText( name );
                if ( index > 0 ) // always leave the first blank field
                cb->removeItem( index );
            }
        }
    }
}

void DetailsWidget::addAssignedToData( const QString &name, const QString &id )
{
    if ( mAssignedToData.values().contains( id ) )
        mAssignedToData.remove( mAssignedToData.key( id ) );
    mAssignedToData.insert( name, id );
    QString key;
    QList<QComboBox*> comboBoxes =  mUi.informationGB->findChildren<QComboBox*>();
    Q_FOREACH( QComboBox* cb, comboBoxes ) {
        key = cb->objectName();
        if ( key == "assignedUserName" || key == "assignedTo" ) {
            if ( cb->findText( name ) < 0 )
                cb->addItem( name );
        }
    }
}

QString DetailsWidget::currentAssignedUserName() const
{
    QList<QComboBox*> comboBoxes =  mUi.informationGB->findChildren<QComboBox*>();
    Q_FOREACH( QComboBox* cb, comboBoxes ) {
        QString key;
        key = cb->objectName();
        if ( key == "assignedUserName" || key == "assignedTo" )
            return cb->currentText();
    }
    return QString();
}

void DetailsWidget::addReportsToData( const QString &name, const QString &id )
{
    if ( mReportsToData.values().contains( id ) )
        mReportsToData.remove( mReportsToData.key( id ) );
    mReportsToData.insert( name, id );
    if ( mType == Contact ) {
        QString key;
        QList<QComboBox*> comboBoxes =  mUi.informationGB->findChildren<QComboBox*>();
        Q_FOREACH( QComboBox* cb, comboBoxes ) {
            key = cb->objectName();
            if ( key == "reportsTo" ) {
                if ( cb->findText( name ) < 0 )
                    cb->addItem( name );
            }
        }
    }
}

QString DetailsWidget::currentReportsToName() const
{
    if ( mType == Contact ) {
        QString key;
        QList<QComboBox*> comboBoxes =  mUi.informationGB->findChildren<QComboBox*>();
        Q_FOREACH( QComboBox* cb, comboBoxes ) {
            key = cb->objectName();
            if ( key == "reportsTo" )
                return cb->currentText();
        }
    }
    return QString();
}

void DetailsWidget::slotResetCursor( const QString& text)
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    if ( !w )
        return;
    if ( !text.isEmpty() ) {
        do {
            QApplication::restoreOverrideCursor();
        } while ( QApplication::overrideCursor() != 0 );
        if ( !w->isEnabled() )
            w->setEnabled( true );
    }
}
