#ifndef MODULEDEBUGINTERFACE_H
#define MODULEDEBUGINTERFACE_H

#include <QObject>
#include <QStringList>

class SugarCRMResource;

class ModuleDebugInterface : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.kdab.SugarCRM.ModuleDebug")

public:
    ModuleDebugInterface(const QString &moduleName, SugarCRMResource *resource);
    ~ModuleDebugInterface();

public Q_SLOTS:
    Q_SCRIPTABLE int getCount() const;
    Q_SCRIPTABLE QStringList availableFields() const;
    Q_SCRIPTABLE QStringList supportedFields() const;
    Q_SCRIPTABLE QString lastestTimestamp() const;
    Q_SCRIPTABLE void resetLatestTimestamp();

private:
    const QString mModuleName;
    SugarCRMResource *const mResource;

    mutable QStringList mAvailableFields;
};

#endif
