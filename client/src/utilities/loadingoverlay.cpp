#include "loadingoverlay.h"

#include <QResizeEvent>
#include <QPainter>

LoadingOverlay::LoadingOverlay(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    if (parent) {
        parent->installEventFilter(this);
        raise();
    }
}

LoadingOverlay::~LoadingOverlay()
{
}

bool LoadingOverlay::eventFilter(QObject *object, QEvent *event)
{
    if (object == parent()) {
        if (event->type() == QEvent::Resize) {
            QResizeEvent *resizeEvent = static_cast<QResizeEvent*>(event);
            resize(resizeEvent->size());
        }
    }
    return QWidget::eventFilter(object, event);
}

void LoadingOverlay::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(100, 100, 100, 128));
    p.setPen(QColor(200, 200, 255, 255));
    QFont overlayFont = p.font();
    overlayFont.setPointSize(30);
    p.setFont(overlayFont);
    p.drawText(rect(), mMessage, QTextOption(Qt::AlignCenter));
}

void LoadingOverlay::setMessage(const QString &message)
{
    mMessage = message;
    update();
}


