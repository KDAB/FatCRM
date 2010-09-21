#ifndef CONTACTDETAILS_H
#define CONTACTDETAILS_H

#include "details.h"
#include "editcalendarbutton.h"

#include <kabc/addressee.h>

class ContactDetails : public Details
{
    Q_OBJECT
public:
    explicit ContactDetails( QWidget *parent = 0 );

    ~ContactDetails();

private:
    /*reimp*/ void initialize();
    /*reimp*/ QMap<QString, QString> data( const Akonadi::Item &item ) const;

    QGroupBox *buildDetailsGroupBox();
    QGroupBox *buildOtherDetailsGroupBox();
    QGroupBox *buildAddressesGroupBox();
    QMap<QString, QString> contactData( const KABC::Addressee &contact ) const;

    QLineEdit* mBirthDate;
    QToolButton *mClearDateButton;
    EditCalendarButton *mCalendarButton;


private Q_SLOTS:
    void slotSetBirthday();
    void slotClearDate();

};

#endif /* CONTACTDETAILS_H */

