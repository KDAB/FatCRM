#ifndef CONTACTDETAILS_H
#define CONTACTDETAILS_H

#include "ui_contactdetails.h"
#include "abstractdetails.h"
#include "editcalendarbutton.h"

#include <akonadi/item.h>

#include <QtGui/QWidget>


class ContactDetails : public AbstractDetails
{
    Q_OBJECT
public:
    explicit ContactDetails( QWidget *parent = 0 );

    ~ContactDetails();

    friend class ContactsPage;
    friend class CampaignsPage;
protected:
    /*reimp*/ void setItem( const Akonadi::Item &item );
    /*reimp*/ void clearFields();
    /*reimp*/ void addAccountData( const QString &name, const QString &id );
    /*reimp*/ void removeAccountData( const QString &accountName );
    /*reimp*/ void addCampaignData( const QString &name,  const QString &id );
    /*reimp*/ void removeCampaignData( const QString &campaignName );
    /*reimp*/ void addReportsToData( const QString &name, const QString &id );
    /*reimp*/ void addAssignedToData( const QString &name, const QString &id );
    /*reimp*/ void reset();
    /*reimp*/ void initialize();

    /*reimp*/ inline QMap<QString, QString> data() {return mData;}

private:
    QMap<QString, QString> mData;
    EditCalendarButton *mCalendarButton;
    bool mModifyFlag;
    Ui_ContactDetails mUi;

private Q_SLOTS:
    void slotEnableSaving();
    void slotSaveContact();
    void slotSetBirthday();
    void slotSetModifyFlag( bool );
    void slotClearDate();

};

#endif /* CONTACTDETAILS_H */

