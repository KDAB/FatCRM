#ifndef CONTACTDETAILS_H
#define CONTACTDETAILS_H

#include "ui_contactdetails.h"
#include "editcalendarbutton.h"

#include <akonadi/item.h>

#include <QtGui/QWidget>


class ContactDetails : public QWidget
{
    Q_OBJECT
public:
    explicit ContactDetails( QWidget *parent = 0 );

    ~ContactDetails();

    void setItem( const Akonadi::Item &item );
    void clearFields();
    void addAccountData( const QString &name, const QString &id );
    void addCampaignData( const QString &name,  const QString &id );
    void addReportsToData( const QString &name, const QString &id );
    void addAssignedToData( const QString &name, const QString &id );
    void fillCombos();
    void disableGroupBoxes();
    inline QMap<QString, QString> contactData() {return mContactData;}

Q_SIGNALS:
    void saveContact();
    void modifyContact();

private:
    void initialize();

    EditCalendarButton *mCalendarButton;
    QMap<QString, QString> mReportsToData;
    QMap<QString, QString> mAssignedToData;
    QMap<QString, QString> mCampaignsData;
    QMap<QString, QString> mAccountsData;
    QMap<QString, QString> mContactData;
    bool mModifyFlag;
    Ui_ContactDetails mUi;

private Q_SLOTS:
    void slotEnableSaving();
    void slotSaveContact();
    void slotSetBirthday();
    void slotSetModifyFlag( bool );
};

#endif /* CONTACTDETAILS_H */

