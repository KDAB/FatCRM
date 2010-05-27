#ifndef MODULEDEBUGINTERFACE_H
#define MODULEDEBUGINTERFACE_H

#include <QObject>
#include <QStringList>

class SalesforceResource;

class ModuleDebugInterface : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.kdab.Salesforce.ModuleDebug")

    public:
        ModuleDebugInterface( const QString &moduleName, SalesforceResource *resource );
        ~ModuleDebugInterface();

    public Q_SLOTS:
        Q_SCRIPTABLE QStringList availableFields() const;
        Q_SCRIPTABLE QStringList supportedFields() const;

    private:
        const QString mModuleName;
        SalesforceResource *const mResource;
};

#endif
