#ifndef LEADDETAILS_H
#define LEADDETAILS_H

#include "ui_leaddetails.h"
#include "abstractdetails.h"
#include "editcalendarbutton.h"

#include <akonadi/item.h>

#include <QtGui/QWidget>
#include <QToolButton>


class LeadDetails : public AbstractDetails
{
    Q_OBJECT
public:
    explicit LeadDetails( QWidget *parent = 0 );

    ~LeadDetails();


    /*reimp*/ void setItem( const Akonadi::Item &item );
    /*reimp*/ void clearFields();
    /*reimp*/ void addCampaignData( const QString &name,  const QString &id );
    /*reimp*/ void removeCampaignData( const QString &campaignName );
    /*reimp*/ void addAssignedToData( const QString &name, const QString &id );
    /*reimp*/ void reset();
    /*reimp*/ void initialize();

    /*reimp*/ inline QMap<QString, QString> data() {return mData;}

    friend class LeadsPage;
private:
    EditCalendarButton *mCalendarButton;
    QMap<QString, QString> mData; // this
    bool mModifyFlag;
    Ui_LeadDetails mUi;

private Q_SLOTS:
    void slotEnableSaving();
    void slotSaveLead();
    void slotSetModifyFlag( bool );
    void slotCopyFromPrimary( bool );
    void slotSetBirthDate();
    void slotClearDate();
};

#endif /* LEADDETAILS_H */

