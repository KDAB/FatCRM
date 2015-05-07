#include "editlistdialog.h"

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <keditlistwidget.h>

EditListDialog::EditListDialog(QWidget *parent) :
    QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    mEditListWidget = new KEditListWidget(this);
    layout->addWidget(mEditListWidget);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, Qt::Horizontal, this);
    layout->addWidget(buttonBox);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void EditListDialog::setItems(const QStringList &items)
{
    mEditListWidget->setItems(items);
}

QStringList EditListDialog::items() const
{
    return mEditListWidget->items();
}
