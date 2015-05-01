#ifndef OPPORTUNITIESPAGE_H
#define OPPORTUNITIESPAGE_H

#include "page.h"

class OpportunitiesPage : public Page
{
    Q_OBJECT
public:
    explicit OpportunitiesPage(QWidget *parent = 0);

    ~OpportunitiesPage();

    /*reimp*/ void setupModel();

protected:
    /*reimp*/ QString reportTitle() const;
};

#endif /* OPPORTUNITIESPAGE_H */

