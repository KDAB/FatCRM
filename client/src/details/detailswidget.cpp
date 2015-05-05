#include "detailswidget.h"

#include "accountdetails.h"
#include "campaigndetails.h"
#include "contactdetails.h"
#include "leaddetails.h"
#include "opportunitydetails.h"

#include "kdcrmdata/kdcrmutils.h"

#include <akonadi/item.h>

using namespace Akonadi;

DetailsWidget::DetailsWidget(DetailsType type, QWidget *parent)
    : QWidget(parent),
      mType(type),
      mModified(false),
      mCreateNew(false),
      mIgnoreModifications(false)
{
    mUi.setupUi(this);
    initialize();
}

DetailsWidget::~DetailsWidget()
{

}

/*
 * Create a detail widget of mType and layout it
 * (called by constructor)
 */
void DetailsWidget::initialize()
{
    if (mUi.detailsContainer->layout()) {
        delete mUi.detailsContainer->layout();
    }

    mDetails = createDetailsForType(mType);

    setConnections();

    connect(mUi.saveButton, SIGNAL(clicked()),
            this, SLOT(saveData()));
    connect(mUi.discardButton, SIGNAL(clicked()),
            this, SLOT(slotDiscardData()));

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(mDetails);
    mUi.detailsContainer->setLayout(hlayout);

}

/*
 * Enable the save button when changes happen
 *
 */
void DetailsWidget::setConnections()
{
    connect(mDetails, SIGNAL(modified()),
            this, SLOT(slotModified()));
    connect(mUi.description, SIGNAL(textChanged()),
            this,  SLOT(slotModified()));
    setModified(false);
}

/*
 * Disconnect changes signal. e.g. when filling the details
 * widgets with existing data, selection change etc ....
 *
 */
void DetailsWidget::reset()
{
    disconnect(mDetails, SIGNAL(modified()),
               this, SLOT(slotModified()));
    disconnect(mUi.description, SIGNAL(textChanged()),
               this,  SLOT(slotModified()));
}

/*
 * Reset all the widgets and properties, to let the user create a new entry.
 */
void DetailsWidget::clearFields()
{
    mDetails->clear();

    mUi.dateModified->clear();

    // initialize other fields
    mUi.description->setPlainText(QString());

    // hide creation/modification date/user
    mUi.createdModifiedContainer->hide();

    // we are creating a new entry
    mCreateNew = true;

    setConnections();
}

/*
 * Retrieves the @p item data, before calling setData.
 */
void DetailsWidget::setItem(const Item &item)
{
    mItem = item;

    // new item selected reset flag and saving
    mCreateNew = false;
    reset();
    setData(mDetails->data(item));
    setConnections();
    mUi.createdModifiedContainer->show();
}

/*
 * Fill in the widgets with their data and properties.
 * It covers all the item types.
 */
void DetailsWidget::setData(const QMap<QString, QString> &data)
{
    mDetails->setData(data, mUi.createdModifiedContainer);
    // Transform the time returned by the server to system time
    // before it is displayed.
    const QString localTime = KDCRMUtils::formatTimestamp(data.value("dateModified"));
    mUi.dateModified->setText(localTime);

    mUi.description->setPlainText((mType != Campaign) ?
                                  data.value("description") :
                                  data.value("content"));
}

/*
 * Return a map containing the data (and properties) from all the widgets
 *
 */
QMap<QString, QString> DetailsWidget::data() const
{
    QMap<QString, QString> currentData = mDetails->getData();

    currentData["description"] = mUi.description->toPlainText();
    currentData["content"] = mUi.description->toPlainText();

    return currentData;
}

void DetailsWidget::slotModified()
{
    setModified(true);
}

/*
 * Fill in the data map with the current data and emit a signal
 *
 */
void DetailsWidget::saveData()
{
    if (!mData.empty()) {
        mData.clear();
    }

    QMap<QString, QString> detailsMap = data();
    QMap<QString, QString>::const_iterator i = detailsMap.constBegin();
    while (i != detailsMap.constEnd()) {
        mData[i.key()] = mData[i.value()];
        ++i;
    }

    if (mCreateNew) {
        emit createItem();
    } else {
        emit modifyItem();
    }
    setModified(false);
}

void DetailsWidget::slotDiscardData()
{
    setItem(mItem);
}

void DetailsWidget::setModified(bool modified)
{
    if (modified && mIgnoreModifications)
        return;
    mModified = modified;
    mUi.saveButton->setEnabled(modified);
    mUi.discardButton->setEnabled(modified);
}

void DetailsWidget::setModificationsIgnored(bool b)
{
    mIgnoreModifications = b;
}

Details *DetailsWidget::createDetailsForType(DetailsType type)
{
    switch (type) {
    case Account: return new AccountDetails;
    case Opportunity: return new OpportunityDetails;
    case Contact: return new ContactDetails;
    case Lead: return new LeadDetails;
    case Campaign: return new CampaignDetails;
    case Note: Q_ASSERT(0); return 0; // not implemented
    }

    return 0;
}
