#ifndef ACCOUNTSPAGE_H
#define ACCOUNTSPAGE_H

#include "page.h"

#include <akonadi/item.h>


class AccountsPage : public Page
{
    Q_OBJECT
public:
    explicit AccountsPage( QWidget *parent = 0 );

    ~AccountsPage();

protected:
    /*reimp*/ void addItem();
    /*reimp*/ void modifyItem();
    /*reimp*/ void setupModel();

private:
    void updateAccountCombo( const QString& name, const QString& id );
    void addAccountsData();
    void removeAccountsData( const Akonadi::Item &item);
};
#endif /* ACCOUNTSPAGE_H */

