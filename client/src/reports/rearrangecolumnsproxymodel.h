#ifndef REARRANGECOLUMNSPROXYMODEL_H
#define REARRANGECOLUMNSPROXYMODEL_H

#include <QIdentityProxyModel>
#include <QDebug>

/**
 * Selects columns from the source model, in any order.
 *
 * Supported: not selecting all columns (i.e. this is not just rearranging,
 *   but also filtering out some columns).
 * Supported: source model being a tree, dataChanged.
 * Not supported: selecting the same column more than once.
 *
 */
class RearrangeColumnsProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit RearrangeColumnsProxyModel(QObject *parent = 0);

    // API

    // Set the chosen source columns, in the desired order for the proxy columns
    // columns[proxyColumn=0] is the source column to show in the first proxy column, etc.
    void setSourceColumns(const QVector<int> &columns);

    // Implementation

    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &child) const Q_DECL_OVERRIDE;

    QModelIndex mapFromSource(const QModelIndex & sourceIndex) const Q_DECL_OVERRIDE;
    QModelIndex mapToSource(const QModelIndex & proxyIndex) const Q_DECL_OVERRIDE;

private:
    int proxyColumnForSourceColumn(int sourceColumn) const;
    int sourceColumnForProxyColumn(int proxyColumn) const;

private:
    // Configuration
    QVector<int> m_sourceColumns;
};

#endif
