/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "sugaraccount.h"
#include "kdcrmfields.h"
#include "kdcrmutils.h"

#include <KLocalizedString>

#include <QDebug>
#include <QHash>
#include <QMap>
#include <QSharedData>
#include <QString>

class SugarAccount::Private : public QSharedData
{
public:
    Private()
    {
    }

    Private(const Private &other)
        : QSharedData(other),
          mEmpty(other.mEmpty),
          mId(other.mId),
          mName(other.mName),
          mDateEntered(other.mDateEntered),
          mDateModified(other.mDateModified),
          mModifiedUserId(other.mModifiedUserId),
          mModifiedByName(other.mModifiedByName),
          mCreatedBy(other.mCreatedBy),
          mCreatedByName(other.mCreatedByName),
          mDescription(other.mDescription),
          mDeleted(other.mDeleted),
          mAssignedUserId(other.mAssignedUserId),
          mAssignedUserName(other.mAssignedUserName),
          mAccountType(other.mAccountType),
          mIndustry(other.mIndustry),
          mAnnualRevenue(other.mAnnualRevenue),
          mPhoneFax(other.mPhoneFax),
          mBillingAddressStreet(other.mBillingAddressStreet),
          mBillingAddressCity(other.mBillingAddressCity),
          mBillingAddressState(other.mBillingAddressState),
          mBillingAddressPostalcode(other.mBillingAddressPostalcode),
          mBillingAddressCountry(other.mBillingAddressCountry),
          mRating(other.mRating),
          mPhoneOffice(other.mPhoneOffice),
          mPhoneAlternate(other.mPhoneAlternate),
          mWebsite(other.mWebsite),
          mOwnership(other.mOwnership),
          mEmployees(other.mEmployees),
          mTickerSymbol(other.mTickerSymbol),
          mShippingAddressStreet(other.mShippingAddressStreet),
          mShippingAddressCity(other.mShippingAddressCity),
          mShippingAddressState(other.mShippingAddressState),
          mShippingAddressPostalcode(other.mShippingAddressPostalcode),
          mShippingAddressCountry(other.mShippingAddressCountry),
          mEmail1(other.mEmail1),
          mParentId(other.mParentId),
          mParentName(other.mParentName),
          mSicCode(other.mSicCode),
          mCampaignId(other.mCampaignId),
          mCampaignName(other.mCampaignName),
          mCustomFields(other.mCustomFields)
    {
    }

    bool mEmpty = true;

    QString mId;
    QString mName;
    QString mDateEntered;
    QString mDateModified;
    QString mModifiedUserId;
    QString mModifiedByName;
    QString mCreatedBy;
    QString mCreatedByName;
    QString mDescription;
    QString mDeleted;
    QString mAssignedUserId;
    QString mAssignedUserName;
    QString mAccountType;
    QString mIndustry;
    QString mAnnualRevenue;
    QString mPhoneFax;
    QString mBillingAddressStreet;
    QString mBillingAddressCity;
    QString mBillingAddressState;
    QString mBillingAddressPostalcode;
    QString mBillingAddressCountry;
    QString mRating;
    QString mPhoneOffice;
    QString mPhoneAlternate;
    QString mWebsite;
    QString mOwnership;
    QString mEmployees;
    QString mTickerSymbol;
    QString mShippingAddressStreet;
    QString mShippingAddressCity;
    QString mShippingAddressState;
    QString mShippingAddressPostalcode;
    QString mShippingAddressCountry;
    QString mEmail1;
    QString mParentId;
    QString mParentName;
    QString mSicCode;
    QString mCampaignId;
    QString mCampaignName;
    QMap<QString, QString> mCustomFields;
};

SugarAccount::SugarAccount()
    : d(new Private)
{
}

SugarAccount::SugarAccount(const SugarAccount &other)
    : d(other.d)
{
}

SugarAccount::~SugarAccount()
{
}

SugarAccount &SugarAccount::operator=(const SugarAccount &other)
{
    if (this != &other) {
        d = other.d;
    }

    return *this;
}

// The code below will try adding a dot, and prepending a comma
static const char* s_extensions[] = {
    "Inc", "LLC", "Ltd", "Limited", // USA, UK
    "GmbH and Co. KG", "GmbH + Co. KG", "GmbH", "e.V", // Germany
    "S.A.S", "SAS", "SASU", "S.A", "SA", // France
    "S.A.U", // Spain
    "S.p.A", "Spa", // Italy
    "AB", // Sweden
    "AG", // Switzerland
    "BV", "B.V", "BVBA", // Netherlands
    "N.V", // Belgium
    "AS", // Norway
    "Oy", // Finland
    "Lda", // Portugal
    "(individual)" // Anywhere ;)
};
static const int s_extensionCount = sizeof(s_extensions) / sizeof(*s_extensions);

// The rule is: one account of a given name, in a given city.
// E.g. HP (city: Barcelona) != HP (city: Chicago) != HP (city: London)
bool SugarAccount::isSameAccount(const SugarAccount &other) const
{
    if (!d->mId.isEmpty() && !other.d->mId.isEmpty() && d->mId != other.d->mId) {
        return false;
    }

    if (cleanAccountName() != other.cleanAccountName()) {
        return false;
    }

    if (QString::compare(d->mBillingAddressCountry, other.d->mBillingAddressCountry, Qt::CaseInsensitive) != 0) {
        return false;
    }

    if (QString::compare(d->mBillingAddressCity, other.d->mBillingAddressCity, Qt::CaseInsensitive) != 0) {
        return false;
    }

    return true;
}

QString SugarAccount::key() const
{
    return cleanAccountName() + '_' + d->mBillingAddressCountry + '_' + d->mBillingAddressCity;
}

QString SugarAccount::cleanAccountName() const
{
    QString result = d->mName;
    for (int i = 0; i < s_extensionCount; ++i) {
        const QString extension = s_extensions[i];
        result.remove(", " + extension + '.', Qt::CaseInsensitive);
        result.remove(", " + extension, Qt::CaseInsensitive);
        result.remove(QChar(' ') + extension + '.', Qt::CaseInsensitive);
        result.remove(QChar(' ') + extension, Qt::CaseInsensitive);
        result.replace(QChar('&'), QStringLiteral("and"));
    }
    if (result.endsWith(']')) {
        int pos = result.lastIndexOf('[');
        if (pos > 0) {
            if (result.at(pos - 1) == ' ') {
                --pos;
            }
            result.truncate(pos);
        }
    }
    return result.toLower();
}

QString SugarAccount::countryForGui() const
{
    const QString billingCountry = d->mBillingAddressCountry;
    const QString country = billingCountry.isEmpty() ? d->mShippingAddressCountry : billingCountry;
    return country.trimmed();
}

QString SugarAccount::cityForGui() const
{
    const QString billingCity = d->mBillingAddressCity;
    const QString city = billingCity.isEmpty() ? d->mShippingAddressCity : billingCity;
    return city.trimmed();
}

QString SugarAccount::postalCodeForGui() const
{
    const QString billingPostalcode = d->mBillingAddressPostalcode;
    const QString postalCode = billingPostalcode.isEmpty() ? d->mShippingAddressPostalcode : billingPostalcode;
    return postalCode.trimmed();
}

bool SugarAccount::isEmpty() const
{
    return d->mEmpty;
}

void SugarAccount::clear()
{
    *this = SugarAccount();
}

void SugarAccount::setId(const QString &id)
{
    d->mEmpty = false;
    d->mId = id;
}

QString SugarAccount::id() const
{
    return d->mId;
}

void SugarAccount::setName(const QString &name)
{
    d->mEmpty = false;
    d->mName = name;
}

QString SugarAccount::name() const
{
    return d->mName;
}

void SugarAccount::setDateEntered(const QString &date)
{
    d->mEmpty = false;
    d->mDateEntered = date;
}

QString SugarAccount::dateEntered() const
{
    return d->mDateEntered;
}

void SugarAccount::setDateModifiedRaw(const QString &dateStr)
{
    d->mEmpty = false;
    d->mDateModified = dateStr;
}

void SugarAccount::setDateModified(const QDateTime &date)
{
    d->mEmpty = false;
    d->mDateModified = KDCRMUtils::dateTimeToString(date);
}

QString SugarAccount::dateModifiedRaw() const
{
    return d->mDateModified;
}

QDateTime SugarAccount::dateModified() const
{
    return KDCRMUtils::dateTimeFromString(d->mDateModified);
}

void SugarAccount::setModifiedUserId(const QString &value)
{
    d->mEmpty = false;
    d->mModifiedUserId = value;
}

QString SugarAccount::modifiedUserId() const
{
    return d->mModifiedUserId;
}

void SugarAccount::setModifiedByName(const QString &value)
{
    d->mEmpty = false;
    d->mModifiedByName = value;
}

QString SugarAccount::modifiedByName() const
{
    return d->mModifiedByName;
}

void SugarAccount::setCreatedBy(const QString &value)
{
    d->mEmpty = false;
    d->mCreatedBy = value;
}

QString SugarAccount::createdBy() const
{
    return d->mCreatedBy;
}

void SugarAccount::setCreatedByName(const QString &value)
{
    d->mEmpty = false;
    d->mCreatedByName = value;
}

QString SugarAccount::createdByName() const
{
    return d->mCreatedByName;
}

void SugarAccount::setDescription(const QString &value)
{
    d->mEmpty = false;
    d->mDescription = value;
}

QString SugarAccount::description() const
{
    return d->mDescription;
}

QString SugarAccount::limitedDescription(int wantedParagraphs) const
{
    return KDCRMUtils::limitString(d->mDescription, wantedParagraphs);
}

void SugarAccount::setDeleted(const QString &value)
{
    d->mEmpty = false;
    d->mDeleted = value;
}

QString SugarAccount::deleted() const
{
    return d->mDeleted;
}

void SugarAccount::setAssignedUserId(const QString &value)
{
    d->mEmpty = false;
    d->mAssignedUserId = value;
}

QString SugarAccount::assignedUserId() const
{
    return d->mAssignedUserId;
}

void SugarAccount::setAssignedUserName(const QString &value)
{
    d->mEmpty = false;
    d->mAssignedUserName = value;
}

QString SugarAccount::assignedUserName() const
{
    return d->mAssignedUserName;
}

void SugarAccount::setAccountType(const QString &value)
{
    d->mEmpty = false;
    d->mAccountType = value;
}

QString SugarAccount::accountType() const
{
    return d->mAccountType;
}

void SugarAccount::setIndustry(const QString &value)
{
    d->mEmpty = false;
    d->mIndustry = value;
}

QString SugarAccount::industry() const
{
    return d->mIndustry;
}

void SugarAccount::setAnnualRevenue(const QString &value)
{
    d->mEmpty = false;
    d->mAnnualRevenue = value;
}

QString SugarAccount::annualRevenue() const
{
    return d->mAnnualRevenue;
}

void SugarAccount::setPhoneFax(const QString &value)
{
    d->mEmpty = false;
    d->mPhoneFax = value;
}

QString SugarAccount::phoneFax() const
{
    return d->mPhoneFax;
}

void SugarAccount::setBillingAddressStreet(const QString &value)
{
    d->mEmpty = false;
    d->mBillingAddressStreet = value;
}

QString SugarAccount::billingAddressStreet() const
{
    return d->mBillingAddressStreet;
}

void SugarAccount::setBillingAddressCity(const QString &value)
{
    d->mEmpty = false;
    d->mBillingAddressCity = value;
}

QString SugarAccount::billingAddressCity() const
{
    return d->mBillingAddressCity;
}

void SugarAccount::setBillingAddressState(const QString &value)
{
    d->mEmpty = false;
    d->mBillingAddressState = value;
}

QString SugarAccount::billingAddressState() const
{
    return d->mBillingAddressState;
}

void SugarAccount::setBillingAddressPostalcode(const QString &value)
{
    d->mEmpty = false;
    d->mBillingAddressPostalcode = value;
}

QString SugarAccount::billingAddressPostalcode() const
{
    return d->mBillingAddressPostalcode;
}

void SugarAccount::setBillingAddressCountry(const QString &value)
{
    d->mEmpty = false;
    d->mBillingAddressCountry = value;
}

QString SugarAccount::billingAddressCountry() const
{
    return d->mBillingAddressCountry;
}

void SugarAccount::setRating(const QString &value)
{
    d->mEmpty = false;
    d->mRating = value;
}

QString SugarAccount::rating() const
{
    return d->mRating;
}

void SugarAccount::setPhoneOffice(const QString &value)
{
    d->mEmpty = false;
    d->mPhoneOffice = value;
}

QString SugarAccount::phoneOffice() const
{
    return d->mPhoneOffice;
}

void SugarAccount::setPhoneAlternate(const QString &value)
{
    d->mEmpty = false;
    d->mPhoneAlternate = value;
}

QString SugarAccount::phoneAlternate() const
{
    return d->mPhoneAlternate;
}

void SugarAccount::setWebsite(const QString &value)
{
    d->mEmpty = false;
    d->mWebsite = value;
}

QString SugarAccount::website() const
{
    return d->mWebsite;
}

void SugarAccount::setOwnership(const QString &value)
{
    d->mEmpty = false;
    d->mOwnership = value;
}

QString SugarAccount::ownership() const
{
    return d->mOwnership;
}

void SugarAccount::setEmployees(const QString &value)
{
    d->mEmpty = false;
    d->mEmployees = value;
}

QString SugarAccount::employees() const
{
    return d->mEmployees;
}

void SugarAccount::setTickerSymbol(const QString &value)
{
    d->mEmpty = false;
    d->mTickerSymbol = value;
}

QString SugarAccount::tickerSymbol() const
{
    return d->mTickerSymbol;
}

void SugarAccount::setShippingAddressStreet(const QString &value)
{
    d->mEmpty = false;
    d->mShippingAddressStreet = value;
}

QString SugarAccount::shippingAddressStreet() const
{
    return d->mShippingAddressStreet;
}

void SugarAccount::setShippingAddressCity(const QString &value)
{
    d->mEmpty = false;
    d->mShippingAddressCity = value;
}

QString SugarAccount::shippingAddressCity() const
{
    return d->mShippingAddressCity;
}

void SugarAccount::setShippingAddressState(const QString &value)
{
    d->mEmpty = false;
    d->mShippingAddressState = value;
}

QString SugarAccount::shippingAddressState() const
{
    return d->mShippingAddressState;
}

void SugarAccount::setShippingAddressPostalcode(const QString &value)
{
    d->mEmpty = false;
    d->mShippingAddressPostalcode = value;
}

QString SugarAccount::shippingAddressPostalcode() const
{
    return d->mShippingAddressPostalcode;
}

void SugarAccount::setShippingAddressCountry(const QString &value)
{
    d->mEmpty = false;
    d->mShippingAddressCountry = value;
}

QString SugarAccount::shippingAddressCountry() const
{
    return d->mShippingAddressCountry;
}

void SugarAccount::setEmail1(const QString &value)
{
    d->mEmpty = false;
    d->mEmail1 = value;
}

QString SugarAccount::email1() const
{
    return d->mEmail1;
}

void SugarAccount::setParentId(const QString &value)
{
    d->mEmpty = false;
    d->mParentId = value;
}

QString SugarAccount::parentId() const
{
    return d->mParentId;
}

void SugarAccount::setParentName(const QString &value)
{
    d->mEmpty = false;
    d->mParentName = value;
}

QString SugarAccount::parentName() const
{
    return d->mParentName;
}

void SugarAccount::setSicCode(const QString &value)
{
    d->mEmpty = false;
    d->mSicCode = value;
}

QString SugarAccount::sicCode() const
{
    return d->mSicCode;
}

void SugarAccount::setCampaignId(const QString &value)
{
    d->mEmpty = false;
    d->mCampaignId = value;
}

QString SugarAccount::campaignId() const
{
    return d->mCampaignId;
}

void SugarAccount::setCampaignName(const QString &value)
{
    d->mEmpty = false;
    d->mCampaignName = value;
}

QString SugarAccount::campaignName() const
{
    return d->mCampaignName;
}

void SugarAccount::setCustomField(const QString &name, const QString &value)
{
    d->mEmpty = false;
    d->mCustomFields.insert(name, value);
}

QMap<QString, QString> SugarAccount::customFields() const
{
    return d->mCustomFields;
}

void SugarAccount::setData(const QMap<QString, QString>& data)
{
    d->mEmpty = false;

    const SugarAccount::AccessorHash accessors = SugarAccount::accessorHash();
    for (auto it = data.constBegin(); it != data.constEnd() ; ++it) {
        auto accessIt = accessors.constFind(it.key());
        if (accessIt != accessors.constEnd()) {
            (this->*(accessIt.value().setter))(it.value());
        } else {
            d->mCustomFields.insert(it.key(), it.value());
        }
    }

    // equivalent to this, but fully automated:
    //d->mId = data.value("id");
    //d->mName = data.value("name");
    // ...
}

QMap<QString, QString> SugarAccount::data() const
{
    QMap<QString, QString> data;

    const SugarAccount::AccessorHash accessors = SugarAccount::accessorHash();
    for (auto it = accessors.constBegin(); it != accessors.constEnd(); ++it) {
        const SugarAccount::valueGetter getter = (*it).getter;
        data.insert(it.key(), (this->*getter)());
    }

    // equivalent to this, but fully automated:
    //data.insert("id", d->mId);
    //data.insert("name", d->mName);
    // ...

    // plus custom fields
    for (auto cit = d->mCustomFields.constBegin(); cit != d->mCustomFields.constEnd(); ++cit ) {
        data.insert(cit.key(), cit.value());
    }

    return data;
}

QString SugarAccount::mimeType()
{
    return QStringLiteral("application/x-vnd.kdab.crm.account");
}

Q_GLOBAL_STATIC(SugarAccount::AccessorHash, s_accessors)

SugarAccount::AccessorHash SugarAccount::accessorHash()
{
    AccessorHash &accessors = *s_accessors();
    if (accessors.isEmpty()) {
        accessors.insert(KDCRMFields::id(),
                          AccountAccessorPair(&SugarAccount::id, &SugarAccount::setId, QString()));
        accessors.insert(KDCRMFields::name(),
                          AccountAccessorPair(&SugarAccount::name, &SugarAccount::setName,
                                            i18nc("@item:intable account name", "Name")));
        accessors.insert(KDCRMFields::dateEntered(),
                          AccountAccessorPair(&SugarAccount::dateEntered, &SugarAccount::setDateEntered, QString()));
        accessors.insert(KDCRMFields::dateModified(),
                          AccountAccessorPair(&SugarAccount::dateModifiedRaw, &SugarAccount::setDateModifiedRaw, QString()));
        accessors.insert(KDCRMFields::modifiedUserId(),
                          AccountAccessorPair(&SugarAccount::modifiedUserId, &SugarAccount::setModifiedUserId, QString()));
        accessors.insert(KDCRMFields::modifiedByName(),
                          AccountAccessorPair(&SugarAccount::modifiedByName, &SugarAccount::setModifiedByName, QString()));
        accessors.insert(KDCRMFields::createdBy(),
                          AccountAccessorPair(&SugarAccount::createdBy, &SugarAccount::setCreatedBy, QString()));
        accessors.insert(KDCRMFields::createdByName(),
                          AccountAccessorPair(&SugarAccount::createdByName, &SugarAccount::setCreatedByName, QString()));
        accessors.insert(KDCRMFields::description(),
                          AccountAccessorPair(&SugarAccount::description, &SugarAccount::setDescription,
                                            i18nc("@item:intable", "Description")));
        accessors.insert(KDCRMFields::deleted(),
                          AccountAccessorPair(&SugarAccount::deleted, &SugarAccount::setDeleted, QString()));
        accessors.insert(KDCRMFields::assignedUserId(),
                          AccountAccessorPair(&SugarAccount::assignedUserId, &SugarAccount::setAssignedUserId, QString()));
        accessors.insert(KDCRMFields::assignedUserName(),
                          AccountAccessorPair(&SugarAccount::assignedUserName, &SugarAccount::setAssignedUserName,
                                            i18nc("@item:intable", "Assigned To")));
        accessors.insert(KDCRMFields::accountType(),
                          AccountAccessorPair(&SugarAccount::accountType, &SugarAccount::setAccountType,
                                            i18nc("@item:intable", "Type")));
        accessors.insert(KDCRMFields::industry(),
                          AccountAccessorPair(&SugarAccount::industry, &SugarAccount::setIndustry,
                                            i18nc("@item:intable", "Industry")));
        accessors.insert(KDCRMFields::annualRevenue(),
                          AccountAccessorPair(&SugarAccount::annualRevenue, &SugarAccount::setAnnualRevenue,
                                            i18nc("@item:intable", "Annual Revenue")));
        accessors.insert(KDCRMFields::phoneFax(),
                          AccountAccessorPair(&SugarAccount::phoneFax, &SugarAccount::setPhoneFax,
                                            i18nc("@item:intable", "Fax")));
        accessors.insert(KDCRMFields::billingAddressStreet(),
                          AccountAccessorPair(&SugarAccount::billingAddressStreet, &SugarAccount::setBillingAddressStreet, QString()));
        accessors.insert(KDCRMFields::billingAddressCity(),
                          AccountAccessorPair(&SugarAccount::billingAddressCity, &SugarAccount::setBillingAddressCity, QString()));
        accessors.insert(KDCRMFields::billingAddressState(),
                          AccountAccessorPair(&SugarAccount::billingAddressState, &SugarAccount::setBillingAddressState, QString()));
        accessors.insert(KDCRMFields::billingAddressPostalcode(),
                          AccountAccessorPair(&SugarAccount::billingAddressPostalcode, &SugarAccount::setBillingAddressPostalcode, QString()));
        accessors.insert(KDCRMFields::billingAddressCountry(),
                          AccountAccessorPair(&SugarAccount::billingAddressCountry, &SugarAccount::setBillingAddressCountry, QString()));
        accessors.insert(KDCRMFields::rating(),
                          AccountAccessorPair(&SugarAccount::rating, &SugarAccount::setRating,
                                            i18nc("@item:intable", "Rating")));
        accessors.insert(KDCRMFields::accountPhoneWork(),
                          AccountAccessorPair(&SugarAccount::phoneOffice, &SugarAccount::setPhoneOffice,
                                            i18nc("@item:intable", "Phone (Office)")));
        accessors.insert(KDCRMFields::accountPhoneOther(),
                          AccountAccessorPair(&SugarAccount::phoneAlternate, &SugarAccount::setPhoneAlternate,
                                            i18nc("@item:intable", "Phone (Other)")));
        accessors.insert(KDCRMFields::website(),
                          AccountAccessorPair(&SugarAccount::website, &SugarAccount::setWebsite,
                                            i18nc("@item:intable", "Website")));
        accessors.insert(KDCRMFields::ownership(),
                          AccountAccessorPair(&SugarAccount::ownership, &SugarAccount::setOwnership,
                                            i18nc("@item:intable", "Ownership")));
        accessors.insert(KDCRMFields::employees(),
                          AccountAccessorPair(&SugarAccount::employees, &SugarAccount::setEmployees,
                                            i18nc("@item:intable", "Employees")));
        accessors.insert(KDCRMFields::tickerSymbol(),
                          AccountAccessorPair(&SugarAccount::tickerSymbol, &SugarAccount::setTickerSymbol,
                                            i18nc("@item:intable", "Ticker Symbol")));
        accessors.insert(KDCRMFields::shippingAddressStreet(),
                          AccountAccessorPair(&SugarAccount::shippingAddressStreet, &SugarAccount::setShippingAddressStreet, QString()));
        accessors.insert(KDCRMFields::shippingAddressCity(),
                          AccountAccessorPair(&SugarAccount::shippingAddressCity, &SugarAccount::setShippingAddressCity, QString()));
        accessors.insert(KDCRMFields::shippingAddressState(),
                          AccountAccessorPair(&SugarAccount::shippingAddressState, &SugarAccount::setShippingAddressState, QString()));
        accessors.insert(KDCRMFields::shippingAddressPostalcode(),
                          AccountAccessorPair(&SugarAccount::shippingAddressPostalcode, &SugarAccount::setShippingAddressPostalcode, QString()));
        accessors.insert(KDCRMFields::shippingAddressCountry(),
                          AccountAccessorPair(&SugarAccount::shippingAddressCountry, &SugarAccount::setShippingAddressCountry, QString()));
        accessors.insert(KDCRMFields::email1(),
                          AccountAccessorPair(&SugarAccount::email1, &SugarAccount::setEmail1,
                                            i18nc("@item:intable", "Primary Email")));
        accessors.insert(KDCRMFields::parentId(),
                          AccountAccessorPair(&SugarAccount::parentId, &SugarAccount::setParentId, QString()));
        accessors.insert(KDCRMFields::parentName(),
                          AccountAccessorPair(&SugarAccount::parentName, &SugarAccount::setParentName,
                                            i18nc("@item:intable", "Member Of")));
        accessors.insert(KDCRMFields::sicCode(),
                          AccountAccessorPair(&SugarAccount::sicCode, &SugarAccount::setSicCode,
                                            i18nc("@item:intable", "SIC Code")));
        accessors.insert(KDCRMFields::campaignId(),
                          AccountAccessorPair(&SugarAccount::campaignId, &SugarAccount::setCampaignId, QString()));
        accessors.insert(KDCRMFields::campaignName(),
                          AccountAccessorPair(&SugarAccount::campaignName, &SugarAccount::setCampaignName,
                                            i18nc("@item:intable", "Campaign")));
    }

    return accessors;
}
