/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Michel Boyer de la Giroday <michel.giroday@kdab.com>

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

#include "loadingoverlay.h"

#include <QResizeEvent>
#include <QPainter>

LoadingOverlay::LoadingOverlay(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    if (parent) {
        parent->installEventFilter(this);
        raise();
    }
}

LoadingOverlay::~LoadingOverlay()
{
}

bool LoadingOverlay::eventFilter(QObject *object, QEvent *event)
{
    if (object == parent()) {
        if (event->type() == QEvent::Resize) {
            QResizeEvent *resizeEvent = static_cast<QResizeEvent*>(event);
            resize(resizeEvent->size());
        }
    }
    return QWidget::eventFilter(object, event);
}

void LoadingOverlay::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(100, 100, 100, 128));
    p.setPen(QColor(200, 200, 255, 255));
    QFont overlayFont = p.font();
    overlayFont.setPointSize(30);
    p.setFont(overlayFont);
    p.drawText(rect(), mMessage, QTextOption(Qt::AlignCenter));
}

void LoadingOverlay::setMessage(const QString &message)
{
    mMessage = message;
    update();
}


