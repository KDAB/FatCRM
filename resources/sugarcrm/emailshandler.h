#ifndef EMAILSHANDLER_H
#define EMAILSHANDLER_H

#include "modulehandler.h"
#include <kdcrmdata/sugaremail.h>

class EmailAccessorPair;

template <typename U, typename V> class QHash;

class EmailsHandler : public ModuleHandler
{
public:
    EmailsHandler(SugarSession *session);

    ~EmailsHandler();

    QStringList supportedFields() const Q_DECL_OVERRIDE;

    Akonadi::Collection handlerCollection() const Q_DECL_OVERRIDE;

    bool setEntry(const Akonadi::Item &item);

    QString queryStringForListing() const Q_DECL_OVERRIDE;
    QString orderByForListing() const Q_DECL_OVERRIDE;
    QStringList selectedFieldsForListing() const Q_DECL_OVERRIDE;

    virtual bool needsExtraInformation() const Q_DECL_OVERRIDE { return true; }
    virtual void getExtraInformation(Akonadi::Item::List &items) Q_DECL_OVERRIDE;

    Akonadi::Item itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry, const Akonadi::Collection &parentCollection) Q_DECL_OVERRIDE;

    void compare(Akonadi::AbstractDifferencesReporter *reporter,
                 const Akonadi::Item &leftItem, const Akonadi::Item &rightItem) Q_DECL_OVERRIDE;

private:
    SugarEmail::AccessorHash mAccessors;
};

#endif

