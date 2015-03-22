#ifndef DETAILS_H
#define DETAILS_H

#include "enums.h"

#include <akonadi/item.h>

#include <QtGui>

class Details : public QWidget
{
    Q_OBJECT

public:
    explicit Details(DetailsType type, QWidget *parent = 0);

    ~Details();

    virtual QMap<QString, QString> data(const Akonadi::Item &item) const = 0;
    virtual void updateItem(Akonadi::Item &item, const QMap<QString, QString> &data) const = 0;

    void setData(const QMap<QString, QString> &data) const;
    const QMap<QString, QString> getData() const;
    void clear();

    DetailsType type() const
    {
        return mType;
    }

    QString windowTitle() const;

protected:
    virtual void initialize();

    QStringList industryItems() const;
    QStringList sourceItems() const;
    QStringList currencyItems() const;
    QStringList stageItems() const;
    QStringList salutationItems() const;

private:
    const DetailsType mType;
};
#endif /* DETAILS_H */

