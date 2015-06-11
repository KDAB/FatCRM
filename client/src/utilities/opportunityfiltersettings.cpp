#include "opportunityfiltersettings.h"
#include <klocalizedstring.h>
#include <kdcrmdata/kdcrmutils.h>

OpportunityFilterSettings::OpportunityFilterSettings()
    : mShowOpen(true),
      mShowClosed(false)
{
}

void OpportunityFilterSettings::setAssignees(const QStringList &assignees, const QString &assigneeGroup)
{
    mAssignees = assignees;
    mAssigneeGroup = assigneeGroup;
}

void OpportunityFilterSettings::setCountries(const QStringList &countries, const QString &countryGroup)
{
    mCountries = countries;
    mCountryGroup = countryGroup;
}

void OpportunityFilterSettings::setMaxDate(const QDate &maxDate)
{
    mMaxDate = maxDate;
}

void OpportunityFilterSettings::setModifiedAfter(const QDate &modifiedAfter)
{
    mMaxDate = modifiedAfter;
}

void OpportunityFilterSettings::setModifiedBefore(const QDate &modifiedBefore)
{
    mModifiedBefore = modifiedBefore;
}

void OpportunityFilterSettings::setShowOpenClosed(bool showOpen, bool showClosed)
{
    mShowOpen = showOpen;
    mShowClosed = showClosed;
}

QString OpportunityFilterSettings::filterDescription() const
{
    QString openOrClosed;
    if (!mShowOpen && mShowClosed)
        openOrClosed = i18n("closed");
    else if (mShowOpen && !mShowClosed)
        openOrClosed = i18n("open");

    QString txt;
    if (!mAssignees.isEmpty()) {
        txt = i18n("Assigned to %1", mAssigneeGroup);
    } else if (!mCountries.isEmpty()) {
        txt = i18n("In country %1", mCountryGroup);
    }

    if (!openOrClosed.isEmpty()) {
        txt = i18n("%1, %2", txt, openOrClosed);
    }

    if (!mMaxDate.isNull()) {
        txt = i18n("%1, next step before %2", txt, KDCRMUtils::formatDate(mMaxDate));
    }

    if (!mModifiedAfter.isNull()) {
        const QString after = KDCRMUtils::formatDate(mModifiedAfter);
        const QString before = KDCRMUtils::formatDate(mModifiedBefore);
        if (!mModifiedBefore.isNull()) {
            txt = i18n("%1, modified between %2 and %3", txt, after, before);
        } else {
            txt = i18n("%1, modified after %2", txt, after);
        }
    } else if (!mModifiedBefore.isNull()) {
        const QString before = KDCRMUtils::formatDate(mModifiedBefore);
        txt = i18n("%1, modified before %2", txt, before);
    }

    return txt;
}
