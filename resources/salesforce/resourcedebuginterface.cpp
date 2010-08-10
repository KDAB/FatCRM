#include "resourcedebuginterface.h"

#include "modulehandler.h"
#include "salesforceresource.h"

ResourceDebugInterface::ResourceDebugInterface( SalesforceResource *resource )
    : QObject( resource ), mResource( resource )
{
}

ResourceDebugInterface::~ResourceDebugInterface()
{
}

void ResourceDebugInterface::setKDSoapDebugEnabled( bool enable )
{
    qputenv( "KDSOAP_DEBUG", ( enable ? "1" : "0" ) );
}

bool ResourceDebugInterface::kdSoapDebugEnabled() const
{
    const QByteArray value = qgetenv( "KDSOAP_DEBUG" );
    return value.toInt() != 0;
}

QStringList ResourceDebugInterface::availableModules() const
{
    return mResource->mAvailableModules;
}

QStringList ResourceDebugInterface::supportedModules() const
{
    return mResource->mModuleHandlers->keys();
}

#include "resourcedebuginterface.moc"
