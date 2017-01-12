/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Tobias Koenig <tobias.koenig@kdab.com>

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
#include "sugardocument.h"

#include "kdcrmfields.h"
#include "kdcrmutils.h"

#include <KLocalizedString>

#include <QHash>
#include <QMap>
#include <QSharedData>
#include <QString>

class SugarDocument::Private : public QSharedData
{
public:
    Private()
        : mEmpty(true)
    {
    }

    bool mEmpty;

    QString mId;
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
    QString mDocumentName;
    QString mDocumentId;
    QString mDocumentType;
    QString mDocumentUrl;
    QString mActiveDate;
    QString mExpDate;
    QString mCategoryId;
    QString mSubcategoryId;
    QString mStatusId;
    QString mDocumentRevisionId;
    QString mRelatedDocumentId;
    QString mRelatedDocumentName;
    QString mRelatedDocumentRevisionId;
    QString mIsTemplate;
    QString mTemplateType;
    QStringList mLinkedAccountIds;
    QStringList mLinkedOpportunityIds;

    QMap<QString, QString> mCustomFields;
};

SugarDocument::SugarDocument()
    : d(new Private)
{
}

SugarDocument::SugarDocument(const SugarDocument &other)
    : d(other.d)
{
}

SugarDocument::~SugarDocument()
{
}

SugarDocument &SugarDocument::operator=(const SugarDocument &other)
{
    if (this != &other) {
        d = other.d;
    }

    return *this;
}

bool SugarDocument::isEmpty() const
{
    return d->mEmpty;
}

void SugarDocument::clear()
{
    *this = SugarDocument();
}

void SugarDocument::setId(const QString &id)
{
    d->mEmpty = false;
    d->mId = id;
}

QString SugarDocument::id() const
{
    return d->mId;
}

void SugarDocument::setDateEntered(const QString &value)
{
    d->mEmpty = false;
    d->mDateEntered = value;
}

QString SugarDocument::dateEntered() const
{
    return d->mDateEntered;
}

void SugarDocument::setDateModified(const QString &value)
{
    d->mEmpty = false;
    d->mDateModified = value;
}

QString SugarDocument::dateModified() const
{
    return d->mDateModified;
}

void SugarDocument::setModifiedUserId(const QString &value)
{
    d->mEmpty = false;
    d->mModifiedUserId = value;
}

QString SugarDocument::modifiedUserId() const
{
    return d->mModifiedUserId;
}

void SugarDocument::setModifiedByName(const QString &value)
{
    d->mEmpty = false;
    d->mModifiedByName = value;
}

QString SugarDocument::modifiedByName() const
{
    return d->mModifiedByName;
}

void SugarDocument::setCreatedBy(const QString &value)
{
    d->mEmpty = false;
    d->mCreatedBy = value;
}

QString SugarDocument::createdBy() const
{
    return d->mCreatedBy;
}

void SugarDocument::setCreatedByName(const QString &value)
{
    d->mEmpty = false;
    d->mCreatedByName = value;
}

QString SugarDocument::createdByName() const
{
    return d->mCreatedByName;
}

void SugarDocument::setDescription(const QString &value)
{
    d->mEmpty = false;
    d->mDescription = value;
}

QString SugarDocument::description() const
{
    return d->mDescription;
}

QString SugarDocument::limitedDescription(int wantedParagraphs) const
{
    return KDCRMUtils::limitString(d->mDescription, wantedParagraphs);
}

void SugarDocument::setDeleted(const QString &value)
{
    d->mEmpty = false;
    d->mDeleted = value;
}

QString SugarDocument::deleted() const
{
    return d->mDeleted;
}

void SugarDocument::setAssignedUserId(const QString &value)
{
    d->mEmpty = false;
    d->mAssignedUserId = value;
}

QString SugarDocument::assignedUserId() const
{
    return d->mAssignedUserId;
}

void SugarDocument::setAssignedUserName(const QString &value)
{
    d->mEmpty = false;
    d->mAssignedUserName = value;
}

QString SugarDocument::assignedUserName() const
{
    return d->mAssignedUserName;
}

void SugarDocument::setDocumentName(const QString &name)
{
    d->mEmpty = false;
    d->mDocumentName = name;
}

QString SugarDocument::documentName() const
{
    return d->mDocumentName;
}

void SugarDocument::setDocumentId(const QString &id)
{
    d->mEmpty = false;
    d->mDocumentId = id;
}

QString SugarDocument::documentId() const
{
    return d->mDocumentId;
}

void SugarDocument::setDocumentType(const QString &type)
{
    d->mEmpty = false;
    d->mDocumentType = type;
}

QString SugarDocument::documentType() const
{
    return d->mDocumentType;
}

void SugarDocument::setDocumentUrl(const QString &url)
{
    d->mEmpty = false;
    d->mDocumentUrl = url;
}

QString SugarDocument::documentUrl() const
{
    return d->mDocumentUrl;
}

void SugarDocument::setActiveDate(const QString &date)
{
    d->mEmpty = false;
    d->mActiveDate = date;
}

QString SugarDocument::activeDate() const
{
    return d->mActiveDate;
}

void SugarDocument::setExpDate(const QString &date)
{
    d->mEmpty = false;
    d->mExpDate = date;
}

QString SugarDocument::expDate() const
{
    return d->mExpDate;
}

void SugarDocument::setCategoryId(const QString &id)
{
    d->mEmpty = false;
    d->mCategoryId = id;
}

QString SugarDocument::categoryId() const
{
    return d->mCategoryId;
}

void SugarDocument::setSubcategoryId(const QString &id)
{
    d->mEmpty = false;
    d->mSubcategoryId = id;
}

QString SugarDocument::subcategoryId() const
{
    return d->mSubcategoryId;
}

void SugarDocument::setStatusId(const QString &id)
{
    d->mEmpty = false;
    d->mStatusId = id;
}

QString SugarDocument::statusId() const
{
    return d->mStatusId;
}

void SugarDocument::setDocumentRevisionId(const QString &id)
{
    d->mEmpty = false;
    d->mDocumentRevisionId = id;
}

QString SugarDocument::documentRevisionId() const
{
    return d->mDocumentRevisionId;
}

void SugarDocument::setRelatedDocumentId(const QString &id)
{
    d->mEmpty = false;
    d->mRelatedDocumentId = id;
}

QString SugarDocument::relatedDocumentId() const
{
    return d->mRelatedDocumentId;
}

void SugarDocument::setRelatedDocumentName(const QString &name)
{
    d->mEmpty = false;
    d->mRelatedDocumentName = name;
}

QString SugarDocument::relatedDocumentName() const
{
    return d->mRelatedDocumentName;
}

void SugarDocument::setRelatedDocumentRevisionId(const QString &id)
{
    d->mEmpty = false;
    d->mRelatedDocumentRevisionId = id;
}

QString SugarDocument::relatedDocumentRevisionId() const
{
    return d->mRelatedDocumentRevisionId;
}

void SugarDocument::setIsTemplate(const QString &isTemplate)
{
    d->mEmpty = false;
    d->mIsTemplate = isTemplate;
}

QString SugarDocument::isTemplate() const
{
    return d->mIsTemplate;
}

void SugarDocument::setTemplateType(const QString &type)
{
    d->mEmpty = false;
    d->mTemplateType = type;
}

QString SugarDocument::templateType() const
{
    return d->mTemplateType;
}

void SugarDocument::setLinkedAccountIds(const QStringList &ids)
{
    d->mEmpty = false;
    d->mLinkedAccountIds = ids;
}

QStringList SugarDocument::linkedAccountIds() const
{
    return d->mLinkedAccountIds;
}

void SugarDocument::setLinkedOpportunityIds(const QStringList &ids)
{
    d->mEmpty = false;
    d->mLinkedOpportunityIds = ids;
}

QStringList SugarDocument::linkedOpportunityIds() const
{
    return d->mLinkedOpportunityIds;
}

void SugarDocument::setCustomField(const QString &name, const QString &value)
{
    d->mEmpty = false;
    d->mCustomFields.insert(name, value);
}

QMap<QString, QString> SugarDocument::customFields() const
{
    return d->mCustomFields;
}

void SugarDocument::setData(const QMap<QString, QString>& data)
{
    d->mEmpty = false;

    const SugarDocument::AccessorHash accessors = SugarDocument::accessorHash();
    QMap<QString, QString>::const_iterator it = data.constBegin();
    for ( ; it != data.constEnd() ; ++it) {
        const SugarDocument::AccessorHash::const_iterator accessIt = accessors.constFind(it.key());
        if (accessIt != accessors.constEnd()) {
            (this->*(accessIt.value().setter))(it.value());
        } else {
            d->mCustomFields.insert(it.key(), it.value());
        }
    }
}

QMap<QString, QString> SugarDocument::data() const
{
    QMap<QString, QString> data;

    const SugarDocument::AccessorHash accessors = SugarDocument::accessorHash();
    SugarDocument::AccessorHash::const_iterator it    = accessors.constBegin();
    SugarDocument::AccessorHash::const_iterator endIt = accessors.constEnd();
    for (; it != endIt; ++it) {
        const SugarDocument::valueGetter getter = (*it).getter;
        data.insert(it.key(), (this->*getter)());
    }

    QMap<QString, QString>::const_iterator cit = d->mCustomFields.constBegin();
    const QMap<QString, QString>::const_iterator end = d->mCustomFields.constEnd();
    for ( ; cit != end ; ++cit ) {
        data.insert(cit.key(), cit.value());
    }

    return data;
}

QString SugarDocument::mimeType()
{
    return QStringLiteral("application/x-vnd.kdab.crm.document");
}

/* coverity[leaked_storage] */
Q_GLOBAL_STATIC(SugarDocument::AccessorHash, s_accessors)

SugarDocument::AccessorHash SugarDocument::accessorHash()
{
    AccessorHash &accessors = *s_accessors();
    if (accessors.isEmpty()) {
        accessors.insert(KDCRMFields::id(),
                         DocumentAccessorPair(&SugarDocument::id, &SugarDocument::setId, QString()));
        accessors.insert(KDCRMFields::dateEntered(),
                         DocumentAccessorPair(&SugarDocument::dateEntered, &SugarDocument::setDateEntered, QString()));
        accessors.insert(KDCRMFields::dateModified(),
                          DocumentAccessorPair(&SugarDocument::dateModified, &SugarDocument::setDateModified, QString()));
        accessors.insert(KDCRMFields::modifiedUserId(),
                          DocumentAccessorPair(&SugarDocument::modifiedUserId, &SugarDocument::setModifiedUserId, QString()));
        accessors.insert(KDCRMFields::modifiedByName(),
                          DocumentAccessorPair(&SugarDocument::modifiedByName, &SugarDocument::setModifiedByName, QString()));
        accessors.insert(KDCRMFields::createdBy(),
                          DocumentAccessorPair(&SugarDocument::createdBy, &SugarDocument::setCreatedBy, QString()));
        accessors.insert(KDCRMFields::createdByName(),
                          DocumentAccessorPair(&SugarDocument::createdByName, &SugarDocument::setCreatedByName, QString()));
        accessors.insert(KDCRMFields::description(),
                          DocumentAccessorPair(&SugarDocument::description, &SugarDocument::setDescription,
                                            i18nc("@item:intable", "Description")));
        accessors.insert(KDCRMFields::deleted(),
                          DocumentAccessorPair(&SugarDocument::deleted, &SugarDocument::setDeleted, QString()));
        accessors.insert(KDCRMFields::assignedUserId(),
                          DocumentAccessorPair(&SugarDocument::assignedUserId, &SugarDocument::setAssignedUserId, QString()));
        accessors.insert(KDCRMFields::assignedUserName(),
                          DocumentAccessorPair(&SugarDocument::assignedUserName, &SugarDocument::setAssignedUserName,
                                            i18nc("@item:intable", "Assigned To")));

        accessors.insert(KDCRMFields::documentName(),
                          DocumentAccessorPair(&SugarDocument::documentName, &SugarDocument::setDocumentName, QString()));
        accessors.insert(KDCRMFields::docId(),
                          DocumentAccessorPair(&SugarDocument::documentId, &SugarDocument::setDocumentId, QString()));
        accessors.insert(KDCRMFields::docType(),
                          DocumentAccessorPair(&SugarDocument::documentType, &SugarDocument::setDocumentType, QString()));
        accessors.insert(KDCRMFields::docUrl(),
                          DocumentAccessorPair(&SugarDocument::documentUrl, &SugarDocument::setDocumentUrl, QString()));
        accessors.insert(KDCRMFields::activeDate(),
                          DocumentAccessorPair(&SugarDocument::activeDate, &SugarDocument::setActiveDate, QString()));
        accessors.insert(KDCRMFields::expDate(),
                          DocumentAccessorPair(&SugarDocument::expDate, &SugarDocument::setExpDate, QString()));
        accessors.insert(KDCRMFields::categoryId(),
                          DocumentAccessorPair(&SugarDocument::categoryId, &SugarDocument::setCategoryId, QString()));
        accessors.insert(KDCRMFields::subcategoryId(),
                          DocumentAccessorPair(&SugarDocument::subcategoryId, &SugarDocument::setSubcategoryId, QString()));
        accessors.insert(KDCRMFields::statusId(),
                          DocumentAccessorPair(&SugarDocument::statusId, &SugarDocument::setStatusId, QString()));
        accessors.insert(KDCRMFields::documentRevisionId(),
                          DocumentAccessorPair(&SugarDocument::documentRevisionId, &SugarDocument::setDocumentRevisionId, QString()));
        accessors.insert(KDCRMFields::relatedDocId(),
                          DocumentAccessorPair(&SugarDocument::relatedDocumentId, &SugarDocument::setRelatedDocumentId, QString()));
        accessors.insert(KDCRMFields::relatedDocName(),
                          DocumentAccessorPair(&SugarDocument::relatedDocumentName, &SugarDocument::setRelatedDocumentName, QString()));
        accessors.insert(KDCRMFields::relatedDocRevId(),
                          DocumentAccessorPair(&SugarDocument::relatedDocumentRevisionId, &SugarDocument::setRelatedDocumentRevisionId, QString()));
        accessors.insert(KDCRMFields::isTemplate(),
                          DocumentAccessorPair(&SugarDocument::isTemplate, &SugarDocument::setIsTemplate, QString()));
        accessors.insert(KDCRMFields::templateType(),
                          DocumentAccessorPair(&SugarDocument::templateType, &SugarDocument::setTemplateType, QString()));
    }

    return accessors;
}
