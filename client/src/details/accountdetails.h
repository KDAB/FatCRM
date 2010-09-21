#ifndef ACCOUNTDETAILS_H
#define ACCOUNTDETAILS_H

#include "details.h"

class AccountDetails : public Details
{
    Q_OBJECT
public:
    explicit AccountDetails( QWidget *parent = 0 );

    ~AccountDetails();

private:
    /*reimp*/ void initialize();
    /*reimp*/ QMap<QString, QString> data( const Akonadi::Item &item ) const;

    QGroupBox *buildDetailsGroupBox();
    QGroupBox *buildOtherDetailsGroupBox();
    QGroupBox *buildAddressesGroupBox();

    QStringList typeItems() const;
};

#endif /* ACCOUNTDETAILS_H */

