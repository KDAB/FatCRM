#include "moduledebuginterface.h"

#include "resourcedebuginterface.h"
#include "modulehandler.h"
#include "sugarcrmresource.h"
#include "sugarsession.h"
#include "sugarsoap.h"

using namespace KDSoapGenerated;
#include <QStringList>

ModuleDebugInterface::ModuleDebugInterface(const QString &moduleName, SugarCRMResource *resource)
    : QObject(resource), mModuleName(moduleName), mResource(resource)
{
}

ModuleDebugInterface::~ModuleDebugInterface()
{
}

QStringList ModuleDebugInterface::availableFields() const
{
    if (mAvailableFields.isEmpty()) {
        kDebug() << "Available Fields for " << mModuleName
                 << "not fetch yet, getting them now";

        mAvailableFields = mResource->mDebugInterface->availableFields(mModuleName);
    }

    return mAvailableFields;
}

QStringList ModuleDebugInterface::supportedFields() const
{
    ModuleHandler *handler = (*mResource->mModuleHandlers)[ mModuleName ];
    if (handler != 0) {
        return handler->supportedFields();
    }

    return QStringList();
}

QString ModuleDebugInterface::lastestTimestamp() const
{
    ModuleHandler *handler = (*mResource->mModuleHandlers)[ mModuleName ];
    if (handler != 0) {
        return handler->latestTimestamp();
    }

    return QString();
}

void ModuleDebugInterface::resetLatestTimestamp()
{
    ModuleHandler *handler = (*mResource->mModuleHandlers)[ mModuleName ];
    if (handler != 0) {
        handler->resetLatestTimestamp();
    }
}

#include "moduledebuginterface.moc"
