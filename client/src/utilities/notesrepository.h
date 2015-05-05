#ifndef NOTESREPOSITORY_H
#define NOTESREPOSITORY_H

#include <QObject>

#include <akonadi/item.h>
#include "kdcrmdata/sugarnote.h"
#include "kdcrmdata/sugaremail.h"

class NotesRepository : public QObject
{
    Q_OBJECT
public:
    explicit NotesRepository(QObject *parent = 0);

    void setNotesCollection(const Akonadi::Collection &collection);
    void setEmailsCollection(const Akonadi::Collection &collection);

    QVector<SugarNote> notesForOpportunity(const QString &id) const;
    QVector<SugarEmail> emailsForOpportunity(const QString &id) const;

signals:

private Q_SLOTS:
    void slotNotesReceived(const Akonadi::Item::List &items);
    void slotEmailsReceived(const Akonadi::Item::List &items);

private:
    Akonadi::Collection mNotesCollection;
    typedef QHash<QString, QVector<SugarNote> > NotesHash;
    NotesHash mNotesHash;

    Akonadi::Collection mEmailsCollection;
    typedef QHash<QString, QVector<SugarEmail> > EmailsHash;
    EmailsHash mEmailsHash;
};

#endif // NOTESREPOSITORY_H
