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

#include "detailsdialog.h"

#include "details.h"
#include "ui_detailsdialog.h"
#include "referenceddatamodel.h"
#include "clientsettings.h"

#include "kdcrmdata/kdcrmutils.h"
#include "kdcrmdata/kdcrmfields.h"

#include <AkonadiCore/Collection>
#include <AkonadiCore/Item>
#include <AkonadiCore/ItemCreateJob>
#include <AkonadiCore/ItemModifyJob>

#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>
#include <KDebug>

using namespace Akonadi;

class DetailsDialog::Private
{
    DetailsDialog *const q;

public:
    explicit Private(Details *details, DetailsDialog *parent)
        : q(parent), mDetails(details),
          mButtonBox(0), mSaveButton(0)
    {
    }

    void setData(const QMap<QString, QString> &data);
    QMap<QString, QString> data() const;

public:
    Ui::DetailsDialog mUi;

    Item mItem;
    Collection mCollection;
    Details *mDetails;
    QDialogButtonBox *mButtonBox;
    QPushButton *mSaveButton;

public: // slots
    void saveClicked();
    void dataModified();
    void saveResult(KJob *job);
};

// similar with detailswidget
void DetailsDialog::Private::setData(const QMap<QString, QString> &data)
{
    mDetails->setData(data, mUi.createdModifiedContainer);
    // Transform the time returned by the server to system time
    // before it is displayed.
    const QString localTime = KDCRMUtils::formatTimestamp(data.value(KDCRMFields::dateModified()));
    mUi.date_modified->setText(localTime);

    mUi.description->setPlainText((mDetails->type() != Campaign) ?
                                  data.value(KDCRMFields::description()) :
                                  data.value(KDCRMFields::content()));
}

// similar with detailswidget
QMap<QString, QString> DetailsDialog::Private::data() const
{
    QMap<QString, QString> currentData = mDetails->getData();

    currentData[KDCRMFields::description()] = mUi.description->toPlainText();
    if (mDetails->type() == Campaign) {
        currentData[KDCRMFields::content()] = mUi.description->toPlainText();
    }

    return currentData;
}

void DetailsDialog::Private::saveClicked()
{
    Item item = mItem;
    mDetails->updateItem(item, data());

    Job *job = 0;
    if (item.isValid()) {
        qDebug() << "Item modify";
        job = new ItemModifyJob(item, q);
    } else {
        qDebug() << "Item create";
        Q_ASSERT(mCollection.isValid());
        job = new ItemCreateJob(item, mCollection, q);
    }

    QObject::connect(job, SIGNAL(result(KJob*)), q, SLOT(saveResult(KJob*)));
}

void DetailsDialog::Private::dataModified()
{
    mSaveButton->setEnabled(true);
}

void DetailsDialog::Private::saveResult(KJob *job)
{
    qDebug() << "save result=" << job->error();
    if (job->error() != 0) {
        qCritical() << job->errorText();
        // TODO
        return;
    }
    ItemModifyJob *modifyJob = qobject_cast<ItemModifyJob *>(job);
    if (modifyJob) {
        emit q->itemSaved(modifyJob->item());
    } else {
        ItemCreateJob *createJob = qobject_cast<ItemCreateJob *>(job);
        Q_ASSERT(createJob);
        emit q->itemSaved(createJob->item());
    }
    q->accept();
}

DetailsDialog::DetailsDialog(Details *details, QWidget *parent)
    : QDialog(parent), d(new Private(details, this))
{
    d->mUi.setupUi(this);

    setWindowTitle(details->windowTitle());

    QVBoxLayout *detailsLayout = new QVBoxLayout(d->mUi.detailsContainer);
    detailsLayout->addWidget(details);

    connect(d->mDetails, SIGNAL(modified()), this, SLOT(dataModified()));

    connect(d->mUi.description, SIGNAL(textChanged()), this,  SLOT(dataModified()));

    d->mSaveButton = d->mUi.buttonBox->button(QDialogButtonBox::Save);
    d->mSaveButton->setEnabled(false);
    connect(d->mSaveButton, SIGNAL(clicked()), this, SLOT(saveClicked()));

    QPushButton *cancelButton = d->mUi.buttonBox->button(QDialogButtonBox::Cancel);
    connect(cancelButton, &QAbstractButton::clicked, this, &QDialog::reject);

    ClientSettings::self()->restoreWindowSize("details", this);
}

DetailsDialog::~DetailsDialog()
{
    ClientSettings::self()->saveWindowSize("details", this);
    delete d;
}

// open for creation
void DetailsDialog::showNewItem(const QMap<QString, QString> &data, const Akonadi::Collection &collection)
{
    d->setData(data);

    d->mCollection = collection;

    // hide creation/modification date/user
    d->mUi.createdModifiedContainer->hide();

    d->mDetails->assignToMe();

    d->mSaveButton->setEnabled(false);
}

// open for modification
void DetailsDialog::setItem(const Akonadi::Item &item)
{
    d->mItem = item;

    Q_ASSERT(item.isValid());
    d->setData(d->mDetails->data(item));

    d->mSaveButton->setEnabled(false);
}

void DetailsDialog::updateItem(const Akonadi::Item &item)
{
    if (item == d->mItem) {
        setItem(item);
    }
}

#include "moc_detailsdialog.cpp"
