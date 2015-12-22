/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>
           Michel Boyer de la Giroday <michel.giroday@kdab.com>
           Kevin Krammer <kevin.krammer@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DETAILSWIDGET_H
#define DETAILSWIDGET_H

#include "ui_detailswidget.h"
#include "enums.h"
#include "details.h"

#include "kdcrmdata/sugaraccount.h"
#include "kdcrmdata/sugarlead.h"
#include "kdcrmdata/sugaropportunity.h"
#include "kdcrmdata/sugarcampaign.h"

#include <AkonadiCore/Item>

#include <KContacts/Addressee>

#include <QWidget>

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
    void setModificationsIgnored(bool b);
    void initialLoadingDone();

    void setItem(const Akonadi::Item &item);
    void setItemRevision(const Akonadi::Item &item);
    void clearFields();
    QMap<QString, QString> data() const;
    Details *details() const { return mDetails; }
    void setData(const QMap<QString, QString> &data);

    static Details *createDetailsForType(DetailsType type);

public Q_SLOTS:
    void saveData();

Q_SIGNALS:
    void createItem();
    void modifyItem(const Akonadi::Item &item);

protected:

private Q_SLOTS:
    void slotModified();
    void slotDiscardData();

private:
    void setModified(bool modified);
    void initialize();
    void reset();
    void setConnections();

private:
    Details *mDetails;

    DetailsType mType;
    bool mModified;
    bool mCreateNew;
    bool mIgnoreModifications;
    Ui_detailswidget mUi;
    Akonadi::Item mItem;
};

#endif /* DETAILSWIDGET_H */
