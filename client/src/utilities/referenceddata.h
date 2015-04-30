#ifndef REFERENCEDDATA_H
#define REFERENCEDDATA_H

#include "enums.h"

#include <QObject>

template <typename K, typename V> class QPair;
template <typename K, typename V> class QMap;

/**
 * @brief Per-type singleton holding all reference data, for comboboxes
 * (accounts list, assigned-to list, etc.)
 *
 * Used with a ReferencedDataModel on top.
 */
class ReferencedData : public QObject
{
    Q_OBJECT

public:
    static ReferencedData *instance(ReferencedDataType type);

    ~ReferencedData();

//    void clear();

    void setReferencedData(const QString &id, const QString &data);
    void addMap(const QMap<QString, QString> &idDataMap);

    QString referencedData(const QString &id) const;

    void removeReferencedData(const QString &id);

    QPair<QString, QString> data(int row) const;
    int count() const;

Q_SIGNALS:
    void dataChanged(int row);
    void rowsAboutToBeInserted(int start, int end);
    void rowsInserted();
    void rowsAboutToBeRemoved(int start, int end);
    void rowsRemoved();

private:
    explicit ReferencedData(ReferencedDataType type, QObject *parent = 0);

private:
    class Private;
    Private *const d;
};

#endif
