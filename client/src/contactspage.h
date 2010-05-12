#ifndef CONTACTSPAGE_H
#define CONTACTSPAGE_H

#include "ui_contactspage.h"
#include <contactdetails.h>

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
    void contactItemChanged();
    void statusMessage( const QString& );

public Q_SLOTS:
    void syncronize();

private:
    void initialize();
    void setupCachePolicy();

    ContactDetails *mDetailsWidget;
    Akonadi::ChangeRecorder *mChangeRecorder;
    Akonadi::Collection mContactsCollection;
    Akonadi::Item mCurrentItem;
    Ui_ContactsPage mUi;

private Q_SLOTS:
    void slotResourceSelectionChanged( const QByteArray &identifier );
    void slotCollectionFetchResult( KJob *job );
    void slotContactChanged( const Akonadi::Item &item );
    void slotNewContactClicked();
    void slotFilterChanged( const QString& text );
    void slotSearchItem( const QString& text );
    void slotAddContact();
    void slotModifyContact();
    void slotRemoveContact();
    void slotSetCurrent( const QModelIndex&,int,int );
    void cachePolicyJobCompleted( KJob* );
};

#endif
