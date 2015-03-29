#ifndef REFERENCEDDATAMODEL_H
#define REFERENCEDDATAMODEL_H

#include "enums.h"

#include <QAbstractListModel>

// List model for filling comboboxes referencing an account/campaign/user/etc.
class ReferencedDataModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        IdRole = Qt::UserRole + 1
    };

    explicit ReferencedDataModel(ReferencedDataType type, QObject *parent = 0);

    ~ReferencedDataModel();

    /* reimpl */ QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    /* reimpl */ int rowCount(const QModelIndex &index) const;

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void dataChanged(ReferencedDataType type))
};

#endif
