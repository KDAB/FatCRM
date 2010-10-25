#ifndef DETAILSWIDGET_H
#define DETAILSWIDGET_H

#include "ui_detailswidget.h"
#include "enums.h"
#include "details.h"

#include <kdcrmdata/sugaraccount.h>
#include <kdcrmdata/sugarlead.h>
#include <kdcrmdata/sugaropportunity.h>
#include <kdcrmdata/sugarcampaign.h>

#include <kabc/addressee.h>

#include <akonadi/item.h>
#include <QtGui/QWidget>

class DetailsWidget : public QWidget
{
    friend class Page;
    Q_OBJECT
public:
    explicit DetailsWidget( DetailsType type, QWidget * parent = 0 );
    ~DetailsWidget();

    inline bool isEditing() { return mEditing; }

Q_SIGNALS:
    void saveItem();
    void modifyItem();

protected:
    inline void setEditing( bool value ) { mEditing = value; }

    void setItem( const Akonadi::Item &item );
    void clearFields();

    QMap<QString, QString> data();

    static Details *createDetailsForType( DetailsType type );

private Q_SLOTS:
    void slotEnableSaving();
    void slotSetModifyFlag( bool );
    void slotSaveData();
    void slotDiscardData();

private:
    void initialize();
    void setConnections();
    void reset();
    QString currentAccountId() const;
    QString currentAssignedToId() const;
    QString currentCampaignId() const;
    QString currentReportsToId() const;

    void setData( const QMap<QString,QString> &data );

private:
    Details *mDetails;

    QMap<QString, QString> mData;

    DetailsType mType;
    bool mEditing;
    bool mModifyFlag;
    Ui_detailswidget mUi;
    Akonadi::Item mItem;
};

#endif /* DETAILSWIDGET_H */

