/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef DOCUMENTSWINDOW_H
#define DOCUMENTSWINDOW_H

#include <QWidget>

namespace Ui {
class DocumentsWindow;
}

class SugarDocument;
class QUrl;

class DocumentsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DocumentsWindow(QWidget *parent = Q_NULLPTR);
    ~DocumentsWindow();

    void addDocument(const SugarDocument &document);

    void setVisible(bool visible) Q_DECL_OVERRIDE;

    void setResourceIdentifier(const QString &identifier);

private slots:
    void on_buttonBox_rejected();

    void urlClicked(const QUrl &url);

private:
    QVector<SugarDocument> m_documents;
    Ui::DocumentsWindow *ui;
    QString mResourceIdentifier;
};

#endif // DOCUMENTSWINDOW_H
