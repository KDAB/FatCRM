/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2020-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>

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

#include "contactfilterwidget.h"
#include "filterproxymodel.h"

#include <KLocalizedString>

#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QCoreApplication>

ContactFilterWidget::ContactFilterWidget(FilterProxyModel *proxyModel, bool showGDPR, QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    if (showGDPR) {
        if (proxyModel->hasGDPRProtectedEmails()) {
            mGDPRComboBox = new QComboBox(this);
            // Should match FilterProxyModel::Action
            mGDPRComboBox->addItems({
                                        i18n("All contacts"),
                                        i18n("Candidates for GDPR deletion [no account, created 5+ years ago, old description]"),
                                        i18n("Candidates for GDPR anonymization [created 5+ years ago, old description, no recent opps]")
                                    });
            layout->addWidget(mGDPRComboBox);
            connect(mGDPRComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int idx) {
                proxyModel->setGDPRFilter(static_cast<FilterProxyModel::Action>(idx));
            });
        } else {
            QLabel *label = new QLabel(i18n("newsletter.txt not found in %1", QCoreApplication::applicationDirPath()), this);
            layout->addWidget(label);
        }
    }

    layout->addStretch(1);
}
