#include "opportunitydetails.h"

#include <akonadi/item.h>

#include <kdcrmdata/sugaropportunity.h>

using namespace Akonadi;


OpportunityDetails::OpportunityDetails( QWidget *parent )
    : QWidget( parent )

{
    mUi.setupUi( this );
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

    QList<QComboBox*> comboBoxes =  mUi.opportunityInformationGB->findChildren<QComboBox*>();
    Q_FOREACH( QComboBox* cb, comboBoxes )
        connect( cb, SIGNAL( currentIndexChanged( int ) ),
                 this, SLOT( slotEnableSaving() ) );

     QList<QGroupBox*> groupBoxes =
        mUi.opportunityInformationGB->findChildren<QGroupBox*>();
    Q_FOREACH( QGroupBox* gb, groupBoxes ) {
       connect( gb, SIGNAL( toggled( bool ) ),
                this, SLOT( slotEnableSaving() ) );
       connect( gb, SIGNAL( toggled( bool ) ),
                this, SLOT( slotSetModifyFlag( bool ) ) );
    }

    mModifyFlag = false;

    mCalendarButton = new EditCalendarButton(this);
    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget( mCalendarButton );
    mUi.calendarWidget->setLayout( buttonLayout );

    connect( mCalendarButton->calendarWidget(), SIGNAL( selectionChanged() ),
             this, SLOT( slotSetCloseDate() ) );

    connect( mUi.description, SIGNAL( textChanged() ),
             this,  SLOT( slotEnableSaving() ) );

    connect( mUi.saveButton, SIGNAL( clicked() ),
             this, SLOT( slotSaveOpportunity() ) );

    mUi.saveButton->setEnabled( false );
}

void OpportunityDetails::setItem (const Item &item )
{
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

    // enable
    QList<QGroupBox*> groupBoxes =
        mUi.opportunityInformationGB->findChildren<QGroupBox*>();
    Q_FOREACH( QGroupBox* gb, groupBoxes )
        gb->setChecked( true );

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
    QList<QGroupBox*> groupBoxes =
        mUi.opportunityInformationGB->findChildren<QGroupBox*>();

    Q_FOREACH( QGroupBox* gb, groupBoxes )
        if ( gb->isChecked() ) {
            mUi.saveButton->setEnabled( true );
            return;
        }
    mUi.saveButton->setEnabled( false );
}

void OpportunityDetails::slotSaveOpportunity()
{
    if ( !mData.empty() )
        mData.clear();

    mUi.modifiedDate->setText( QDateTime::currentDateTime().toString( QString( "yyyy-MM-dd hh:mm:ss") ) );

    QList<QLineEdit*> lineEdits =
        mUi.opportunityInformationGB->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits )
        mData[le->objectName()] = le->text();

    QList<QLabel*> labels =
        mUi.opportunityInformationGB->findChildren<QLabel*>();
    Q_FOREACH( QLabel* lab, labels ) {
        QString objName = lab->objectName();
        if ( objName == "modifiedDate" )
            mData["modifiedDate"] = lab->text();
        else if ( objName == "modifiedBy" ) {
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

    mData["opportunityType"] = mUi.opportunityType->currentText();

    mData["campaignName"] = mUi.campaignName->currentText();
    mData["campaignId"] = mCampaignsData.value( mUi.campaignName->currentText() );

    mData["assignedUserName"] = mUi.assignedUserName->currentText();
    mData["assignedUserId"] = mAssignedToData.value( mUi.assignedUserName->currentText() );
    mData["leadSource"] = mUi.leadSource->currentText();
    mData["salesStage"] = mUi.salesStage->currentText();
    mData["description"] = mUi.description->toPlainText();
    // read currency combo and parse - Name : Symbol
    QString currencyName = mUi.currency->currentText().split( ":" )[0].trimmed();
    mData["currencyName"] = currencyName;
    QString currencySymbol = mUi.currency->currentText().split( ":")[1].trimmed();
    mData["currencySymbol"] = currencySymbol;
    mData["currencyId"] = mUi.currency->property( "currencyId" ).toString();

    if ( !mModifyFlag )
        emit saveOpportunity();
    else
        emit modifyOpportunity();

}

void OpportunityDetails::addAccountData( const QString &accountName,  const QString &accountId )
{
    mAccountsData.insert( accountName, accountId );
}

void OpportunityDetails::addCampaignData( const QString &campaignName,  const QString &campaignId )
{
    mCampaignsData.insert( campaignName, campaignId );
}

void OpportunityDetails::addAssignedToData( const QString &name, const QString &id )
{
    mAssignedToData.insert( name, id );
}

void OpportunityDetails::fillCombos()
{

    QList<QString> names = mAccountsData.uniqueKeys();
    // fill
    // accountName combo
    for ( int i = 0; i < names.count(); ++i )
        mUi.accountName->addItem( names[i] );
    // campaign
    names = mCampaignsData.uniqueKeys();
    for ( int i = 0; i < names.count(); ++i )
        mUi.campaignName->addItem( names[i] );
    // assigned to
    names = mAssignedToData.uniqueKeys();
    for ( int i = 0; i < names.count(); ++i )
        mUi.assignedUserName->addItem( names[i] );
}

void OpportunityDetails::disableGroupBoxes()
{
    QList<QGroupBox*> groupBoxes =
        mUi.opportunityInformationGB->findChildren<QGroupBox*>();

    Q_FOREACH( QGroupBox* gb, groupBoxes )
        gb->setChecked( false );
}


void OpportunityDetails::slotSetDateClosed()
{
    mUi.dateClosed->setText( mCalendarButton->calendarWidget()->selectedDate().toString( QString("yyyy-MM-dd" ) ) );
}
