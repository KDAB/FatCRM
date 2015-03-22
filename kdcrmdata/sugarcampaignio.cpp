#include "sugarcampaignio.h"
#include "sugarcampaign.h"

#include <QtCore/QIODevice>
#include <QtCore/QXmlStreamWriter>
#include <QtCore/QDebug>

SugarCampaignIO::SugarCampaignIO()
{
}

bool SugarCampaignIO::readSugarCampaign(QIODevice *device, SugarCampaign &campaign)
{
    if (device == 0 || !device->isReadable()) {
        return false;
    }

    campaign = SugarCampaign();
    xml.setDevice(device);
    if (xml.readNextStartElement()) {
        if (xml.name() == "sugarCampaign"
                && xml.attributes().value("version") == "1.0") {
            readCampaign(campaign);
        } else {
            xml.raiseError(QObject::tr("It is not a sugarCampaign version 1.0 data."));
        }

    }
    return !xml.error();
}

QString SugarCampaignIO::errorString() const
{
    return QObject::tr("%1\nLine %2, column %3")
           .arg(xml.errorString())
           .arg(xml.lineNumber())
           .arg(xml.columnNumber());
}

void SugarCampaignIO::readCampaign(SugarCampaign &campaign)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "sugarCampaign");

    while (xml.readNextStartElement()) {
        if (xml.name() == "id") {
            campaign.setId(xml.readElementText());
        } else if (xml.name() == "name") {
            campaign.setName(xml.readElementText());
        } else if (xml.name() == "date_entered") {
            campaign.setDateEntered(xml.readElementText());
        } else if (xml.name() == "date_modified") {
            campaign.setDateModified(xml.readElementText());
        } else if (xml.name() == "modified_user_id") {
            campaign.setModifiedUserId(xml.readElementText());
        } else if (xml.name() == "modified_by_name") {
            campaign.setModifiedByName(xml.readElementText());
        } else if (xml.name() == "created_by") {
            campaign.setCreatedBy(xml.readElementText());
        } else if (xml.name() == "created_by_name") {
            campaign.setCreatedByName(xml.readElementText());
        } else if (xml.name() == "deleted") {
            campaign.setDeleted(xml.readElementText());
        } else if (xml.name() == "assigned_user_id") {
            campaign.setAssignedUserId(xml.readElementText());
        } else if (xml.name() == "assigned_user_name") {
            campaign.setAssignedUserName(xml.readElementText());
        } else if (xml.name() == "tracker_key") {
            campaign.setTrackerKey(xml.readElementText());
        } else if (xml.name() == "tracker_count") {
            campaign.setTrackerCount(xml.readElementText());
        } else if (xml.name() == "refer_url") {
            campaign.setReferUrl(xml.readElementText());
        } else if (xml.name() == "tracker_text") {
            campaign.setTrackerText(xml.readElementText());
        } else if (xml.name() == "start_date") {
            campaign.setStartDate(xml.readElementText());
        } else if (xml.name() == "end_date") {
            campaign.setEndDate(xml.readElementText());
        } else if (xml.name() == "status") {
            campaign.setStatus(xml.readElementText());
        } else if (xml.name() == "impressions") {
            campaign.setImpressions(xml.readElementText());
        } else if (xml.name() == "currency_id") {
            campaign.setCurrencyId(xml.readElementText());
        } else if (xml.name() == "budget") {
            campaign.setBudget(xml.readElementText());
        } else if (xml.name() == "expected_cost") {
            campaign.setExpectedCost(xml.readElementText());
        } else if (xml.name() == "actual_cost") {
            campaign.setActualCost(xml.readElementText());
        } else if (xml.name() == "expected_revenue") {
            campaign.setExpectedRevenue(xml.readElementText());
        } else if (xml.name() == "campaign_type") {
            campaign.setCampaignType(xml.readElementText());
        } else if (xml.name() == "objective") {
            campaign.setObjective(xml.readElementText());
        } else if (xml.name() == "content") {
            campaign.setContent(xml.readElementText());
        } else if (xml.name() == "frequency") {
            campaign.setFrequency(xml.readElementText());
        } else {
            xml.skipCurrentElement();
        }
    }
}

bool SugarCampaignIO::writeSugarCampaign(const SugarCampaign &campaign, QIODevice *device)
{
    if (device == 0 || !device->isWritable()) {
        return false;
    }

    QXmlStreamWriter writer(device);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeDTD("<!DOCTYPE sugarCampaign>");
    writer.writeStartElement("sugarCampaign");
    writer.writeAttribute("version", "1.0");
    writer.writeTextElement(QString("id"), campaign.id());
    writer.writeTextElement(QString("name"), campaign.name());
    writer.writeTextElement(QString("date_entered"), campaign.dateEntered());
    writer.writeTextElement(QString("date_modified"), campaign.dateModified());
    writer.writeTextElement(QString("modified_user_id"), campaign.modifiedUserId());
    writer.writeTextElement(QString("modified_by_name"), campaign.modifiedByName());
    writer.writeTextElement(QString("created_by"), campaign.createdBy());
    writer.writeTextElement(QString("created_by_name"), campaign.createdByName());
    writer.writeTextElement(QString("deleted"), campaign.deleted());
    writer.writeTextElement(QString("assigned_user_id"), campaign.assignedUserId());
    writer.writeTextElement(QString("assigned_user_name"), campaign.assignedUserName());
    writer.writeTextElement(QString("tracker_key"), campaign.trackerKey());
    writer.writeTextElement(QString("tracker_count"), campaign.trackerCount());
    writer.writeTextElement(QString("refer_url"), campaign.referUrl());
    writer.writeTextElement(QString("tracker_text"), campaign.trackerText());
    writer.writeTextElement(QString("start_date"), campaign.startDate());
    writer.writeTextElement(QString("end_date"), campaign.endDate());
    writer.writeTextElement(QString("status"), campaign.status());
    writer.writeTextElement(QString("impressions"), campaign.impressions());
    writer.writeTextElement(QString("currency_id"), campaign.currencyId());
    writer.writeTextElement(QString("budget"), campaign.budget());
    writer.writeTextElement(QString("expected_cost"), campaign.expectedCost());
    writer.writeTextElement(QString("actual_cost"), campaign.actualCost());
    writer.writeTextElement(QString("expected_revenue"), campaign.expectedRevenue());
    writer.writeTextElement(QString("campaign_type"), campaign.campaignType());
    writer.writeTextElement(QString("objective"), campaign.objective());
    writer.writeTextElement(QString("content"), campaign.content());
    writer.writeTextElement(QString("frequency"), campaign.frequency());
    writer.writeEndDocument();

    return true;
}

