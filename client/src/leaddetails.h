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

    // Details
    QGroupBox *mDetailsBox;
    QComboBox *mLeadSource;
    QTextEdit *mLeadSourceDescription;
    QComboBox *mCampaignName;
    QLineEdit *mReferedBy;
    QComboBox *mSalutation;
    QLineEdit *mFirstName;
    QLineEdit *mLastName;
    QLineEdit *mBirthdate;
    QLineEdit *mAccountName;
    QLineEdit *mTitle;
    QLineEdit *mDepartment;
    QComboBox *mAssignedUserName;

    //Other Details
    QGroupBox *mOtherDetailsBox;
    QComboBox *mStatus;
    QTextEdit *mStatusDescription;
    QLineEdit *mOpportunityAmount;
    QLineEdit *mPhoneWork;
    QLineEdit *mPhoneMobile;
    QLineEdit *mPhoneHome;
    QLineEdit *mPhoneOther;
    QLineEdit *mPhoneFax;
    QCheckBox *mDoNotCall;
    QLineEdit *mEmail1;
    QLineEdit *mEmail2;

    //Addresses
    QGroupBox *mAddressesBox;
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
    QToolButton *mClearDateButton;
};

#endif /* LEADDETAILS_H */

