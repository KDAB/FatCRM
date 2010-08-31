#ifndef ACCOUNTDETAILS_H
#define ACCOUNTDETAILS_H

#include "ui_accountdetails.h"
#include "abstractdetails.h"

#include <akonadi/item.h>

#include <QtGui/QWidget>
#include <QToolButton>


class AccountDetails : public AbstractDetails
{
    Q_OBJECT
public:
    explicit AccountDetails( QWidget *parent = 0 );

    ~AccountDetails();

    friend class CampaignsPage;
    friend class Page;
protected:
    /*reimp*/ void initialize();
    /*reimp*/ void setItem( const Akonadi::Item &item );
    /*reimp*/ void clearFields();
    /*reimp*/ void addData( const QString &name,  const QString &id );
    /*reimp*/ void removeData( const QString &Name );
    /*reimp*/ void reset();
    /*reimp*/ void addAccountData( const QString &name,  const QString &id );
    /*reimp*/ void removeAccountData( const QString &name );
    /*reimp*/ void addCampaignData( const QString &name,  const QString &id );
    /*reimp*/ void removeCampaignData( const QString &name );
    /*reimp*/ void addAssignedToData( const QString &name, const QString &id );

    /*reimp*/ inline QMap<QString, QString> data() {return mData;}

private:
    QMap<QString, QString> mData; // this
    bool mModifyFlag;
    Ui_AccountDetails mUi;

private Q_SLOTS:
    void slotEnableSaving();
    void slotSaveAccount();
    void slotSetModifyFlag( bool );
};

#endif /* ACCOUNTDETAILS_H */

