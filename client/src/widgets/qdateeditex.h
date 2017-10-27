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

#ifndef DATEEDITEX_H
#define DATEEDITEX_H

#include <QDateEdit>
#include "fatcrmprivate_export.h"
class FATCRMPRIVATE_EXPORT QDateEditEx : public QDateEdit
{
    Q_OBJECT

    Q_PROPERTY(bool nullable READ isNullable WRITE setNullable)
public:
    explicit QDateEditEx(QWidget *parent = nullptr);
    ~QDateEditEx() override;

    QDateTime dateTime() const;
    QDate date() const;
    QTime time() const;

    bool isNullable() const;
    void setNullable(bool enable);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void clear() override;

protected:
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    bool focusNextPrevChild(bool next) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    QValidator::State validate(QString &input, int &pos) const override;

public Q_SLOTS:
    /*! \reimp */ void setDateTime(const QDateTime &dateTime);
    /*! \reimp */ void setDate(const QDate &date);
    /*! \reimp */ void setTime(const QTime &time);

private slots:
    void clearButtonClicked();

private:
    Q_DISABLE_COPY(QDateEditEx)
    class Private;
    friend class Private;
    Private* d;

};

#endif // DATEEDITEX_H
