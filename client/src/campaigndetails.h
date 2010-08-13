#ifndef CAMPAIGNDETAILS_H
#define CAMPAIGNDETAILS_H

#include "ui_campaigndetails.h"
#include "editcalendarbutton.h"

#include <akonadi/item.h>

#include <QtGui/QWidget>
#include <QToolButton>


class CampaignDetails : public QWidget
{
    Q_OBJECT
public:
    explicit CampaignDetails( QWidget *parent = 0 );

    ~CampaignDetails();

    void setItem( const Akonadi::Item &item );
    void clearFields();
    void addCampaignData( const QString &name,  const QString &id );
    void removeCampaignData( const QString &campaignName );
    void addAssignedToData( const QString &name, const QString &id );
    void reset();

    inline QMap<QString, QString> campaignData() {return mData;}
    inline  QMap<QString, QString> campaignsData() {return mCampaignsData;}

Q_SIGNALS:
    void saveCampaign();
    void modifyCampaign();

    friend class CampaignsPage;
private:
    void initialize();
    bool isEditing();

    EditCalendarButton *mStartDateCalendarButton;
    EditCalendarButton *mEndDateCalendarButton;
    QMap<QString, QString> mData; // this
    QMap<QString, QString> mCampaignsData;
    QMap<QString, QString> mAssignedToData;
    bool mModifyFlag;

    Ui_CampaignDetails mUi;

private Q_SLOTS:
    void slotEnableSaving();
    void slotSaveCampaign();
    void slotSetModifyFlag( bool );
    void slotSetStartDate();
    void slotSetEndDate();
    void slotClearDate();
    void slotResetCursor( const QString& );
};


#endif /* CAMPAIGNDETAILS_H */

