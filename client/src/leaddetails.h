#ifndef LEADDETAILS_H
#define LEADDETAILS_H

#include "ui_leaddetails.h"
#include "editcalendarbutton.h"

#include <akonadi/item.h>

#include <QtGui/QWidget>
#include <QToolButton>


class LeadDetails : public QWidget
{
    Q_OBJECT
public:
    explicit LeadDetails( QWidget *parent = 0 );

    ~LeadDetails();

    void setItem( const Akonadi::Item &item );
    void clearFields();
    void addCampaignData( const QString &name,  const QString &id );
    void removeCampaignData( const QString &campaignName );
    void addAssignedToData( const QString &name, const QString &id );
    void reset();

    inline QMap<QString, QString> leadData() {return mData;}

Q_SIGNALS:
    void saveLead();
    void modifyLead();

    friend class LeadsPage;
private:
    void initialize();

    EditCalendarButton *mCalendarButton;
    QMap<QString, QString> mData; // this
    QMap<QString, QString> mAssignedToData;
    QMap<QString, QString> mCampaignsData;
    bool mModifyFlag;
    Ui_LeadDetails mUi;

private Q_SLOTS:
    void slotEnableSaving();
    void slotSaveLead();
    void slotSetModifyFlag( bool );
    void slotCopyFromPrimary( bool );
    void slotSetDateClosed();
};

#endif /* LEADDETAILS_H */

