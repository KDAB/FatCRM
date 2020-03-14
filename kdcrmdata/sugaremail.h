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

#ifndef SUGAREMAIL_H
#define SUGAREMAIL_H

#include "kdcrmdata_export.h"

#include <QMetaType>
#include <QSharedDataPointer>

/**
 Short: sugar email entry

  This class represents an email entry in sugarcrm.

  The data of this class is implicitly shared. You can pass this class by value.
 */
class KDCRMDATA_EXPORT SugarEmail
{

public:

    /**
      Construct an empty SugarEmail entry.
     */
    SugarEmail();

    /**
      Destroys the SugarEmail entry.
     */
    ~SugarEmail();

    /**
      Copy constructor.
     */
    SugarEmail(const SugarEmail &);

    /**
      Assignment operator.

      return a reference to this
    */
    SugarEmail &operator=(const SugarEmail &);

    /**
      Return, if the SugarEmail entry is empty.
     */
    bool isEmpty() const;

    /**
      Clears all entries of the SugarEmail.
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
      Set the Email Name.
     */
    void setName(const QString &id);
    /**
      Return the Email Name.
     */
    QString name() const;

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
    void setDateModified(const QDateTime &date);
    void setDateModifiedRaw(const QString &date);

    /**
      Return Modification date.
     */
    QDateTime dateModified() const;
    QString dateModifiedRaw() const;

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

    void setDateSent(const QString &value);
    QString dateSent() const;
    void setMessageId(const QString &value);
    QString messageId() const;
    void setParentType(const QString &value);
    QString parentType() const;
    void setParentId(const QString &value);
    QString parentId() const;
    void setFromAddrName(const QString &value);
    QString fromAddrName() const;
    void setToAddrNames(const QString &value);
    QString toAddrNames() const;
    void setCcAddrNames(const QString &value);
    QString ccAddrNames() const;

    // The mail body
    void setDescription(const QString &value);
    QString description() const;

    // The mail body in HTML formatting
    void setDescriptionHtml(const QString &value);
    QString descriptionHtml() const;

    /**
      Convenience: set the data for this email
     */
    void setData(const QMap<QString, QString> &data);

    /**
      Convenience: retrieve the data for this email
     */
    QMap<QString, QString> data() const;

    /**
       Return the Mime type
     */
    static QString mimeType();

    using valueGetter = QString (SugarEmail::*)() const;
    using valueSetter = void (SugarEmail::*)(const QString &);

    class EmailAccessorPair
    {
    public:
        EmailAccessorPair(valueGetter get, valueSetter set, const QString &name)
            : getter(get), setter(set), diffName(name)
        {}

    public:
        valueGetter getter;
        valueSetter setter;
        QString diffName;
    };

    typedef QHash<QString, EmailAccessorPair> AccessorHash;

    static AccessorHash accessorHash();

private:
    class Private;
    QSharedDataPointer<Private> d;
};

Q_DECLARE_METATYPE(SugarEmail)

#endif /* SUGAREMAIL_H */
