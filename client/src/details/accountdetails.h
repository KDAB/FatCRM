#ifndef ACCOUNTDETAILS_H
#define ACCOUNTDETAILS_H

#include "details.h"

namespace Ui
{
class AccountDetails;
}

class AccountDetails : public Details
{
    Q_OBJECT
public:
    explicit AccountDetails(QWidget *parent = 0);

    ~AccountDetails();

private:
    Ui::AccountDetails *mUi;

private:
    void initialize();
    QMap<QString, QString> data(const Akonadi::Item &item) const Q_DECL_OVERRIDE;
    void updateItem(Akonadi::Item &item, const QMap<QString, QString> &data) const Q_DECL_OVERRIDE;
    void setDataInternal(const QMap<QString, QString> &data) const Q_DECL_OVERRIDE;

    QStringList typeItems() const;
};

#endif /* ACCOUNTDETAILS_H */

