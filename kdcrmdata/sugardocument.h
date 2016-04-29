/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef SUGARDOCUMENT_H
#define SUGARDOCUMENT_H

#include "kdcrmdata_export.h"

#include <QMetaType>
#include <QSharedDataPointer>

/**
  @short sugar document entry

  This class represents an document entry in sugarcrm.

  The data of this class is implicitly shared. You can pass this class by value.
 */
class KDCRMDATA_EXPORT SugarDocument
{

public:

    /**
      Construct an empty SugarDocument entry.
     */
    SugarDocument();

    /**
      Destroys the SugarDocument entry.
     */
    ~SugarDocument();

    /**
      Copy constructor.
     */
    SugarDocument(const SugarDocument &);

    /**
      Assignment operator.

      @return a reference to @c this
    */
    SugarDocument &operator=(const SugarDocument &);

private:
    /**
      Equality operator.

      @return @c true if @c this and the given SugarDocument are equal,
              otherwise @c false
    */
    bool operator==(const SugarDocument &) const;

    /**
      Not-equal operator.

      @return @c true if @c this and the given SugarDocument are not equal,
              otherwise @c false
    */
    bool operator!=(const SugarDocument &) const;
public:

    /**
      Return, if the SugarDocument entry is empty.
     */
    bool isEmpty() const;

    /**
      Clears all entries of the SugarDocument.
    */
    void clear();

    /**
      Set unique identifier.
     */
    void setId(const QString &id);
    /**
      Return unique identifier.
     */
    QString id() const;

    /**
      Set Creation date.
     */
    void setDateEntered(const QString &name);
    /**
      Return Creation date.
     */
    QString dateEntered() const;

    /**
      Set Modification date.
     */
    void setDateModified(const QString &name);
    /**
      Return Modification date.
     */
    QString dateModified() const;

    /**
      Set Modified User id.
     */
    void setModifiedUserId(const QString &name);
    /**
      Return Modified User id.
     */
    QString modifiedUserId() const;

    /**
      Set Modified By name.
     */
    void setModifiedByName(const QString &name);
    /**
      Return Modified By name.
     */
    QString modifiedByName() const;

    /**
      Set Created By id.
     */
    void setCreatedBy(const QString &name);
    /**
      Return created by id.
     */
    QString createdBy() const;

    /**
      Set Created By Name.
     */
    void setCreatedByName(const QString &name);
    /**
      Return created by Name.
     */
    QString createdByName() const;

    /**
      Set Description.
    */
    void setDescription(const QString &value);
    /**
      Return Description.
     */
    QString description() const;
    /**
      Return the wanted number of paragraphs in the description (or the full
      description if the number of paragraphs is less than the wanted number).
     */
    QString limitedDescription(int wantedParagraphs) const;

    /**
      Set Deleted.
    */
    void setDeleted(const QString &value);
    /**
      Return Deleted.
     */
    QString deleted() const;

    /**
      Set Assigned User Id.
    */
    void setAssignedUserId(const QString &value);
    /**
      Return Assigned User Id.
     */
    QString assignedUserId() const;

    /**
      Set Assigned User Name.
     */
    void setAssignedUserName(const QString &value);
    /**
      Return assigned User Name.
     */
    QString assignedUserName() const;

    /**
      Set document name.
     */
    void setDocumentName(const QString &name);
    /**
      Return document name.
     */
    QString documentName() const;

    /**
      Set document id.
     */
    void setDocumentId(const QString &id);
    /**
      Return document id.
     */
    QString documentId() const;

    /**
      Set document type.
     */
    void setDocumentType(const QString &type);
    /**
      Return document type.
     */
    QString documentType() const;

    /**
      Set document url.
     */
    void setDocumentUrl(const QString &url);
    /**
      Return document url.
     */
    QString documentUrl() const;

    /**
      Set active date.
     */
    void setActiveDate(const QString &date);
    /**
      Return active date.
     */
    QString activeDate() const;

    /**
      Set exp date.
     */
    void setExpDate(const QString &date);
    /**
      Return exp date.
     */
    QString expDate() const;

    /**
      Set category id.
     */
    void setCategoryId(const QString &id);
    /**
      Return category id.
     */
    QString categoryId() const;

    /**
      Set subcategory id.
     */
    void setSubcategoryId(const QString &id);
    /**
      Return subcategory id.
     */
    QString subcategoryId() const;

    /**
      Set status id.
     */
    void setStatusId(const QString &id);
    /**
      Return status id.
     */
    QString statusId() const;

    /**
      Set document revision id.
     */
    void setDocumentRevisionId(const QString &id);
    /**
      Return document revision id.
     */
    QString documentRevisionId() const;

    /**
      Set related document id.
     */
    void setRelatedDocumentId(const QString &id);
    /**
      Return related document id.
     */
    QString relatedDocumentId() const;

    /**
      Set related document name.
     */
    void setRelatedDocumentName(const QString &name);
    /**
      Return related document name.
     */
    QString relatedDocumentName() const;

    /**
      Set related document revision id.
     */
    void setRelatedDocumentRevisionId(const QString &id);
    /**
      Return related document revision id.
     */
    QString relatedDocumentRevisionId() const;

    /**
      Set is template.
     */
    void setIsTemplate(const QString &isTemplate);
    /**
      Return is template.
     */
    QString isTemplate() const;

    /**
      Set template type.
     */
    void setTemplateType(const QString &type);
    /**
      Return template type.
     */
    QString templateType() const;

    // Unknown fields (missing in this code) and custom fields (added by Sugar configuration)
    void setCustomField(const QString &name, const QString &value);
    QMap<QString, QString> customFields() const;

    /**
      Convenience: Set the data for this document
     */
    void setData(const QMap<QString, QString> &data);

    /**
      Convenience: Retrieve the data for this document
     */
    QMap<QString, QString> data() const;

    /**
       Return the Mime type
     */
    static QString mimeType();

    typedef QString(SugarDocument::*valueGetter)() const;
    typedef void (SugarDocument::*valueSetter)(const QString &);

    class DocumentAccessorPair
    {
    public:
        DocumentAccessorPair(valueGetter get, valueSetter set, const QString &name)
            : getter(get), setter(set), diffName(name)
        {}

    public:
        valueGetter getter;
        valueSetter setter;
        QString diffName;
    };

    typedef QHash<QString, DocumentAccessorPair> AccessorHash;

    static AccessorHash accessorHash();

private:
    class Private;
    QSharedDataPointer<Private> d;
};

Q_DECLARE_METATYPE(SugarDocument)

#endif /* SUGARDOCUMENT_H */
