#include "detailsdialog.h"

#include "details.h"
#include "ui_detailsdialog.h"
#include "referenceddatamodel.h"
#include "sugarclient.h"

#include "kdcrmdata/kdcrmutils.h"

#include <akonadi/collection.h>
#include <akonadi/item.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemmodifyjob.h>

#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

using namespace Akonadi;

class DetailsDialog::Private
{
    DetailsDialog *const q;

public:
    explicit Private( Details *details, DetailsDialog *parent )
        : q( parent ), mDetails( details ),
          mButtonBox( 0 ), mSaveButton( 0 ), mDiscardButton( 0 )
    {
    }

    void setData( const QMap<QString, QString> data );
    QMap<QString, QString> data() const;

public:
    Ui::DetailsDialog mUi;

    Item mItem;
    Details *mDetails;
    QDialogButtonBox *mButtonBox;
    QPushButton *mSaveButton;
    QPushButton *mDiscardButton;

public: // slots
    void saveClicked();
    void discardClicked();
    void dataModified();
    void saveResult( KJob *job );

private:
    QString currentAccountId() const;
    QString currentAssignedToId() const;
    QString currentCampaignId() const;
    QString currentReportsToId() const;
};

// TODO copied from detailswidget, this needs to be refactored
void DetailsDialog::Private::setData( const QMap<QString, QString> data )
{
    mDetails->setData( data );
    // Transform the time returned by the server to system time
    // before it is displayed.
    const QString localTime = KDCRMUtils::formatTimestamp( data.value( "dateModified" ) );
    mUi.dateModified->setText( localTime );

    QString key;

    QList<QLabel*> labels = mUi.informationGB->findChildren<QLabel*>();
    Q_FOREACH( QLabel *lb, labels ) {
        key = lb->objectName();
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
            lb->setText( KDCRMUtils::formatTimestamp( data.value( "dateEntered" ) ) );
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

    mUi.description->setPlainText( ( mDetails->type() != Campaign )?
                                   data.value( "description" ):
                                   data.value( "content") );
}

// TODO copied from detailswidget, this needs to be refactored
QMap<QString, QString> DetailsDialog::Private::data() const
{
    QMap<QString, QString> currentData = mDetails->getData();

    // will be overwritten by the server, but good to have for comparison in case of change conflict
    currentData["dateModified"] = KDCRMUtils::currentTimestamp();

    QList<QLabel*> labels = mUi.informationGB->findChildren<QLabel*>();
    Q_FOREACH( QLabel* lb, labels ) {
        const QString key = lb->objectName();
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

    // TODO FIXME
    SugarClient *client = dynamic_cast<SugarClient*>( q->parentWidget()->window() );
    if ( client != 0 ) {
        const QString accountId = currentAccountId();
        currentData["parentId"] = accountId;
        currentData["accountId"] = accountId;
        currentData["campaignId"] = currentCampaignId();
        const QString assignedToId = currentAssignedToId();
        currentData["assignedUserId"] = assignedToId;
        currentData["assignedToId"] = assignedToId;
        currentData["reportsToId"] = currentReportsToId();
    }

    return currentData;
}

void DetailsDialog::Private::saveClicked()
{
    Item item = mItem;
    mDetails->updateItem( item, data() );

    Job *job = 0;
    if ( item.isValid() ) {
        kDebug() << "Item modify";
        job = new ItemModifyJob( item, q );
    } else {
        kDebug() << "Item create";
        Q_ASSERT( item.parentCollection().isValid() );
        job = new ItemCreateJob( item, item.parentCollection(), q );
    }

    QObject::connect( job, SIGNAL( result( KJob* ) ), q, SLOT( saveResult( KJob* ) ) );
}

void DetailsDialog::Private::discardClicked()
{
    q->setItem( mItem );
}

void DetailsDialog::Private::dataModified()
{
    mSaveButton->setEnabled( true );
    mDiscardButton->setEnabled( true );
}

void DetailsDialog::Private::saveResult( KJob *job )
{
    kDebug() << "save result=" << job->error();
    if ( job->error() != 0 ) {
        kError() << job->errorText();
        // TODO
        return;
    }

    ItemCreateJob *createJob = qobject_cast<ItemCreateJob*>( job );
    if ( createJob != 0 ) {
        kDebug() << "item" << createJob->item().id() << "created";
        q->setItem( createJob->item() );
    }
}

QString DetailsDialog::Private::currentAccountId() const
{
    if ( mDetails->type() != Campaign && mDetails->type() != Lead ) {
        const QList<QComboBox*> comboBoxes =  mUi.informationGB->findChildren<QComboBox*>();
        Q_FOREACH( QComboBox* cb, comboBoxes ) {
            const QString key = cb->objectName();
            if ( key == QLatin1String( "parentName" ) || key == QLatin1String( "accountName" ) ) {
                return cb->itemData( cb->currentIndex(), ReferencedDataModel::IdRole ).toString();
            }
        }
    }
    return QString();
}

QString DetailsDialog::Private::currentAssignedToId() const
{
    const QList<QComboBox*> comboBoxes =  mUi.informationGB->findChildren<QComboBox*>();
    Q_FOREACH( QComboBox* cb, comboBoxes ) {
        const QString key = cb->objectName();
        if ( key == QLatin1String( "assignedUserName" ) || key == QLatin1String( "assignedTo" ) ) {
            return cb->itemData( cb->currentIndex(), ReferencedDataModel::IdRole ).toString();
        }
    }
    return QString();
}

QString DetailsDialog::Private::currentCampaignId() const
{
    if ( mDetails->type() != Campaign ) {
        const QList<QComboBox*> comboBoxes =  mUi.informationGB->findChildren<QComboBox*>();
        Q_FOREACH( QComboBox* cb, comboBoxes ) {
            const QString key = cb->objectName();
            if ( key == QLatin1String( "campaignName" ) || key == QLatin1String( "campaign" ) ) {
                return cb->itemData( cb->currentIndex(), ReferencedDataModel::IdRole ).toString();
            }
        }
    }
    return QString();
}

QString DetailsDialog::Private::currentReportsToId() const
{
    if ( mDetails->type() == Contact ) {
        const QList<QComboBox*> comboBoxes =  mUi.informationGB->findChildren<QComboBox*>();
        Q_FOREACH( QComboBox* cb, comboBoxes ) {
            const QString key = cb->objectName();
            if ( key == QLatin1String( "reportsTo" ) ) {
                return cb->itemData( cb->currentIndex(), ReferencedDataModel::IdRole ).toString();
            }
        }
    }
    return QString();
}

DetailsDialog::DetailsDialog( Details *details, QWidget *parent )
    : QDialog( parent ), d( new Private( details, this ) )
{
    d->mUi.setupUi( this );

    QVBoxLayout* detailsLayout = new QVBoxLayout( d->mUi.detailsContainer );
    detailsLayout->addWidget( details );

    QList<QLineEdit*> lineEdits = d->mDetails->findChildren<QLineEdit*>();
    Q_FOREACH( QLineEdit* le, lineEdits ) {
        connect( le, SIGNAL( textChanged( QString ) ), this, SLOT( dataModified() ) );
    }

    QList<QComboBox*> comboBoxes = d->mDetails->findChildren<QComboBox*>();
    Q_FOREACH( QComboBox* cb, comboBoxes ) {
        connect( cb, SIGNAL( currentIndexChanged( int ) ), this, SLOT( dataModified() ) );
    }

    QList<QCheckBox*> checkBoxes = d->mDetails->findChildren<QCheckBox*>();
    Q_FOREACH( QCheckBox* cb, checkBoxes ) {
        connect( cb, SIGNAL( toggled( bool ) ), this, SLOT( dataModified() ) );
    }

    QList<QTextEdit*> textEdits = d->mDetails->findChildren<QTextEdit*>();
    Q_FOREACH( QTextEdit* te, textEdits ) {
        connect( te, SIGNAL( textChanged() ), this, SLOT( dataModified() ) );
    }

    connect( d->mUi.description, SIGNAL( textChanged() ), this,  SLOT( dataModified() ) );

    d->mSaveButton = d->mUi.buttonBox->button( QDialogButtonBox::Save );
    d->mSaveButton->setEnabled( false );
    connect( d->mSaveButton, SIGNAL( clicked() ), this, SLOT( saveClicked() ) );

    d->mDiscardButton = d->mUi.buttonBox->button( QDialogButtonBox::Discard );
    d->mDiscardButton->setEnabled( false );
    connect( d->mDiscardButton, SIGNAL( clicked() ), this, SLOT( discardClicked() ) );

    QPushButton *closeButton = d->mUi.buttonBox->button( QDialogButtonBox::Close );
    connect( closeButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

DetailsDialog::~DetailsDialog()
{
    delete d;
}

void DetailsDialog::setItem( const Akonadi::Item &item )
{
    d->mItem = item;

    if ( item.isValid() ) {
        d->setData( d->mDetails->data( item ) );
    } else {
        d->setData( QMap<QString, QString>() );
    }

    d->mSaveButton->setEnabled( false );
    d->mDiscardButton->setEnabled( false );
}

void DetailsDialog::updateItem( const Akonadi::Item &item )
{
    if ( item == d->mItem ) {
        setItem( item );
    }
}

#include "detailsdialog.moc"
