/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "details.h"

#include "accountrepository.h"
#include "clientsettings.h"
#include "collectionmanager.h"
#include "kdcrmfields.h"
#include "kdcrmutils.h"
#include "nullabledatecombobox.h"
#include "referenceddatamodel.h"
#include "referenceddata.h"
#include "itemdataextractor.h"
#include "itemstreemodel.h"

#include <KLocalizedString>

#include <QCheckBox>
#include <QComboBox>
#include <QCompleter>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QTextEdit>
#include <QGroupBox>
#include <QMap>
#include <QGuiApplication>
#include <QClipboard>

#include "fatcrm_client_debug.h"

using namespace Akonadi;

// These are basically the "readonly" fields, that the user cannot see or modify
// but we need to preserve when saving.
static QStringList storedProperties()
{
    static QStringList props;
    if (props.isEmpty()) {
        props << KDCRMFields::modifiedByName();
        props << KDCRMFields::modifiedUserId();
        props << KDCRMFields::dateEntered();
        props << KDCRMFields::deleted();
        props << KDCRMFields::id();
        props << KDCRMFields::createdBy();
        props << KDCRMFields::createdByName();
        props << KDCRMFields::contactId(); // leads, notes

        props << KDCRMFields::opportunityRoleFields(); // contacts
        props << KDCRMFields::cAcceptStatusFields(); // contacts
        props << KDCRMFields::mAcceptStatusFields(); // contacts
    }
    return props;
}

static bool isQtPrivateObject(const QString &objectName)
{
    return (objectName.startsWith(QLatin1String("qt_")) ||
            objectName.startsWith(QLatin1String("_qt_")));
}


Details::Details(DetailsType type, QWidget *parent)
    : QWidget(parent), mItemsTreeModel(nullptr), mType(type)
{
    // delayed init, wait for subclasses to create GUI
    QMetaObject::invokeMethod(this, "doConnects", Qt::QueuedConnection);
}

Details::~Details()
{
}

void Details::doConnects()
{
    // connect to changed signals
    Q_FOREACH (QLineEdit *le, findChildren<QLineEdit *>()) {
        if (!qobject_cast<QAbstractSpinBox *>(le->parentWidget()))
            connect(le, &QLineEdit::textChanged, this, &Details::modified);
    }
    Q_FOREACH (QComboBox *cb, findChildren<QComboBox *>())
        connect(cb, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
    Q_FOREACH (QCheckBox *cb, findChildren<QCheckBox *>())
        connect(cb, &QAbstractButton::toggled, this, &Details::modified);
    Q_FOREACH (QTextEdit *te, findChildren<QTextEdit *>())
        connect(te, &QTextEdit::textChanged, this, &Details::modified);
    Q_FOREACH (QPlainTextEdit *te, findChildren<QPlainTextEdit *>())
        connect(te, &QPlainTextEdit::textChanged, this, &Details::modified);
    Q_FOREACH (QSpinBox *w, findChildren<QSpinBox *>())
        connect(w, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
    Q_FOREACH (QDoubleSpinBox *w, findChildren<QDoubleSpinBox *>())
        connect(w, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
    Q_FOREACH (NullableDateComboBox *w, findChildren<NullableDateComboBox *>())
        connect(w, &KDateComboBox::dateChanged, this, &Details::modified);
}

void Details::hideIfUnsupported(QWidget *widget)
{
    const QString key = widget->objectName();
    if (!mKeys.contains(key)) {
        //qCDebug(FATCRM_CLIENT_LOG) << "HIDING" << key << "because of the contents of the EntityAnnotationsAttribute for the collection";
        widget->hide();
        // Find the label whose buddy is <widget>
        foreach (QLabel *label, widget->parentWidget()->findChildren<QLabel *>()) {
            if (label->buddy() == widget) {
                label->hide();
                break;
            }
        }
    }
}

void Details::fillComboBox(QComboBox *combo, const QString &objectName) const
{
    if (mKeys.contains(combo->objectName()) && combo->count() == 0) {
        const int enumIndex = mEnumDefinitions.indexOf(objectName);
        if (enumIndex == -1) {
            qCWarning(FATCRM_CLIENT_LOG) << "Enum definition not found for" << objectName << "in" << typeToString(mType);
        } else {
            const EnumDefinitions::Enum &def = mEnumDefinitions.at(enumIndex);
            for (const auto &keyValue : def.mEnumValues) {
                combo->addItem(keyValue.value, keyValue.key);
            }
        }
    }
}

/*
 *  Reset all widgets
 *
 */
void Details::clear()
{
    Q_FOREACH (QLineEdit *le, findChildren<QLineEdit *>()) {
        le->clear();
    }
    Q_FOREACH (QComboBox *cb, findChildren<QComboBox *>()) {
        cb->setCurrentIndex(0);
    }
    Q_FOREACH (QCheckBox *cb, findChildren<QCheckBox *>()) {
        cb->setChecked(false);
    }
    Q_FOREACH (QTextEdit *te, findChildren<QTextEdit *>()) {
        te->clear();
    }
    Q_FOREACH (QPlainTextEdit *te, findChildren<QPlainTextEdit *>()) {
        te->clear();
    }
    Q_FOREACH (QSpinBox *w, findChildren<QSpinBox *>()) {
        w->clear();
    }
    Q_FOREACH (QDoubleSpinBox *w, findChildren<QDoubleSpinBox *>()) {
        w->clear();
    }
    Q_FOREACH (NullableDateComboBox *w, findChildren<NullableDateComboBox *>()) {
        w->setDate(QDate());
    }
    Q_FOREACH (const QString &prop, storedProperties()) {
        setProperty(prop.toLatin1(), QVariant());
    }
}

void Details::setResourceIdentifier(const QByteArray &ident, const QString &baseUrl)
{
    mResourceIdentifier = ident;
    mResourceBaseUrl = baseUrl;
}

void Details::setSupportedFields(const QStringList &fields)
{
    mKeys = fields;
    Q_ASSERT(mKeys.contains("id"));
}

void Details::setEnumDefinitions(const EnumDefinitions &enums)
{
    mEnumDefinitions = enums;
}

void Details::setCollectionManager(CollectionManager *collectionManager)
{
    Akonadi::Collection::Id coll = collectionManager->collectionIdForType(mType);
    qCDebug(FATCRM_CLIENT_LOG) << typeToString(mType) << coll << collectionManager->supportedFields(coll);
    setSupportedFields(collectionManager->supportedFields(coll));
    setEnumDefinitions(collectionManager->enumDefinitions(coll));
}

/*
 * Fill in the widgets with the data and properties that belong to
 * them
 *
 */
void Details::setData(const QMap<QString, QString> &data,
                      QWidget *createdModifiedContainer)
{
    Q_FOREACH (const QString &prop, storedProperties()) {
        if (data.contains(prop)) {
            setProperty(prop.toLatin1(), data.value(prop));
        }
    }
    setProperty("name", data.value(QStringLiteral("name"))); // displayed in lineedit, but useful for subclasses (e.g. NotesDialog title)

    if (mKeys.isEmpty()) {
        mKeys = data.keys(); // remember what are the expected keys, so getData can skip internal widgets
        Q_ASSERT(mKeys.contains("id"));
    }

    // Ensure comboboxes are filled
    setDataInternal(data);

    QString key;

    QList<QLineEdit *> lineEdits = findChildren<QLineEdit *>();
    Q_FOREACH (QLineEdit *w, lineEdits) {
        key = w->objectName();
        if (isQtPrivateObject(key)) {
            continue; // skip internal lineedits (e.g. in spinbox)
        }
        hideIfUnsupported(w);
        w->setText(data.value(key));
    }
    QList<QComboBox *> comboBoxes = findChildren<QComboBox *>();
    Q_FOREACH (QComboBox *cb, comboBoxes) {
        key = cb->objectName();
        if (isQtPrivateObject(key)) {
            continue; // skip internal combos (e.g. in NullableDateComboBox)
        }
        hideIfUnsupported(cb);
        const int idx = cb->findData(data.value(key));
        if (idx == -1 && cb->count() > 1) {
            qCDebug(FATCRM_CLIENT_LOG) << "Didn't find" << data.value(key) << "in combo" << key;
            //for (int row = 0; row < cb->count(); ++row) {
            //    qCDebug(FATCRM_CLIENT_LOG) << "  " << cb->itemData(row);
            //}
        }
        cb->setCurrentIndex(idx);
    }

    QList<QCheckBox *> checkBoxes = findChildren<QCheckBox *>();
    Q_FOREACH (QCheckBox *w, checkBoxes) {
        key = w->objectName();
        hideIfUnsupported(w);
        w->setChecked(data.value(key) == QLatin1String("1"));
    }

    QList<QTextEdit *> textEdits = findChildren<QTextEdit *>();
    Q_FOREACH (QTextEdit *w, textEdits) {
        key = w->objectName();
        hideIfUnsupported(w);
        w->setPlainText(data.value(key));
    }

    QList<QPlainTextEdit *> plainTextEdits = findChildren<QPlainTextEdit *>();
    Q_FOREACH (QPlainTextEdit *w, plainTextEdits) {
        key = w->objectName();
        hideIfUnsupported(w);
        w->setPlainText(data.value(key));
    }

    Q_FOREACH (QSpinBox *w, findChildren<QSpinBox *>()) {
        key = w->objectName();
        if (isQtPrivateObject(key)) {
            continue; // skip internal widgets (e.g. in QCalendarWidget)
        }
        hideIfUnsupported(w);
        w->setValue(data.value(key).toInt());
    }

    Q_FOREACH (QDoubleSpinBox *w, findChildren<QDoubleSpinBox *>()) {
        key = w->objectName();
        hideIfUnsupported(w);
        //qCDebug(FATCRM_CLIENT_LOG) << data.value(key);
        w->setValue(QLocale::c().toDouble(data.value(key)));
    }

    Q_FOREACH (NullableDateComboBox *w, findChildren<NullableDateComboBox *>()) {
        key = w->objectName();
        hideIfUnsupported(w);
        //qCDebug(FATCRM_CLIENT_LOG) << w << "setDate" << key << data.value(key) << KDCRMUtils::dateFromString(data.value(key));
        w->setDate(KDCRMUtils::dateFromString(data.value(key)));
    }

    QList<QLabel *> labels = createdModifiedContainer->findChildren<QLabel *>();
    Q_FOREACH (QLabel *lb, labels) {
        key = lb->objectName();
        if (key == KDCRMFields::modifiedByName()) {
            lb->setText(data.value(KDCRMFields::modifiedByName()));
        } else if (key == KDCRMFields::dateEntered()) {
            lb->setText(KDCRMUtils::formatTimestamp(data.value(KDCRMFields::dateEntered())));
        } else if (key == KDCRMFields::createdByName()) {
            lb->setText(data.value(KDCRMFields::createdByName()));
        }
    }
}

/*
 *
 * Return a Map with the widgets data
 */
const QMap<QString, QString> Details::getData() const
{
    Q_ASSERT(mKeys.contains("id"));

    QMap<QString, QString> currentData;
    QString key;
    QList<QLineEdit *> lineEdits = findChildren<QLineEdit *>();
    Q_FOREACH (QLineEdit *le, lineEdits) {
        key = le->objectName();
        if (!mKeys.contains(key)) continue;
        if (qobject_cast<QAbstractSpinBox *>(le->parent())) continue;
        currentData[key] = le->text();
    }

    QList<QComboBox *> comboBoxes = findChildren<QComboBox *>();
    Q_FOREACH (QComboBox *cb, comboBoxes) {
        key = cb->objectName();
        if (!mKeys.contains(key)) { qCDebug(FATCRM_CLIENT_LOG) << "skipping" << key; continue; }
        currentData[key] = cb->itemData(cb->currentIndex()).toString();
    }

    QList<QCheckBox *> checkBoxes = findChildren<QCheckBox *>();
    Q_FOREACH (QCheckBox *cb, checkBoxes) {
        key = cb->objectName();
        if (!mKeys.contains(key)) continue;
        currentData[key] = cb->isChecked() ? "1" : "0";
    }

    QList<QTextEdit *> textEdits = findChildren<QTextEdit *>();
    Q_FOREACH (QTextEdit *w, textEdits) {
        key = w->objectName();
        if (!mKeys.contains(key)) continue;
        currentData[key] = w->toPlainText();
    }

    QList<QPlainTextEdit *> plainTextEdits = findChildren<QPlainTextEdit *>();
    Q_FOREACH (QPlainTextEdit *w, plainTextEdits) {
        key = w->objectName();
        if (!mKeys.contains(key)) continue;
        currentData.insert(key, w->toPlainText());
    }

    Q_FOREACH (QSpinBox *w, findChildren<QSpinBox *>()) {
        key = w->objectName();
        if (!mKeys.contains(key)) continue;
        currentData[key] = QString::number(w->value());
    }

    Q_FOREACH (QDoubleSpinBox *w, findChildren<QDoubleSpinBox *>()) {
        key = w->objectName();
        if (!mKeys.contains(key)) continue;
        currentData[key] = QString::number(w->value());
    }

    Q_FOREACH (NullableDateComboBox *w, findChildren<NullableDateComboBox *>()) {
        key = w->objectName();
        if (!mKeys.contains(key)) continue;
        currentData.insert(key, KDCRMUtils::dateToString(w->date()));
    }

    Q_FOREACH (const QString &prop, storedProperties()) {
        QVariant val = property(prop.toLatin1());
        if (val.isValid()) {
            currentData.insert(prop, val.toString());
        }
    }

    // Fill assignee username from assignee userid so it shows up in the model.
    // Alternatively we could resolve assignedUserId in the model....
    const QString assigneeId = currentData.value(KDCRMFields::assignedUserId());
    if (!assigneeId.isEmpty()) {
        currentData.insert(KDCRMFields::assignedUserName(), ReferencedData::instance(AssignedToRef)->referencedData(assigneeId));
    }
    // Do the same with accounts just in case.
    const QString accountId = currentData.value(KDCRMFields::accountId());
    if (!accountId.isEmpty()) {
        currentData.insert(KDCRMFields::accountName(), ReferencedData::instance(AccountRef)->referencedData(accountId));
    }

    // will be overwritten by the server, but good to have for comparison in case of change conflict
    // (and for showing in the GUI until the next sync)
    currentData[KDCRMFields::dateModified()] = KDCRMUtils::currentTimestamp();
    const QString fullUserName = ClientSettings::self()->fullUserName();
    currentData[KDCRMFields::modifiedByName()] = fullUserName.isEmpty() ? QStringLiteral("me") : fullUserName;

    // Missing when arriving here (note that description is added later)
    // for opps: amount_usdollar, campaign_id, campaign_name
    // for accounts: parent_name, campaign_id, campaign_name

    return currentData;
}

/*
 * Return the selected Id of "Account Name" or "Parent Name"
 */
QString Details::currentAccountId() const
{
    // Account has KDCRMFields::parentId()
    // Contact, Leads, Opportunity have KDCRMFields::accountId()
    if (mType != DetailsType::Campaign) {
        const QList<QComboBox *> comboBoxes = findChildren<QComboBox *>();
        Q_FOREACH (QComboBox *cb, comboBoxes) {
            const QString key = cb->objectName();
            if (key == KDCRMFields::parentId() || key == KDCRMFields::accountId()) {
                return cb->itemData(cb->currentIndex()).toString();
            }
        }
    }
    return QString();
}

void Details::assignToMe()
{
    const QString fullUserName = ClientSettings::self()->fullUserName();
    if (fullUserName.isEmpty())
        return;
    const QList<QComboBox *> comboBoxes = findChildren<QComboBox *>();
    Q_FOREACH (QComboBox *cb, comboBoxes) {
        const QString key = cb->objectName();
        if (key == KDCRMFields::assignedUserId()) {
            const int idx = cb->findText(fullUserName);
            if (idx >= 0) {
                cb->setCurrentIndex(idx);
            }
        }
    }
}

QString Details::name() const
{
    if (mType == DetailsType::Contact) {
        return findChild<QLineEdit *>(KDCRMFields::firstName())->text() + ' ' + findChild<QLineEdit *>(KDCRMFields::lastName())->text();
    }
    return property("name").toString();
}

QString Details::id() const
{
    return property("id").toString();
}

QMap<QString, QString> Details::fillAddressFieldsMap(QGroupBox *) const
{
    qWarning() << "Missing fillAddressFieldsMap implementation";
    Q_ASSERT(false);
    return QMap<QString, QString>();
}

QCompleter *Details::createCountriesCompleter()
{
    QCompleter *completer = new QCompleter(AccountRepository::instance()->countries(), this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    return completer;
}

void Details::setItemsTreeModel(ItemsTreeModel *model)
{
    // keep the member - could be useful later
    mItemsTreeModel = model;
}

void Details::copyAddressFromGroup(QGroupBox *box)
{
    QMap<QString, QString> fieldNames = fillAddressFieldsMap(box);

    QMap<QString, QString> address;
    for (auto it = fieldNames.constBegin(); it != fieldNames.constEnd(); ++it) {
        auto *field = box->findChild<QWidget*>(it.value());
        if (field) {
            auto *plainTextEdit = qobject_cast<QPlainTextEdit*>(field);
            if (plainTextEdit) {
                address.insert(it.key(), plainTextEdit->toPlainText());
                continue;
            }
            auto *lineEdit = qobject_cast<QLineEdit*>(field);
            if (lineEdit) {
                address.insert(it.key(), lineEdit->text());
                continue;
            }
            qWarning() << "Failed to retrieve value from field:" << it.value();
        } else {
            qWarning() << "Failed to find widget for object name:" << it.value();
        }
    }
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(QString("%1\n%2 %3 %4\n%5")
                       .arg(address.value(QStringLiteral("street")),
                            address.value(QStringLiteral("postalcode")),
                            address.value(QStringLiteral("city")),
                            address.value(QStringLiteral("state")),
                            address.value(QStringLiteral("country"))));
}
