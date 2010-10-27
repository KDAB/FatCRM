#ifndef CAMPAIGNDETAILS_H
#define CAMPAIGNDETAILS_H

#include "details.h"

namespace Ui {
    class CampaignDetails;
}

class CampaignDetails : public Details
{
    Q_OBJECT
public:
    explicit CampaignDetails( QWidget *parent = 0 );

    ~CampaignDetails();

private:
    Ui::CampaignDetails *mUi;

private:
    /*reimp*/ void initialize();
    /*reimp*/ QMap<QString, QString> data( const Akonadi::Item &item ) const;
    /*reimp*/ void updateItem( Akonadi::Item &item, const QMap<QString, QString> &data ) const;

    QStringList typeItems() const;
    QStringList statusItems() const;

private Q_SLOTS:
    void slotSetStartDate();
    void slotSetEndDate();
    void slotClearStartDate();
    void slotClearEndDate();
};


#endif /* CAMPAIGNDETAILS_H */

