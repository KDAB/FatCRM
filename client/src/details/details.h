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

    void setData(const QMap<QString, QString> &data, QGroupBox *informationGB);
    const QMap<QString, QString> getData() const;
    void clear();

    void setResourceIdentifier(const QByteArray &ident);

    DetailsType type() const
    {
        return mType;
    }

    QString windowTitle() const;

Q_SIGNALS:
    void modified();

protected:
    virtual void initialize();
    QByteArray resourceIdentifier() const { return mResourceIdentifier; }

    QStringList industryItems() const;
    QStringList sourceItems() const;
    QStringList currencyItems() const;
    QStringList stageItems() const;
    QStringList salutationItems() const;

    virtual void setDataInternal(const QMap<QString, QString> &) const {}

private:
    const DetailsType mType;
    QByteArray mResourceIdentifier;
    QStringList mKeys;
};
#endif /* DETAILS_H */

