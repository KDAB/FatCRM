#ifndef EDITLISTDIALOG_H
#define EDITLISTDIALOG_H

#include <QDialog>

class KEditListWidget;

/**
 * A dialog for editing a list of strings
 * (with KEditListWidget)
 */
class EditListDialog : public QDialog
{
    Q_OBJECT
public:
    explicit EditListDialog(QWidget *parent = 0);

    void setItems(const QStringList &items);
    QStringList items() const;

signals:

private:
    KEditListWidget *mEditListWidget;

};

#endif // EDITLISTDIALOG_H
