#ifndef KDCRMFIELDS_H
#define KDCRMFIELDS_H

#include "kdcrmdata_export.h"
#include <QString>

// Name of the elements in the XML serialization, and in the GUI (i.e. in the QMap<QString,QString> data)
// Not necesssarily the same as in the sugar soap, although right now AccessorPair doesn't support
// a different naming between soap and xml (could be useful later though, e.g. for salesforce support).
// So don't use this as the soap name (the key in the AccessorPair)
namespace KDCRMFields
{
    KDCRMDATA_EXPORT QString assignedUserName();
    KDCRMDATA_EXPORT QString dateModified();
    KDCRMDATA_EXPORT QString dateEntered();
    KDCRMDATA_EXPORT QString id();
    KDCRMDATA_EXPORT QString name();
    KDCRMDATA_EXPORT QString modifiedUserId();
    KDCRMDATA_EXPORT QString modifiedByName();
    KDCRMDATA_EXPORT QString createdBy();
    KDCRMDATA_EXPORT QString createdByName();
    KDCRMDATA_EXPORT QString assignedUserId();
    KDCRMDATA_EXPORT QString accountName();
    KDCRMDATA_EXPORT QString campaignName();
    KDCRMDATA_EXPORT QString campaignType();
    KDCRMDATA_EXPORT QString campaignId();
    KDCRMDATA_EXPORT QString parentName();
    KDCRMDATA_EXPORT QString tickerSymbol();
    KDCRMDATA_EXPORT QString campaign();
    KDCRMDATA_EXPORT QString assignedTo();
    KDCRMDATA_EXPORT QString reportsTo();
    KDCRMDATA_EXPORT QString parentId();
    KDCRMDATA_EXPORT QString accountId();
    KDCRMDATA_EXPORT QString assignedToId();
    KDCRMDATA_EXPORT QString reportsToId();
    KDCRMDATA_EXPORT QString opportunityType();
    KDCRMDATA_EXPORT QString description();
    KDCRMDATA_EXPORT QString deleted();
    KDCRMDATA_EXPORT QString content();
    KDCRMDATA_EXPORT QString leadSource();
    KDCRMDATA_EXPORT QString amount();
    KDCRMDATA_EXPORT QString amountUsDollar();
    KDCRMDATA_EXPORT QString contactId();
    KDCRMDATA_EXPORT QString currencyId();
    KDCRMDATA_EXPORT QString currencyName();
    KDCRMDATA_EXPORT QString currencySymbol();
    KDCRMDATA_EXPORT QString dateClosed();
    KDCRMDATA_EXPORT QString nextStep();
    KDCRMDATA_EXPORT QString salesStage();
    KDCRMDATA_EXPORT QString probability();
    KDCRMDATA_EXPORT QString nextCallDate();
    KDCRMDATA_EXPORT QString billingAddressStreet();
    KDCRMDATA_EXPORT QString billingAddressCity();
    KDCRMDATA_EXPORT QString billingAddressState();
    KDCRMDATA_EXPORT QString billingAddressCountry();
    KDCRMDATA_EXPORT QString billingAddressPostalcode();
    KDCRMDATA_EXPORT QString shippingAddressStreet();
    KDCRMDATA_EXPORT QString shippingAddressCity();
    KDCRMDATA_EXPORT QString shippingAddressState();
    KDCRMDATA_EXPORT QString shippingAddressCountry();
    KDCRMDATA_EXPORT QString shippingAddressPostalcode();
}

#endif // KDCRMFIELDS_H
