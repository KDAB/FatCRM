#ifndef REFERENCEDDATA_H
#define REFERENCEDDATA_H

#include "enums.h"

#include <QObject>

template <typename K, typename V> class QMap;
template <typename K, typename V> class QPair;

/**
 * @brief Singleton holding all reference data, for comboboxes
 * (accounts list, assigned-to list, etc.)
 *
 * Used with a ReferencedDataModel on top.
 */
class ReferencedData : public QObject
{
    Q_OBJECT

public:
    static ReferencedData *instance();

    ~ReferencedData();

    void clear(ReferencedDataType type);

    void clearAll();

    void setReferencedData(ReferencedDataType type, const QString &id, const QString &data);

    void removeReferencedData(ReferencedDataType type, const QString &id);

    QPair<QString, QString> data(ReferencedDataType type, int row) const;
    int count(ReferencedDataType type) const;

Q_SIGNALS:
    void cleared(ReferencedDataType type);
    void dataChanged(ReferencedDataType type);

private:
    explicit ReferencedData(QObject *parent = 0);

private:
    class Private;
    Private *const d;
};

#endif
