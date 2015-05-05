#ifndef OPPORTUNITIESPAGE_H
#define OPPORTUNITIESPAGE_H

#include "page.h"

class OpportunitiesPage : public Page
{
    Q_OBJECT
public:
    explicit OpportunitiesPage(QWidget *parent = 0);

    ~OpportunitiesPage();

    enum {
        NextStepDateColumn = 5,
        LastModifiedColumn = 6
    };

    /*reimp*/ void setupModel();

protected:
    /*reimp*/ QString reportTitle() const;
};

#endif /* OPPORTUNITIESPAGE_H */

