#include "enums.h"
#include <klocalizedstring.h>

QString typeToString(DetailsType type)
{
    if (type == Account) {
        return QString("Accounts");
    } else if (type == Opportunity) {
        return QString("Opportunities");
    } else if (type == Lead) {
        return QString("Leads");
    } else if (type == Contact) {
        return QString("Contacts");
    } else if (type == Campaign) {
        return QString("Campaigns");
    } else {
        return QString();
    }
}

QString typeToTranslatedString(DetailsType type)
{
    if (type == Account) {
        return i18n("accounts");
    } else if (type == Opportunity) {
        return i18n("opportunities");
    } else if (type == Lead) {
        return i18n("leads");
    } else if (type == Contact) {
        return i18n("contacts");
    } else if (type == Campaign) {
        return i18n("campaigns");
    } else {
        return QString();
    }
}

