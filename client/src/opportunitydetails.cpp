#include "opportunitydetails.h"
#include "sugarclient.h"

#include <akonadi/item.h>

#include <kdcrmdata/sugaropportunity.h>

using namespace Akonadi;


OpportunityDetails::OpportunityDetails( QWidget *parent )
    : QWidget( parent )

{
    mUi.setupUi( this );

    // calendars
    mCalendarButton = new EditCalendarButton(this);
    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget( mCalendarButton );
    mUi.calendarWidget->setLayout( buttonLayout );

    connect( mUi.clearButton, SIGNAL( clicked() ), this,
             SLOT( slotClearDate() ) );

    connect( mCalendarButton->calendarWidget(),
             SIGNAL( clicked( const QDate& ) ),
             this, SLOT( slotSetDateClosed() ) );

    initialize();
}

OpportunityDetails::~OpportunityDetails()
{
    delete mCalendarButton;
}

void OpportunityDetails::initialize()
{
    QList<QLineEdit*> lineEdits =  mUi.opportunityInformationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits )
        connect( le, SIGNAL( textChanged( const QString& ) ),
                 this, SLOT( slotEnableSaving() ) );
    connect( mUi.modifiedDate, SIGNAL( textChanged( const QString& ) ),
             this, SLOT( slotResetCursor( const QString& ) ) );

    QList<QComboBox*> comboBoxes =  mUi.opportunityInformationGB->findChildren<QComboBox*>();
    Q_FOREACH( QComboBox* cb, comboBoxes )
        connect( cb, SIGNAL( currentIndexChanged( int ) ),
                 this, SLOT( slotEnableSaving() ) );

    connect( mUi.description, SIGNAL( textChanged() ),
             this,  SLOT( slotEnableSaving() ) );

    connect( mUi.saveButton, SIGNAL( clicked() ),
             this, SLOT( slotSaveOpportunity() ) );

    mUi.saveButton->setEnabled( false );
}

void OpportunityDetails::reset()
{
    QList<QLineEdit*> lineEdits =  mUi.opportunityInformationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits )
        disconnect( le, SIGNAL( textChanged( const QString& ) ),
                    this, SLOT( slotEnableSaving() ) );

    QList<QComboBox*> comboBoxes =  mUi.opportunityInformationGB->findChildren<QComboBox*>();
    Q_FOREACH( QComboBox* cb, comboBoxes )
        disconnect( cb, SIGNAL( currentIndexChanged( int ) ),
                    this, SLOT( slotEnableSaving() ) );

    disconnect( mUi.description, SIGNAL( textChanged() ),
                this,  SLOT( slotEnableSaving() ) );
    mUi.saveButton->setEnabled( false );
}

void OpportunityDetails::setItem (const Item &item )
{
    // new item selected reset flag and saving
    mModifyFlag = true;
    reset();

    // opportunity info
    const SugarOpportunity opportunity = item.payload<SugarOpportunity>();
    mUi.modifiedDate->setText( opportunity.dateModified() );
    mUi.modifiedBy->setText( opportunity.modifiedByName() );
    mUi.modifiedBy->setProperty( "modifiedUserId", qVariantFromValue<QString>( opportunity.modifiedUserId() ) );
    mUi.createdDate->setText( opportunity.dateEntered() );
    mUi.createdDate->setProperty( "id", qVariantFromValue<QString>( opportunity.id( ) ) );
    mUi.createdDate->setProperty( "deleted",  qVariantFromValue<QString>( opportunity.deleted( ) ) );
    mUi.createdBy->setText( opportunity.createdByName() );
    mUi.createdBy->setProperty( "createdBy", qVariantFromValue<QString>( opportunity.createdBy( ) ) ); // id
    mUi.name->setText( opportunity.name() );
    mUi.accountName->setCurrentIndex( mUi.accountName->findText( opportunity.accountName() ) );
    mUi.opportunityType->setCurrentIndex( mUi.opportunityType->findText( opportunity.opportunityType() ) );
    mUi.leadSource->setCurrentIndex( mUi.leadSource->findText( opportunity.leadSource() ) );
    mUi.campaignName->setCurrentIndex(mUi.campaignName->findText( opportunity.campaignName() ) );
    mUi.salesStage->setCurrentIndex(mUi.salesStage->findText( opportunity.salesStage() ) );
    mUi.assignedUserName->setCurrentIndex(mUi.assignedUserName->findText( opportunity.assignedUserName() ) );
    // Pending (michel)
    // we dont get the name and symbol value atm
    // setting a default value so long
    mUi.currency->setProperty( "currencyId",qVariantFromValue<QString>( opportunity.currencyId() ) );
    QString currencyName = opportunity.currencyName();
    if ( currencyName.isEmpty() )
        currencyName = "US Dollars";
    mUi.currency->setProperty( "currencyName", qVariantFromValue<QString>( currencyName ) );
    currencyName += QString( " : " );
    QString currencySymbol = opportunity.currencySymbol();
    if ( currencySymbol.isEmpty() )
        currencySymbol = "$";
    mUi.currency->setProperty( "currencySymbol", qVariantFromValue<QString>(  currencySymbol ) );
    QString currencyItem = currencyName + currencySymbol;
    int index = mUi.currency->findText( currencyItem );
    if ( index < 0  ) {
        mUi.currency->addItem( currencyItem );
        mUi.currency->setCurrentIndex( mUi.currency->findText( currencyItem ) );
    }
    else
        mUi.currency->setCurrentIndex( index );

    mUi.amount->setText( opportunity.amount() );
    mUi.dateClosed->setText( opportunity.dateClosed() );
    mUi.nextStep->setText( opportunity.nextStep() );
    mUi.probability->setText( opportunity.probability() );
    mUi.description->setPlainText( opportunity.description() );
    initialize();
}

void OpportunityDetails::clearFields ()
{
    // reset line edits
    QList<QLineEdit*> lineEdits =
        mUi.opportunityInformationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits )
        le->setText(QString());

    // reset label and properties
    QList<QLabel*> labels =
        mUi.opportunityInformationGB->findChildren<QLabel*>();
    Q_FOREACH( QLabel* lab, labels ) {
        QString value = lab->objectName();
        if ( value == "modifiedBy" ) {
            lab->clear();
            lab->setProperty( "modifiedUserId", qVariantFromValue<QString>( QString() ) );
        }
        else if ( value == "createdDate" ) {
            lab->clear();
            lab->setProperty( "id", qVariantFromValue<QString>( QString() ) );
            lab->setProperty( "deleted", qVariantFromValue<QString>( QString() ) );
        }
        else if ( value == "createdBy" ) {
            lab->clear();
            lab->setProperty( "createdBy", qVariantFromValue<QString>( QString() ) );
        }
    }

    // reset combos
    QList<QComboBox*> comboBoxes =
        mUi.opportunityInformationGB->findChildren<QComboBox*>();
    Q_FOREACH( QComboBox* cb, comboBoxes ) {
        QString value = cb->objectName();
        cb->setCurrentIndex( 0 );
        if ( value =="currency" ) {
            cb->setProperty( "currencyName", qVariantFromValue<QString>( QString() ) );
            cb->setProperty( "currencyId", qVariantFromValue<QString>( QString() ) );
            cb->setProperty( "currencySymbol", qVariantFromValue<QString>( QString() ) );

        }
    }
    // initialize other fields
    mUi.description->clear();
    mUi.name->setFocus();

    // we are creating a new opportunity
    slotSetModifyFlag( false );
}

void OpportunityDetails::slotSetModifyFlag( bool value )
{
    mModifyFlag = value;
}

void OpportunityDetails::slotEnableSaving()
{
    if ( sender()->objectName() != "modifiedDate" )
        mUi.saveButton->setEnabled( true );
}

bool OpportunityDetails::isEditing()
{
    return ( mUi.saveButton->isEnabled() );
}

void OpportunityDetails::slotSaveOpportunity()
{
    if ( !mData.empty() )
        mData.clear();

    mData["remoteRevision"] = mUi.modifiedDate->text();

    QList<QLineEdit*> lineEdits =
        mUi.opportunityInformationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits )
        mData[le->objectName()] = le->text();

    QList<QLabel*> labels =
        mUi.opportunityInformationGB->findChildren<QLabel*>();
    Q_FOREACH( QLabel* lab, labels ) {
        QString objName = lab->objectName();
        if ( objName == "modifiedBy" ) {
            mData["modifiedBy"] = lab->text();
            mData["modifiedUserId"] = lab->property( "modifiedUserId" ).toString();
            mData["modifiedUserName"] = lab->property( "modifiedUserName" ).toString();
        }
        else if ( objName == "createdDate" ) {
            mData["createdDate"] = lab->text();
            mData["id"] = lab->property( "id" ).toString();
            mData["deleted"] = lab->property( "deleted" ).toString();
        }
        else if ( objName == "createdBy" ) {
            mData["createdByName"] = lab->text();
            mData["createdBy"] = lab->property( "createdBy" ).toString();
        }
    }

    mData["accountName"] = mUi.accountName->currentText();
    mData["accountId"] = mAccountsData.value(  mUi.accountName->currentText() );
    mData["opportunityType"] = mUi.opportunityType->currentText();

    mData["campaignName"] = mUi.campaignName->currentText();
    mData["campaignId"] = mCampaignsData.value( mUi.campaignName->currentText() );

    mData["assignedUserName"] = mUi.assignedUserName->currentText();
    mData["assignedUserId"] = mAssignedToData.value( mUi.assignedUserName->currentText() );
    mData["leadSource"] = mUi.leadSource->currentText();
    mData["salesStage"] = mUi.salesStage->currentText();
    mData["description"] = mUi.description->toPlainText();
    QString currencyName = QString();
    QString currencySymbol = QString();
    // read currency combo and parse - Name : Symbol
    if ( !mUi.currency->currentText().isEmpty() ) {
        currencyName = mUi.currency->currentText().split( ":" )[0].trimmed();
        currencySymbol = mUi.currency->currentText().split( ":")[1].trimmed();
    }
    mData["currencyName"] = currencyName;
    mData["currencySymbol"] = currencySymbol;
    mData["currencyId"] = mUi.currency->property( "currencyId" ).toString();

    if ( !mModifyFlag )
        emit saveOpportunity();
    else
        emit modifyOpportunity();
}

void OpportunityDetails::addAccountData( const QString &accountName,  const QString &accountId )
{
    QString dataKey;
    dataKey = mAccountsData.key( accountId );
    removeAccountData( dataKey );

    mAccountsData.insert( accountName, accountId );

    if ( mUi.accountName->findText( accountName ) < 0 )
        mUi.accountName->addItem( accountName );
}

void OpportunityDetails::removeAccountData( const QString &accountName )
{
    if ( accountName.isEmpty() )
        return;
    mAccountsData.remove( accountName );
    int index = mUi.accountName->findText( accountName );
    if ( index > 0 ) // always leave the first blank field
        mUi.accountName->removeItem( index );
}

void OpportunityDetails::addCampaignData( const QString &campaignName,  const QString &campaignId )
{
    QString dataKey;
    dataKey = mCampaignsData.key( campaignId );
    removeCampaignData( dataKey );

    mCampaignsData.insert( campaignName, campaignId );

    if ( mUi.campaignName->findText( campaignName ) < 0 )
        mUi.campaignName->addItem( campaignName );
}

void OpportunityDetails::removeCampaignData( const QString &campaignName )
{
    if ( campaignName.isEmpty() )
        return;
    mCampaignsData.remove( campaignName );
    int index = mUi.campaignName->findText( campaignName );
    if ( index > 0 )// always leave the first blank field
        mUi.campaignName->removeItem( index );
}

void OpportunityDetails::addAssignedToData( const QString &name, const QString &id )
{
    mAssignedToData.insert( name, id );
    if ( mUi.assignedUserName->findText( name ) < 0 )
        mUi.assignedUserName->addItem( name );
}

void OpportunityDetails::slotSetDateClosed()
{
    mUi.dateClosed->setText( mCalendarButton->calendarWidget()->selectedDate().toString( QString("yyyy-MM-dd" ) ) );
    mCalendarButton->calendarWidget()->setSelectedDate( QDate::currentDate() );
    mCalendarButton->calendarDialog()->close();
}

void OpportunityDetails::slotClearDate()
{
    mUi.dateClosed->clear();
}

void OpportunityDetails::slotResetCursor( const QString& text)
{
    SugarClient *w = dynamic_cast<SugarClient*>( window() );
    if ( !text.isEmpty() ) {
        do {
            QApplication::restoreOverrideCursor();
        } while ( QApplication::overrideCursor() != 0 );
        w->setEnabled( true );
    }
}
