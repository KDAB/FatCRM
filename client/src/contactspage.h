#ifndef CONTACTSPAGE_H
#define CONTACTSPAGE_H

#include "page.h"

#include <akonadi/item.h>


class ContactsPage : public Page
{
    Q_OBJECT
public:
    explicit ContactsPage( QWidget *parent = 0 );

    ~ContactsPage();

protected:
    /*reimp*/ void addItem( QMap<QString, QString> data );
    /*reimp*/ void modifyItem( Akonadi::Item &item, QMap<QString, QString> data );
    /*reimp*/ void setupModel();
};

#endif /* CONTACTSPAGE_H */
