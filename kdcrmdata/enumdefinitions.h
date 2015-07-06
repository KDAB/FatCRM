#ifndef ENUMDEFINITIONS_H
#define ENUMDEFINITIONS_H

#include <vector>
#include <QMap>
#include <QString>
#include <QMetaType>
#include "kdcrmdata_export.h"

/**
 * @brief The EnumDefinitions class lists enum definitions for one type of item.
 * E.g. for Contacts, enum definitions exist for
 * salutation, lead_source, and portal_user_type
 *
 * Each enum definition contains a list of values (ID and display string).
 * E.g. one of the values in the 'lead source' definition is: ID="QtDevDays", DisplayString="Qt Developer Days"
 */
class KDCRMDATA_EXPORT EnumDefinitions
{
public:
    EnumDefinitions();

    struct KDCRMDATA_EXPORT Enum
    {
        Enum(const QString &name) : mEnumName(name) {}
        QString toString() const;
        static Enum fromString(const QString &str);

        QString mEnumName;
        typedef QMap<QString, QString> Map;
        Map mEnumValues; // ID, display string
    };

    void append(const Enum &e) { mDefinitions.push_back(e); }
    EnumDefinitions &operator<<(const Enum &e) { mDefinitions.push_back(e); return *this; }

    int count() const { return mDefinitions.size(); }
    const Enum & at(int i) const { return mDefinitions.at(i); }
    int indexOf(const QString &enumName) const;

    // serialization
    QString toString() const;
    static EnumDefinitions fromString(const QString &str);

private:
    std::vector<Enum> mDefinitions;
};

Q_DECLARE_METATYPE(EnumDefinitions)

#endif // ENUMDEFINITIONS_H
