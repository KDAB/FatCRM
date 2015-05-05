#ifndef SUGAREMAIL_H
#define SUGAREMAIL_H

#include <QtCore/QMetaType>
#include <QtCore/QSharedDataPointer>

#include "kdcrmdata_export.h"

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
      Equality operator.

      return true if this and the given SugarEmail are equal,
              otherwise false
    */
    bool operator==(const SugarEmail &) const;

    /**
      Not-equal operator.

      return true if  this and the given SugarEmail are not equal,
              otherwise false
    */
    bool operator!=(const SugarEmail &) const;

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

    typedef QString(SugarEmail::*valueGetter)() const;
    typedef void (SugarEmail::*valueSetter)(const QString &);

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

