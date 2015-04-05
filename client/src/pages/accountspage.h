#ifndef ACCOUNTSPAGE_H
#define ACCOUNTSPAGE_H

#include "page.h"

#include <akonadi/item.h>

class AccountsPage : public Page
{
    Q_OBJECT
public:
    explicit AccountsPage(QWidget *parent = 0);

    ~AccountsPage();

protected:
    /*reimp*/ QString reportTitle() const;
    /*reimp*/ void addItem(const QMap<QString, QString> &data);
    /*reimp*/ void modifyItem(Akonadi::Item &item, const QMap<QString, QString> &data);

};
#endif /* ACCOUNTSPAGE_H */

