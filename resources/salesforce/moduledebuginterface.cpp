#include "moduledebuginterface.h"

#include "modulehandler.h"
#include "salesforceresource.h"
#include "salesforcesoap.h"

#include <QStringList>

ModuleDebugInterface::ModuleDebugInterface(const QString &moduleName, SalesforceResource *resource)
    : QObject(resource), mModuleName(moduleName), mResource(resource)
{
}

ModuleDebugInterface::~ModuleDebugInterface()
{
}

QStringList ModuleDebugInterface::availableFields() const
{
    ModuleHandler *handler = (*mResource->mModuleHandlers)[ mModuleName ];
    if (handler != 0) {
        return handler->availableFields();
    }

    return QStringList();
}

QStringList ModuleDebugInterface::supportedFields() const
{
    ModuleHandler *handler = (*mResource->mModuleHandlers)[ mModuleName ];
    if (handler != 0) {
        return handler->supportedFields();
    }

    return QStringList();
}

#include "moduledebuginterface.moc"
