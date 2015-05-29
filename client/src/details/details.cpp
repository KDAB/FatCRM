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

#include "details.h"
#include "kdcrmutils.h"
#include "kdcrmfields.h"
#include "clientsettings.h"

#include <qdateeditex.h>
#include <referenceddatamodel.h>

using namespace Akonadi;

// These are basically the "readonly" fields, that the user cannot see or modify
// but we need to preserve when saving.
static QStringList storedProperties()
{
    static QStringList props;
    if (props.isEmpty()) {
        props << KDCRMFields::modifiedBy();
        props << KDCRMFields::modifiedByName();
        props << KDCRMFields::modifiedUserId();
        props << KDCRMFields::dateEntered();
        props << KDCRMFields::deleted();
        props << KDCRMFields::id();
        props << KDCRMFields::createdBy();
        props << KDCRMFields::createdByName();
        props << KDCRMFields::currencyId(); // campaign, opportunity
        props << KDCRMFields::currencyName(); // opportunity
        props << KDCRMFields::currencySymbol(); // opportunity
        props << KDCRMFields::contactId(); // leads, notes

        props << "opportunityRoleFields"; // contacts
        props << "cAcceptStatusFields"; // contacts
        props << "mAcceptStatusFields"; // contacts
    }
    return props;
}

Details::Details(DetailsType type, QWidget *parent)
    : QWidget(parent), mType(type)
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
            connect(le, SIGNAL(textChanged(QString)), this, SIGNAL(modified()));
    }
    Q_FOREACH (QComboBox *cb, findChildren<QComboBox *>())
        connect(cb, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
    Q_FOREACH (QCheckBox *cb, findChildren<QCheckBox *>())
        connect(cb, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
    Q_FOREACH (QTextEdit *te, findChildren<QTextEdit *>())
        connect(te, SIGNAL(textChanged()), this, SIGNAL(modified()));
    Q_FOREACH (QPlainTextEdit *te, findChildren<QPlainTextEdit *>())
        connect(te, SIGNAL(textChanged()), this, SIGNAL(modified()));
    Q_FOREACH (QSpinBox *w, findChildren<QSpinBox *>())
        connect(w, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
    Q_FOREACH (QDoubleSpinBox *w, findChildren<QDoubleSpinBox *>())
        connect(w, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
    Q_FOREACH (QDateEditEx *w, findChildren<QDateEditEx *>())
        connect(w, SIGNAL(dateChanged(QDate)), this, SIGNAL(modified()));
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
    Q_FOREACH (QDateEditEx *w, findChildren<QDateEditEx *>()) {
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

// TODO should probably be virtual and include item specific data, e.g. a contact's full name
QString Details::windowTitle() const
{
    switch (mType) {
    case Account: return tr("Account Details");
    case Campaign: return tr("Campaign Details");
    case Contact: return tr("Contact Details");
    case Lead: return tr("Lead Details");
    case Opportunity: return tr("Opportunity Details");
    }

    return QString();
}

/*
 * Fill in the widgets with the data and properties that belong to
 * them
 *
 */
void Details::setData(const QMap<QString, QString> &data,
                      QWidget *createdModifiedContainer)
{
    QString key;

    QList<QLineEdit *> lineEdits = findChildren<QLineEdit *>();
    Q_FOREACH (QLineEdit *le, lineEdits) {
        key = le->objectName();
        if (!data.contains(key)) continue; // skip internal lineedits (e.g. in spinbox)
        le->setText(data.value(key));
    }
    QList<QComboBox *> comboBoxes = findChildren<QComboBox *>();
    Q_FOREACH (QComboBox *cb, comboBoxes) {
        key = cb->objectName();
        if (!data.contains(key)) continue; // skip internal combos (e.g. in QDateEditEx)
        //qDebug() << cb << "setCurrentIndex" << cb->findText(data.value(key)) << "from findText" << data.value(key);
        cb->setCurrentIndex(cb->findText(data.value(key)));
    }

    QList<QCheckBox *> checkBoxes = findChildren<QCheckBox *>();
    Q_FOREACH (QCheckBox *cb, checkBoxes) {
        key = cb->objectName();
        if (!data.contains(key)) continue;
        cb->setChecked(data.value(key) == "1" ? true : false);
    }

    QList<QTextEdit *> textEdits = findChildren<QTextEdit *>();
    Q_FOREACH (QTextEdit *w, textEdits) {
        key = w->objectName();
        if (!data.contains(key)) continue;
        w->setPlainText(data.value(key));
    }

    QList<QPlainTextEdit *> plainTextEdits = findChildren<QPlainTextEdit *>();
    Q_FOREACH (QPlainTextEdit *w, plainTextEdits) {
        key = w->objectName();
        if (!data.contains(key)) continue;
        w->setPlainText(data.value(key));
    }

    Q_FOREACH (QSpinBox *w, findChildren<QSpinBox *>()) {
        key = w->objectName();
        if (!data.contains(key)) continue;
        w->setValue(data.value(key).toInt());
    }

    Q_FOREACH (QDoubleSpinBox *w, findChildren<QDoubleSpinBox *>()) {
        key = w->objectName();
        if (!data.contains(key)) continue;
        //qDebug() << data.value(key);
        w->setValue(QLocale::c().toDouble(data.value(key)));
        if (key == KDCRMFields::amount())
            w->setSuffix(data.value(KDCRMFields::currencySymbol()));
    }

    Q_FOREACH (QDateEditEx *w, findChildren<QDateEditEx *>()) {
        key = w->objectName();
        if (!data.contains(key)) continue;
        //qDebug() << w << "setDate" << key << data.value(key) << KDCRMUtils::dateFromString(data.value(key));
        w->setDate(KDCRMUtils::dateFromString(data.value(key)));
    }

    QList<QLabel *> labels = createdModifiedContainer->findChildren<QLabel *>();
    Q_FOREACH (QLabel *lb, labels) {
        key = lb->objectName();
        if (key == KDCRMFields::modifiedBy()) {
            if (!data.value(KDCRMFields::modifiedByName()).isEmpty()) {
                lb->setText(data.value(KDCRMFields::modifiedByName()));
            } else if (!data.value(KDCRMFields::modifiedBy()).isEmpty()) {
                lb->setText(data.value(KDCRMFields::modifiedBy()));
            } else {
                lb->setText(data.value("modifiedUserName"));
            }
        } else if (key == KDCRMFields::dateEntered()) {
            lb->setText(KDCRMUtils::formatTimestamp(data.value(KDCRMFields::dateEntered())));
        } else if (key == KDCRMFields::createdBy()) {
            if (!data.value(KDCRMFields::createdByName()).isEmpty()) {
                lb->setText(data.value(KDCRMFields::createdByName()));
            } else {
                lb->setText(data.value(KDCRMFields::createdBy()));
            }
        }
    }

    Q_FOREACH (const QString &prop, storedProperties()) {
        if (data.contains(prop)) {
            setProperty(prop.toLatin1(), data.value(prop));
        }
    }
    setProperty("name", data.value("name")); // displayed in lineedit, but useful for subclasses (e.g. NotesDialog title)

    setDataInternal(data);

    if (mKeys.isEmpty()) {
        mKeys = data.keys(); // remember what are the expected keys, so getData can skip internal widgets
    }
}

/*
 *
 * Return a Map with the widgets data
 */
const QMap<QString, QString> Details::getData() const
{
    Q_ASSERT(!mKeys.isEmpty());

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
        if (!mKeys.contains(key)) continue;
        currentData[key] = cb->currentText();
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

    Q_FOREACH (QDateEditEx *w, findChildren<QDateEditEx *>()) {
        key = w->objectName();
        if (!mKeys.contains(key)) continue;
        currentData.insert(key, KDCRMUtils::dateToString(w->date()));
    }

    Q_FOREACH (const QString &prop, storedProperties()) {
        currentData.insert(prop, property(prop.toLatin1()).toString());
    }

    // will be overwritten by the server, but good to have for comparison in case of change conflict
    // (and for showing in the GUI until the next sync)
    currentData[KDCRMFields::dateModified()] = KDCRMUtils::currentTimestamp();
    const QString fullUserName = ClientSettings::self()->fullUserName();
    currentData[KDCRMFields::modifiedByName()] = fullUserName.isEmpty() ? QString("me") : fullUserName;

    const QString accountId = currentAccountId();
    currentData[KDCRMFields::parentId()] = accountId;
    currentData[KDCRMFields::accountId()] = accountId;
    currentData[KDCRMFields::campaignId()] = currentCampaignId();
    const QString assignedToId = currentAssignedToId();
    currentData[KDCRMFields::assignedUserId()] = assignedToId;
    currentData[KDCRMFields::assignedToId()] = assignedToId;
    currentData[KDCRMFields::reportsToId()] = currentReportsToId();

    return currentData;
}

/*
 * Return the selected Id of "Account Name" or "Parent Name"
 */
QString Details::currentAccountId() const
{
    // Account has KDCRMFields::parentName()
    // Contact, Leads, Opportunity have KDCRMFields::accountName()
    if (mType != Campaign) {
        const QList<QComboBox *> comboBoxes = findChildren<QComboBox *>();
        Q_FOREACH (QComboBox *cb, comboBoxes) {
            const QString key = cb->objectName();
            if (key == KDCRMFields::parentName() || key == KDCRMFields::accountName()) {
                return cb->itemData(cb->currentIndex(), ReferencedDataModel::IdRole).toString();
            }
        }
    }
    return QString();
}

/*
 * Return the selected Campaign Id.
 */
QString Details::currentCampaignId() const
{
    if (mType != Campaign) {
        const QList<QComboBox *> comboBoxes = findChildren<QComboBox *>();
        Q_FOREACH (QComboBox *cb, comboBoxes) {
            const QString key = cb->objectName();
            if (key == KDCRMFields::campaignName() || key == KDCRMFields::campaign()) {
                return cb->itemData(cb->currentIndex(), ReferencedDataModel::IdRole).toString();
            }
        }
    }
    return QString();
}

/*
 * Return the selected "Assigned To" Id
 */
QString Details::currentAssignedToId() const
{
    const QList<QComboBox *> comboBoxes = findChildren<QComboBox *>();
    Q_FOREACH (QComboBox *cb, comboBoxes) {
        const QString key = cb->objectName();
        if (key == KDCRMFields::assignedUserName() || key == KDCRMFields::assignedTo()) {
            return cb->itemData(cb->currentIndex(), ReferencedDataModel::IdRole).toString();
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
        if (key == KDCRMFields::assignedUserName() || key == KDCRMFields::assignedTo()) {
            const int idx = cb->findText(fullUserName);
            if (idx >= 0) {
                cb->setCurrentIndex(idx);
            }
        }
    }
}


/*
 * Return the selected "Reports To" Id
 */
QString Details::currentReportsToId() const
{
    if (mType == Contact) {
        const QList<QComboBox *> comboBoxes = findChildren<QComboBox *>();
        Q_FOREACH (QComboBox *cb, comboBoxes) {
            const QString key = cb->objectName();
            if (key == KDCRMFields::reportsTo()) {
                return cb->itemData(cb->currentIndex(), ReferencedDataModel::IdRole).toString();
            }
        }
    }
    return QString();
}

/*
 * Return the list of items for the industry combo boxes
 *
 */
QStringList Details::industryItems() const
{
    QStringList industries;
    industries << QString("") << QString("Apparel") << QString("Banking")
               << QString("Biotechnology") << QString("Chemicals")
               << QString("Communications") << QString("Construction")
               << QString("Consulting") << QString("Education")
               << QString("Electronics") << QString("Energy")
               << QString("Engineering") << QString("Entertainment")
               << QString("Environmental") << QString("Finance")
               << QString("Government") << QString("Healthcare")
               << QString("Hospitality") << QString("Insurance")
               << QString("Machinery") << QString("Manufacturing")
               << QString("Media") << QString("Not For Profit")
               << QString("Recreation") << QString("Retail")
               << QString("Shipping") << QString("Technology")
               << QString("Telecommunication") << QString("Transportation")
               << QString("Utilities") << QString("Other");
    return industries;
}

/*
 * Return the list of items for the source combo boxes
 *
 */
QStringList Details::sourceItems() const
{
    QStringList sources;
    sources << QString("") << QString("Cold Call")
            << QString("Existing Customer") << QString("Self Generated")
            << QString("Employee") << QString("Partner")
            << QString("Public Relations") << QString("Direct Mail")
            << QString("Conference") << QString("Trade Show")
            << QString("Web Site") << QString("Word of mouth")
            << QString("Email") << QString("Campaign")
            << QString("Other");
    return sources;
}

/*
 * Return the list of items for the salutation combo boxes
 *
 */
QStringList Details::salutationItems() const
{
    QStringList salutations;
    salutations << QString("") << QString("Mr.")
                << QString("Ms.") << QString("Mrs.")
                << QString("Dr.") << QString("Prof.");
    return salutations;
}

QString Details::id() const
{
    return property("id").toString();
}
