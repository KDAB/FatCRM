#ifndef LEADDETAILS_H
#define LEADDETAILS_H

#include "editcalendarbutton.h"

#include <kdcrmdata/sugarlead.h>

#include <QtGui/QWidget>
#include <QComboBox>
#include <QGroupBox>
#include <QTextEdit>
#include <QCheckBox>


class LeadDetails : public QWidget
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
    void initialize();
    QGroupBox *buildDetailsGroupBox();
    QGroupBox *buildOtherDetailsGroupBox();
    QGroupBox *buildAddressesGroupBox();
    QStringList sourceItems() const;
    QStringList salutationItems() const;
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

