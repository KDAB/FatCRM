#include "accountdetails.h"

#include "ui_accountdetails.h"
#include "referenceddatamodel.h"

#include <kdcrmdata/sugaraccount.h>

AccountDetails::AccountDetails(QWidget *parent)
    : Details(Account, parent), mUi(new Ui::AccountDetails)

{
    mUi->setupUi(this);
    mUi->urllabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    initialize();
}

AccountDetails::~AccountDetails()
{
    delete mUi;
}

void AccountDetails::initialize()
{
    setObjectName("accountDetails");

    ReferencedDataModel::setModelForCombo(mUi->parentName, AccountRef);
    mUi->industry->addItems(industryItems());
    mUi->accountType->addItems(typeItems());
    ReferencedDataModel::setModelForCombo(mUi->campaignName, CampaignRef);
    ReferencedDataModel::setModelForCombo(mUi->assignedUserName, AssignedToRef);
}

QStringList AccountDetails::typeItems() const
{
    QStringList types;
    types << QString("") << QString("Analyst") << QString("Competitor")
          << QString("Customer") << QString("Integrator")
          << QString("Investor") << QString("Partner")
          << QString("Press") << QString("Prospect")
          << QString("Reseller") << QString("Other");
    return types;
}

QMap<QString, QString> AccountDetails::data(const Akonadi::Item &item) const
{
    SugarAccount account = item.payload<SugarAccount>();
    return account.data();
}

void AccountDetails::updateItem(Akonadi::Item &item, const QMap<QString, QString> &data) const
{
    SugarAccount account;
    if (item.hasPayload<SugarAccount>()) {
        account = item.payload<SugarAccount>();
    }
    account.setData(data);

    item.setMimeType(SugarAccount::mimeType());
    item.setPayload<SugarAccount>(account);
}

void AccountDetails::setDataInternal(const QMap<QString, QString> &) const
{
    const QString baseUrl = resourceBaseUrl();
    if (!baseUrl.isEmpty() && !id().isEmpty()) {
        const QString url = baseUrl + "?action=DetailView&module=Accounts&record=" + id();
        mUi->urllabel->setText(QString("<a href=\"%1\">Open Account in Web Browser</a>").arg(url));
    }
}
