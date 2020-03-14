/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "conflictresolvedialog.h"

#include <AkonadiCore/abstractdifferencesreporter.h> //krazy:exclude=camelcase
#include <AkonadiCore/differencesalgorithminterface.h>
#include <AkonadiCore/Item>

#include <KColorScheme>
#include <KLocalizedString>
#include <KConfigGroup>
#include <KWindowConfig>

#include <QLabel>
#include <QDir>
#include <QTextBrowser>
#include <QPushButton>
#include <QWindow>
#include <QScreen>
#include <QVBoxLayout>
#include <QDesktopServices>
#include <QTemporaryFile>

using namespace Akonadi;

static inline QString textToHTML(const QString &text)
{
    return Qt::convertFromPlainText(text);
}

class HtmlDifferencesReporter : public AbstractDifferencesReporter
{
public:
    HtmlDifferencesReporter()
    {
    }

    QString toHtml() const
    {
        return header() + mContent + footer();
    }

    QString plainText() const
    {
        return mTextContent;
    }

    void setPropertyNameTitle(const QString &title) override
    {
        mNameTitle = title;
    }

    void setLeftPropertyValueTitle(const QString &title) override
    {
        mLeftTitle = title;
    }

    void setRightPropertyValueTitle(const QString &title) override
    {
        mRightTitle = title;
    }

    void addProperty(Mode mode, const QString &name, const QString &leftValue, const QString &rightValue) override
    {
        switch (mode) {
        case NormalMode:
            mContent.append(QStringLiteral("<tr><td align=\"right\"><b>%1:</b></td><td>%2</td><td></td><td>%3</td></tr>")
                            .arg(name, textToHTML(leftValue), textToHTML(rightValue)));
            mTextContent.append(QStringLiteral("%1:\n%2\n%3\n\n").arg(name, leftValue, rightValue));
            break;
        case ConflictMode:
            mContent.append(QStringLiteral("<tr><td align=\"right\"><b>%1:</b></td><td bgcolor=\"#ff8686\">%2</td><td></td><td bgcolor=\"#ff8686\">%3</td></tr>")
                            .arg(name, textToHTML(leftValue), textToHTML(rightValue)));
            mTextContent.append(QStringLiteral("%1:\n%2\n%3\n\n").arg(name, leftValue, rightValue));
            break;
        case AdditionalLeftMode:
            mContent.append(QStringLiteral("<tr><td align=\"right\"><b>%1:</b></td><td bgcolor=\"#9cff83\">%2</td><td></td><td></td></tr>")
                            .arg(name, textToHTML(leftValue)));
            mTextContent.append(QStringLiteral("%1:\n%2\n\n").arg(name, leftValue));
            break;
        case AdditionalRightMode:
            mContent.append(QStringLiteral("<tr><td align=\"right\"><b>%1:</b></td><td></td><td></td><td bgcolor=\"#9cff83\">%2</td></tr>")
                            .arg(name, textToHTML(rightValue)));
            mTextContent.append(QStringLiteral("%1:\n%2\n\n").arg(name, rightValue));
            break;
        }
    }

private:
    QString header() const
    {
        QString header = QStringLiteral("<html>");
        header += QStringLiteral("<body text=\"%1\" bgcolor=\"%2\">")
                  .arg(KColorScheme(QPalette::Active, KColorScheme::View).foreground().color().name(),
                       KColorScheme(QPalette::Active, KColorScheme::View).background().color().name());
        header += QLatin1String("<center><table>");
        header += QStringLiteral("<tr><th align=\"center\">%1</th><th align=\"center\">%2</th><td>&nbsp;</td><th align=\"center\">%3</th></tr>")
                  .arg(mNameTitle,
                       mLeftTitle,
                       mRightTitle);

        return header;
    }

    QString footer() const
    {
        return QStringLiteral("</table></center>"
                             "</body>"
                             "</html>");
    }

    QString mContent;
    QString mNameTitle;
    QString mLeftTitle;
    QString mRightTitle;
    QString mTextContent;
};

class ConflictResolveDialog::Private
{
    ConflictResolveDialog *const q;
public:
    explicit Private(ConflictResolveDialog *parent)
        : q(parent), mStrategy(ConflictHandler::UseLocalItem), mDiffInterface(nullptr), mView(nullptr)
    {
    }

public:
    ConflictHandler::ResolveStrategy mStrategy;

    Item mLocalItem;
    Item mOtherItem;

    DifferencesAlgorithmInterface *mDiffInterface;

    QTextBrowser *mView;
    QString mTextContent;

public: // slots
    void useLocalItem();
    void useOtherItem();
    void useBothItems();
    void openEditor();
    void createReport();
};

void ConflictResolveDialog::Private::useLocalItem()
{
    mStrategy = ConflictHandler::UseLocalItem;
    q->accept();
}

void ConflictResolveDialog::Private::useOtherItem()
{
    mStrategy = ConflictHandler::UseOtherItem;
    q->accept();
}

void ConflictResolveDialog::Private::useBothItems()
{
    mStrategy = ConflictHandler::UseBothItems;
    q->accept();
}

void ConflictResolveDialog::Private::createReport()
{
    Q_ASSERT(mDiffInterface != nullptr);

    HtmlDifferencesReporter reporter;

    mDiffInterface->compare(&reporter, mLocalItem, mOtherItem);

    mView->setHtml(reporter.toHtml());
    mTextContent = reporter.plainText();
}

void ConflictResolveDialog::Private::openEditor()
{
    QTemporaryFile file(QDir::tempPath() + "/FatCRM-XXXXXX.txt");
    if (file.open()) {
        file.setAutoRemove(false);
        file.write(mTextContent.toLocal8Bit());
        const QString fileName = file.fileName();
        file.close();
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    }
}

ConflictResolveDialog::ConflictResolveDialog(QWidget *parent)
    : QDialog(parent), d(new Private(this))
{
    auto *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    // Don't use QDialogButtonBox, order is very important (left on the left, right on the right)
    auto *buttonLayout = new QHBoxLayout();
    auto *takeLeftButton = new QPushButton(this);
    takeLeftButton->setText(i18nc("@action:button", "Take my version"));
    connect(takeLeftButton, SIGNAL(clicked()), this, SLOT(useLocalItem()));
    buttonLayout->addWidget(takeLeftButton);

    auto *takeRightButton = new QPushButton(this);
    takeRightButton->setText(i18nc("@action:button", "Take their version"));
    connect(takeRightButton, SIGNAL(clicked()), this, SLOT(useOtherItem()));
    buttonLayout->addWidget(takeRightButton);

    auto *keepBothButton = new QPushButton(this);
    keepBothButton->setText(i18nc("@action:button", "Keep both versions"));
    buttonLayout->addWidget(keepBothButton);
    connect(keepBothButton, SIGNAL(clicked()), this, SLOT(useBothItems()));

    //keepBothButton->setDefault(true);
    // Don't make "Keep both" available in FatCRM.
    // Keeping two accounts makes a mess, and if the user then cleans up and deletes the wrong
    // one, it breaks the associations with opps and contacts.
    keepBothButton->hide();

    d->mView = new QTextBrowser(this);
    d->mView->setOpenLinks(false);

    QLabel *docuLabel = new QLabel(i18n("Your changes conflict with those made by someone else meanwhile.\n"
                "Unless one version can just be thrown away, you will have to integrate those changes manually.\n"
                "Click on \"Open text editor\" to keep a copy of the texts, then select which version is most correct, then re-open it and modify it again to add what's missing."));
    // TODO it would be even better if this was a clickable link in the label...
    auto *openEditorButton = new QPushButton(this);
    openEditorButton->setText(i18nc("@action:button", "Open text editor"));
    connect(openEditorButton, SIGNAL(clicked()), this, SLOT(openEditor()));
    auto *separateLayout = new QHBoxLayout;
    separateLayout->addWidget(openEditorButton);
    separateLayout->addStretch();

    mainLayout->addWidget(d->mView);
    mainLayout->addWidget(docuLabel);
    mainLayout->addLayout(separateLayout);
    mainLayout->addLayout(buttonLayout);

    // default size is tiny, and there's usually lots of text, so make it much bigger
    create(); // ensure there's a window created
    const QSize availableSize = windowHandle()->screen()->availableSize();
    windowHandle()->resize(availableSize.width() * 0.7, availableSize.height() * 0.5);
    KWindowConfig::restoreWindowSize(windowHandle(), KSharedConfig::openConfig()->group("FatCRMConflictResolveDialog"));
    resize(windowHandle()->size()); // workaround for QTBUG-40584

    QMetaObject::invokeMethod(this, "createReport", Qt::QueuedConnection);
}

ConflictResolveDialog::~ConflictResolveDialog()
{
    KConfigGroup group(KSharedConfig::openConfig()->group("FatCRMConflictResolveDialog"));
    KWindowConfig::saveWindowSize(windowHandle(), group);
    delete d;
}

void ConflictResolveDialog::setConflictingItems(const Item &localItem, const Item &otherItem)
{
    d->mLocalItem = localItem;
    d->mOtherItem = otherItem;
}

void ConflictResolveDialog::setDifferencesInterface(DifferencesAlgorithmInterface *interface)
{
    d->mDiffInterface = interface;
}

ConflictHandler::ResolveStrategy ConflictResolveDialog::resolveStrategy() const
{
    return d->mStrategy;
}

void ConflictResolveDialog::reject()
{
    // Do nothing, don't allow rejecting the dialog. The user must make a choice.
}

#include "moc_conflictresolvedialog.cpp"
