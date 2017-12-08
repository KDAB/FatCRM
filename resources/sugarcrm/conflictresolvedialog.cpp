/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QTextBrowser>
#include <QPushButton>
#include <QWindow>
#include <QScreen>
#include <QVBoxLayout>
#include <QDialogButtonBox>

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
                            .arg(name)
                            .arg(textToHTML(leftValue))
                            .arg(textToHTML(rightValue)));
            break;
        case ConflictMode:
            mContent.append(QStringLiteral("<tr><td align=\"right\"><b>%1:</b></td><td bgcolor=\"#ff8686\">%2</td><td></td><td bgcolor=\"#ff8686\">%3</td></tr>")
                            .arg(name)
                            .arg(textToHTML(leftValue))
                            .arg(textToHTML(rightValue)));
            break;
        case AdditionalLeftMode:
            mContent.append(QStringLiteral("<tr><td align=\"right\"><b>%1:</b></td><td bgcolor=\"#9cff83\">%2</td><td></td><td></td></tr>")
                            .arg(name)
                            .arg(textToHTML(leftValue)));
            break;
        case AdditionalRightMode:
            mContent.append(QStringLiteral("<tr><td align=\"right\"><b>%1:</b></td><td></td><td></td><td bgcolor=\"#9cff83\">%2</td></tr>")
                            .arg(name)
                            .arg(textToHTML(rightValue)));
            break;
        }
    }

private:
    QString header() const
    {
        QString header = QStringLiteral("<html>");
        header += QStringLiteral("<body text=\"%1\" bgcolor=\"%2\">")
                  .arg(KColorScheme(QPalette::Active, KColorScheme::View).foreground().color().name())
                  .arg(KColorScheme(QPalette::Active, KColorScheme::View).background().color().name());
        header += QLatin1String("<center><table>");
        header += QStringLiteral("<tr><th align=\"center\">%1</th><th align=\"center\">%2</th><td>&nbsp;</td><th align=\"center\">%3</th></tr>")
                  .arg(mNameTitle)
                  .arg(mLeftTitle)
                  .arg(mRightTitle);

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

public: // slots
    void useLocalItem();
    void useOtherItem();
    void useBothItems();
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
}

ConflictResolveDialog::ConflictResolveDialog(QWidget *parent)
    : QDialog(parent), d(new Private(this))
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    QPushButton *user1Button = new QPushButton;
    buttonBox->addButton(user1Button, QDialogButtonBox::ActionRole);
    QPushButton *user2Button = new QPushButton;
    buttonBox->addButton(user2Button, QDialogButtonBox::ActionRole);
    QPushButton *user3Button = new QPushButton;
    buttonBox->addButton(user3Button, QDialogButtonBox::ActionRole);
    user2Button->setText(i18nc("@action:button", "Take right one"));
    user3Button->setText(i18nc("@action:button", "Take left one"));
    user1Button->setText(i18nc("@action:button", "Keep both"));

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    user3Button->setDefault(true);
    connect(user1Button, SIGNAL(clicked()), this, SLOT(useBothItems()));
    connect(user2Button, SIGNAL(clicked()), this, SLOT(useOtherItem()));
    connect(user3Button, SIGNAL(clicked()), this, SLOT(useLocalItem()));

    d->mView = new QTextBrowser(this);
    d->mView->setOpenLinks(false);
    mainLayout->addWidget(d->mView);
    mainLayout->addWidget(buttonBox);

    // default size is tiny, and there's usually lots of text, so make it much bigger
    winId(); // ensure there's a window created
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

#include "moc_conflictresolvedialog.cpp"
