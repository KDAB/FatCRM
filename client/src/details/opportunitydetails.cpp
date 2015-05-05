#include "opportunitydetails.h"

#include "ui_opportunitydetails.h"
#include "referenceddatamodel.h"
#include "kdcrmutils.h"
#include "notesrepository.h"

#include <kdcrmdata/sugaropportunity.h>
#include "sugarresourcesettings.h"
#include <notesdialog.h>
#include <KLocale>

OpportunityDetails::OpportunityDetails(QWidget *parent)
    : Details(Opportunity, parent), mUi(new Ui::OpportunityDetails)
{
    mUi->setupUi(this);
    mUi->urllabel->setOpenExternalLinks(true);
    mUi->urllabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    mUi->nextCallDate->calendarWidget()->setVerticalHeaderFormat(QCalendarWidget::ISOWeekNumbers);
    mUi->nextCallDate->setNullable(true);
    mUi->dateClosed->calendarWidget()->setVerticalHeaderFormat(QCalendarWidget::ISOWeekNumbers);
    mUi->dateClosed->setNullable(true);
    initialize();
}

OpportunityDetails::~OpportunityDetails()
{
    delete mUi;
}

void OpportunityDetails::initialize()
{
    ReferencedDataModel::setModelForCombo(mUi->accountName, AccountRef);
    mUi->opportunityType->addItems(typeItems());
    mUi->leadSource->addItems(sourceItems());
    mUi->salesStage->addItems(stageItems());
    ReferencedDataModel::setModelForCombo(mUi->assignedUserName, AssignedToRef);
    connect(mUi->nextStepDateAutoButton, SIGNAL(clicked()), this, SLOT(slotAutoNextStepDate()));
}

void OpportunityDetails::slotAutoNextStepDate()
{
    mUi->nextCallDate->setDate(QDate::currentDate().addDays(14));
}

QStringList OpportunityDetails::typeItems() const
{
    QStringList types;
    types << QString("") << QString("Existing Business")
          << QString("New Business");
    return types;
}

QStringList OpportunityDetails::stageItems() const
{
    QStringList stages;
    stages << QString("")
           << QString("Prospecting")
           << QString("Qualification")
           << QString("Needs Analysis")
           << QString("Value Proposition")
           << QString("Id.Decision Makers")
           << QString("Perception Analysis")
           << QString("Proposal/Price Quote")
           << QString("Negociation/Review")
           << QString("Closed Won")
           << QString("Closed Lost");
    return stages;
}

QMap<QString, QString> OpportunityDetails::data(const Akonadi::Item &item) const
{
    SugarOpportunity opportunity = item.payload<SugarOpportunity>();
    return opportunity.data();
}

void OpportunityDetails::updateItem(Akonadi::Item &item, const QMap<QString, QString> &data) const
{
    SugarOpportunity opportunity;
    if (item.hasPayload<SugarOpportunity>()) {
        opportunity = item.payload<SugarOpportunity>();
    }
    opportunity.setData(data);

    item.setMimeType(SugarOpportunity::mimeType());
    item.setPayload<SugarOpportunity>(opportunity);
}

void OpportunityDetails::setDataInternal(const QMap<QString, QString> &data) const
{
    const QString resourceId = resourceIdentifier();
    if (!resourceId.isEmpty()) {

        // Cache, to avoid making DBus calls every time
        static QString lastResourceId;
        static QString lastBaseUrl;
        if (lastResourceId != resourceId) {
            OrgKdeAkonadiSugarCRMSettingsInterface iface(
                        QLatin1String("org.freedesktop.Akonadi.Resource.") + resourceId, QLatin1String("/Settings"), QDBusConnection::sessionBus() );
            lastBaseUrl = iface.host();
            if (!lastBaseUrl.isEmpty()) // i.e. success
                lastResourceId = resourceId;
        }

        const QString id = data.value("id");
        if (!id.isEmpty()) {
            const QString url = lastBaseUrl + "?action=DetailView&module=Opportunities&record=" + id;
            mUi->urllabel->setText(QString("<a href=\"%1\">Open Opportunity in Web Browser</a>").arg(url));
        }
    }
    const int notes = mNotesRepository->notesForOpportunity(id()).count() + mNotesRepository->emailsForOpportunity(id()).count();
    mUi->viewNotesButton->setEnabled(notes > 0);
    const QString buttonText = (notes == 0) ? i18n("View Notes") : i18np("View 1 Note", "View %1 Notes", notes);
    mUi->viewNotesButton->setText(buttonText);
}

void OpportunityDetails::on_viewNotesButton_clicked()
{
    const QVector<SugarNote> notes = mNotesRepository->notesForOpportunity(id());
    kDebug() << notes.count() << "notes found for opp" << id();
    const QVector<SugarEmail> emails = mNotesRepository->emailsForOpportunity(id());
    kDebug() << emails.count() << "emails found for opp" << id();
    NotesDialog *dlg = new NotesDialog(this);
    foreach(const SugarNote &note, notes) {
        dlg->addNote(note);
    }
    foreach(const SugarEmail &email, emails) {
        dlg->addEmail(email);
    }
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}
