/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "kdcrmdata/enumdefinitions.h"
#include "kdcrmdata/sugardocument.h"

#include <QWidget>

namespace Ui {
class DocumentsWindow;
}

class KJob;
class LinkedItemsRepository;
class QComboBox;
class QLabel;
class QPlainTextEdit;
class QPushButton;
class QUrl;

class DocumentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DocumentWidget(EnumDefinitions *definitions, QWidget *parent = nullptr);

    void setDocument(const SugarDocument &document);
    SugarDocument document() const;

    SugarDocument modifiedDocument() const;

    void setNewDocumentFilePath(const QString &filePath);
    QString newDocumentFilePath() const;

    bool isModified() const;

signals:
    void urlClicked(const QString &url);
    void deleteDocument();

private:
    QLabel *mNameLabel;
    QComboBox *mStatusBox;
    QPlainTextEdit *mDescriptionEdit;
    QPushButton *mDeleteButton;

    SugarDocument mDocument;
    QString mNewDocumentFilePath;

    EnumDefinitions *mEnumDefinitions;
};

class DocumentsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DocumentsWindow(QWidget *parent = nullptr);
    ~DocumentsWindow() override;

    void setResourceIdentifier(const QString &identifier);
    void setLinkedItemsRepository(LinkedItemsRepository *repository);

    enum LinkedItemType
    {
        Account,
        Opportunity
    };

    void loadDocumentsFor(const QString &id, LinkedItemType itemType);

signals:
    void documentsCreated();

protected:
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

    void urlClicked(const QString &url);
    void deleteDocument();
    void attachDocument();
    void attachDocument(const QString &filePath);

    void slotJobResult(KJob *job);

private:
    DocumentWidget* addDocument(const SugarDocument &document);

    bool isModified() const;
    void saveChanges();

private:
    QVector<SugarDocument> mDocuments;
    QVector<DocumentWidget*> mDocumentWidgets;

    Ui::DocumentsWindow *ui;
    QString mResourceIdentifier;
    LinkedItemsRepository *mLinkedItemsRepository = nullptr;
    EnumDefinitions mEnumDefinitions;
    bool mIsNotModifiedOverride = false;

    QString mLinkedItemId;
    LinkedItemType mLinkedItemType = Account;

    int mPendingJobCount = 0;
};

#endif // DOCUMENTSWINDOW_H
