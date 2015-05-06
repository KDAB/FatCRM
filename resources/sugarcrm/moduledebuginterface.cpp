#include "moduledebuginterface.h"

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

int ModuleDebugInterface::getCount() const
{
    SugarSession *session = mResource->mSession;
    Sugarsoap *soap = session->soap();
    const QString sessionId = session->sessionId();

    // for notes and emails, use this:
    //const QString query = QString("parent_type=\"Opportunities\"");
    const QString query = QString();
    KDSoapGenerated::TNS__Get_entries_count_result response = soap->get_entries_count(sessionId, mModuleName, query, 0);
    kDebug() << response.result_count() << "entries";

    // Let's also take a peek at the first entry
    KDSoapGenerated::TNS__Select_fields fields;
    fields.setItems(availableFields());
    KDSoapGenerated::TNS__Get_entry_list_result listResponse = soap->get_entry_list(sessionId, mModuleName,  query, QString() /*orderBy*/, 0, fields, 1 /*maxResults*/, 0 /*fetchDeleted*/);
    QList<KDSoapGenerated::TNS__Name_value> values = listResponse.entry_list().items().at(0).name_value_list().items();
    Q_FOREACH (const KDSoapGenerated::TNS__Name_value &value, values) {
        qDebug() << value.name() << "=" << value.value();
    }

    return response.result_count();
}

QStringList ModuleDebugInterface::availableFields() const
{
    if (mAvailableFields.isEmpty()) {
        kDebug() << "Available Fields for " << mModuleName
                 << "not fetch yet, getting them now";

        SugarSession *session = mResource->mSession;
        Sugarsoap *soap = session->soap();
        const QString sessionId = session->sessionId();

        const KDSoapGenerated::TNS__Module_fields response = soap->get_module_fields(sessionId, mModuleName);

        const KDSoapGenerated::TNS__Error_value error = response.error();
        if (error.number().isEmpty() || error.number() == QLatin1String("0")) {
            const KDSoapGenerated::TNS__Field_list fieldList = response.module_fields();
            Q_FOREACH (const KDSoapGenerated::TNS__Field &field, fieldList.items()) {
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
