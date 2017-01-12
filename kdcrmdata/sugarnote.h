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

#ifndef SUGARNOTE_H
#define SUGARNOTE_H

#include "kdcrmdata_export.h"

#include <QMetaType>
#include <QSharedDataPointer>

/**
 Short: sugar note entry

  This class represents an note entry in sugarcrm.

  The data of this class is implicitly shared. You can pass this class by value.
 */
class KDCRMDATA_EXPORT SugarNote
{

public:

    /**
      Construct an empty SugarNote entry.
     */
    SugarNote();

    /**
      Destroys the SugarNote entry.
     */
    ~SugarNote();

    /**
      Copy constructor.
     */
    SugarNote(const SugarNote &);

    /**
      Assignment operator.

      return a reference to this
    */
    SugarNote &operator=(const SugarNote &);

    /**
      Return, if the SugarNote entry is empty.
     */
    bool isEmpty() const;

    /**
      Clears all entries of the SugarNote.
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
      Set the Note Name.
     */
    void setName(const QString &id);
    /**
      Return the Note Name.
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

    void setFileMimeType(const QString &value);
    QString fileMimeType() const;
    void setFileName(const QString &value);
    QString fileName() const;
    void setParentType(const QString &value);
    QString parentType() const;
    void setParentId(const QString &value);
    QString parentId() const;
    void setContactId(const QString &value);
    QString contactId() const;
    void setContactName(const QString &value);
    QString contactName() const;
    void setDescription(const QString &value);
    QString description() const;

    /**
      Convenience: set the data for this note
     */
    void setData(const QMap<QString, QString> &data);

    /**
      Convenience: retrieve the data for this note
     */
    QMap<QString, QString> data() const;

    /**
       Return the Mime type
     */
    static QString mimeType();

    typedef QString(SugarNote::*valueGetter)() const;
    typedef void (SugarNote::*valueSetter)(const QString &);

    class NoteAccessorPair
    {
    public:
        NoteAccessorPair(valueGetter get, valueSetter set, const QString &name)
            : getter(get), setter(set), diffName(name)
        {}

    public:
        valueGetter getter;
        valueSetter setter;
        QString diffName;
    };

    typedef QHash<QString, NoteAccessorPair> AccessorHash;

    static AccessorHash accessorHash();

private:
    class Private;
    QSharedDataPointer<Private> d;
};

KDCRMDATA_EXPORT QDebug operator<<(QDebug stream, const SugarNote &note);

Q_DECLARE_METATYPE(SugarNote)

#endif /* SUGARNOTE_H */
