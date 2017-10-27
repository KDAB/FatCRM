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

#ifndef NULLABLEDATECOMBOBOX_H
#define NULLABLEDATECOMBOBOX_H

#include <KDateComboBox>

#include "fatcrmprivate_export.h"
class FATCRMPRIVATE_EXPORT NullableDateComboBox : public KDateComboBox
{
    Q_OBJECT

    Q_PROPERTY(bool nullable READ isNullable WRITE setNullable)
public:
    explicit NullableDateComboBox(QWidget *parent = nullptr);
    ~NullableDateComboBox() override;

    QDate date() const;
    QString text() const;

    bool isNullable() const;
    void setNullable(bool enable);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void clear();

    void setCalendarPopup(bool flag);

    bool isClearButtonVisible() const;

protected:
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void focusInEvent(QFocusEvent *event) Q_DECL_OVERRIDE;
    void focusOutEvent(QFocusEvent *event) Q_DECL_OVERRIDE;
    void assignDate(const QDate &date) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void onMenuSelected(QAction *action);

private:
    Q_DISABLE_COPY(NullableDateComboBox)
    class Private;
    friend class Private;
    Private* d;

};

#endif // NULLABLEDATECOMBOBOX_H
