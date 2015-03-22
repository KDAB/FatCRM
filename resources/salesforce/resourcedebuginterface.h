#ifndef RESOURCEDEBUGINTERFACE_H
#define RESOURCEDEBUGINTERFACE_H

#include <QObject>

class QStringList;
class SalesforceResource;

class ResourceDebugInterface : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.kdab.Salesforce.ResourceDebug")

public:
    explicit ResourceDebugInterface(SalesforceResource *resource);
    ~ResourceDebugInterface();

public Q_SLOTS:
    Q_SCRIPTABLE void setKDSoapDebugEnabled(bool enable);
    Q_SCRIPTABLE bool kdSoapDebugEnabled() const;
    Q_SCRIPTABLE QStringList availableModules() const;
    Q_SCRIPTABLE QStringList supportedModules() const;

private:
    SalesforceResource *const mResource;
};

#endif
