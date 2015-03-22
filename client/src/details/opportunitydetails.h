#ifndef OPPORTUNITYDETAILS_H
#define OPPORTUNITYDETAILS_H

#include "details.h"

namespace Ui
{
class OpportunityDetails;
}

class OpportunityDetails : public Details
{
    Q_OBJECT
public:
    explicit OpportunityDetails(QWidget *parent = 0);

    ~OpportunityDetails();

private Q_SLOTS:
    void slotClearDate();
    void slotSetDateClosed();

private:
    Ui::OpportunityDetails *mUi;

private:
    /*reimp*/ void initialize();
    /*reimp*/ QMap<QString, QString> data(const Akonadi::Item &item) const;
    /*reimp*/ void updateItem(Akonadi::Item &item, const QMap<QString, QString> &data) const;

    QStringList typeItems() const;
    QStringList stageItems() const;
};

#endif /* OPPORTUNITYDETAILS_H */

