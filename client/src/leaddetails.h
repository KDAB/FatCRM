#ifndef LEADDETAILS_H
#define LEADDETAILS_H

#include "details.h"
#include "editcalendarbutton.h"

class LeadDetails : public Details
{
    Q_OBJECT
public:
    explicit LeadDetails( QWidget *parent = 0 );

    ~LeadDetails();

private Q_SLOTS:
    void slotCopyFromPrimary( bool );
    void slotSetBirthDate();
    void slotClearDate();

private:
    /*reimp*/ void initialize();
    /*reimp*/ QMap<QString, QString> data( const Akonadi::Item item ) const;

    QGroupBox *buildDetailsGroupBox();
    QGroupBox *buildOtherDetailsGroupBox();
    QGroupBox *buildAddressesGroupBox();
    QStringList statusItems() const;

    //Addresses
    QLineEdit *mPrimaryAddressStreet;
    QLineEdit *mPrimaryAddressCity;
    QLineEdit *mPrimaryAddressState;
    QLineEdit *mPrimaryAddressPostalcode;
    QLineEdit *mPrimaryAddressCountry;
    QLineEdit *mAltAddressStreet;
    QLineEdit *mAltAddressCity;
    QLineEdit *mAltAddressState;
    QLineEdit *mAltAddressPostalcode;
    QLineEdit *mAltAddressCountry;
    QCheckBox *mCopyAddressFromPrimary;

    EditCalendarButton *mCalendarButton;
    QLineEdit* mBirthdate;
    QToolButton *mClearDateButton;
};

#endif /* LEADDETAILS_H */

