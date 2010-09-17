#ifndef DETAILS_H
#define DETAILS_H

#include <QtGui>

#include <akonadi/item.h>

class Details : public QWidget
{
    Q_OBJECT

public:
    explicit Details( QWidget *parent = 0 );

    ~Details();

    virtual QMap<QString, QString> data( const Akonadi::Item item ) const = 0;

protected:
    virtual void initialize();

    QStringList industryItems() const;
    QStringList sourceItems() const;
    QStringList currencyItems() const;
    QStringList stageItems() const;
    QStringList salutationItems() const;

};
#endif /* DETAILS_H */

