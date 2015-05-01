#ifndef OPPORTUNITYDETAILS_H
#define OPPORTUNITYDETAILS_H

#include "details.h"

namespace Ui
{
class OpportunityDetails;
}
class NotesRepository;

class OpportunityDetails : public Details
{
    Q_OBJECT
public:
    explicit OpportunityDetails(QWidget *parent = 0);

    ~OpportunityDetails();

    void setNotesRepository(NotesRepository *notesRepo) Q_DECL_OVERRIDE { mNotesRepository = notesRepo; }

private Q_SLOTS:
    void slotAutoNextStepDate();

    void on_viewNotesButton_clicked();

private:
    /*reimp*/ void initialize();
    /*reimp*/ QMap<QString, QString> data(const Akonadi::Item &item) const;
    /*reimp*/ void updateItem(Akonadi::Item &item, const QMap<QString, QString> &data) const;
    /*reimp*/ void setDataInternal(const QMap<QString, QString> &data) const;

    QStringList typeItems() const;
    QStringList stageItems() const;

private:
    Ui::OpportunityDetails *mUi;
    NotesRepository *mNotesRepository;
};

#endif /* OPPORTUNITYDETAILS_H */

