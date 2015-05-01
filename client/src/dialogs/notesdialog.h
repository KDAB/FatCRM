#ifndef NOTESDIALOG_H
#define NOTESDIALOG_H

#include <QDialog>

namespace Ui {
class NotesDialog;
}

class NotesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NotesDialog(QWidget *parent = 0);
    ~NotesDialog();

    void addNote(const QString &user, const QString &modified, const QString &text);

private:
    Ui::NotesDialog *ui;
};

#endif // NOTESDIALOG_H
