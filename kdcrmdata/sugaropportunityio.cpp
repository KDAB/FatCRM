#include "sugaropportunityio.h"
#include "sugaropportunity.h"

#include <QtCore/QIODevice>
#include <QtCore/QXmlStreamWriter>
#include <QtCore/QDebug>

SugarOpportunityIO::SugarOpportunityIO()
{
}

bool SugarOpportunityIO::readSugarOpportunity(QIODevice *device, SugarOpportunity &opportunity)
{
    if (device == 0 || !device->isReadable()) {
        return false;
    }

    opportunity = SugarOpportunity();
    xml.setDevice(device);
    if (xml.readNextStartElement()) {
        if (xml.name() == "sugarOpportunity"
                && xml.attributes().value("version") == "1.0") {
            readOpportunity(opportunity);
        } else {
            xml.raiseError(QObject::tr("It is not a sugarOpportunity version 1.0 data."));
        }

    }
    return !xml.error();
}

QString SugarOpportunityIO::errorString() const
{
    return QObject::tr("%1\nLine %2, column %3")
           .arg(xml.errorString())
           .arg(xml.lineNumber())
           .arg(xml.columnNumber());
}

void SugarOpportunityIO::readOpportunity(SugarOpportunity &opportunity)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "sugarOpportunity");

    while (xml.readNextStartElement()) {
        if (xml.name() == "id") {
            opportunity.setId(xml.readElementText());
        } else if (xml.name() == "name") {
            opportunity.setName(xml.readElementText());
        } else if (xml.name() == "date_entered") {
            opportunity.setDateEntered(xml.readElementText());
        } else if (xml.name() == "date_modified") {
            opportunity.setDateModified(xml.readElementText());
        } else if (xml.name() == "modified_user_id") {
            opportunity.setModifiedUserId(xml.readElementText());
        } else if (xml.name() == "modified_by_name") {
            opportunity.setModifiedByName(xml.readElementText());
        } else if (xml.name() == "created_by") {
            opportunity.setCreatedBy(xml.readElementText());
        } else if (xml.name() == "created_by_name") {
            opportunity.setCreatedByName(xml.readElementText());
        } else if (xml.name() == "description") {
            opportunity.setDescription(xml.readElementText());
        } else if (xml.name() == "deleted") {
            opportunity.setDeleted(xml.readElementText());
        } else if (xml.name() == "assigned_user_id") {
            opportunity.setAssignedUserId(xml.readElementText());
        } else if (xml.name() == "assigned_user_name") {
            opportunity.setAssignedUserName(xml.readElementText());
        } else if (xml.name() == "opportunity_type") {
            opportunity.setOpportunityType(xml.readElementText());
        } else if (xml.name() == "account_name") {
            opportunity.setAccountName(xml.readElementText());
        } else if (xml.name() == "account_id") {
            opportunity.setAccountId(xml.readElementText());
        } else if (xml.name() == "campaign_id") {
            opportunity.setCampaignId(xml.readElementText());
        } else if (xml.name() == "campaign_name") {
            opportunity.setCampaignName(xml.readElementText());
        } else if (xml.name() == "lead_source") {
            opportunity.setLeadSource(xml.readElementText());
        } else if (xml.name() == "amount") {
            opportunity.setAmount(xml.readElementText());
        } else if (xml.name() == "amount_usdollar") {
            opportunity.setAmountUsDollar(xml.readElementText());
        } else if (xml.name() == "currency_id") {
            opportunity.setCurrencyId(xml.readElementText());
        } else if (xml.name() == "currency_name") {
            opportunity.setCurrencyName(xml.readElementText());
        } else if (xml.name() == "currency_symbol") {
            opportunity.setCurrencySymbol(xml.readElementText());
        } else if (xml.name() == "date_closed") {
            opportunity.setDateClosed(xml.readElementText());
        } else if (xml.name() == "next_step") {
            opportunity.setNextStep(xml.readElementText());
        } else if (xml.name() == "sales_stage") {
            opportunity.setSalesStage(xml.readElementText());
        } else if (xml.name() == "probability") {
            opportunity.setProbability(xml.readElementText());
        } else if (xml.name() == "nextCallDate") {
            opportunity.setNextCallDate(xml.readElementText());
        } else {
            qDebug() << "Unexpected XML field" << xml.name();
            xml.skipCurrentElement();
        }
    }
}

bool SugarOpportunityIO::writeSugarOpportunity(const SugarOpportunity &opportunity, QIODevice *device)
{
    if (device == 0 || !device->isWritable()) {
        return false;
    }

    QXmlStreamWriter writer(device);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeDTD("<!DOCTYPE sugarOpportunity>");
    writer.writeStartElement("sugarOpportunity");
    writer.writeAttribute("version", "1.0");
    writer.writeTextElement(QString("id"), opportunity.id());
    writer.writeTextElement(QString("name"), opportunity.name());
    writer.writeTextElement(QString("date_entered"), opportunity.dateEntered());
    writer.writeTextElement(QString("date_modified"), opportunity.dateModified());
    writer.writeTextElement(QString("modified_user_id"), opportunity.modifiedUserId());
    writer.writeTextElement(QString("modified_by_name"), opportunity.modifiedByName());
    writer.writeTextElement(QString("created_by"), opportunity.createdBy());
    writer.writeTextElement(QString("created_by_name"), opportunity.createdByName());
    writer.writeTextElement(QString("description"), opportunity.description());
    writer.writeTextElement(QString("deleted"), opportunity.deleted());
    writer.writeTextElement(QString("assigned_user_id"), opportunity.assignedUserId());
    writer.writeTextElement(QString("assigned_user_name"), opportunity.assignedUserName());
    writer.writeTextElement(QString("opportunity_type"), opportunity.opportunityType());
    writer.writeTextElement(QString("account_name"), opportunity.accountName());
    writer.writeTextElement(QString("account_id"), opportunity.accountId());
    writer.writeTextElement(QString("campaign_id"), opportunity.campaignId());
    writer.writeTextElement(QString("campaign_name"), opportunity.campaignName());
    writer.writeTextElement(QString("lead_source"), opportunity.leadSource());
    writer.writeTextElement(QString("amount"), opportunity.amount());
    writer.writeTextElement(QString("amount_usdollar"), opportunity.amountUsDollar());
    writer.writeTextElement(QString("currency_id"), opportunity.currencyId());
    writer.writeTextElement(QString("currency_name"), opportunity.currencyName());
    writer.writeTextElement(QString("currency_symbol"), opportunity.currencySymbol());
    writer.writeTextElement(QString("date_closed"), opportunity.dateClosed());
    writer.writeTextElement(QString("next_step"), opportunity.nextStep());
    writer.writeTextElement(QString("sales_stage"), opportunity.salesStage());
    writer.writeTextElement(QString("probability"), opportunity.probability());
    writer.writeTextElement(QString("nextCallDate"), opportunity.nextCallDate());
    writer.writeEndDocument();

    return true;
}

