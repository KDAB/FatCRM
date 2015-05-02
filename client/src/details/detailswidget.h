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
    Q_OBJECT
public:
    explicit DetailsWidget(DetailsType type, QWidget *parent = 0);
    ~DetailsWidget();

    inline bool isModified() const
    {
        return mModified;
    }

    void setItem(const Akonadi::Item &item);
    void clearFields();
    QMap<QString, QString> data() const;
    Details *details() const { return mDetails; }

    static Details *createDetailsForType(DetailsType type);

public Q_SLOTS:
    void saveData();

Q_SIGNALS:
    void createItem();
    void modifyItem();

private Q_SLOTS:
    void slotModified();
    void slotDiscardData();

private:
    void setModified(bool modified);
    void initialize();
    void reset();
    void setConnections();

    void setData(const QMap<QString, QString> &data);

private:
    Details *mDetails;

    QMap<QString, QString> mData;

    DetailsType mType;
    bool mModified;
    bool mCreateNew;
    Ui_detailswidget mUi;
    Akonadi::Item mItem;
};

#endif /* DETAILSWIDGET_H */

