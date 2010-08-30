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
    void removeAccountData( const QString &accountName );
    void addCampaignData( const QString &name,  const QString &id );
    void removeCampaignData( const QString &campaignName );
    void addAssignedToData( const QString &name, const QString &id );
    void reset();

    inline QMap<QString, QString> accountData() {return mData;}

Q_SIGNALS:
    void saveItem();
    void modifyItem();

    friend class Page;
private:
    void initialize();
    bool isEditing();

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
    void slotResetCursor( const QString& );
};

#endif /* ACCOUNTDETAILS_H */

