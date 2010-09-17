#ifndef DETAILS_H
#define DETAILS_H

#include <QtGui>

class Details : public QWidget
{
    Q_OBJECT

public:
    explicit Details( QWidget *parent = 0 );

    ~Details();

protected:
    virtual void initialize();
    QStringList industryItems() const;
    QStringList sourceItems() const;
    QStringList currencyItems() const;
    QStringList stageItems() const;
    QStringList salutationItems() const;

};
#endif /* DETAILS_H */

