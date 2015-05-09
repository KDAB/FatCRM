#ifndef NOTESDIALOG_H
#define NOTESDIALOG_H

#include <QDialog>
#include <QDateTime>

namespace Ui {
class NotesDialog;
}
class SugarEmail;
class SugarNote;

// internal. Generic representation for notes and emails, to sort them by date.
struct NoteText
{
    NoteText() {}
    NoteText(const QDateTime &dt, const QString &text)
        : m_date(dt), m_text(text) {}

    bool operator<(const NoteText &other) const {
        // Most recent at the top
        return m_date > other.m_date;
    }

    QString text() const { return m_text; }
private:
    QDateTime m_date;
    QString m_text;
};

class NotesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NotesDialog(QWidget *parent = 0);
    ~NotesDialog();

    void addNote(const SugarNote &note);
    void addEmail(const SugarEmail &email);

    void setVisible(bool visible) Q_DECL_OVERRIDE;

private:
    QVector<NoteText> m_notes;
    Ui::NotesDialog *ui;
};

#endif // NOTESDIALOG_H
