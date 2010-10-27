#ifndef CONTACTDETAILS_H
#define CONTACTDETAILS_H

#include "details.h"

namespace KABC {
    class Addressee;
}

namespace Ui {
    class ContactDetails;
}

class ContactDetails : public Details
{
    Q_OBJECT
public:
    explicit ContactDetails( QWidget *parent = 0 );

    ~ContactDetails();

private:
    Ui::ContactDetails *mUi;

private:
    /*reimp*/ void initialize();
    /*reimp*/ QMap<QString, QString> data( const Akonadi::Item &item ) const;
    /*reimp*/ void updateItem( Akonadi::Item &item, const QMap<QString, QString> &data ) const;

    QMap<QString, QString> contactData( const KABC::Addressee &contact ) const;

private Q_SLOTS:
    void slotSetBirthday();
    void slotClearDate();
};

#endif /* CONTACTDETAILS_H */

