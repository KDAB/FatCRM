/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "itemeditwidgetbase.h"

#include "accountdetails.h"
#include "campaigndetails.h"
#include "contactdetails.h"
#include "leaddetails.h"
#include "opportunitydetails.h"

#include <QCloseEvent>
#include <QKeyEvent>
#include <QMessageBox>

#include <KLocalizedString>


ItemEditWidgetBase::ItemEditWidgetBase(QWidget *parent)
    : QWidget(parent)
{
}

void ItemEditWidgetBase::keyPressEvent(QKeyEvent *e)
{
    if (!this->isWindow()) {
        e->ignore(); // we're an embedded simple edit widget, let the event propagate up
        return;
    }
    if (e->modifiers() == Qt::KeypadModifier || !e->modifiers()) {
        switch (e->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter: {
            accept();
            return;
        }
        case Qt::Key_Escape:
            close();
            return;
        default:
            break;
        }
    }
    e->ignore();
}

void ItemEditWidgetBase::closeEvent(QCloseEvent *event)
{
    if (this->isWindow() && isModified()) {
        QMessageBox msgBox(this);
        msgBox.setText(i18n("The data for %1 has been modified.", this->detailsName()));
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        switch(msgBox.exec()) {
        case QMessageBox::Save:
            // Save was clicked
            accept();
            event->ignore();
            return;
        case QMessageBox::Discard:
            // Don't Save was clicked
            break;
        default:
            // Cancel
            event->ignore();
            return;
        }
    }
    emit closing();
    event->accept();
}

Details *ItemEditWidgetBase::createDetailsForType(DetailsType type)
{
    switch (type) {
    case Account: return new AccountDetails;
    case Opportunity: return new OpportunityDetails;
    case Contact: return new ContactDetails;
    case Lead: return new LeadDetails;
    case Campaign: return new CampaignDetails;
    }

    return nullptr;
}

