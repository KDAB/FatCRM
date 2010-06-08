#ifndef OPPORTUNITYDETAILS_H
#define OPPORTUNITYDETAILS_H

#include "ui_opportunitydetails.h"
#include "editcalendarbutton.h"

#include <akonadi/item.h>

#include <QtGui/QWidget>

class OpportunityDetails : public QWidget
{
    Q_OBJECT
public:
    explicit OpportunityDetails( QWidget *parent = 0 );

    ~OpportunityDetails();

    void setItem( const Akonadi::Item &item );
    void clearFields();
    void addAccountData( const QString &name,  const QString &id );
    void addCampaignData( const QString &name,  const QString &id );
    void addAssignedToData( const QString &name, const QString &id );
    void disableGroupBoxes();
    inline QMap<QString, QString> opportunityData() {return mData;}

Q_SIGNALS:
    void saveOpportunity();
    void modifyOpportunity();

private:
    void initialize();

    EditCalendarButton *mCalendarButton;
    QMap<QString, QString> mData; // this
    QMap<QString, QString> mAccountsData;
    QMap<QString, QString> mAssignedToData;
    QMap<QString, QString> mCampaignsData;
    bool mModifyFlag;
    Ui_OpportunityDetails mUi;

private Q_SLOTS:
    void slotEnableSaving();
    void slotSaveOpportunity();
    void slotSetModifyFlag( bool );
    void slotSetDateClosed();
};


#endif /* OPPORTUNITYDETAILS_H */

