#ifndef OPPORTUNITYDETAILS_H
#define OPPORTUNITYDETAILS_H

#include "editcalendarbutton.h"

#include <kdcrmdata/sugaropportunity.h>

#include <QWidget>
#include <QGroupBox>
#include <QComboBox>
#include <QLineEdit>


class OpportunityDetails : public QWidget
{
    Q_OBJECT
public:
    explicit OpportunityDetails( QWidget *parent = 0 );

    ~OpportunityDetails();

private Q_SLOTS:
    void slotClearDate();
    void slotSetDateClosed();

private:
    void initialize();
    QGroupBox *buildDetailsGroupBox();
    QGroupBox *buildOtherDetailsGroupBox();
    QStringList typeItems() const;
    QStringList sourceItems() const;
    QStringList stageItems() const;
    QStringList currencyItems() const;

    // Details
    QGroupBox *mDetailsBox;
    QLineEdit *mName;
    QComboBox *mAccountName;
    QComboBox *mOpportunityType;
    QComboBox *mLeadSource;
    QComboBox *mCampaignName;
    QComboBox *mSalesStage;
    QComboBox *mAssignedUserName;

    //Other Details
    QGroupBox *mOtherDetailsBox;
    QComboBox *mCurrency;
    QLineEdit *mAmount;
    QLineEdit *mDateClosed;
    QLineEdit *mNextStep;
    QLineEdit *mProbability;

    EditCalendarButton *mCalendarButton;
    QToolButton *mClearDateButton;
};


#endif /* OPPORTUNITYDETAILS_H */

