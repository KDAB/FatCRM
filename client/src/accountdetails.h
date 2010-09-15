#ifndef ACCOUNTDETAILS_H
#define ACCOUNTDETAILS_H

#include <kdcrmdata/sugaraccount.h>


#include <QtGui/QWidget>
#include <QComboBox>
#include <QGroupBox>

class AccountDetails : public QWidget
{
    Q_OBJECT
public:
    explicit AccountDetails( QWidget *parent = 0 );

    ~AccountDetails();

private:
    void initialize();
    QGroupBox *buildDetailsGroupBox();
    QGroupBox *buildOtherDetailsGroupBox();
    QGroupBox *buildAddressesGroupBox();
    QStringList industryItems() const;
    QStringList typeItems() const;

    // Details
    QGroupBox *mDetailsBox;
    QLineEdit *mName;
    QLineEdit *mWebsite;
    QLineEdit *mTyckerSymbol;
    QComboBox *mParentName;
    QLineEdit *mOwnership;
    QComboBox *mIndustry;
    QComboBox *mAccountType;
    QComboBox *mCampaignName;
    QComboBox *mAssignedUserName;

    //Other Details
    QGroupBox *mOtherDetailsBox;
    QLineEdit *mPhoneOffice;
    QLineEdit *mPhoneFax;
    QLineEdit *mPhoneAlternate;
    QLineEdit *mEmployees;
    QLineEdit *mRating;
    QLineEdit *mSicCode;
    QLineEdit *mAnnualRevenue;
    QLineEdit *mEmail1;

    //Addresses
    QGroupBox *mAddressesBox;
    QLineEdit *mBillingAddressStreet;
    QLineEdit *mBillingAddressCity;
    QLineEdit *mBillingAddressState;
    QLineEdit *mBillingAddressPostalCode;
    QLineEdit *mBillingAddressCountry;
    QLineEdit *mShippingAddressStreet;
    QLineEdit *mShippingAddressCity;
    QLineEdit *mShippingAddressState;
    QLineEdit *mShippingAddressPostalCode;
    QLineEdit *mShippingAddressCountry;

};

#endif /* ACCOUNTDETAILS_H */

