#ifndef ACCOUNTDETAILS_H
#define ACCOUNTDETAILS_H

#include "ui_accountdetails.h"

#include <akonadi/item.h>

#include <QtGui/QWidget>
#include <QToolButton>


class AccountDetails : public QWidget
{
    Q_OBJECT
public:
    explicit AccountDetails( QWidget *parent = 0 );

    ~AccountDetails();

    void setItem( const Akonadi::Item &item );
    void clearFields();
    void addAccountData( const QString &name,  const QString &id );
    void addCampaignData( const QString &name,  const QString &id );
    void addAssignedToData( const QString &name, const QString &id );
    void fillCombos();
    void disableGroupBoxes();
    inline QMap<QString, QString> accountData() {return mData;}

Q_SIGNALS:
    void saveAccount();
    void modifyAccount();

private:
    void initialize();
    QMap<QString, QString> mData; // this
    QMap<QString, QString> mAccountsData;
    QMap<QString, QString> mAssignedToData;
    QMap<QString, QString> mCampaignsData;
    bool mModifyFlag;
    Ui_AccountDetails mUi;

private Q_SLOTS:
    void slotEnableSaving();
    void slotSaveAccount();
    void slotSetModifyFlag( bool );
};

#endif /* ACCOUNTDETAILS_H */

