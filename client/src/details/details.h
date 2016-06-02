/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef DETAILS_H
#define DETAILS_H

#include "enums.h"

#include "kdcrmdata/enumdefinitions.h"

#include <AkonadiCore/Item>

#include <QWidget>

class QComboBox;
class QCompleter;
class LinkedItemsRepository;
class ItemDataExtractor;
class ItemsTreeModel;

class Details : public QWidget
{
    Q_OBJECT

public:
    explicit Details(DetailsType type, QWidget *parent = Q_NULLPTR);

    ~Details();

    virtual QMap<QString, QString> data(const Akonadi::Item &item) const = 0;
    virtual void updateItem(Akonadi::Item &item, const QMap<QString, QString> &data) const = 0;

    void setData(const QMap<QString, QString> &data, QWidget *createdModifiedContainer);
    const QMap<QString, QString> getData() const;
    void clear();

    void setResourceIdentifier(const QByteArray &ident, const QString &baseUrl);
    void setSupportedFields(const QStringList &fields);
    void setEnumDefinitions(const EnumDefinitions &enums);
    virtual void setLinkedItemsRepository(LinkedItemsRepository *repo) { Q_UNUSED(repo); }
    virtual void setItemsTreeModel(ItemsTreeModel *model);
    virtual ItemDataExtractor *itemDataExtractor() const = 0;

    DetailsType type() const
    {
        return mType;
    }

    QString name() const;
    QString currentAccountId() const;
    void assignToMe();

Q_SIGNALS:
    void modified();
    void openObject(DetailsType type, const QString &id);

protected:
    QByteArray resourceIdentifier() const { return mResourceIdentifier; }
    QString resourceBaseUrl() const { return mResourceBaseUrl; }
    QCompleter *createCountriesCompleter();

    void fillComboBox(QComboBox *combo, const QString &objectName) const;

    virtual void setDataInternal(const QMap<QString, QString> &) {}
    QString id() const;
    ItemsTreeModel *mItemsTreeModel;

private Q_SLOTS:
    void doConnects();

private:
    const DetailsType mType;
    QByteArray mResourceIdentifier;
    QString mResourceBaseUrl;
    QStringList mKeys;
    EnumDefinitions mEnumDefinitions;
};
#endif /* DETAILS_H */
