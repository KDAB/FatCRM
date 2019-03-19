/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Robin Jonsson <robin.jonsson@kdab.com>

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

#ifndef ADDSEARCHDIALOG_H
#define ADDSEARCHDIALOG_H

#include <QDialog>
#include <QDate>

namespace Ui {
class AddSearchDialog;
}

class OpportunityFilterWidget;
class OpportunityFilterProxyModel;

class AddSearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddSearchDialog(QWidget *parent = nullptr, const bool searchBeingEdited = false, const QString searchName = "", const QString filterString = "");
    ~AddSearchDialog();

private slots:
    void slotUpdateSearchName(const QString &searchName);
    void accept() override;

private:
    Ui::AddSearchDialog *ui;
    void initialize(const bool searchBeingEdited, const QString &searchName, const QString &filterString);

    void updateSaveButton();
    void saveSearch();
    QString searchPrefix() const;

    OpportunityFilterWidget *mFilterUiWidget;
    OpportunityFilterProxyModel *mOppFilterProxyModel;
    QString mSearchName;
    bool mEditingSearch;
};

#endif // ADDSEARCHDIALOG_H
