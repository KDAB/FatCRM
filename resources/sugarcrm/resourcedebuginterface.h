#ifndef RESOURCEDEBUGINTERFACE_H
#define RESOURCEDEBUGINTERFACE_H

#include <QObject>

class QStringList;
class SugarCRMResource;

class ResourceDebugInterface : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.kdab.SugarCRM.ResourceDebug")

    public:
        explicit ResourceDebugInterface( SugarCRMResource *resource );
        ~ResourceDebugInterface();

    public Q_SLOTS:
        Q_SCRIPTABLE void setKDSoapDebugEnabled( bool enable );
        Q_SCRIPTABLE bool kdSoapDebugEnabled() const;
        Q_SCRIPTABLE QStringList availableModules() const;
        Q_SCRIPTABLE QStringList supportedModules() const;

    private:
        SugarCRMResource *const mResource;
};

#endif
