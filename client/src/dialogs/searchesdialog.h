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

#ifndef SEARCHESDIALOG_H
#define SEARCHESDIALOG_H

#include "addsearchdialog.h"

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class SearchesDialog;
}

class SearchesDialog : public QDialog
{
    Q_OBJECT

public slots:
    void on_addSearch_clicked();
    void on_editSearch_clicked();
    void on_removeSearch_clicked();

public:
    explicit SearchesDialog(QWidget *parent = nullptr);
    ~SearchesDialog();

    QString selectedItemName() const { return mSelectedItemName; }

private slots:
    void slotItemChanged();

private:
    Ui::SearchesDialog *ui;
    void initialize();

    void updateButtons();
    void populateListView();
    void openAddSearchDialog();

    QString mSelectedItemName;

    AddSearchDialog *mDialog;
};

#endif // SEARCHESDIALOG_H
