#ifndef CONTACTSPAGE_H
#define CONTACTSPAGE_H

#include "page.h"

#include <akonadi/item.h>

class ContactsPage : public Page
{
    Q_OBJECT
public:
    explicit ContactsPage(QWidget *parent = 0);

    ~ContactsPage();

protected:
    /*reimp*/ void addItem(const QMap<QString, QString> &data);
    /*reimp*/ void modifyItem(Akonadi::Item &item, const QMap<QString, QString> &data);
    /*reimp*/ QString reportTitle() const;
};

#endif /* CONTACTSPAGE_H */
