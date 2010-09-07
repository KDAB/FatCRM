#include "moduledebuginterface.h"

#include "modulehandler.h"
#include "sugarcrmresource.h"
#include "sugarsession.h"
#include "sugarsoap.h"

#include <QStringList>

ModuleDebugInterface::ModuleDebugInterface( const QString &moduleName, SugarCRMResource *resource )
    : QObject( resource ), mModuleName( moduleName ), mResource( resource )
{
}

ModuleDebugInterface::~ModuleDebugInterface()
{
}

QStringList ModuleDebugInterface::availableFields() const
{
    if ( mAvailableFields.isEmpty() ) {
        kDebug() << "Available Fields for " << mModuleName
                 << "not fetch yet, getting them now";

        SugarSession *session = mResource->mSession;
        Sugarsoap *soap = session->soap();
        const QString sessionId = session->sessionId();

        const TNS__Module_fields response = soap->get_module_fields( sessionId, mModuleName );

        const TNS__Error_value error = response.error();
        if ( error.number().isEmpty() || error.number() == QLatin1String( "0" ) ) {
            const TNS__Field_list fieldList = response.module_fields();
            Q_FOREACH( const TNS__Field &field, fieldList.items() ) {
                mAvailableFields << field.name();
            }
            kDebug() << "Got" << mAvailableFields << "fields";
        } else {
            kDebug() << "Got error: number=" << error.number()
                     << "name=" << error.name()
                     << "description=" << error.description();
        }
    }

    return mAvailableFields;
}

QStringList ModuleDebugInterface::supportedFields() const
{
    ModuleHandler *handler = (*mResource->mModuleHandlers)[ mModuleName ];
    if ( handler != 0 ) {
        return handler->supportedFields();
    }

    return QStringList();
}

#include "moduledebuginterface.moc"
