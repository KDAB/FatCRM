/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "editlistdialog.h"
#include "clientsettings.h"

#include <KLocalizedString>

#include <QDialogButtonBox>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>

EditListDialog::EditListDialog(const QString &labelText, QWidget *parent) :
    QDialog(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(labelText, this));
    mTextEdit = new QTextEdit(this);
    mTextEdit->setAcceptRichText(false);
    layout->addWidget(mTextEdit);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, Qt::Horizontal, this);
    layout->addWidget(buttonBox);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    ClientSettings::self()->restoreWindowSize("editlistdialog", this);
}

EditListDialog::~EditListDialog()
{
    ClientSettings::self()->saveWindowSize("editlistdialog", this);
}

void EditListDialog::setItems(const QStringList &items)
{
    mTextEdit->setPlainText(items.join("\n"));
}

QStringList EditListDialog::items() const
{
    return mTextEdit->toPlainText().trimmed().split('\n', QString::SkipEmptyParts);
}
