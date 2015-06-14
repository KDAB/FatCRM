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

QSize BetterPlainTextEdit::minimumSizeHint() const
{
    QFontMetrics fm(document()->defaultFont());
    const int lineHeight = fm.height();
    QSize ds = document()->documentLayout()->documentSize().toSize();
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
                 ds.height() + hsbExt + extra);;
}

QSize BetterPlainTextEdit::sizeHint() const
{
    return minimumSizeHint();
}

void BetterPlainTextEdit::slotDocumentSizeChanged()
{
    updateGeometry();
}
