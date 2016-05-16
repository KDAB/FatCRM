#include "fatcrmoverlay.h"

#include <QResizeEvent>
#include <QPainter>

FatCRMOverlay::FatCRMOverlay(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    if (parent) {
        parent->installEventFilter(this);
        raise();
    }
}

FatCRMOverlay::~FatCRMOverlay()
{
}

bool FatCRMOverlay::eventFilter(QObject *object, QEvent *event)
{
    if (object == parent()) {
        if (event->type() == QEvent::Resize) {
            QResizeEvent *resizeEvent = static_cast<QResizeEvent*>(event);
            resize(resizeEvent->size());
        }
    }
    return QWidget::eventFilter(object, event);
}

void FatCRMOverlay::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(100, 100, 100, 128));
    p.setPen(QColor(200, 200, 255, 255));
    QFont overlayFont = p.font();
    overlayFont.setPointSize(30);
    p.setFont(overlayFont);
    p.drawText(rect(), mMessage, QTextOption(Qt::AlignCenter));
}

void FatCRMOverlay::setMessage(const QString &message)
{
    mMessage = message;
    update();
}


