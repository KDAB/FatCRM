/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>
           Michel Boyer de la Giroday <michel.giroday@kdab.com>
           Kevin Krammer <kevin.krammer@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "detailswidget.h"

#include "accountdetails.h"
#include "campaigndetails.h"
#include "contactdetails.h"
#include "leaddetails.h"
#include "opportunitydetails.h"

#include "kdcrmdata/kdcrmutils.h"
#include "kdcrmdata/kdcrmfields.h"

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

    mUi.date_modified->clear();

    // initialize other fields
    mUi.description->setPlainText(QString());

    // hide creation/modification date/user
    mUi.createdModifiedContainer->hide();

    // assign to me by default
    mDetails->assignToMe();

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
    const QString localTime = KDCRMUtils::formatTimestamp(data.value(KDCRMFields::dateModified()));
    mUi.date_modified->setText(localTime);

    mUi.description->setPlainText((mType != Campaign) ?
                                  data.value(KDCRMFields::description()) :
                                  data.value(KDCRMFields::content()));
}

/*
 * Return a map containing the data (and properties) from all the widgets
 *
 */
QMap<QString, QString> DetailsWidget::data() const
{
    QMap<QString, QString> currentData = mDetails->getData();

    currentData[KDCRMFields::description()] = mUi.description->toPlainText();
    if (mDetails->type() == Campaign) {
        currentData[KDCRMFields::content()] = mUi.description->toPlainText();
    }

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
        Item item = mItem;
        mDetails->updateItem(item, data());
        emit modifyItem(item);
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

void DetailsWidget::initialLoadingDone()
{
    // A combo was just loaded; if the user didn't make any changes yet,
    // retry selecting the right account/etc. from the combo
    if (!mModified && mItem.isValid()) {
        setItem(mItem);
    }
}

Details *DetailsWidget::createDetailsForType(DetailsType type)
{
    switch (type) {
    case Account: return new AccountDetails;
    case Opportunity: return new OpportunityDetails;
    case Contact: return new ContactDetails;
    case Lead: return new LeadDetails;
    case Campaign: return new CampaignDetails;
    }

    return 0;
}
