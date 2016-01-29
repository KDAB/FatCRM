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

#include "betterplaintextedit.h"

#include <QDebug>
#include <QTextDocument>
#include <QScrollBar>
#include <QStyle>
#include <QStyleOption>

BetterPlainTextEdit::BetterPlainTextEdit(QWidget *parent) :
    QPlainTextEdit(parent)
{
    connect(document()->documentLayout(), SIGNAL(documentSizeChanged(QSizeF)),
            this, SLOT(slotDocumentSizeChanged()));
}

QSize BetterPlainTextEdit::widgetSizeForTextSize(const QSize &size) const
{
    QFontMetrics fm(document()->defaultFont());
    const int lineHeight = fm.height();
    QSize ds(size);
    ds.rheight() *= lineHeight;

    // Taken from QAbstractScrollArea::minimumSizeHint() in Qt4
    const int hsbExt = horizontalScrollBar()->sizeHint().height();
    const int vsbExt = verticalScrollBar()->sizeHint().width();
    int extra = 2 * frameWidth();
    QStyleOption opt;
    opt.initFrom(this);
    if ((frameStyle() != QFrame::NoFrame)
        && style()->styleHint(QStyle::SH_ScrollView_FrameOnlyAroundContents, &opt, this)) {
        extra += style()->pixelMetric(QStyle::PM_ScrollView_ScrollBarSpacing, &opt, this);
    }

    return QSize(ds.width() + vsbExt + extra,
                 ds.height() + hsbExt + extra);
}

QSize BetterPlainTextEdit::minimumSizeHint() const
{
    QSize ds = document()->documentLayout()->documentSize().toSize();
    ds.setWidth(50);
    return widgetSizeForTextSize(ds);
}

QSize BetterPlainTextEdit::sizeHint() const
{
    QSize ds = document()->documentLayout()->documentSize().toSize();
    return widgetSizeForTextSize(ds);
}

void BetterPlainTextEdit::slotDocumentSizeChanged()
{
    updateGeometry();
}
