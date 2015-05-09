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

void NotesDialog::addNote(const SugarNote &note)
{
    const QDateTime modified = KDCRMUtils::dateTimeFromString(note.dateModified());
    QString text;
    text += (QString("Note by %1, last modified %2:\n").arg(note.createdByName()).arg(KDCRMUtils::formatDateTime(modified)));
    text += note.name() + '\n'; // called "Subject" in the web gui
    if (!note.description().isEmpty()) {
        text += note.description() + '\n';
    }
    text += '\n';
    m_notes.append(NoteText(modified, text));
}

void NotesDialog::addEmail(const SugarEmail &email)
{
    const QString toList = email.toAddrNames();
    const QDateTime dateSent = KDCRMUtils::dateTimeFromString(email.dateSent());
    QString text;
    text += QString("Mail from %1, to %2. Date: %3\n").arg(email.fromAddrName(), toList, KDCRMUtils::formatDateTime(dateSent));
    text += QString("Subject: %1\n\n").arg(email.name());
    text += email.description() + "\n\n";
    m_notes.append(NoteText(dateSent, text));
}

void NotesDialog::setVisible(bool visible)
{
    if (ui->textEdit->document()->isEmpty()) {
        qSort(m_notes);
        // ascii art FTW
        static const char s_separator[] = "==============================================================================\n";
        foreach (const NoteText &note, m_notes) {
            ui->textEdit->append(s_separator);
            ui->textEdit->append(note.text());
        }
    }
    QDialog::setVisible(visible);
}
