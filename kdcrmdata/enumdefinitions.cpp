#include "enumdefinitions.h"
#include <QStringList>

QString EnumDefinitions::Enum::toString() const
{
    QString ret = mEnumName;
    ret += '|';
    for (Map::const_iterator it = mEnumValues.constBegin(); it != mEnumValues.constEnd(); ++it) {
        Q_ASSERT(!it.key().contains(':'));
        Q_ASSERT(!it.key().contains('|'));
        Q_ASSERT(!it.value().contains(':'));
        Q_ASSERT(!it.value().contains('|'));
        ret += it.key() + ':' + it.value() + '|';
    }
    return ret;
}

EnumDefinitions::Enum EnumDefinitions::Enum::fromString(const QString &str)
{
    const int nameSep = str.indexOf('|');
    Q_ASSERT(nameSep > -1);
    Enum ret(str.left(nameSep));
    int pos = nameSep + 1;
    while (pos < str.length()) {
        const int sep = str.indexOf(':', pos);
        Q_ASSERT(sep > -1);
        const int end = str.indexOf('|', sep + 1);
        Q_ASSERT(end > -1);
        const QString key = str.mid(pos, sep - pos);
        const QString value = str.mid(sep + 1, end - sep - 1);
        ret.mEnumValues.insert(key, value);
        pos = end + 1;
    }

    return ret;
}


EnumDefinitions::EnumDefinitions()
{
}

int EnumDefinitions::indexOf(const QString &enumName) const
{
    for (size_t i = 0; i < mDefinitions.size(); ++i) {
        if (mDefinitions.at(i).mEnumName == enumName) {
            return i;
        }
    }
    return -1;
}

// E.g. "lead_source|Key1:Value1|Key2:Value2|Key3:Value3|%another_enum|K:V"
QString EnumDefinitions::toString() const
{
    QString ret;
    for (size_t i = 0; i < mDefinitions.size(); ++i) {
        ret += mDefinitions.at(i).toString();
        if (i + 1 < mDefinitions.size()) {
            ret += '%';
        }
    }
    return ret;
}

EnumDefinitions EnumDefinitions::fromString(const QString &str)
{
    EnumDefinitions ret;
    // a bit slow but I'm lazy
    foreach (const QString &s, str.split('%', QString::SkipEmptyParts)) {
        ret.append(EnumDefinitions::Enum::fromString(s));
    }

    return ret;
}
