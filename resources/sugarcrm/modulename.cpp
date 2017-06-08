#include <QString>
#include <QMap>
#include <KDebug>
#include "modulename.h"

struct ModuleEntry {
    const char* name;
    const Module module;
};

static const ModuleEntry s_moduleNames[] = {
    {"Accounts", Accounts},
    {"Opportunities", Opportunities},
    {"Campaigns", Campaigns},
    {"Leads", Leads},
    {"Contacts", Contacts},
    {"Documents", Documents},
    {"Emails", Emails},
    {"Notes", Notes},
    {"Tasks", Tasks}
};

static const int count = sizeof(s_moduleNames) / sizeof(*s_moduleNames);

QString moduleToName(Module moduleName)
{
    auto result = std::find_if(s_moduleNames, s_moduleNames + count, [moduleName](const ModuleEntry &a){return a.module == moduleName;});
    if (result == s_moduleNames + count) {
        return QString();
    } else {
        return result->name;
    }
}

Module nameToModule(const QString &name)
{
    auto result = std::find_if(s_moduleNames, s_moduleNames + count, [name](const ModuleEntry &a){return a.name == name;});
    if (result == s_moduleNames + count) {
        return Error;
    } else {
        return result->module;
    }
}

QDebug operator<<(QDebug dbg, Module module)
{
    return dbg << moduleToName(module);
}
