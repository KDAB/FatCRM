#ifndef NOTESHANDLER_H
#define NOTESHANDLER_H

#include "modulehandler.h"
#include <kdcrmdata/sugarnote.h>

class NoteAccessorPair;

template <typename U, typename V> class QHash;

class NotesHandler : public ModuleHandler
{
public:
    NotesHandler(SugarSession *session);

    ~NotesHandler();

    QStringList supportedFields() const;

    Akonadi::Collection handlerCollection() const;

    bool setEntry(const Akonadi::Item &item);

    QString queryStringForListing() const;
    QString orderByForListing() const;
    QStringList selectedFieldsForListing() const;

    Akonadi::Item itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry, const Akonadi::Collection &parentCollection);

    void compare(Akonadi::AbstractDifferencesReporter *reporter,
                 const Akonadi::Item &leftItem, const Akonadi::Item &rightItem);

private:
    SugarNote::AccessorHash mAccessors;
};

#endif

