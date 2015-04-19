#ifndef LEADSPAGE_H
#define LEADSPAGE_H

#include "page.h"

#include <akonadi/item.h>

class LeadsPage : public Page
{
    Q_OBJECT
public:
    explicit LeadsPage(QWidget *parent = 0);

    ~LeadsPage();

protected:
    /*reimp*/ QString reportTitle() const;
};

#endif /* LEADSPAGE_H */

