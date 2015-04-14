#include "campaigndetails.h"

#include "editcalendarbutton.h"
#include "ui_campaigndetails.h"
#include "referenceddatamodel.h"
#include "kdcrmutils.h"

#include <kdcrmdata/sugarcampaign.h>

CampaignDetails::CampaignDetails(QWidget *parent)
    : Details(Campaign, parent), mUi(new Ui::CampaignDetails)

{
    mUi->setupUi(this);
    initialize();
}

CampaignDetails::~CampaignDetails()
{
    delete mUi;
}

void CampaignDetails::initialize()
{
    mUi->status->addItems(statusItems());
    mUi->campaignType->addItems(typeItems());
    connect(mUi->clearStartDateButton, SIGNAL(clicked()), this, SLOT(slotClearStartDate()));
    connect(mUi->startDateCalendarButton->calendarWidget(), SIGNAL(clicked(QDate)),
            this, SLOT(slotSetStartDate()));
    connect(mUi->clearEndDateButton, SIGNAL(clicked()), this, SLOT(slotClearEndDate()));
    connect(mUi->endDateCalendarButton->calendarWidget(), SIGNAL(clicked(QDate)),
            this, SLOT(slotSetEndDate()));

    mUi->assignedUserName->setModel(new ReferencedDataModel(AssignedToRef, this));
}

void CampaignDetails::slotSetStartDate()
{
    // TODO FIXME: use KDateTimeEdit
    mUi->startDate->setText(KDCRMUtils::dateToString(mUi->startDateCalendarButton->calendarWidget()->selectedDate()));
    mUi->startDateCalendarButton->calendarWidget()->setSelectedDate(QDate::currentDate());
    mUi->startDateCalendarButton->calendarDialog()->close();
}

void CampaignDetails::slotSetEndDate()
{
    mUi->endDate->setText(KDCRMUtils::dateToString(mUi->endDateCalendarButton->calendarWidget()->selectedDate()));
    mUi->endDateCalendarButton->calendarWidget()->setSelectedDate(QDate::currentDate());
    mUi->endDateCalendarButton->calendarDialog()->close();
}

void CampaignDetails::slotClearStartDate()
{
    mUi->startDate->clear();
}

void CampaignDetails::slotClearEndDate()
{
    mUi->endDate->clear();
}

QStringList CampaignDetails::statusItems() const
{
    QStringList status;
    status << QString("") << QString("Planning")
           << QString("Active") << QString("Inactive")
           << QString("Complete") << QString("In Queue")
           << QString("Sending");
    return status;
}

QStringList CampaignDetails::typeItems() const
{
    QStringList types;
    types << QString("") << QString("Telesales")
          << QString("Mail") << QString("Email")
          << QString("Print") << QString("Web")
          << QString("Radio") << QString("Television")
          << QString("Newsletter");
    return types;
}

QMap<QString, QString> CampaignDetails::data(const Akonadi::Item &item) const
{
    SugarCampaign campaign = item.payload<SugarCampaign>();
    return campaign.data();
}

void CampaignDetails::updateItem(Akonadi::Item &item, const QMap<QString, QString> &data) const
{
    SugarCampaign campaign;
    if (item.hasPayload<SugarCampaign>()) {
        campaign = item.payload<SugarCampaign>();
    }
    campaign.setData(data);

    item.setMimeType(SugarCampaign::mimeType());
    item.setPayload<SugarCampaign>(campaign);
}
