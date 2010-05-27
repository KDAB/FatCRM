#include "modulehandler.h"

#include "salesforcesoap.h"

ModuleHandler::ModuleHandler( const QString &moduleName )
    : mModuleName( moduleName )
{
}

ModuleHandler::~ModuleHandler()
{
}

QString ModuleHandler::moduleName() const
{
    return mModuleName;
}

QStringList ModuleHandler::availableFields() const
{
    return mAvailableFields;
}

void ModuleHandler::setDescriptionResult( const TNS__DescribeSObjectResult &description )
{
    mAvailableFields.clear();

    const QList<TNS__Field> fields = description.fields();
    Q_FOREACH( const TNS__Field &field, fields ) {
        mAvailableFields << field.name();
    }

    kDebug() << "Module" << moduleName() << "has" << mAvailableFields.count() << "fields:"
             << mAvailableFields;
}
