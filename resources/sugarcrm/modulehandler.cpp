#include "modulehandler.h"

#include "sugarsession.h"
#include "sugarsoap.h"
#include "listentriesscope.h"

using namespace KDSoapGenerated;
#include "kdcrmdata/kdcrmutils.h"

#include <KLocale>

#include <QInputDialog>
#include <QStringList>

ModuleHandler::ModuleHandler(const QString &moduleName, SugarSession *session)
    : mSession(session), mModuleName(moduleName)
{
    Q_ASSERT(mSession != 0);
}

ModuleHandler::~ModuleHandler()
{
}

QString ModuleHandler::moduleName() const
{
    return mModuleName;
}

QString ModuleHandler::latestTimestamp() const
{
    return mLatestTimestamp;
}

void ModuleHandler::resetLatestTimestamp()
{
    mLatestTimestamp = QString();
}

Akonadi::Collection ModuleHandler::collection() const
{
    Akonadi::Collection coll = handlerCollection();
    coll.setRemoteId(moduleName());
    return coll;
}

void ModuleHandler::getEntriesCount(const ListEntriesScope &scope)
{
    const QString query = scope.query(queryStringForListing(), mModuleName.toLower());
    soap()->asyncGet_entries_count(sessionId(), moduleName(), query, scope.deleted());
}

void ModuleHandler::listEntries(const ListEntriesScope &scope)
{
    const QString query = scope.query(queryStringForListing(), mModuleName.toLower());
    const QString orderBy = orderByForListing();
    const int offset = scope.offset();
    const int maxResults = 100;
    const int fetchDeleted = scope.deleted();

    KDSoapGenerated::TNS__Select_fields selectedFields;
    selectedFields.setItems(selectedFieldsForListing());

    soap()->asyncGet_entry_list(sessionId(), moduleName(), query, orderBy, offset, selectedFields, maxResults, fetchDeleted);
}

bool ModuleHandler::getEntry(const Akonadi::Item &item)
{
    if (item.remoteId().isEmpty()) {
        kError() << "Item remoteId is empty. id=" << item.id();
        return false;
    }

    KDSoapGenerated::TNS__Select_fields selectedFields;
    selectedFields.setItems(supportedFields());

    soap()->asyncGet_entry(sessionId(), mModuleName, item.remoteId(), selectedFields);
    return true;
}

Akonadi::Item::List ModuleHandler::itemsFromListEntriesResponse(const KDSoapGenerated::TNS__Entry_list &entryList, const Akonadi::Collection &parentCollection)
{
    Akonadi::Item::List items;

    Q_FOREACH (const KDSoapGenerated::TNS__Entry_value &entry, entryList.items()) {
        const Akonadi::Item item = itemFromEntry(entry, parentCollection);
        if (!item.remoteId().isEmpty()) {
            items << item;
            if (mLatestTimestamp.isNull() || item.remoteRevision() > mLatestTimestamp) {
                mLatestTimestamp = item.remoteRevision();
            }
        }
    }

    return items;
}

bool ModuleHandler::needBackendChange(const Akonadi::Item &item, const QSet<QByteArray> &modifiedParts) const
{
    Q_UNUSED(item);

    return modifiedParts.contains(partIdFromPayloadPart(Akonadi::Item::FullPayload));
}

QString ModuleHandler::formatDate(const QString &dateString)
{
    return KDCRMUtils::formatTimestamp(dateString);
}

QByteArray ModuleHandler::partIdFromPayloadPart(const char *part)
{
    return QByteArray("PLD:") + part;
}

QString ModuleHandler::sessionId() const
{
    return mSession->sessionId();
}

Sugarsoap *ModuleHandler::soap() const
{
    return mSession->soap();
}
