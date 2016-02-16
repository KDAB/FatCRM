/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Tobias Koenig <tobias.koenig@kdab.com>

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

#ifndef CONTACTSIMPORTPAGE_H
#define CONTACTSIMPORTPAGE_H

#include "contactsset.h"

#include <AkonadiCore/Collection>
#include <AkonadiCore/Item>

#include <QWizardPage>

namespace Ui {
class ContactsImportPage;
}

class FilterProxyModel;
class ItemsTreeModel;
class QButtonGroup;
class QCheckBox;
class QLabel;
class QVBoxLayout;

struct MatchPair
{
    KContacts::Addressee contact;
    Akonadi::Item item;
};

class MergeWidget : public QWidget
{
    Q_OBJECT

public:
    MergeWidget(const SugarAccount &account, const KContacts::Addressee &importedAddressee,
                const QVector<MatchPair> &possibleMatches, QWidget *parent = Q_NULLPTR);

    bool skipItem() const;
    Akonadi::Item finalItem() const;

private slots:
    void updateFinalContact();

private:
    QCheckBox *addFieldCheckBox(const QString &str);

    SugarAccount mAccount;
    KContacts::Addressee mImportedAddressee;
    QVector<MatchPair> mPossibleMatches;
    KContacts::Addressee mFinalContact;

    QLabel *mImportedContactLabel;
    struct UpdateCheckBoxes
    {
        UpdateCheckBoxes();

        QCheckBox *prefix;
        QCheckBox *givenName;
        QCheckBox *familyName;
        QCheckBox *emailAddress;
        QCheckBox *phoneNumber;
        QCheckBox *jobTitle;
    };
    UpdateCheckBoxes mUpdateCheckBoxes;

    QButtonGroup *mButtonGroup;

    QVBoxLayout *mImportedContactLayout;
    QLabel *mFinalContactLabel;
};

class ContactsImportPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit ContactsImportPage(QWidget *parent = 0);
    ~ContactsImportPage();

    void setContactsModel(ItemsTreeModel *model);

    void cleanup();

    bool validatePage() Q_DECL_OVERRIDE;

public slots:
    void setChosenContacts(const QVector<ContactsSet> &contacts);

signals:
    void importedItems(const QVector<Akonadi::Item> &items);
    void layoutChanged();

private slots:
    void adjustPageSize();

private:
    void addMergeWidget(const SugarAccount &account, const KContacts::Addressee &importedAddressee,
                        const QVector<MatchPair> &possibleMatches);

    Ui::ContactsImportPage *mUi;

    FilterProxyModel *mContactsModel;
};

#endif // CONTACTSIMPORTPAGE_H
