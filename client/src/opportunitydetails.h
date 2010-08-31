#ifndef OPPORTUNITYDETAILS_H
#define OPPORTUNITYDETAILS_H

#include "ui_opportunitydetails.h"
#include "abstractdetails.h"
#include "editcalendarbutton.h"

#include <akonadi/item.h>

#include <QtGui/QWidget>

class OpportunityDetails : public AbstractDetails
{
    Q_OBJECT
public:
    explicit OpportunityDetails( QWidget *parent = 0 );

    ~OpportunityDetails();

    /*reimp*/ void setItem( const Akonadi::Item &item );
    /*reimp*/ void clearFields();
    /*reimp*/ void addAccountData( const QString &name,  const QString &id );
    /*reimp*/ void removeAccountData( const QString &accountName );
    /*reimp*/ void addCampaignData( const QString &name,  const QString &id );
    /*reimp*/ void removeCampaignData( const QString &campaignName );
    /*reimp*/ void addAssignedToData( const QString &name, const QString &id );
    /*reimp*/ void reset();
    /*reimp*/ void initialize();

    /*reimp*/ inline QMap<QString, QString> data() {return mData;}


    friend class OpportunitiesPage;
private:
    EditCalendarButton *mCalendarButton;
    QMap<QString, QString> mData; // this
    bool mModifyFlag;
    Ui_OpportunityDetails mUi;

private Q_SLOTS:
    void slotEnableSaving();
    void slotSaveOpportunity();
    void slotSetModifyFlag( bool );
    void slotSetDateClosed();
    void slotClearDate();
};


#endif /* OPPORTUNITYDETAILS_H */

