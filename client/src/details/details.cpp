#include "details.h"
#include "kdcrmutils.h"

#include <KDateTimeEdit>

using namespace Akonadi;

static QStringList storedProperties()
{
    static QStringList props;
    if (props.isEmpty()) {
        props << "modifiedBy";
        props << "modifiedByName";
        props << "modifiedUserId";
        props << "modifiedUserName";
        props << "dateEntered";
        props << "deleted";
        props << "id";
        props << "contactId";
        props << "opportunityRoleFields";
        props << "cAcceptStatusFields";
        props << "mAcceptStatusFields";
        props << "createdBy";
        props << "createdByName";
        props << "createdById";
    }
    return props;
}

Details::Details(DetailsType type, QWidget *parent)
    : QWidget(parent), mType(type)
{
    // connect to changed signals
    QList<QLineEdit *> lineEdits =  findChildren<QLineEdit *>();
    Q_FOREACH (QLineEdit *le, lineEdits)
        connect(le, SIGNAL(textChanged(QString)), this, SIGNAL(modified()));
    QList<QComboBox *> comboBoxes =  findChildren<QComboBox *>();
    Q_FOREACH (QComboBox *cb, comboBoxes)
        connect(cb, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
    QList<QCheckBox *> checkBoxes =  findChildren<QCheckBox *>();
    Q_FOREACH (QCheckBox *cb, checkBoxes)
        connect(cb, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
    QList<QTextEdit *> textEdits = findChildren<QTextEdit *>();
    Q_FOREACH (QTextEdit *te, textEdits)
        connect(te, SIGNAL(textChanged()), this, SIGNAL(modified()));
    Q_FOREACH (KDateTimeEdit *w, findChildren<KDateTimeEdit *>())
        connect(w, SIGNAL(dateChanged(QDate)), this, SIGNAL(modified()));

    initialize();
}

Details::~Details()
{
}

void Details::initialize()
{
}

/*
 *  Reset all widgets
 *
 */
void Details::clear()
{
    Q_FOREACH (QLineEdit *le, findChildren<QLineEdit *>()) {
        le->setText(QString());
    }
    Q_FOREACH (QComboBox *cb, findChildren<QComboBox *>()) {
        cb->setCurrentIndex(0);
    }
    Q_FOREACH (QCheckBox *cb, findChildren<QCheckBox *>()) {
        cb->setChecked(false);
    }
    Q_FOREACH (QTextEdit *te, findChildren<QTextEdit *>()) {
        te->setPlainText(QString());
    }
    Q_FOREACH (KDateTimeEdit *w, findChildren<KDateTimeEdit *>()) {
        w->setDate(QDate());
    }
    Q_FOREACH (const QString &prop, storedProperties()) {
        setProperty(prop.toLatin1(), QVariant());
    }
}

void Details::setResourceIdentifier(const QByteArray &ident)
{
    mResourceIdentifier = ident;
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
void Details::setData(const QMap<QString, QString> &data, QGroupBox *informationGB)
{
    QString key;

    QList<QLineEdit *> lineEdits =  findChildren<QLineEdit *>();
    Q_FOREACH (QLineEdit *le, lineEdits) {
        key = le->objectName();
        if (key.isEmpty()) continue;
        le->setText(data.value(key));
    }
    QList<QComboBox *> comboBoxes =  findChildren<QComboBox *>();
    Q_FOREACH (QComboBox *cb, comboBoxes) {
        key = cb->objectName();
        if (key.isEmpty()) continue;
        cb->setCurrentIndex(cb->findText(data.value(key)));
        // currency is unique an cannot be changed from the client atm
        if (key == "currency") {
            cb->setCurrentIndex(0);   // default
            cb->setProperty("currencyId",
                            qVariantFromValue<QString>(data.value("currencyId")));
            cb->setProperty("currencyName",
                            qVariantFromValue<QString>(data.value("currencyName")));
            cb->setProperty("currencySymbol",
                            qVariantFromValue<QString>(data.value("currencySymbol")));
        }
    }

    QList<QCheckBox *> checkBoxes = findChildren<QCheckBox *>();
    Q_FOREACH (QCheckBox *cb, checkBoxes) {
        key = cb->objectName();
        if (key.isEmpty()) continue;
        cb->setChecked(data.value(key) == "1" ? true : false);
    }

    QList<QTextEdit *> textEdits = findChildren<QTextEdit *>();
    Q_FOREACH (QTextEdit *te, textEdits) {
        key = te->objectName();
        if (key.isEmpty()) continue;
        te->setPlainText(data.value(key));
    }

    Q_FOREACH (KDateTimeEdit *w, findChildren<KDateTimeEdit *>()) {
        key = w->objectName();
        if (key.isEmpty()) continue;
        w->setDate(KDCRMUtils::dateFromString(data.value(key)));
    }

    QList<QLabel *> labels = informationGB->findChildren<QLabel *>();
    Q_FOREACH (QLabel *lb, labels) {
        key = lb->objectName();
        if (key == "modifiedBy") {
            if (!data.value("modifiedByName").isEmpty()) {
                lb->setText(data.value("modifiedByName"));
            } else if (!data.value("modifiedBy").isEmpty()) {
                lb->setText(data.value("modifiedBy"));
            } else {
                lb->setText(data.value("modifiedUserName"));
            }
        } else if (key == "dateEntered") {
            lb->setText(KDCRMUtils::formatTimestamp(data.value("dateEntered")));
        } else if (key == "createdBy") {
            if (!data.value("createdByName").isEmpty()) {
                lb->setText(data.value("createdByName"));
            } else {
                lb->setText(data.value("createdBy"));
            }
        }
    }

    Q_FOREACH (const QString &prop, storedProperties()) {
        setProperty(prop.toLatin1(), data.value(prop));
    }

    setDataInternal(data);
}

/*
 *
 * Return a Map with the widgets data
 */
const QMap<QString, QString> Details::getData() const
{
    QMap<QString, QString> currentData;
    QString key;
    QList<QLineEdit *> lineEdits = findChildren<QLineEdit *>();
    Q_FOREACH (QLineEdit *le, lineEdits) {
        key = le->objectName();
        if (key.isEmpty()) continue;
        currentData[key] = le->text();
    }

    QList<QComboBox *> comboBoxes = findChildren<QComboBox *>();
    Q_FOREACH (QComboBox *cb, comboBoxes) {
        key = cb->objectName();
        if (key.isEmpty()) continue;
        currentData[key] = cb->currentText();
        if (key == "currency") {
            currentData["currencyId"] = cb->property("currencyId").toString();
            currentData["currencyName"] = cb->property("currencyName").toString();
            currentData["currencySymbol"] = cb->property("currencySymbol").toString();
        }
    }

    QList<QCheckBox *> checkBoxes = findChildren<QCheckBox *>();
    Q_FOREACH (QCheckBox *cb, checkBoxes) {
        key = cb->objectName();
        if (key.isEmpty()) continue;
        currentData[key] = cb->isChecked() ? "1" : "0";
    }

    QList<QTextEdit *> textEdits = findChildren<QTextEdit *>();
    Q_FOREACH (QTextEdit *te, textEdits) {
        key = te->objectName();
        if (key.isEmpty()) continue;
        currentData[key] = te->toPlainText();
    }

    Q_FOREACH (KDateTimeEdit *w, findChildren<KDateTimeEdit *>()) {
        key = w->objectName();
        if (key.isEmpty()) continue;
        currentData.insert(key, KDCRMUtils::dateToString(w->date()));
    }

    // will be overwritten by the server, but good to have for comparison in case of change conflict
    currentData["dateModified"] = KDCRMUtils::currentTimestamp();

    Q_FOREACH (const QString &prop, storedProperties()) {
        currentData.insert(prop, property(prop.toLatin1()).toString());
    }

    return currentData;
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
 * Return the list of items for the currency combo boxes
 *
 */
QStringList Details::currencyItems() const
{
    // we do not have the choice here
    // Should be set remotely by admin
    QStringList currencies;
    currencies << QString("US Dollars : $");
    return currencies;
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
