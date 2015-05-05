#ifndef NOTESDIALOG_H
#define NOTESDIALOG_H

#include <QDialog>

namespace Ui {
class NotesDialog;
}
class SugarEmail;
class SugarNote;

class NotesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NotesDialog(QWidget *parent = 0);
    ~NotesDialog();

    void addNote(const SugarNote &note);
    void addEmail(const SugarEmail &email);

private:
    Ui::NotesDialog *ui;
};

#endif // NOTESDIALOG_H
