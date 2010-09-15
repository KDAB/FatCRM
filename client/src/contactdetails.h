#ifndef CONTACTDETAILS_H
#define CONTACTDETAILS_H

#include "editcalendarbutton.h"

#include <kabc/addressee.h>

#include <QtGui>

class ContactDetails : public QWidget
{
    Q_OBJECT
public:
    explicit ContactDetails( QWidget *parent = 0 );

    ~ContactDetails();

private:
    void initialize();
    QGroupBox *buildDetailsGroupBox();
    QGroupBox *buildOtherDetailsGroupBox();
    QGroupBox *buildAddressesGroupBox();
    QStringList salutationItems() const;
    QStringList sourceItems() const;

     // Details
    QGroupBox *mDetailsBox;
    QComboBox *mSalutation;
    QLineEdit *mFirstName;
    QLineEdit *mLastName;
    QLineEdit *mTitle;
    QLineEdit *mDepartment;
    QComboBox *mAccountName;
    QComboBox *mLeadSource;
    QComboBox *mCampaign;
    QComboBox *mAssignedTo;
    QComboBox *mReportsTo;
    QLineEdit *mPrimaryEmail;

    //Other Details
    QGroupBox *mOtherDetailsBox;
    QLineEdit *mOfficePhone;
    QLineEdit *mMobilePhone;
    QLineEdit *mHomePhone;
    QLineEdit *mOtherPhone;
    QLineEdit *mFax;
    QToolButton *mClearDateButton;
    QLineEdit *mBirthDate;
    QLineEdit *mAssistant;
    QLineEdit *mAssistantPhone;
    QCheckBox *mDoNotCall;

    //Addresses
    QGroupBox *mAddressesBox;
    QLineEdit *mPrimaryAddress;
    QLineEdit *mCity;
    QLineEdit *mState;
    QLineEdit *mPostalCode;
    QLineEdit *mCountry;
    QLineEdit *mOtherAddress;
    QLineEdit *mOtherCity;
    QLineEdit *mOtherState;
    QLineEdit *mOtherPostalCode;
    QLineEdit *mOtherCountry;

    QMap<QString, QString> mData;
    EditCalendarButton *mCalendarButton;


private Q_SLOTS:
    void slotSetBirthday();
    void slotClearDate();

};

#endif /* CONTACTDETAILS_H */

