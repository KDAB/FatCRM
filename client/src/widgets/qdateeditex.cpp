/**************************************************************************
**
** Copyright (c) 2013 Qualiant Software GmbH
**
** Author: Andreas Holzammer, KDAB (andreas.holzammer@kdab.com)
**
** Contact: Qualiant Software (d.oberkofler@qualiant.at)
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** Qualiant Software at d.oberkofler@qualiant.at.
**
**************************************************************************/

#include "qdateeditex.h"
#include <QStyle>
#include <QPushButton>
#include <QLineEdit>
#include <QStyleOptionSpinBox>
#include <QKeyEvent>
#include <QCalendarWidget>
#include "fatcrm_client_debug.h"

/*!
  \class DateEditEx dateeditex.h
  \brief A DateEdit with a nullable date

  This is a subclass of QDateEdit that has the additional feature
  of allowing to select a empty date. This can be achived with the
  clear button or by selecting the whole date and press backspace.

  To set an empty date from code, use the setter with an invalid
  date. To check whether the date is empty check if the date is valid,
  which comes from the getter.

  \sa QDateEdit
*/

class QDateEditEx::Private {
public:
    Private( QDateEditEx* qq ) : q(qq),  clearButton(nullptr), null(false), nullable(false) {}

    QDateEditEx* const q;

    QPushButton* clearButton;

    bool null;
    bool nullable;

    void setNull(bool n) {
        null = n;
        if (null) {
            QLineEdit *edit = qFindChild<QLineEdit *>(q, QStringLiteral("qt_spinbox_lineedit"));
            if (!edit->text().isEmpty()) {
                edit->clear();
            }
        }
        if (nullable) {
            clearButton->setVisible(!null);
        }

    }
};

/*!
  \reimp
*/
QDateEditEx::QDateEditEx(QWidget *parent) :
    QDateEdit(parent), d(new Private(this))
{
}

QDateEditEx::~QDateEditEx()
{
    delete d;
}

/*!
 * \brief returns date, if empty date is invalid
 * \return date, if empty date is invalid
 */
QDateTime QDateEditEx::dateTime() const
{
    if (d->nullable && d->null) {
        return QDateTime();
    } else {
        return QDateEdit::dateTime();
    }
}

/*!
 * \brief returns date, if empty date is invalid
 * \return date, if empty date is invalid
 */
QDate QDateEditEx::date() const
{
    if (d->nullable && d->null) {
        return QDate();
    } else {
        return QDateEdit::date();
    }
}

/*!
 * \brief returns date, if empty date is invalid
 * \return date, if empty date is invalid
 */
QTime QDateEditEx::time() const
{
    if (d->nullable && d->null) {
        return QTime();
    } else {
        return QDateEdit::time();
    }
}

/*!
 * \brief sets a date, if date is invalid a
 * empty date is shown
 */
void QDateEditEx::setDateTime(const QDateTime &dateTime)
{
    if (d->nullable && !dateTime.isValid()) {
        d->setNull(true);
    } else {
        d->setNull(false);
        QDateEdit::setDateTime(dateTime);
    }
}

/*!
 * \brief sets a date, if date is invalid a
 * empty date is shown
 */
void QDateEditEx::setDate(const QDate &date)
{
    if (d->nullable && !date.isValid()) {
        d->setNull(true);
    } else {
        d->setNull(false);
        QDateEdit::setDate(date);
    }
}

/*!
 * \brief sets a date, if date is invalid a
 * empty date is shown
 */
void QDateEditEx::setTime(const QTime &time)
{
    if (d->nullable && !time.isValid()) {
        d->setNull(true);
    } else {
        d->setNull(false);
        QDateEdit::setTime(time);
    }
}

/*!
 * \brief returns date can be empty
 * \return true, if date can be emtpy
 */
bool QDateEditEx::isNullable() const
{
    return d->nullable;
}

/*!
 * \brief sets whether the date can be empty
 */
void QDateEditEx::setNullable(bool enable)
{
    d->nullable = enable;

    if (enable && !d->clearButton) {
        d->clearButton = new QPushButton(this);
        d->clearButton->setFlat(true);
#if 1 // defined(WIDGETS_LIBRARY)
        static bool initres = false;
        if (!initres) {
            Q_INIT_RESOURCE(widgets);
            initres = true;
        }
#endif // defined(WIDGETS_LIBRARY)
        Q_ASSERT(QFile::exists(":/widgets/edit-clear-locationbar-rtl.png"));
        d->clearButton->setIcon(QIcon(":/widgets/edit-clear-locationbar-rtl.png"));
        d->clearButton->setFocusPolicy(Qt::NoFocus);
        d->clearButton->setFixedSize(17, d->clearButton->sizeHint().height()-6);
        connect(d->clearButton,SIGNAL(clicked()),this,SLOT(clearButtonClicked()));
        d->clearButton->setVisible(!d->null);
    } else if (d->clearButton) {
        disconnect(d->clearButton,SIGNAL(clicked()),this,SLOT(clearButtonClicked()));
        delete d->clearButton;
        d->clearButton = nullptr;
    }

    update();
}

/*!
  \reimp
*/
QSize QDateEditEx::sizeHint() const
{
    const QSize sz = QDateEdit::sizeHint();
    if (!d->clearButton)
        return sz;
    return QSize(sz.width() + d->clearButton->width() + 3, sz.height());
}

/*!
  \reimp
*/
QSize QDateEditEx::minimumSizeHint() const
{
    const QSize sz = QDateEdit::minimumSizeHint();
    if (!d->clearButton)
        return sz;
    return QSize(sz.width() + d->clearButton->width() + 3, sz.height());
}

void QDateEditEx::clear()
{
    d->setNull(true);
}

void QDateEditEx::showEvent(QShowEvent *event)
{
    QDateEdit::showEvent(event);
    d->setNull(d->null); // force empty string back in
}

/*!
  \reimp
*/
void QDateEditEx::resizeEvent(QResizeEvent *event)
{
    if (d->clearButton) {
        QStyleOptionSpinBox opt;
        initStyleOption(&opt);
        opt.subControls = QStyle::SC_SpinBoxUp;

        int left = style()->subControlRect(QStyle::CC_SpinBox, &opt, QStyle::SC_SpinBoxUp, this).left() - d->clearButton->width() - 3;
        d->clearButton->move(left, (height() - d->clearButton->height()) / 2);
    }

    QDateEdit::resizeEvent(event);
}

/*!
  \reimp
*/
void QDateEditEx::paintEvent(QPaintEvent *event)
{
    QDateEdit::paintEvent(event);
}

/*!
  \reimp
*/
void QDateEditEx::keyPressEvent(QKeyEvent *event)
{
    if (d->nullable &&
        (event->key() >= Qt::Key_0) &&
        (event->key() <= Qt::Key_9) &&
        d->null) {
            setDateTime(QDateTime::currentDateTime());
        }
    if (event->key() == Qt::Key_Tab && d->nullable && d->null) {
        QAbstractSpinBox::keyPressEvent(event);
        return;
    }
    if (event->key() == Qt::Key_Backspace && d->nullable){
        QLineEdit *edit = qFindChild<QLineEdit *>(this, QStringLiteral("qt_spinbox_lineedit"));
        if (edit->selectedText() == edit->text()) {
            setDateTime(QDateTime());
            event->accept();
            return;
        }
    }

    QDateEdit::keyPressEvent(event);
}

/*!
  \reimp
*/
void QDateEditEx::mousePressEvent(QMouseEvent *event)
{
    bool saveNull = d->null;
    QDateEdit::mousePressEvent(event);
    if (d->nullable && saveNull && calendarWidget() && calendarWidget()->isVisible()) {
        setDateTime(QDateTime::currentDateTime());
    }
}

/*!
  \reimp
*/
bool QDateEditEx::focusNextPrevChild(bool next)
{
    if (d->nullable && d->null){
        return QAbstractSpinBox::focusNextPrevChild(next);
    } else {
        return QDateEdit::focusNextPrevChild(next);
    }
}

void QDateEditEx::focusInEvent(QFocusEvent *event)
{
    QDateEdit::focusInEvent(event);
    d->setNull(d->null); // force empty string back in
}

void QDateEditEx::focusOutEvent(QFocusEvent *event)
{
    QDateEdit::focusOutEvent(event);
    d->setNull(d->null); // force empty string back in
}

QValidator::State QDateEditEx::validate(QString &input, int &pos) const
{
    if (d->nullable && d->null){
        return QValidator::Acceptable;
    }
    return QDateEdit::validate(input, pos);
}

void QDateEditEx::clearButtonClicked()
{
    d->setNull(true);
}
