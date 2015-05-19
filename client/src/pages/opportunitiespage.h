#ifndef OPPORTUNITIESPAGE_H
#define OPPORTUNITIESPAGE_H

#include "page.h"

class OpportunitiesPage : public Page
{
    Q_OBJECT
public:
    explicit OpportunitiesPage(QWidget *parent = 0);

    ~OpportunitiesPage();

    void setupModel() Q_DECL_OVERRIDE;

protected:
    QString reportTitle() const Q_DECL_OVERRIDE;
    QMap<QString, QString> dataForNewObject() Q_DECL_OVERRIDE;
};

#endif /* OPPORTUNITIESPAGE_H */

