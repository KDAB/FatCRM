#ifndef BETTERPLAINTEXTEDIT_H
#define BETTERPLAINTEXTEDIT_H

#include <QPlainTextEdit>

/**
 * @brief A QPlainTextEdit that implements a Qt5-like QAbstractScrollArea::AdjustToContents
 * to avoid it being too big when there's just one line of text in it.
 * (leaving not enough room for more important fields below).
 */
class BetterPlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit BetterPlainTextEdit(QWidget *parent = 0);

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

signals:

private Q_SLOTS:
    void slotDocumentSizeChanged();

};

#endif // BETTERPLAINTEXTEDIT_H
