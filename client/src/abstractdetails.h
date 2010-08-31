#ifndef ABSTRACTDETAILS_H
#define ABSTRACTDETAILS_H

#include <akonadi/item.h>
#include <QtGui/QWidget>

class AbstractDetails : public QWidget
{
    Q_OBJECT
public:
    explicit AbstractDetails( QWidget * parent = 0 );
    ~AbstractDetails();

    inline bool isEditing() { return mEditing; }

Q_SIGNALS:
    void saveItem();
    void modifyItem();

    friend class CampaignsPage;
    friend class Page;
protected:
    inline QMap<QString, QString> accountsData() { return mAccountsData; }
    inline QMap<QString, QString> campaignsData() { return mCampaignsData; }
    inline QMap<QString, QString> assignedToData() { return mAssignedToData; }
    inline QMap<QString, QString> reportsToData() { return mReportsToData; }

    inline void setEditing( bool value ) { mEditing = value; }

    virtual void setItem( const Akonadi::Item &item ) = 0;
    virtual void clearFields() = 0;
    virtual void reset() = 0;
    virtual void initialize() = 0;
    virtual QMap<QString, QString> data() = 0;

    virtual void addAccountData( const QString &name,  const QString &id );
    virtual void removeAccountData( const QString &name );
    virtual void addCampaignData( const QString &name,  const QString &id );
    virtual void removeCampaignData( const QString &name );
    virtual void addAssignedToData( const QString &name, const QString &id );
    virtual void addReportsToData( const QString &name, const QString &id );

protected Q_SLOTS:
    void slotResetCursor( const QString& );

private:
    QMap<QString, QString> mReportsToData;
    QMap<QString, QString> mAccountsData;
    QMap<QString, QString> mCampaignsData;
    QMap<QString, QString> mAssignedToData;
    bool mEditing;
};

#endif /* ABSTRACTDETAILS_H */

