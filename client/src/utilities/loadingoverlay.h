#ifndef LOADINGOVERLAY_H
#define LOADINGOVERLAY_H

#include <QWidget>

class QEvent;
class QPaintEvent;

class LoadingOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit LoadingOverlay(QWidget *parent = 0);
    ~LoadingOverlay();

    void setMessage(const QString &message);

protected:
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

private:
    QString mMessage;

};

#endif // LOADINGOVERLAY_H
