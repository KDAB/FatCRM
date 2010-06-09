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

     void addAccountsData();

Q_SIGNALS:
    void contactItemChanged();
    void statusMessage( const QString& );
    void showDetails();

public Q_SLOTS:
    void syncronize();

private:
    void initialize();
    void setupCachePolicy();

    Akonadi::ChangeRecorder *mChangeRecorder;
    Akonadi::Collection mContactsCollection;
    Ui_ContactsPage mUi;

private Q_SLOTS:
    void slotResourceSelectionChanged( const QByteArray &identifier );
    void slotCollectionFetchResult( KJob *job );
    void slotContactChanged( const Akonadi::Item &item );
    void slotNewContactClicked();
    void slotAddContact();
    void slotModifyContact();
    void slotRemoveContact();
    void slotSetCurrent( const QModelIndex&,int,int );
    void cachePolicyJobCompleted( KJob* );
};

#endif
