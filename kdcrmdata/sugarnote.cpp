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

#include "sugarnote.h"
#include "kdcrmfields.h"
#include "kdcrmutils.h"

#include <QSharedData>
#include <QString>
#include <QMap>
#include <QDebug>

#include <KLocalizedString>

class SugarNote::Private : public QSharedData
{
public:
    Private()
    {
    }

    bool mEmpty = true;

    QString mId;
    QString mName;
    QString mDateEntered;
    QDateTime mDateModified;
    QString mModifiedUserId;
    QString mModifiedByName;
    QString mCreatedBy;
    QString mCreatedByName;
    QString mDeleted;
    QString mAssignedUserId;
    QString mAssignedUserName;

    QString mFileMimeType;
    QString mFileName;
    QString mParentType;
    QString mParentId;
    QString mContactId;
    QString mContactName;
    QString mDescription;

};

SugarNote::SugarNote()
    : d(new Private)
{
}

SugarNote::SugarNote(const SugarNote &other)
    : d(other.d)
{
}

SugarNote::~SugarNote()
{
}

SugarNote &SugarNote::operator=(const SugarNote &other)
{
    if (this != &other) {
        d = other.d;
    }

    return *this;
}

bool SugarNote::isEmpty() const
{
    return d->mEmpty;
}

void SugarNote::clear()
{
    *this = SugarNote();
}

void SugarNote::setId(const QString &id)
{
    d->mEmpty = false;
    d->mId = id;
}

QString SugarNote::id() const
{
    return d->mId;
}

void SugarNote::setName(const QString &name)
{
    d->mEmpty = false;
    d->mName = name;
}

QString SugarNote::name() const
{
    return d->mName;
}

void SugarNote::setDateEntered(const QString &value)
{
    d->mEmpty = false;
    d->mDateEntered = value;
}

QString SugarNote::dateEntered() const
{
    return d->mDateEntered;
}

void SugarNote::setDateModified(const QDateTime &date)
{
    d->mEmpty = false;
    d->mDateModified = date;
}

void SugarNote::setDateModifiedRaw(const QString &date)
{
    setDateModified(KDCRMUtils::dateTimeFromString(date));
}

QDateTime SugarNote::dateModified() const
{
    return d->mDateModified;
}

QString SugarNote::dateModifiedRaw() const
{
    return KDCRMUtils::dateTimeToString(d->mDateModified);
}

void SugarNote::setModifiedUserId(const QString &value)
{
    d->mEmpty = false;
    d->mModifiedUserId = value;
}

QString SugarNote::modifiedUserId() const
{
    return d->mModifiedUserId;
}

void SugarNote::setModifiedByName(const QString &value)
{
    d->mEmpty = false;
    d->mModifiedByName = value;
}

QString SugarNote::modifiedByName() const
{
    return d->mModifiedByName;
}

void SugarNote::setCreatedBy(const QString &value)
{
    d->mEmpty = false;
    d->mCreatedBy = value;
}

QString SugarNote::createdBy() const
{
    return d->mCreatedBy;
}

void SugarNote::setCreatedByName(const QString &value)
{
    d->mEmpty = false;
    d->mCreatedByName = value;
}

QString SugarNote::createdByName() const
{
    return d->mCreatedByName;
}

void SugarNote::setDeleted(const QString &value)
{
    d->mEmpty = false;
    d->mDeleted = value;
}

QString SugarNote::deleted() const
{
    return d->mDeleted;
}

void SugarNote::setAssignedUserId(const QString &value)
{
    d->mEmpty = false;
    d->mAssignedUserId = value;
}

QString SugarNote::assignedUserId() const
{
    return d->mAssignedUserId;
}

void SugarNote::setAssignedUserName(const QString &value)
{
    d->mEmpty = false;
    d->mAssignedUserName = value;
}

QString SugarNote::assignedUserName() const
{
    return d->mAssignedUserName;
}

void SugarNote::setFileMimeType(const QString &value)
{
    d->mEmpty = false;
    d->mFileMimeType = value;
}

QString SugarNote::fileMimeType() const
{
    return d->mFileMimeType;
}

void SugarNote::setFileName(const QString &value)
{
    d->mEmpty = false;
    d->mFileName = value;
}

QString SugarNote::fileName() const
{
    return d->mFileName;
}

void SugarNote::setParentType(const QString &value)
{
    d->mEmpty = false;
    d->mParentType = value;
}

QString SugarNote::parentType() const
{
    return d->mParentType;
}

void SugarNote::setParentId(const QString &value)
{
    d->mEmpty = false;
    d->mParentId = value;
}

QString SugarNote::parentId() const
{
    return d->mParentId;
}

void SugarNote::setContactId(const QString &value)
{
    d->mEmpty = false;
    d->mContactId = value;
}

QString SugarNote::contactId() const
{
    return d->mContactId;
}

void SugarNote::setContactName(const QString &value)
{
    d->mEmpty = false;
    d->mContactName = value;
}

QString SugarNote::contactName() const
{
    return d->mContactName;
}

void SugarNote::setDescription(const QString &value)
{
    d->mEmpty = false;
    d->mDescription = value;
}

QString SugarNote::description() const
{
    return d->mDescription;
}

void SugarNote::setData(const QMap<QString, QString>& data)
{
    d->mEmpty = false;

    const SugarNote::AccessorHash accessors = SugarNote::accessorHash();
    QMap<QString, QString>::const_iterator it = data.constBegin();
    for ( ; it != data.constEnd() ; ++it) {
        const SugarNote::AccessorHash::const_iterator accessIt = accessors.constFind(it.key());
        if (accessIt != accessors.constEnd()) {
            (this->*(accessIt.value().setter))(it.value());
        }
    }

    // equivalent to this, but fully automated:
    //d->mId = data.value("id");
    //d->mName = data.value("name");
    // ...
}

QMap<QString, QString> SugarNote::data() const
{
    QMap<QString, QString> data;

    const SugarNote::AccessorHash accessors = SugarNote::accessorHash();
    SugarNote::AccessorHash::const_iterator it    = accessors.constBegin();
    SugarNote::AccessorHash::const_iterator endIt = accessors.constEnd();
    for (; it != endIt; ++it) {
        const SugarNote::valueGetter getter = (*it).getter;
        data.insert(it.key(), (this->*getter)());
    }

    // equivalent to this, but fully automated:
    //data.insert("id", d->mId);
    //data.insert("name", d->mName);
    // ...

    return data;
}

QString SugarNote::mimeType()
{
    return QStringLiteral("application/x-vnd.kdab.crm.note");
}

Q_GLOBAL_STATIC(SugarNote::AccessorHash, s_accessors)

SugarNote::AccessorHash SugarNote::accessorHash()
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
assigned_user_name
assigned_user_id

file_mime_type
filename
parent_type
parent_id
contact_id
portal_flag --> skipped, what is that?
embed_flag  --> skipped, what is that?
description
contact_name
*/
        accessors.insert(KDCRMFields::id(),
                         NoteAccessorPair(&SugarNote::id, &SugarNote::setId, QString()));
        accessors.insert(KDCRMFields::name(),
                         NoteAccessorPair(&SugarNote::name, &SugarNote::setName,
                                          i18nc("@item:intable note name", "Name")));
        accessors.insert(KDCRMFields::dateEntered(),
                         NoteAccessorPair(&SugarNote::dateEntered, &SugarNote::setDateEntered, QString()));
        accessors.insert(KDCRMFields::dateModified(),
                         NoteAccessorPair(&SugarNote::dateModifiedRaw, &SugarNote::setDateModifiedRaw, QString()));
        accessors.insert(KDCRMFields::modifiedUserId(),
                         NoteAccessorPair(&SugarNote::modifiedUserId, &SugarNote::setModifiedUserId, QString()));
        accessors.insert(KDCRMFields::modifiedByName(),
                         NoteAccessorPair(&SugarNote::modifiedByName, &SugarNote::setModifiedByName, QString()));
        accessors.insert(KDCRMFields::createdBy(),
                         NoteAccessorPair(&SugarNote::createdBy, &SugarNote::setCreatedBy, QString()));
        accessors.insert(KDCRMFields::createdByName(),
                         NoteAccessorPair(&SugarNote::createdByName, &SugarNote::setCreatedByName, QString()));
        accessors.insert(KDCRMFields::deleted(),
                         NoteAccessorPair(&SugarNote::deleted, &SugarNote::setDeleted, QString()));
        accessors.insert(KDCRMFields::assignedUserId(),
                         NoteAccessorPair(&SugarNote::assignedUserId, &SugarNote::setAssignedUserId, QString()));
        accessors.insert(KDCRMFields::assignedUserName(),
                         NoteAccessorPair(&SugarNote::assignedUserName, &SugarNote::setAssignedUserName,
                                          i18nc("@item:intable", "Assigned To")));

        accessors.insert(KDCRMFields::fileMimeType(),
                         NoteAccessorPair(&SugarNote::fileMimeType, &SugarNote::setFileMimeType, QString()));
        accessors.insert(KDCRMFields::fileName(),
                         NoteAccessorPair(&SugarNote::fileName, &SugarNote::setFileName, QString()));
        accessors.insert(KDCRMFields::parentType(),
                         NoteAccessorPair(&SugarNote::parentType, &SugarNote::setParentType, QString()));
        accessors.insert(KDCRMFields::parentId(),
                         NoteAccessorPair(&SugarNote::parentId, &SugarNote::setParentId, QString()));
        accessors.insert(KDCRMFields::contactId(),
                         NoteAccessorPair(&SugarNote::contactId, &SugarNote::setContactId, QString()));
        accessors.insert(KDCRMFields::contactName(),
                         NoteAccessorPair(&SugarNote::contactName, &SugarNote::setContactName, QString()));
        accessors.insert(KDCRMFields::description(),
                         NoteAccessorPair(&SugarNote::description, &SugarNote::setDescription,
                                          i18nc("@item:intable", "Description")));
    }
    return accessors;
}

QDebug operator<<(QDebug stream, const SugarNote &note)
{
    stream << "Note:" << note.data();
    return stream;
}
