#ifndef CAMPAIGNDETAILS_H
#define CAMPAIGNDETAILS_H

#include "editcalendarbutton.h"

#include <kdcrmdata/sugarcampaign.h>

#include <QtGui/QWidget>
#include <QGroupBox>
#include <QToolButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>

class CampaignDetails : public QWidget
{
    Q_OBJECT
public:
    explicit CampaignDetails( QWidget *parent = 0 );

    ~CampaignDetails();

private:
    void initialize();
    QGroupBox *buildDetailsGroupBox();
    QGroupBox *buildOtherDetailsGroupBox();
    QStringList statusItems() const;
    QStringList typeItems() const;
    QStringList currencyItems() const;

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

