/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "fatcrminputdialog.h"

#include "referenceddatamodel.h"
#include "widgets/nullabledatecombobox.h"

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

QPair<QString, QString> FatCRMInputDialog::getAssignedUser(const QString &caption, const QString &labelText, bool *ok)
{
    QDialog dialog;
    dialog.setWindowTitle(caption);

    auto *layout = new QVBoxLayout(&dialog);

    QLabel *label = new QLabel(&dialog);
    label->setText(labelText);

    auto *comboBox = new QComboBox(&dialog);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);

    layout->addWidget(label);
    layout->addWidget(comboBox);
    layout->addStretch();
    layout->addWidget(buttonBox);

    ReferencedDataModel::setModelForCombo(comboBox, AssignedToRef);

    QObject::connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    const bool result = (dialog.exec() == QDialog::Accepted);

    if (ok) {
        *ok = result;
    }

    if (result)
        return qMakePair(comboBox->itemData(comboBox->currentIndex()).toString(), comboBox->currentText());

    return qMakePair(QString(), QString());
}

QDate FatCRMInputDialog::getDate(const QString &caption, const QString &labelText, const QDate &value, bool *ok)
{
    QDialog dialog;
    dialog.setWindowTitle(caption);

    auto *layout = new QVBoxLayout(&dialog);

    QLabel *label = new QLabel(&dialog);
    label->setText(labelText);

    auto *dateEdit = new NullableDateComboBox(&dialog);
    dateEdit->setCalendarPopup(true);
    dateEdit->setDate(value);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);

    layout->addWidget(label);
    layout->addWidget(dateEdit);
    layout->addStretch();
    layout->addWidget(buttonBox);

    QObject::connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    const bool result = (dialog.exec() == QDialog::Accepted);

    if (ok) {
        *ok = result;
    }

    if (result)
        return dateEdit->date();

    return QDate();
}
