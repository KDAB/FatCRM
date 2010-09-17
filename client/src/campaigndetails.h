#ifndef CAMPAIGNDETAILS_H
#define CAMPAIGNDETAILS_H

#include "details.h"
#include "editcalendarbutton.h"

class CampaignDetails : public Details
{
    Q_OBJECT
public:
    explicit CampaignDetails( QWidget *parent = 0 );

    ~CampaignDetails();

private:
    /*reimp*/ void initialize();
    /*reimp*/ QMap<QString, QString> data( const Akonadi::Item item ) const;

    QGroupBox *buildDetailsGroupBox();
    QGroupBox *buildOtherDetailsGroupBox();

    QStringList typeItems() const;
    QStringList statusItems() const;


    // Details
    QGroupBox *mDetailsBox;
    QLineEdit *mName;
    QComboBox *mStatus;
    QLineEdit *mStartDate;
    QToolButton *mClearStartDateButton;
    QLineEdit *mEndDate;
    QToolButton *mClearEndDateButton;
    QComboBox *mCampaignType;
    QComboBox *mCurrency;
    QLineEdit *mBudget;
    QLineEdit *mExpectedRevenue;

    //Other Details
    QGroupBox *mOtherDetailsBox;
    QComboBox *mAssignedUserName;
    QLineEdit *mImpressions;
    QLineEdit *mActualCost;
    QLineEdit *mExpectedCost;
    QTextEdit *mObjective;

    EditCalendarButton *mStartDateCalendarButton;
    EditCalendarButton *mEndDateCalendarButton;

    QMap<QString, QString> mData;

private Q_SLOTS:
    void slotSetStartDate();
    void slotSetEndDate();
    void slotClearDate();
};


#endif /* CAMPAIGNDETAILS_H */

