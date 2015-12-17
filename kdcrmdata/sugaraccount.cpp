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

#include "sugaraccount.h"

#include <KLocalizedString>

#include <QHash>
#include <QMap>
#include <QSharedData>
#include <QString>

class SugarAccount::Private : public QSharedData
{
public:
    Private()
        : mEmpty(true)
    {

    }

    Private(const Private &other)
        : QSharedData(other)
    {
        mEmpty = other.mEmpty;

        mId = other.mId;
        mName = other.mName;
        mDateEntered = other.mDateEntered;
        mDateModified = other.mDateModified;
        mModifiedUserId = other.mModifiedUserId;
        mModifiedByName = other.mModifiedByName;
        mCreatedBy = other.mCreatedBy;
        mCreatedByName = other.mCreatedByName;
        mDescription = other.mDescription;
        mDeleted = other.mDeleted;
        mAssignedUserId = other.mAssignedUserId;
        mAssignedUserName = other.mAssignedUserName;
        mAccountType = other.mAccountType;
        mIndustry = other.mIndustry;
        mAnnualRevenue = other.mAnnualRevenue;
        mPhoneFax = other.mPhoneFax;
        mBillingAddressStreet = other.mBillingAddressStreet;
        mBillingAddressCity = other.mBillingAddressCity;
        mBillingAddressState = other.mBillingAddressState;
        mBillingAddressPostalcode = other.mBillingAddressPostalcode;
        mBillingAddressCountry = other.mBillingAddressCountry;
        mRating = other.mRating;
        mPhoneOffice = other.mPhoneOffice;
        mPhoneAlternate = other.mPhoneAlternate;
        mWebsite = other.mWebsite;
        mOwnership = other.mOwnership;
        mEmployees = other.mEmployees;
        mTickerSymbol = other.mTickerSymbol;
        mShippingAddressStreet = other.mShippingAddressStreet;
        mShippingAddressCity = other.mShippingAddressCity;
        mShippingAddressState = other.mShippingAddressState;
        mShippingAddressPostalcode = other.mShippingAddressPostalcode;
        mShippingAddressCountry = other.mShippingAddressCountry;
        mEmail1 = other.mEmail1;
        mParentId = other.mParentId;
        mParentName = other.mParentName;
        mSicCode = other.mSicCode;
        mCampaignId = other.mCampaignId;
        mCampaignName = other.mCampaignName;
        mCustomFields = other.mCustomFields;
    }

    bool mEmpty;

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
    "GmbH", "GmbH & Co. KG", // Germany
    "S.A.S", "SAS", "S.A", "SA", // France
    "S.p.A", // Italy
    "AB", // Sweden
    "AG", // Switzerland
    "BV", "B.V", // Netherlands
    "AS" // Norway
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

    if (d->mBillingAddressCountry != other.d->mBillingAddressCountry) {
        return false;
    }

    if (d->mBillingAddressCity != other.d->mBillingAddressCity) {
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
        result.remove(", " + extension + '.');
        result.remove(", " + extension);
        result.remove(QChar(' ') + extension + '.');
        result.remove(QChar(' ') + extension);
    }
    return result.toLower();
}

#if 0
bool SugarAccount::operator==(const SugarAccount &other) const
{
    if (d->mId != other.d->mId) {
        return false;
    }
    if (d->mName !=  other.d->mName) {
        return false;
    }
    if (d->mDateEntered != other.d->mDateEntered) {
        return false;
    }
    if (d->mDateModified != other.d->mDateModified) {
        return false;
    }
    if (d->mModifiedUserId != other.d->mModifiedUserId) {
        return false;
    }
    if (d->mModifiedByName != other.d->mModifiedByName) {
        return false;
    }
    if (d->mCreatedBy != other.d->mCreatedBy) {
        return false;
    }
    if (d->mCreatedByName != other.d->mCreatedByName) {
        return false;
    }
    if (d->mDescription != other.d->mDescription) {
        return false;
    }
    if (d->mDeleted != other.d->mDeleted) {
        return false;
    }
    if (d->mAssignedUserId != other.d->mAssignedUserId) {
        return false;
    }
    if (d->mAssignedUserName != other.d->mAssignedUserName) {
        return false;
    }
    if (d->mAccountType != other.d->mAccountType) {
        return false;
    }
    if (d->mIndustry != other.d->mIndustry) {
        return false;
    }
    if (d->mAnnualRevenue != other.d->mAnnualRevenue) {
        return false;
    }
    if (d->mPhoneFax != other.d->mPhoneFax) {
        return false;
    }
    if (d->mBillingAddressStreet != other.d->mBillingAddressStreet) {
        return false;
    }
    if (d->mBillingAddressCity != other.d->mBillingAddressCity) {
        return false;
    }
    if (d->mBillingAddressState != other.d->mBillingAddressState) {
        return false;
    }
    if (d->mBillingAddressPostalcode != other.d->mBillingAddressPostalcode) {
        return false;
    }
    if (d->mBillingAddressCountry != other.d->mBillingAddressCountry) {
        return false;
    }
    if (d->mRating != other.d->mRating) {
        return false;
    }
    if (d->mPhoneOffice != other.d->mPhoneOffice) {
        return false;
    }
    if (d->mPhoneAlternate != other.d->mPhoneAlternate) {
        return false;
    }
    if (d->mWebsite != other.d->mWebsite) {
        return false;
    }
    if (d->mOwnership != other.d->mOwnership) {
        return false;
    }
    if (d->mEmployees != other.d->mEmployees) {
        return false;
    }
    if (d->mTickerSymbol != other.d->mTickerSymbol) {
        return false;
    }
    if (d->mShippingAddressStreet != other.d->mShippingAddressStreet) {
        return false;
    }
    if (d->mShippingAddressCity != other.d->mShippingAddressCity) {
        return false;
    }
    if (d->mShippingAddressState != other.d->mShippingAddressState) {
        return false;
    }
    if (d->mShippingAddressPostalcode != other.d->mShippingAddressPostalcode) {
        return false;
    }
    if (d->mShippingAddressCountry != other.d->mShippingAddressCountry) {
        return false;
    }
    if (d->mEmail1 != other.d->mEmail1) {
        return false;
    }
    if (d->mParentId != other.d->mParentId) {
        return false;
    }
    if (d->mParentName != other.d->mParentName) {
        return false;
    }
    if (d->mSicCode != other.d->mSicCode) {
        return false;
    }
    if (d->mCampaignId != other.d->mCampaignId) {
        return false;
    }
    if (d->mCampaignName != other.d->mCampaignName) {
        return false;
    }

    return true;
}

bool SugarAccount::operator!=(const SugarAccount &a) const
{
    return !(a == *this);
}
#endif

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

void SugarAccount::setDateEntered(const QString &value)
{
    d->mEmpty = false;
    d->mDateEntered = value;
}

QString SugarAccount::dateEntered() const
{
    return d->mDateEntered;
}

void SugarAccount::setDateModified(const QString &value)
{
    d->mEmpty = false;
    d->mDateModified = value;
}

QString SugarAccount::dateModified() const
{
    return d->mDateModified;
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
    QMap<QString, QString>::const_iterator it = data.constBegin();
    for ( ; it != data.constEnd() ; ++it) {
        const SugarAccount::AccessorHash::const_iterator accessIt = accessors.constFind(it.key());
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
    SugarAccount::AccessorHash::const_iterator it    = accessors.constBegin();
    SugarAccount::AccessorHash::const_iterator endIt = accessors.constEnd();
    for (; it != endIt; ++it) {
        const SugarAccount::valueGetter getter = (*it).getter;
        data.insert(it.key(), (this->*getter)());
    }

    // equivalent to this, but fully automated:
    //data.insert("id", d->mId);
    //data.insert("name", d->mName);
    // ...

    // plus custom fields
    QMap<QString, QString>::const_iterator cit = d->mCustomFields.constBegin();
    const QMap<QString, QString>::const_iterator end = d->mCustomFields.constEnd();
    for ( ; cit != end ; ++cit ) {
        data.insert(cit.key(), cit.value());
    }

    return data;
}

QString SugarAccount::mimeType()
{
    return QLatin1String("application/x-vnd.kdab.crm.account");
}

Q_GLOBAL_STATIC(SugarAccount::AccessorHash, s_accessors)

SugarAccount::AccessorHash SugarAccount::accessorHash()
{
    AccessorHash &accessors = *s_accessors();
    if (accessors.isEmpty()) {
        accessors.insert(QLatin1String("id"),
                          AccountAccessorPair(&SugarAccount::id, &SugarAccount::setId, QString()));
        accessors.insert(QLatin1String("name"),
                          AccountAccessorPair(&SugarAccount::name, &SugarAccount::setName,
                                            i18nc("@item:intable account name", "Name")));
        accessors.insert(QLatin1String("date_entered"),
                          AccountAccessorPair(&SugarAccount::dateEntered, &SugarAccount::setDateEntered, QString()));
        accessors.insert(QLatin1String("date_modified"),
                          AccountAccessorPair(&SugarAccount::dateModified, &SugarAccount::setDateModified, QString()));
        accessors.insert(QLatin1String("modified_user_id"),
                          AccountAccessorPair(&SugarAccount::modifiedUserId, &SugarAccount::setModifiedUserId, QString()));
        accessors.insert(QLatin1String("modified_by_name"),
                          AccountAccessorPair(&SugarAccount::modifiedByName, &SugarAccount::setModifiedByName, QString()));
        accessors.insert(QLatin1String("created_by"),
                          AccountAccessorPair(&SugarAccount::createdBy, &SugarAccount::setCreatedBy, QString()));
        accessors.insert(QLatin1String("created_by_name"),
                          AccountAccessorPair(&SugarAccount::createdByName, &SugarAccount::setCreatedByName, QString()));
        accessors.insert(QLatin1String("description"),
                          AccountAccessorPair(&SugarAccount::description, &SugarAccount::setDescription,
                                            i18nc("@item:intable", "Description")));
        accessors.insert(QLatin1String("deleted"),
                          AccountAccessorPair(&SugarAccount::deleted, &SugarAccount::setDeleted, QString()));
        accessors.insert(QLatin1String("assigned_user_id"),
                          AccountAccessorPair(&SugarAccount::assignedUserId, &SugarAccount::setAssignedUserId, QString()));
        accessors.insert(QLatin1String("assigned_user_name"),
                          AccountAccessorPair(&SugarAccount::assignedUserName, &SugarAccount::setAssignedUserName,
                                            i18nc("@item:intable", "Assigned To")));
        accessors.insert(QLatin1String("account_type"),
                          AccountAccessorPair(&SugarAccount::accountType, &SugarAccount::setAccountType,
                                            i18nc("@item:intable", "Type")));
        accessors.insert(QLatin1String("industry"),
                          AccountAccessorPair(&SugarAccount::industry, &SugarAccount::setIndustry,
                                            i18nc("@item:intable", "Industry")));
        accessors.insert(QLatin1String("annual_revenue"),
                          AccountAccessorPair(&SugarAccount::annualRevenue, &SugarAccount::setAnnualRevenue,
                                            i18nc("@item:intable", "Annual Revenue")));
        accessors.insert(QLatin1String("phone_fax"),
                          AccountAccessorPair(&SugarAccount::phoneFax, &SugarAccount::setPhoneFax,
                                            i18nc("@item:intable", "Fax")));
        accessors.insert(QLatin1String("billing_address_street"),
                          AccountAccessorPair(&SugarAccount::billingAddressStreet, &SugarAccount::setBillingAddressStreet, QString()));
        accessors.insert(QLatin1String("billing_address_city"),
                          AccountAccessorPair(&SugarAccount::billingAddressCity, &SugarAccount::setBillingAddressCity, QString()));
        accessors.insert(QLatin1String("billing_address_state"),
                          AccountAccessorPair(&SugarAccount::billingAddressState, &SugarAccount::setBillingAddressState, QString()));
        accessors.insert(QLatin1String("billing_address_postalcode"),
                          AccountAccessorPair(&SugarAccount::billingAddressPostalcode, &SugarAccount::setBillingAddressPostalcode, QString()));
        accessors.insert(QLatin1String("billing_address_country"),
                          AccountAccessorPair(&SugarAccount::billingAddressCountry, &SugarAccount::setBillingAddressCountry, QString()));
        accessors.insert(QLatin1String("rating"),
                          AccountAccessorPair(&SugarAccount::rating, &SugarAccount::setRating,
                                            i18nc("@item:intable", "Rating")));
        accessors.insert(QLatin1String("phone_office"),
                          AccountAccessorPair(&SugarAccount::phoneOffice, &SugarAccount::setPhoneOffice,
                                            i18nc("@item:intable", "Phone (Office)")));
        accessors.insert(QLatin1String("phone_alternate"),
                          AccountAccessorPair(&SugarAccount::phoneAlternate, &SugarAccount::setPhoneAlternate,
                                            i18nc("@item:intable", "Phone (Other)")));
        accessors.insert(QLatin1String("website"),
                          AccountAccessorPair(&SugarAccount::website, &SugarAccount::setWebsite,
                                            i18nc("@item:intable", "Website")));
        accessors.insert(QLatin1String("ownership"),
                          AccountAccessorPair(&SugarAccount::ownership, &SugarAccount::setOwnership,
                                            i18nc("@item:intable", "Ownership")));
        accessors.insert(QLatin1String("employees"),
                          AccountAccessorPair(&SugarAccount::employees, &SugarAccount::setEmployees,
                                            i18nc("@item:intable", "Employees")));
        accessors.insert(QLatin1String("ticker_symbol"),
                          AccountAccessorPair(&SugarAccount::tickerSymbol, &SugarAccount::setTickerSymbol,
                                            i18nc("@item:intable", "Ticker Symbol")));
        accessors.insert(QLatin1String("shipping_address_street"),
                          AccountAccessorPair(&SugarAccount::shippingAddressStreet, &SugarAccount::setShippingAddressStreet, QString()));
        accessors.insert(QLatin1String("shipping_address_city"),
                          AccountAccessorPair(&SugarAccount::shippingAddressCity, &SugarAccount::setShippingAddressCity, QString()));
        accessors.insert(QLatin1String("shipping_address_state"),
                          AccountAccessorPair(&SugarAccount::shippingAddressState, &SugarAccount::setShippingAddressState, QString()));
        accessors.insert(QLatin1String("shipping_address_postalcode"),
                          AccountAccessorPair(&SugarAccount::shippingAddressPostalcode, &SugarAccount::setShippingAddressPostalcode, QString()));
        accessors.insert(QLatin1String("shipping_address_country"),
                          AccountAccessorPair(&SugarAccount::shippingAddressCountry, &SugarAccount::setShippingAddressCountry, QString()));
        accessors.insert(QLatin1String("email1"),
                          AccountAccessorPair(&SugarAccount::email1, &SugarAccount::setEmail1,
                                            i18nc("@item:intable", "Primary Email")));
        accessors.insert(QLatin1String("parent_id"),
                          AccountAccessorPair(&SugarAccount::parentId, &SugarAccount::setParentId, QString()));
        accessors.insert(QLatin1String("parent_name"),
                          AccountAccessorPair(&SugarAccount::parentName, &SugarAccount::setParentName,
                                            i18nc("@item:intable", "Member Of")));
        accessors.insert(QLatin1String("sic_code"),
                          AccountAccessorPair(&SugarAccount::sicCode, &SugarAccount::setSicCode,
                                            i18nc("@item:intable", "SIC Code")));
        accessors.insert(QLatin1String("campaign_id"),
                          AccountAccessorPair(&SugarAccount::campaignId, &SugarAccount::setCampaignId, QString()));
        accessors.insert(QLatin1String("campaign_name"),
                          AccountAccessorPair(&SugarAccount::campaignName, &SugarAccount::setCampaignName,
                                            i18nc("@item:intable", "Campaign")));
    }
    return accessors;
}
