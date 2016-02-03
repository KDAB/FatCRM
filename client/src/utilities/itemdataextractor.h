#ifndef ITEMDATAEXTRACTOR_H
#define ITEMDATAEXTRACTOR_H

#include <QObject>

#include <Akonadi/Item>

class ItemDataExtractor : public QObject
{
    Q_OBJECT
public:
    explicit ItemDataExtractor(QObject *parent = 0);
    ~ItemDataExtractor();

    virtual QString itemAddress() const = 0;
    virtual QString idForItem(const Akonadi::Item &item) const = 0;
    QUrl itemUrl(const QString &resourceBaseUrl, const Akonadi::Item &item) const;
    QUrl itemUrl(const QString &resourceBaseUrl, const QString &itemId) const; // called from details dialog
};

#endif // ITEMDATAEXTRACTOR_H
