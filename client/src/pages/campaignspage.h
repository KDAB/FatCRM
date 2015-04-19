#ifndef CAMPAIGNSPAGE_H
#define CAMPAIGNSPAGE_H

#include "page.h"

#include <akonadi/item.h>

class CampaignsPage : public Page
{
    Q_OBJECT
public:
    explicit CampaignsPage(QWidget *parent = 0);

    ~CampaignsPage();

protected:
    /*reimp*/ void addItem(const QMap<QString, QString> &data);
private:
    /*reimp*/ QString reportTitle() const;
};

#endif /* CAMPAIGNSPAGE_H */

