/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "simpleitemeditwidget.h"

#include "details.h"
#include "ui_simpleitemeditwidget.h"
#include "clientsettings.h"
#include "referenceddatamodel.h"

#include "kdcrmdata/kdcrmutils.h"
#include "kdcrmdata/kdcrmfields.h"

#include <Akonadi/Collection>
#include <Akonadi/Item>
#include <Akonadi/ItemCreateJob>
#include <Akonadi/ItemModifyJob>

#include <KPIMUtils/Email>

#include <QMessageBox>
#include <QPushButton>

using namespace Akonadi;

class SimpleItemEditWidget::Private
{
    SimpleItemEditWidget *const q;

public:
    explicit Private(Details *details, SimpleItemEditWidget *parent)
        : q(parent), mDetails(details),
          mButtonBox(nullptr), mIsModified(false)
    {
    }

    void setData(const QMap<QString, QString> &data);
    QMap<QString, QString> data() const;

public:
    Ui::SimpleItemEditWidget mUi;

    Item mItem;
    Collection mCollection;
    Details *mDetails;
    QDialogButtonBox *mButtonBox;
    bool mIsModified;

public: // slots
    void saveClicked();
    void dataModified();
    void descriptionModificationChanged(bool changed);
    void saveResult(KJob *job);
};

// This doesn't derive from QDialog anymore because for some reason KWin (or Qt)
// places all dialogs at the same position on the screen, even in Random or Smart placement policy.
// (FATCRM-76). Anyway we don't need modality, just support for Esc.
void SimpleItemEditWidget::Private::setData(const QMap<QString, QString> &data)
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
QMap<QString, QString> SimpleItemEditWidget::Private::data() const
{
    QMap<QString, QString> currentData = mDetails->getData();

    currentData[KDCRMFields::description()] = mUi.description->toPlainText();
    if (mDetails->type() == Campaign) {
        currentData[KDCRMFields::content()] = mUi.description->toPlainText();
    }
    // Turn "First Name <email@example.com> into "email@example.com so that SugarCRM doesn't reject it.
    if (mDetails->type() == Contact) {
        QString email = currentData.value(KDCRMFields::email1());
        if (!email.isEmpty()) {
            currentData.insert(KDCRMFields::email1(), KPIMUtils::extractEmailAddress(email));
        }

        email = currentData.value(KDCRMFields::email2());
        if (!email.isEmpty()) {
            currentData.insert(KDCRMFields::email2(), KPIMUtils::extractEmailAddress(email));
        }
    }

    return currentData;
}

void SimpleItemEditWidget::Private::saveClicked()
{
    if (!mIsModified) {
        q->close(); // will emit closing
        return;
    }

    if (mDetails->type() == Opportunity) {
        if (mDetails->currentAccountId().isEmpty()) {
            QMessageBox::warning(mDetails, i18n("Invalid opportunity data"), i18n("You need to select an account for this opportunity."));
            return;
        }
    }

    Item item = mItem;
    mDetails->updateItem(item, data());

    Job *job = nullptr;
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

void SimpleItemEditWidget::Private::descriptionModificationChanged(bool changed)
{
    // TODO: KF5: Make this inline, i.e. turn into a lambda inside connect(...)
    if (changed) {
        dataModified();
    }
}

void SimpleItemEditWidget::Private::dataModified()
{
    mIsModified = true;
    q->setWindowTitle(q->title());
    q->setWindowModified(true);
    emit q->dataModified();
}

bool SimpleItemEditWidget::isModified() const
{
    return d->mIsModified;
}

QString SimpleItemEditWidget::title() const
{
    if (d->mItem.isValid()) {
        const QString name = detailsName();
        switch (d->mDetails->type()) {
        case Account: return i18n("Account: %1[*]", name);
        case Campaign: return i18n("Campaign: %1[*]", name);
        case Contact: return i18n("Contact: %1[*]", name);
        case Lead: return i18n("Lead: %1[*]", name);
        case Opportunity: return i18n("Opportunity: %1[*]", name);
        }
    } else {
        switch (d->mDetails->type()) {
        case Account: return i18n("New Account[*]");
        case Campaign: return i18n("New Campaign[*]");
        case Contact: return i18n("New Contact[*]");
        case Lead: return i18n("New Lead[*]");
        case Opportunity: return i18n("New Opportunity[*]");
        }
    }
    return QString(); // for stupid compilers
}

QString SimpleItemEditWidget::detailsName() const
{
    return d->mDetails->name();
}

Item SimpleItemEditWidget::item() const
{
    return d->mItem;
}

void SimpleItemEditWidget::Private::saveResult(KJob *job)
{
    kDebug() << "save result=" << job->error();
    if (job->error() != 0) {
        kError() << job->errorText();
        mUi.labelOffline->setText(job->errorText());
        mUi.labelOffline->show();
        return;
    } else {
        mIsModified = false;
    }
    emit q->itemSaved();
    q->close();
}

SimpleItemEditWidget::SimpleItemEditWidget(Details *details, QWidget *parent)
    : ItemEditWidgetBase(parent), d(new Private(details, this))
{
    d->mUi.setupUi(this);

    QVBoxLayout *detailsLayout = new QVBoxLayout(d->mUi.detailsContainer);
    detailsLayout->addWidget(details);

    connect(d->mDetails, SIGNAL(modified()), this, SLOT(dataModified()));

    // connect to document's modificationChanged instead of QTextEdit's textChanged(),
    // to ignore notifications triggered by the syntax highlighter
    // cf. http://stackoverflow.com/questions/22685463/qt5-tell-qplaintextedit-to-ignore-syntax-highlighting-changes
    connect(d->mUi.description->document(), SIGNAL(modificationChanged(bool)), this, SLOT(descriptionModificationChanged(bool)));

    QPushButton *saveButton = d->mUi.buttonBox->button(QDialogButtonBox::Save);
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveClicked()));

    QPushButton *cancelButton = d->mUi.buttonBox->button(QDialogButtonBox::Cancel);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

    if (isWindow())
        ClientSettings::self()->restoreWindowSize("details", this);
}

SimpleItemEditWidget::~SimpleItemEditWidget()
{
    if (isWindow())
        ClientSettings::self()->saveWindowSize("details", this);
    delete d;
}

void SimpleItemEditWidget::accept()
{
    d->saveClicked();
}

// open for creation
void SimpleItemEditWidget::showNewItem(const QMap<QString, QString> &data, const Akonadi::Collection &collection)
{
    d->setData(data);

    d->mCollection = collection;

    // hide creation/modification date/user
    d->mUi.createdModifiedContainer->hide();

    d->mDetails->assignToMe();

    d->mIsModified = false;
    setWindowTitle(title());
    setWindowModified(false);
}

// open for modification
void SimpleItemEditWidget::setItem(const Akonadi::Item &item)
{
    d->mItem = item;

    Q_ASSERT(item.isValid());
    d->setData(d->mDetails->data(item));

    setWindowTitle(title());
    setWindowModified(false);
    d->mIsModified = false;
}

void SimpleItemEditWidget::updateItem(const Akonadi::Item &item)
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

void SimpleItemEditWidget::setOnline(bool online)
{
    d->mUi.labelOffline->setVisible(!online);
    if (!online) {
        d->mUi.labelOffline->setText(i18n("Warning: FatCRM is currently offline. Changes will only be sent to the server once it's online again."));
    }
}

Details *SimpleItemEditWidget::details()
{
    return d->mDetails;
}

void SimpleItemEditWidget::hideButtonBox()
{
    d->mUi.buttonBox->hide();
}
#include "simpleitemeditwidget.moc"
