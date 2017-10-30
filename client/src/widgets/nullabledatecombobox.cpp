/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#include "nullabledatecombobox.h"
#include <QLineEdit>
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QAbstractButton>
#include <QDate>
#include <QComboBox>
#include <QMenu>

#include "fatcrm_client_debug.h"

/*!
  \class NullableDateComboBox nullabledatecombobox.h
  \brief A KDateComboBox with a nullable date

  This is a subclass of KDateComboBox that has the additional feature
  of allowing to select a empty date. This can be achived with the
  clear button or by selecting the whole date and press backspace.

  To set an empty date from code, use the setter with an invalid
  date. To check whether the date is empty check if the date is valid,
  which comes from the getter.

  \sa KDateComboBox
*/

class NullableDateComboBox::Private {
public:
    Private( NullableDateComboBox* qq ) : q(qq), null(false), nullable(false) {}

    NullableDateComboBox* const q;

    bool null;
    bool nullable;

    void setNull(bool n) {
        null = n;
        if (null) {
            QLineEdit *edit = q->lineEdit();
            if (!edit->text().isEmpty()) {
                edit->clear();
            }
        }

        q->lineEdit()->setClearButtonEnabled(nullable);
    }
};

/*!
  \reimp
*/
NullableDateComboBox::NullableDateComboBox(QWidget *parent) :
    KDateComboBox(parent), d(new Private(this))
{
    // set object names for all internal children to avoid them being hidden by Details widget
    QList<QObject *> children = this->findChildren<QObject*>();
    int index = 0;
    for (QObject *c : children) {
        if (c->objectName().isEmpty()) {
            c->setObjectName(QString("qt_%1").arg(index++));
        }
    }

    //WORKAROUND: Since the QDateEditEx can accept empty dates, we should clear the text
    // if user click on "no date" menu entry
    QMenu *dateMenu = findChild<QMenu*>();
    connect(dateMenu, &QMenu::triggered, this, &NullableDateComboBox::onMenuSelected);
}

NullableDateComboBox::~NullableDateComboBox()
{
    delete d;
}

/*!
 * \brief returns date, if empty date is invalid
 * \return date, if empty date is invalid
 */
QDate NullableDateComboBox::date() const
{
    if (d->nullable && d->null) {
        return QDate();
    } else {
        return KDateComboBox::date();
    }
}

QString NullableDateComboBox::text() const
{
    return currentText();
}

/*!
 * \brief returns date can be empty
 * \return true, if date can be emtpy
 */
bool NullableDateComboBox::isNullable() const
{
    return d->nullable;
}

/*!
 * \brief sets whether the date can be empty
 */
void NullableDateComboBox::setNullable(bool enable)
{
    d->nullable = enable;
    lineEdit()->setClearButtonEnabled(enable);
    update();
}

/*!
  \reimp
*/
QSize NullableDateComboBox::sizeHint() const
{
    const QSize sz = KDateComboBox::sizeHint();
    QAbstractButton *btn = lineEdit()->findChild<QAbstractButton*>();

    if (!btn)
        return sz;
    return QSize(sz.width() + btn->width() + 3, sz.height());
}

/*!
  \reimp
*/
QSize NullableDateComboBox::minimumSizeHint() const
{
    const QSize sz = KDateComboBox::minimumSizeHint();
    QAbstractButton *btn = lineEdit()->findChild<QAbstractButton*>();

    if (!btn)
        return sz;

    return QSize(sz.width() + btn->width() + 3, sz.height());
}

void NullableDateComboBox::clear()
{
    d->setNull(true);
}

void NullableDateComboBox::setCalendarPopup(bool flag)
{
    if (flag)
        setOptions(KDateComboBox::EditDate |
                   KDateComboBox::SelectDate |
                   KDateComboBox::DatePicker |
                   KDateComboBox::DateKeywords);
    else
        setOptions(KDateComboBox::EditDate | KDateComboBox::SelectDate);
}

bool NullableDateComboBox::isClearButtonVisible() const
{
    QAbstractButton *btn = lineEdit()->findChild<QAbstractButton*>();
    if (!btn)
        return false;

    return btn->property(QByteArrayLiteral("opacity")).toReal() == 1.0;
}

/*!
  \reimp
*/
void NullableDateComboBox::showEvent(QShowEvent *event)
{
    KDateComboBox::showEvent(event);
    d->setNull(d->null); // force empty string back in
}

/*!
  \reimp
*/
void NullableDateComboBox::keyPressEvent(QKeyEvent *event)
{
    if (d->nullable &&
        (event->key() >= Qt::Key_0) &&
        (event->key() <= Qt::Key_9) &&
        d->null) {
            setDate(QDate::currentDate());
        }
    if (event->key() == Qt::Key_Backspace && d->nullable){
        const QLineEdit *edit = lineEdit();
        if (edit->selectedText() == edit->text()) {
            setDate(QDate());
            event->accept();
            return;
        }
    }

    KDateComboBox::keyPressEvent(event);
}

/*!
  \reimp
*/
void NullableDateComboBox::mousePressEvent(QMouseEvent *event)
{
    bool saveNull = d->null;
    KDateComboBox::mousePressEvent(event);
    if (d->nullable && saveNull && view() && view()->isVisible()) {
        setDate(QDate::currentDate());
    }
}

/*!
  \reimp
*/
void NullableDateComboBox::focusInEvent(QFocusEvent *event)
{
    KDateComboBox::focusInEvent(event);
    d->setNull(d->null); // force empty string back in
}

/*!
  \reimp
*/
void NullableDateComboBox::focusOutEvent(QFocusEvent *event)
{
    KDateComboBox::focusOutEvent(event);
    d->setNull(d->null); // force empty string back in
}

void NullableDateComboBox::assignDate(const QDate &date)
{
    if (d->nullable && !date.isValid()) {
        d->setNull(true);
    } else {
        d->setNull(false);
        KDateComboBox::assignDate(date);
    }
}

void NullableDateComboBox::onMenuSelected(QAction *action)
{
    //WORKAROUND: "no date" menu contains a invalid date we will clear the combo box
    // text if an invalid date was selected in the menu
    if (d->nullable && !action->data().toDate().isValid()) {
        d->setNull(true);
    }
}

