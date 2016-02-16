/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "sugaremail.h"
#include "kdcrmfields.h"

#include <QSharedData>
#include <QString>
#include <QMap>

#include <KLocalizedString>

class SugarEmail::Private : public QSharedData
{
public:
    Private()
        : mEmpty(true)
    {
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
    QString mDeleted;
    QString mAssignedUserId;
    QString mAssignedUserName;

    QString mDateSent;
    QString mMessageId;
    QString mParentType;
    QString mParentId;
    QString mFromAddrName;
    QString mToAddrNAmes;
    QString mCcAddrNames;

    QString mDescription;
};

SugarEmail::SugarEmail()
    : d(new Private)
{
}

SugarEmail::SugarEmail(const SugarEmail &other)
    : d(other.d)
{
}

SugarEmail::~SugarEmail()
{
}

SugarEmail &SugarEmail::operator=(const SugarEmail &other)
{
    if (this != &other) {
        d = other.d;
    }

    return *this;
}

bool SugarEmail::isEmpty() const
{
    return d->mEmpty;
}

void SugarEmail::clear()
{
    *this = SugarEmail();
}

void SugarEmail::setId(const QString &id)
{
    d->mEmpty = false;
    d->mId = id;
}

QString SugarEmail::id() const
{
    return d->mId;
}

void SugarEmail::setName(const QString &name)
{
    d->mEmpty = false;
    d->mName = name;
}

QString SugarEmail::name() const
{
    return d->mName;
}

void SugarEmail::setDateEntered(const QString &value)
{
    d->mEmpty = false;
    d->mDateEntered = value;
}

QString SugarEmail::dateEntered() const
{
    return d->mDateEntered;
}

void SugarEmail::setDateModified(const QString &value)
{
    d->mEmpty = false;
    d->mDateModified = value;
}

QString SugarEmail::dateModified() const
{
    return d->mDateModified;
}

void SugarEmail::setModifiedUserId(const QString &value)
{
    d->mEmpty = false;
    d->mModifiedUserId = value;
}

QString SugarEmail::modifiedUserId() const
{
    return d->mModifiedUserId;
}

void SugarEmail::setModifiedByName(const QString &value)
{
    d->mEmpty = false;
    d->mModifiedByName = value;
}

QString SugarEmail::modifiedByName() const
{
    return d->mModifiedByName;
}

void SugarEmail::setCreatedBy(const QString &value)
{
    d->mEmpty = false;
    d->mCreatedBy = value;
}

QString SugarEmail::createdBy() const
{
    return d->mCreatedBy;
}

void SugarEmail::setCreatedByName(const QString &value)
{
    d->mEmpty = false;
    d->mCreatedByName = value;
}

QString SugarEmail::createdByName() const
{
    return d->mCreatedByName;
}

void SugarEmail::setDeleted(const QString &value)
{
    d->mEmpty = false;
    d->mDeleted = value;
}

QString SugarEmail::deleted() const
{
    return d->mDeleted;
}

void SugarEmail::setAssignedUserId(const QString &value)
{
    d->mEmpty = false;
    d->mAssignedUserId = value;
}

QString SugarEmail::assignedUserId() const
{
    return d->mAssignedUserId;
}

void SugarEmail::setAssignedUserName(const QString &value)
{
    d->mEmpty = false;
    d->mAssignedUserName = value;
}

QString SugarEmail::assignedUserName() const
{
    return d->mAssignedUserName;
}

void SugarEmail::setDateSent(const QString &value)
{
    d->mEmpty = false;
    d->mDateSent = value;
}

QString SugarEmail::dateSent() const
{
    return d->mDateSent;
}

void SugarEmail::setMessageId(const QString &value)
{
    d->mEmpty = false;
    d->mMessageId = value;
}

QString SugarEmail::messageId() const
{
    return d->mMessageId;
}

void SugarEmail::setParentType(const QString &value)
{
    d->mEmpty = false;
    d->mParentType = value;
}

QString SugarEmail::parentType() const
{
    return d->mParentType;
}

void SugarEmail::setParentId(const QString &value)
{
    d->mEmpty = false;
    d->mParentId = value;
}

QString SugarEmail::parentId() const
{
    return d->mParentId;
}

void SugarEmail::setFromAddrName(const QString &value)
{
    d->mEmpty = false;
    d->mFromAddrName = value;
}

QString SugarEmail::fromAddrName() const
{
    return d->mFromAddrName;
}

void SugarEmail::setToAddrNames(const QString &value)
{
    d->mEmpty = false;
    d->mToAddrNAmes = value;
}

QString SugarEmail::toAddrNames() const
{
    return d->mToAddrNAmes;
}

void SugarEmail::setCcAddrNames(const QString &value)
{
    d->mEmpty = false;
    d->mCcAddrNames = value;
}

QString SugarEmail::ccAddrNames() const
{
    return d->mCcAddrNames;
}

void SugarEmail::setDescription(const QString &value)
{
    d->mEmpty = false;
    d->mDescription = value;
}

QString SugarEmail::description() const
{
    return d->mDescription;
}

void SugarEmail::setData(const QMap<QString, QString>& data)
{
    d->mEmpty = false;

    const SugarEmail::AccessorHash accessors = SugarEmail::accessorHash();
    QMap<QString, QString>::const_iterator it = data.constBegin();
    for ( ; it != data.constEnd() ; ++it) {
        const SugarEmail::AccessorHash::const_iterator accessIt = accessors.constFind(it.key());
        if (accessIt != accessors.constEnd()) {
            (this->*(accessIt.value().setter))(it.value());
        }
    }

    // equivalent to this, but fully automated:
    //d->mId = data.value("id");
    //d->mName = data.value("name");
    // ...
}

QMap<QString, QString> SugarEmail::data() const
{
    QMap<QString, QString> data;

    const SugarEmail::AccessorHash accessors = SugarEmail::accessorHash();
    SugarEmail::AccessorHash::const_iterator it    = accessors.constBegin();
    SugarEmail::AccessorHash::const_iterator endIt = accessors.constEnd();
    for (; it != endIt; ++it) {
        const SugarEmail::valueGetter getter = (*it).getter;
        data.insert(it.key(), (this->*getter)());
    }

    // equivalent to this, but fully automated:
    //data.insert("id", d->mId);
    //data.insert("name", d->mName);
    // ...

    return data;
}

QString SugarEmail::mimeType()
{
    return QStringLiteral("application/x-vnd.kdab.crm.email");
}

Q_GLOBAL_STATIC(SugarEmail::AccessorHash, s_accessors)

SugarEmail::AccessorHash SugarEmail::accessorHash()
{
    AccessorHash &accessors = *s_accessors();
    if (accessors.isEmpty()) {

        /*
id
name
date_entered
date_modified
modified_user_id
modified_by_name
created_by
created_by_name
deleted
assigned_user_id
assigned_user_name is missing !?!?

date_sent
message_id
parent_type
parent_id
from_addr_name
to_addrs_names
cc_addrs_names

Skipped fields:
bcc_addrs_names
reply_to_addr
type
status
flagged
reply_to_status
intent
mailbox_id

// The first example:

"modified_by_name" = "Valérie Blanc"
"created_by_name" = "Valérie Blanc"
"id" = "e37f08d6-fa9f-3b3f-73b4-551a5125fc22"
"date_entered" = "2015-03-31 07:50:49"
"date_modified" = "2015-03-31 12:58:09"
"assigned_user_id" = "e88a49f9-4ddd-2c3a-77ad-49dc7b5e72f9"
"modified_user_id" = "c34a548f-08cc-b8be-c43b-4f5f06f81621"
"created_by" = "c34a548f-08cc-b8be-c43b-4f5f06f81621"
KDCRMFields::deleted() = "0"
"from_addr_name" = "mariececile.roue@orange.com"
"reply_to_addr" = "mariececile.roue@orange.com"
"to_addrs_names" = "David Faure <info-fr@kdab.com>"
"cc_addrs_names" = "GARDES Frederic IMT/OLPS <frederic.gardes@orange.com>, RAVANAT Nathalie IMT/DRH <nathalie.ravanat@orange.com>"
"date_sent" = "2015-03-31 07:43:00"
"message_id" = "2235ccac167495314bc9f50f32ba977e"
"name" = "RE: devis QML"
"type" = "inbound"
"status" = "read"
"flagged" = "0"
"reply_to_status" = "0"
"intent" = "pick"
"mailbox_id" = "418670f2-9466-ee20-75bd-52fdf8fc1e60"
"parent_type" = "Opportunities"
"parent_id" = "75443306-18e2-634a-ed06-54f5a41ccd00"
*/
        accessors.insert(KDCRMFields::id(),
                         EmailAccessorPair(&SugarEmail::id, &SugarEmail::setId, QString()));
        accessors.insert(KDCRMFields::name(),
                         EmailAccessorPair(&SugarEmail::name, &SugarEmail::setName,
                                          i18nc("@item:intable email name", "Name")));
        accessors.insert(KDCRMFields::dateEntered(),
                         EmailAccessorPair(&SugarEmail::dateEntered, &SugarEmail::setDateEntered, QString()));
        accessors.insert(KDCRMFields::dateModified(),
                         EmailAccessorPair(&SugarEmail::dateModified, &SugarEmail::setDateModified, QString()));
        accessors.insert(KDCRMFields::modifiedUserId(),
                         EmailAccessorPair(&SugarEmail::modifiedUserId, &SugarEmail::setModifiedUserId, QString()));
        accessors.insert(KDCRMFields::modifiedByName(),
                         EmailAccessorPair(&SugarEmail::modifiedByName, &SugarEmail::setModifiedByName, QString()));
        accessors.insert(KDCRMFields::createdBy(),
                         EmailAccessorPair(&SugarEmail::createdBy, &SugarEmail::setCreatedBy, QString()));
        accessors.insert(KDCRMFields::createdByName(),
                         EmailAccessorPair(&SugarEmail::createdByName, &SugarEmail::setCreatedByName, QString()));
        accessors.insert(KDCRMFields::deleted(),
                         EmailAccessorPair(&SugarEmail::deleted, &SugarEmail::setDeleted, QString()));
        accessors.insert(KDCRMFields::assignedUserId(),
                         EmailAccessorPair(&SugarEmail::assignedUserId, &SugarEmail::setAssignedUserId, QString()));
        accessors.insert(KDCRMFields::assignedUserName(),
                         EmailAccessorPair(&SugarEmail::assignedUserName, &SugarEmail::setAssignedUserName,
                                          i18nc("@item:intable", "Assigned To")));

        accessors.insert(KDCRMFields::dateSent(),
                         EmailAccessorPair(&SugarEmail::dateSent, &SugarEmail::setDateSent, QString()));
        accessors.insert(KDCRMFields::messageId(),
                         EmailAccessorPair(&SugarEmail::messageId, &SugarEmail::setMessageId, QString()));
        accessors.insert(KDCRMFields::parentType(),
                         EmailAccessorPair(&SugarEmail::parentType, &SugarEmail::setParentType, QString()));
        accessors.insert(KDCRMFields::parentId(),
                         EmailAccessorPair(&SugarEmail::parentId, &SugarEmail::setParentId, QString()));
        accessors.insert(KDCRMFields::fromAddrName(),
                         EmailAccessorPair(&SugarEmail::fromAddrName, &SugarEmail::setFromAddrName, QString()));
        accessors.insert(KDCRMFields::toAddrsNames(),
                         EmailAccessorPair(&SugarEmail::toAddrNames, &SugarEmail::setToAddrNames, QString()));
        accessors.insert(KDCRMFields::ccAddrsNames(),
                         EmailAccessorPair(&SugarEmail::ccAddrNames, &SugarEmail::setCcAddrNames, QString()));

        // This field doesn't come from Emails but from EmailText, so it's set externally.
        // We add an accessor for SugarEmailIO though.
        accessors.insert(KDCRMFields::description(),
                         EmailAccessorPair(&SugarEmail::description, &SugarEmail::setDescription, QString()));

    }
    return accessors;
}
