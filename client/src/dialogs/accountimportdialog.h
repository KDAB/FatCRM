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

#ifndef ACCOUNTIMPORTDIALOG_H
#define ACCOUNTIMPORTDIALOG_H

#include "sugaraccount.h"

#include <Akonadi/Collection>
#include <Akonadi/Item>

#include <QDialog>
#include <QSignalMapper>

namespace Ui {
class AccountImportDialog;
}
class QAbstractButton;
class QGroupBox;
class QButtonGroup;
class KJob;

class AccountImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AccountImportDialog(QWidget *parent = 0);
    ~AccountImportDialog();

    // intput
    void setAccountCollection(const Akonadi::Collection &collection);
    void setImportedAccounts(const QVector<SugarAccount> &accounts);

    // output (valid after exec() returns)
    QVector<SugarAccount> chosenAccounts() const;

protected:
    void accept() Q_DECL_OVERRIDE;
    void reject() Q_DECL_OVERRIDE;

private Q_SLOTS:
    void slotTextChanged(QWidget *lineEdit);
    void slotButtonClicked(QAbstractButton *button);
    void slotCreateAccountResult(KJob *job);
    void updateOKButton();
    void slotAccountAdded(const QString &id, Akonadi::Item::Id akonadiId);

private:
    void fillSimilarAccounts(int row);
    void accountCreated(int row, const QString &id);

    Akonadi::Collection mAccountCollection;

    struct PendingAccount {
        QButtonGroup *buttonGroup;
        QGroupBox *groupBox;
        QPushButton *createButton;
        SugarAccount account;
        Akonadi::Item::Id idBeingCreated;
    };

    QVector<PendingAccount> mPendingAccounts;
    QSignalMapper mLineEditMapper;
    QList<KJob *> mAccountCreationJobs;
    Ui::AccountImportDialog *mUi;
};

#endif // ACCOUNTIMPORTDIALOG_H
