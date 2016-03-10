#ifndef NOTESWINDOW_H
#define NOTESWINDOW_H

#include <QWidget>
#include <QDateTime>

namespace Ui {
class NotesWindow;
}
class SugarEmail;
class SugarNote;

// internal. Generic representation for notes and emails, to sort them by date.
struct NoteText
{
    NoteText() {}
    NoteText(const QDateTime &dt, const QString &htmlHeader, const QString &text)
        : m_date(dt), m_htmlHeader(htmlHeader), m_text(text) {}

    bool operator<(const NoteText &other) const {
        // Most recent at the top
        return m_date > other.m_date;
    }

    QString text() const { return m_text; }
    QString htmlHeader() const { return m_htmlHeader; }
private:
    QDateTime m_date;
    QString m_htmlHeader;
    QString m_text;
};

class NotesWindow : public QWidget
{
    Q_OBJECT

public:
    explicit NotesWindow(QWidget *parent = 0);
    ~NotesWindow();

    void addNote(const SugarNote &note);
    void addEmail(const SugarEmail &email);

    void setVisible(bool visible) Q_DECL_OVERRIDE;

private slots:
    void on_buttonBox_rejected();

private:
    QVector<NoteText> m_notes;
    Ui::NotesWindow *ui;
};

#endif // NotesWindow_H

