#ifndef LEADDETAILS_H
#define LEADDETAILS_H

#include "details.h"

namespace Ui
{
class LeadDetails;
}

class LeadDetails : public Details
{
    Q_OBJECT
public:
    explicit LeadDetails(QWidget *parent = 0);

    ~LeadDetails();

private Q_SLOTS:
    void slotSetBirthDate();
    void slotClearDate();

private:
    Ui::LeadDetails *mUi;

private:
    /*reimp*/ void initialize();
    /*reimp*/ QMap<QString, QString> data(const Akonadi::Item &item) const;
    /*reimp*/ void updateItem(Akonadi::Item &item, const QMap<QString, QString> &data) const;

    QStringList statusItems() const;
};

#endif /* LEADDETAILS_H */

