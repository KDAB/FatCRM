#include "notesdialog.h"
#include "ui_notesdialog.h"
#include "kdcrmutils.h"
#include "clientsettings.h"

NotesDialog::NotesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NotesDialog)
{
    ui->setupUi(this);
    ClientSettings::self()->restoreWindowSize("notesDialog", this);
}

NotesDialog::~NotesDialog()
{
    ClientSettings::self()->saveWindowSize("notesDialog", this);
    delete ui;
}

void NotesDialog::addNote(const QString &user, const QString &modified, const QString &text)
{
    ui->textEdit->append(QString("Note by %1, last modified %2:").arg(user).arg(modified));
    ui->textEdit->append(text);
    ui->textEdit->append("\n\n");
}
