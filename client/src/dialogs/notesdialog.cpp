#include "notesdialog.h"
#include "ui_notesdialog.h"
#include "kdcrmutils.h"
#include "clientsettings.h"
#include <kdcrmdata/sugarnote.h>
#include <kdcrmdata/sugaremail.h>

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

// ascii art FTW
static const char s_separator[] = "==============================================================================\n";

void NotesDialog::addNote(const SugarNote &note)
{
    ui->textEdit->append(s_separator);
    const QString modified = KDCRMUtils::formatTimestamp(note.dateModified());
    ui->textEdit->append(QString("Note by %1, last modified %2:").arg(note.createdByName()).arg(modified));
    ui->textEdit->append(note.name()); // called "Subject" in the web gui
    if (!note.description().isEmpty()) {
        ui->textEdit->append(note.description());
        ui->textEdit->append("\n");
    }
    ui->textEdit->append("\n");
}


void NotesDialog::addEmail(const SugarEmail &email)
{
    ui->textEdit->append(s_separator);
    const QString toList = email.toAddrNames();
    const QString dateSent = KDCRMUtils::formatTimestamp(email.dateSent());
    ui->textEdit->append(QString("Mail from %1, to %2. Date: %3").arg(email.fromAddrName(), toList, dateSent));
    ui->textEdit->append(QString("Subject: %1").arg(email.name()));
    ui->textEdit->append(QString("\n<message body not imported yet>\n"));
    // TODO description, from EmailTexts module
    // https://community.sugarcrm.com/sugarcrm/topics/i_want_email_body_in_database_but_getting_message_id_any_help_on_it
}
