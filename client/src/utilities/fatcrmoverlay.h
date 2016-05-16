#ifndef FATCRMOVERLAY_H
#define FATCRMOVERLAY_H

#include <QWidget>

class QEvent;
class QPaintEvent;

class FatCRMOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit FatCRMOverlay(QWidget *parent = 0);
    ~FatCRMOverlay();

    void setMessage(const QString &message);

protected:
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

private:
    QString mMessage;

};

#endif // FATCRMOVERLAY_H
