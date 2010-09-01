#ifndef CAMPAIGNDETAILS_H
#define CAMPAIGNDETAILS_H

#include "ui_campaigndetails.h"
#include "abstractdetails.h"
#include "editcalendarbutton.h"

#include <akonadi/item.h>

#include <QtGui/QWidget>
#include <QToolButton>


class CampaignDetails : public AbstractDetails
{
    Q_OBJECT
public:
    explicit CampaignDetails( QWidget *parent = 0 );

    ~CampaignDetails();

friend class Page;
protected:
    /*reimp*/ void setItem( const Akonadi::Item &item );
    /*reimp*/ void clearFields();
    /*reimp*/ void addAssignedToData( const QString &name, const QString &id );
    /*reimp*/ void reset();
    /*reimp*/ void initialize();

    /*reimp*/ inline QMap<QString, QString> data() {return mData;}


private:
    EditCalendarButton *mStartDateCalendarButton;
    EditCalendarButton *mEndDateCalendarButton;
    QMap<QString, QString> mData; // this
    bool mModifyFlag;

    Ui_CampaignDetails mUi;

private Q_SLOTS:
    void slotEnableSaving();
    void slotSaveCampaign();
    void slotSetModifyFlag( bool );
    void slotSetStartDate();
    void slotSetEndDate();
    void slotClearDate();
};


#endif /* CAMPAIGNDETAILS_H */

