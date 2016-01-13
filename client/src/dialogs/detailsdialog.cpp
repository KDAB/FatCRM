/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <Akonadi/Collection>
#include <Akonadi/Item>
#include <Akonadi/ItemCreateJob>
#include <Akonadi/ItemModifyJob>

#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>
#include <QMessageBox>
#include <QKeyEvent>

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

    mUi.description->enableFindReplace(true);
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
        kDebug() << "Item modify";
        job = new ItemModifyJob(item, q);
    } else {
        kDebug() << "Item create";
        Q_ASSERT(mCollection.isValid());
        job = new ItemCreateJob(item, mCollection, q);
    }

    QObject::connect(job, SIGNAL(result(KJob*)), q, SLOT(saveResult(KJob*)));
}

void DetailsDialog::Private::dataModified()
{
    mSaveButton->setEnabled(true);
}

bool DetailsDialog::isModified() const
{
    return d->mSaveButton->isEnabled();
}

QString DetailsDialog::title() const
{
    if (d->mItem.isValid()) {
        const QString name = d->mDetails->name();
        switch (d->mDetails->type()) {
        case Account: return i18n("Account: %1", name);
        case Campaign: return i18n("Campaign: %1", name);
        case Contact: return i18n("Contact: %1", name);
        case Lead: return i18n("Lead: %1", name);
        case Opportunity: return i18n("Opportunity: %1", name);
        }
    } else {
        switch (d->mDetails->type()) {
        case Account: return i18n("New Account");
        case Campaign: return i18n("New Campaign");
        case Contact: return i18n("New Contact");
        case Lead: return i18n("New Lead");
        case Opportunity: return i18n("New Opportunity");
        }
    }
    return QString(); // for stupid compilers
}

void DetailsDialog::Private::saveResult(KJob *job)
{
    kDebug() << "save result=" << job->error();
    if (job->error() != 0) {
        kError() << job->errorText();
        mUi.labelOffline->setText(job->errorText());
        mUi.labelOffline->show();
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
    q->close(); // was accept();
}

// This doesn't derive from QDialog anymore because for some reason KWin (or Qt)
// places all dialogs at the same position on the screen, even in Random or Smart placement policy.
// (FATCRM-76). Anyway we don't need modality, just support for Esc.
DetailsDialog::DetailsDialog(Details *details, QWidget *parent)
    : QWidget(parent), d(new Private(details, this))
{
    d->mUi.setupUi(this);

    QVBoxLayout *detailsLayout = new QVBoxLayout(d->mUi.detailsContainer);
    detailsLayout->addWidget(details);

    connect(d->mDetails, SIGNAL(modified()), this, SLOT(dataModified()));

    connect(d->mUi.description, SIGNAL(textChanged()), this,  SLOT(dataModified()));

    d->mSaveButton = d->mUi.buttonBox->button(QDialogButtonBox::Save);
    d->mSaveButton->setEnabled(false);
    connect(d->mSaveButton, SIGNAL(clicked()), this, SLOT(saveClicked()));

    QPushButton *cancelButton = d->mUi.buttonBox->button(QDialogButtonBox::Cancel);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close())); // TODO confirm if modified

    ClientSettings::self()->restoreWindowSize("details", this);
}

DetailsDialog::~DetailsDialog()
{
    ClientSettings::self()->saveWindowSize("details", this);
    delete d;
}

void DetailsDialog::keyPressEvent(QKeyEvent *e)
{
   if (!e->modifiers()) {
       switch (e->key()) {
       case Qt::Key_Enter:
           d->saveClicked();
           return;
       case Qt::Key_Escape:
           reject();
           return;
       default:
           break;
       }
   }
   e->ignore();
}

void DetailsDialog::reject()
{
    if (isModified()) {
        QMessageBox msgBox;
        msgBox.setText(i18n("The data for %1 has been modified.", d->mDetails->name()));
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        switch (msgBox.exec()) {
        case QMessageBox::Save:
            // Save was clicked
            d->saveClicked();
            break;
        case QMessageBox::Discard:
            // Don't Save was clicked
            close();
            break;
        default:
            // Cancel
            break;
        }
    } else {
        close();
    }
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
    setWindowTitle(title());
}

// open for modification
void DetailsDialog::setItem(const Akonadi::Item &item)
{
    d->mItem = item;

    Q_ASSERT(item.isValid());
    d->setData(d->mDetails->data(item));

    setWindowTitle(title());

    d->mSaveButton->setEnabled(false);
}

void DetailsDialog::updateItem(const Akonadi::Item &item)
{
    if (item == d->mItem) {
        if (isModified()) {
            // Don't lose the user's changes (FATCRM-75)
            // Here we could pop up the conflict dialog, but it's private and meant for resources
            // Alternatively we could merge in the fields that haven't been locally modified, if we had that info.
            qWarning() << "Ignoring remote change on" << typeToString(d->mDetails->type()) << item.id() << "while modifying it";
        } else {
            setItem(item);
        }
    }
}

void DetailsDialog::setOnline(bool online)
{
    d->mUi.labelOffline->setVisible(!online);
    if (!online) {
        d->mUi.labelOffline->setText(i18n("Warning: FatCRM is currently offline. Changes will only be sent to the server once it's online again."));
    }
}

void DetailsDialog::closeEvent(QCloseEvent *event)
{
    emit closing();
    QWidget::closeEvent(event);
}

#include "detailsdialog.moc"
