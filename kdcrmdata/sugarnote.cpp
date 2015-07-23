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

#include "sugarnote.h"
#include "kdcrmfields.h"

#include <QSharedData>
#include <QString>
#include <QMap>

#include <KLocalizedString>

class SugarNote::Private : public QSharedData
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
        mDeleted = other.mDeleted;
        mAssignedUserId = other.mAssignedUserId;
        mAssignedUserName = other.mAssignedUserName;

        mFileMimeType = other.mFileMimeType;
        mFileName = other.mFileName;
        mParentType = other.mParentType;
        mParentId = other.mParentId;
        mContactId = other.mContactId;
        mContactName = other.mContactName;
        mDescription = other.mDescription;
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

bool SugarNote::operator==(const SugarNote &other) const
{
    if (d->mId != other.d->mId) {
        return false;
    }
    if (d->mName != other.d->mName) {
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
    if (d->mDeleted != other.d->mDeleted) {
        return false;
    }
    if (d->mAssignedUserId != other.d->mAssignedUserId) {
        return false;
    }
    if (d->mAssignedUserName != other.d->mAssignedUserName) {
        return false;
    }
    if (d->mFileMimeType != other.d->mFileMimeType) {
        return false;
    }
    if (d->mFileName != other.d->mFileName) {
        return false;
    }
    if (d->mParentType != other.d->mParentType) {
        return false;
    }
    if (d->mParentId != other.d->mParentId) {
        return false;
    }
    if (d->mContactId != other.d->mContactId) {
        return false;
    }
    if (d->mContactName != other.d->mContactName) {
        return false;
    }
    if (d->mDescription != other.d->mDescription) {
        return false;
    }
    return true;
}

bool SugarNote::operator!=(const SugarNote &a) const
{
    return !(a == *this);
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

void SugarNote::setDateModified(const QString &value)
{
    d->mEmpty = false;
    d->mDateModified = value;
}

QString SugarNote::dateModified() const
{
    return d->mDateModified;
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
        accessors.insert(QStringLiteral("id"),
                         NoteAccessorPair(&SugarNote::id, &SugarNote::setId, QString()));
        accessors.insert(QStringLiteral("name"),
                         NoteAccessorPair(&SugarNote::name, &SugarNote::setName,
                                          i18nc("@item:intable note name", "Name")));
        accessors.insert(QStringLiteral("date_entered"),
                         NoteAccessorPair(&SugarNote::dateEntered, &SugarNote::setDateEntered, QString()));
        accessors.insert(QStringLiteral("date_modified"),
                         NoteAccessorPair(&SugarNote::dateModified, &SugarNote::setDateModified, QString()));
        accessors.insert(QStringLiteral("modified_user_id"),
                         NoteAccessorPair(&SugarNote::modifiedUserId, &SugarNote::setModifiedUserId, QString()));
        accessors.insert(QStringLiteral("modified_by_name"),
                         NoteAccessorPair(&SugarNote::modifiedByName, &SugarNote::setModifiedByName, QString()));
        accessors.insert(QStringLiteral("created_by"),
                         NoteAccessorPair(&SugarNote::createdBy, &SugarNote::setCreatedBy, QString()));
        accessors.insert(QStringLiteral("created_by_name"),
                         NoteAccessorPair(&SugarNote::createdByName, &SugarNote::setCreatedByName, QString()));
        accessors.insert(QStringLiteral("deleted"),
                         NoteAccessorPair(&SugarNote::deleted, &SugarNote::setDeleted, QString()));
        accessors.insert(QStringLiteral("assigned_user_id"),
                         NoteAccessorPair(&SugarNote::assignedUserId, &SugarNote::setAssignedUserId, QString()));
        accessors.insert(QStringLiteral("assigned_user_name"),
                         NoteAccessorPair(&SugarNote::assignedUserName, &SugarNote::setAssignedUserName,
                                          i18nc("@item:intable", "Assigned To")));

        accessors.insert(QStringLiteral("file_mime_type"),
                         NoteAccessorPair(&SugarNote::fileMimeType, &SugarNote::setFileMimeType, QString()));
        accessors.insert(QStringLiteral("filename"),
                         NoteAccessorPair(&SugarNote::fileName, &SugarNote::setFileName, QString()));
        accessors.insert(QStringLiteral("parent_type"),
                         NoteAccessorPair(&SugarNote::parentType, &SugarNote::setParentType, QString()));
        accessors.insert(QStringLiteral("parent_id"),
                         NoteAccessorPair(&SugarNote::parentId, &SugarNote::setParentId, QString()));
        accessors.insert(QStringLiteral("contact_id"),
                         NoteAccessorPair(&SugarNote::contactId, &SugarNote::setContactId, QString()));
        accessors.insert(QStringLiteral("contact_name"),
                         NoteAccessorPair(&SugarNote::contactName, &SugarNote::setContactName, QString()));
        accessors.insert(QStringLiteral("description"),
                         NoteAccessorPair(&SugarNote::description, &SugarNote::setDescription,
                                          i18nc("@item:intable", "Description")));
    }
    return accessors;
}
