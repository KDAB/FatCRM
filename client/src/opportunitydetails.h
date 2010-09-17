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

    QLineEdit* mDateClosed;
    EditCalendarButton *mCalendarButton;
    QToolButton *mClearDateButton;
};


#endif /* OPPORTUNITYDETAILS_H */

