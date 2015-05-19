#ifndef DETAILSDIALOG_H
#define DETAILSDIALOG_H

#include <QDialog>

namespace Akonadi
{
class Item;
}

class Details;
class KJob;

class DetailsDialog : public QDialog
{
    Q_OBJECT
public:
    DetailsDialog(Details *details, QWidget *parent = 0);

    ~DetailsDialog();

    void showNewItem(const QMap<QString, QString> &data);

public Q_SLOTS:
    void setItem(const Akonadi::Item &item);
    void updateItem(const Akonadi::Item &item);

Q_SIGNALS:
    void itemSaved(const Akonadi::Item &item);

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void saveClicked())
    Q_PRIVATE_SLOT(d, void dataModified())
    Q_PRIVATE_SLOT(d, void saveResult(KJob *job))
};

#endif
