#ifndef OPPORTUNITYDETAILS_H
#define OPPORTUNITYDETAILS_H

#include "details.h"
#include "editcalendarbutton.h"

#include <kdcrmdata/sugaropportunity.h>

class OpportunityDetails : public Details
{
    Q_OBJECT
public:
    explicit OpportunityDetails( QWidget *parent = 0 );

    ~OpportunityDetails();

private Q_SLOTS:
    void slotClearDate();
    void slotSetDateClosed();

private:
    /*reimp*/ void initialize();
    /*reimp*/ QMap<QString, QString> data( const Akonadi::Item item ) const;

    QGroupBox *buildDetailsGroupBox();
    QGroupBox *buildOtherDetailsGroupBox();
    QStringList typeItems() const;
    QStringList stageItems() const;

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

