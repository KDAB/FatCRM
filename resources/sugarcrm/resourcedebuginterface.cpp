#include "resourcedebuginterface.h"
#include "sugarsession.h"
#include "sugarsoap.h"
#include "modulehandler.h"
#include "sugarcrmresource.h"
#include "settings.h"
#include "moduledebuginterface.h"

ResourceDebugInterface::ResourceDebugInterface(SugarCRMResource *resource)
    : QObject(resource), mResource(resource)
{
}

ResourceDebugInterface::~ResourceDebugInterface()
{
}

void ResourceDebugInterface::setKDSoapDebugEnabled(bool enable)
{
    qputenv("KDSOAP_DEBUG", (enable ? "1" : "0"));
}

bool ResourceDebugInterface::kdSoapDebugEnabled() const
{
    const QByteArray value = qgetenv("KDSOAP_DEBUG");
    return value.toInt() != 0;
}

QStringList ResourceDebugInterface::availableModules() const
{
    return Settings::self()->availableModules();
}

QStringList ResourceDebugInterface::supportedModules() const
{
    return mResource->mModuleHandlers->keys();
}

QStringList ResourceDebugInterface::availableFields(const QString &module) const
{
    SugarSession *session = mResource->mSession;
    KDSoapGenerated::Sugarsoap *soap = session->soap();
    const QString sessionId = session->sessionId();

    const KDSoapGenerated::TNS__Module_fields response = soap->get_module_fields(sessionId, module);

    QStringList availableFields;
    const KDSoapGenerated::TNS__Error_value error = response.error();
    if (error.number().isEmpty() || error.number() == QLatin1String("0")) {
        const KDSoapGenerated::TNS__Field_list fieldList = response.module_fields();
        Q_FOREACH (const KDSoapGenerated::TNS__Field &field, fieldList.items()) {
            availableFields << field.name();
        }
        kDebug() << "Got" << availableFields << "fields";
    } else {
        kDebug() << "Got error: number=" << error.number()
                 << "name=" << error.name()
                 << "description=" << error.description();
    }
    return availableFields;
}

int ResourceDebugInterface::getCount(const QString &module) const
{
    SugarSession *session = mResource->mSession;
    KDSoapGenerated::Sugarsoap *soap = session->soap();
    const QString sessionId = session->sessionId();

    // for notes and emails, use this:
    //const QString query = QString("parent_type=\"Opportunities\"");
    const QString query = QString();
    KDSoapGenerated::TNS__Get_entries_count_result response = soap->get_entries_count(sessionId, module, query, 0);
    kDebug() << response.result_count() << "entries";

    // Let's also take a peek at the first entry
    KDSoapGenerated::TNS__Select_fields fields;
    fields.setItems(availableFields(module));
    KDSoapGenerated::TNS__Get_entry_list_result listResponse = soap->get_entry_list(sessionId, module, query, QString() /*orderBy*/, 0, fields, 1 /*maxResults*/, 0 /*fetchDeleted*/);
    QList<KDSoapGenerated::TNS__Name_value> values = listResponse.entry_list().items().at(0).name_value_list().items();
    Q_FOREACH (const KDSoapGenerated::TNS__Name_value &value, values) {
        qDebug() << value.name() << "=" << value.value();
    }

    return response.result_count();
}

#include "resourcedebuginterface.moc"
