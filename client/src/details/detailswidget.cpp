#include "detailswidget.h"
#include "sugarclient.h"
#include "accountdetails.h"
#include "leaddetails.h"
#include "contactdetails.h"
#include "opportunitydetails.h"
#include "campaigndetails.h"

#include "kdcrmdata/kdcrmutils.h"

#include <akonadi/item.h>

#include <KDateTime>

using namespace Akonadi;

DetailsWidget::DetailsWidget( DetailsType type, QWidget* parent )
    : QWidget( parent ),
      mType( type ),
      mEditing( false )
{
    mUi.setupUi( this );
    initialize();
}

DetailsWidget::~DetailsWidget()
{

}

/*
 * Create a detail widget of mType and layout it
 *
 */
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

/*
 * Enable the save button when changes happen
 *
 */
void DetailsWidget::setConnections()
{
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

/*
 * Disconnect changes signal. e.g When filling the details
 * widgets with existing data, selection change etc ....
 *
 */
void DetailsWidget::reset()
{
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

/*
 * Reset all the widgets and properties
 *
 */
void DetailsWidget::clearFields ()
{
    mDetails->clear();

    mUi.dateModified->clear();

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

/*
 * Retrieve the item data, before calling setData.
 *
 */
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

/*
 * Fill in the widgets with their data and properties.
 * It covers all the item types.
 *
 */
void DetailsWidget::setData( const QMap<QString, QString> &data )
{
    mDetails->setData( data );
    // Transform the time returned by the server to system time
    // before it is displayed.
    const QString localTime = KDCRMUtils::formatTimestamp( data.value( "dateModified" ) );
    mUi.dateModified->setText( localTime );

    QString key;

    QList<QLabel*> labels = mUi.informationGB->findChildren<QLabel*>();
    Q_FOREACH( QLabel* lb, labels ) {
        key = lb->objectName();
        if ( key == "dateEntered" )
            lb->setText( KDCRMUtils::formatTimestamp( data.value( "dateEntered" ) ) );
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


/*
 * Return a map containing the data (and properties) from all the widgets
 *
 */
QMap<QString, QString> DetailsWidget::data()
{
    QMap<QString, QString> currentData = mDetails->getData();

    QString key;

    // will be overwritten by the server, but good to have for comparison in case of change conflict
    currentData["dateModified"] = KDCRMUtils::currentTimestamp();

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

/*
 * The modify flag indicate if we are modifying an existing item or creating
 * a new item
 *
 */
void DetailsWidget::slotSetModifyFlag( bool value )
{
    mModifyFlag = value;
}

void DetailsWidget::slotEnableSaving()
{
    mUi.saveButton->setEnabled( true );
}

/*
 * Fill in the data map with the current data and emit a signal
 *
 */
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

    if ( !mModifyFlag )
        emit saveItem();
    else
        emit modifyItem();
}

/*
 * Add accounts to the inventory for the available "Account Name"
 * or "Parent Name", and updates the related combo boxes.
 *
 */
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

/*
 * Return the selected "Account Name" or "Parent Name",
 * and updates the related combo boxes.
 *
 */
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

/*
 * This method remove accounts to the inventory for the available
 * "Account Name" or "Parent Name", and updates the related combo boxes.
 *
 */
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

/*
 * This method add campaing name to the inventory for the available
 * Campaigns and updates the related combo boxes.
 *
 */
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

/*
 * Return the selected Campaign name.
 *
 */
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

/*
 * Remove Campaign from the inventory for the available
 * Campaigns, and updates the related combo boxes.
 *
 */
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

/*
 * Add name to the inventory for the available "Assigned User Name"
 * and updates the related combo boxes.
 *
 */
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

/*
 * Return the selected "Assigned User Name",
 * and updates the related combo boxes.
 *
 */
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

/*
 * Add name to the inventory for the available "Reports to User Name"
 * and updates the related combo boxes.
 *
 */
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

/*
 * Return the selected "Reports to User Name",
 * and updates the related combo boxes.
 *
 */
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
