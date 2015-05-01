#ifndef NOTESREPOSITORY_H
#define NOTESREPOSITORY_H

#include <QObject>

#include <akonadi/item.h>
#include "kdcrmdata/sugarnote.h"

class NotesRepository : public QObject
{
    Q_OBJECT
public:
    explicit NotesRepository(QObject *parent = 0);

    void setNotesCollection(const Akonadi::Collection &collection);

    QVector<SugarNote> notesForOpportunity(const QString &id) const;

signals:

private Q_SLOTS:
    void slotNotesReceived(const Akonadi::Item::List &items);

private:
    Akonadi::Collection mNotesCollection;
    typedef QHash<QString, QVector<SugarNote> > NotesHash;
    NotesHash mNotesHash;
};

#endif // NOTESREPOSITORY_H
