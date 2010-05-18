#include "resourcedebuginterface.h"

#include "modulehandler.h"
#include "sugarcrmresource.h"

#include <stdlib.h>

ResourceDebugInterface::ResourceDebugInterface( SugarCRMResource *resource )
    : QObject( resource ), mResource( resource )
{
}

ResourceDebugInterface::~ResourceDebugInterface()
{
}

void ResourceDebugInterface::setKDSoapDebugEnabled( bool enable )
{
    // Most likely this needs to be done completely different on windows
    // unfortunately there seems to be no equivalent to qgetenv()
    if ( enable ) {
        ::setenv( "KDSOAP_DEBUG", "1", 1 );
    } else {
        ::unsetenv( "KDSOAP_DEBUG" );
    }
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
