#include "details.h"

using namespace Akonadi;

Details::Details(DetailsType type, QWidget *parent)
    : QWidget(parent), mType(type)

{
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
    QList<QLineEdit *> lineEdits =  findChildren<QLineEdit *>();
    Q_FOREACH (QLineEdit *le, lineEdits) {
        le->setText(QString());
    }
    QList<QComboBox *> comboBoxes =  findChildren<QComboBox *>();
    Q_FOREACH (QComboBox *cb, comboBoxes) {
        cb->setCurrentIndex(0);
    }
    QList<QCheckBox *> checkBoxes =  findChildren<QCheckBox *>();
    Q_FOREACH (QCheckBox *cb, checkBoxes) {
        cb->setChecked(false);
    }
    QList<QTextEdit *> textEdits = findChildren<QTextEdit *>();
    Q_FOREACH (QTextEdit *te, textEdits) {
        te->setPlainText(QString());
    }
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
void Details::setData(const QMap<QString, QString> &data) const
{
    QString key;

    QList<QLineEdit *> lineEdits =  findChildren<QLineEdit *>();
    Q_FOREACH (QLineEdit *le, lineEdits) {
        key = le->objectName();
        le->setText(data.value(key));
    }
    QList<QComboBox *> comboBoxes =  findChildren<QComboBox *>();
    Q_FOREACH (QComboBox *cb, comboBoxes) {
        key = cb->objectName();
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
        cb->setChecked(data.value(key) == "1" ? true : false);
    }

    QList<QTextEdit *> textEdits = findChildren<QTextEdit *>();
    Q_FOREACH (QTextEdit *te, textEdits) {
        key = te->objectName();
        te->setPlainText(data.value(key));
    }
}

/*
 *
 * Return a Map with the widgets data.
 *
 */
const QMap<QString, QString> Details::getData() const
{
    QMap<QString, QString> currentData;
    QString key;
    QList<QLineEdit *> lineEdits = findChildren<QLineEdit *>();
    Q_FOREACH (QLineEdit *le, lineEdits) {
        key = le->objectName();
        currentData[key] = le->text();
    }

    QList<QComboBox *> comboBoxes = findChildren<QComboBox *>();
    Q_FOREACH (QComboBox *cb, comboBoxes) {
        key = cb->objectName();
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
        currentData[key] = cb->isChecked() ? "1" : "0";
    }

    QList<QTextEdit *> textEdits = findChildren<QTextEdit *>();
    Q_FOREACH (QTextEdit *te, textEdits) {
        key = te->objectName();
        currentData[key] = te->toPlainText();
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
