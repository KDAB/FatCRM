#ifndef CONTACTSPAGE_H
#define CONTACTSPAGE_H

#include "ui_contactspage.h"

#include <akonadi/collection.h>

#include <QtGui/QWidget>

namespace Akonadi
{
    class ChangeRecorder;
    class Item;
}

class KJob;
class QModelIndex;

class ContactsPage : public QWidget
{
    Q_OBJECT
public:
    explicit ContactsPage( QWidget *parent = 0 );

    ~ContactsPage();

Q_SIGNALS:
    void contactItemChanged(const Akonadi::Item &item );

private:
    void initialize();
    Ui_ContactsPage mUi;

    Akonadi::ChangeRecorder *mChangeRecorder;
    Akonadi::Collection mContactsCollection;

private Q_SLOTS:
    void slotResourceSelectionChanged( const QByteArray &identifier );
    void slotCollectionFetchResult( KJob *job );
    void slotContactChanged( const Akonadi::Item &item );
    void slotNewContactClicked();
    void slotFilterChanged( const QString& text );
};

#endif
