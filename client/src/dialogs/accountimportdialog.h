#ifndef ACCOUNTIMPORTDIALOG_H
#define ACCOUNTIMPORTDIALOG_H

#include <QDialog>
#include "sugaraccount.h"

namespace Ui {
class AccountImportDialog;
}

class AccountImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AccountImportDialog(QWidget *parent = 0);
    ~AccountImportDialog();

    void setImportedAccounts(const QVector<SugarAccount> &accounts);

protected:
    void accept() Q_DECL_OVERRIDE;

private:

    Ui::AccountImportDialog *ui;
};

#endif // ACCOUNTIMPORTDIALOG_H
