#ifndef ACCOUNTDETAILS_H
#define ACCOUNTDETAILS_H

#include "details.h"

namespace Ui {
    class AccountDetails;
}

class AccountDetails : public Details
{
    Q_OBJECT
public:
    explicit AccountDetails( QWidget *parent = 0 );

    ~AccountDetails();

private:
    Ui::AccountDetails *mUi;

private:
    /*reimp*/ void initialize();
    /*reimp*/ QMap<QString, QString> data( const Akonadi::Item &item ) const;
    /*reimp*/ void updateItem( Akonadi::Item &item, const QMap<QString, QString> &data ) const;

    QStringList typeItems() const;
};

#endif /* ACCOUNTDETAILS_H */

