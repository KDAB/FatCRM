#include "opportunityfiltersettings.h"
#include <QSettings>
#include <klocalizedstring.h>
#include <kdcrmdata/kdcrmutils.h>

OpportunityFilterSettings::OpportunityFilterSettings()
    : mMaxDateIndex(0),
      mShowOpen(true),
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

void OpportunityFilterSettings::setMaxDate(const QDate &maxDate, int comboIndex)
{
    mMaxDate = maxDate;
    mMaxDateIndex = comboIndex;
}

void OpportunityFilterSettings::setModifiedAfter(const QDate &modifiedAfter)
{
    mModifiedAfter = modifiedAfter;
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

void OpportunityFilterSettings::save(QSettings &settings, const QString &prefix) const
{
    // I'm not using settings.beginGroup(prefix) because
    // in the load method that would break const-ness...
    // KConfigGroup is missing in QSettings :)
    settings.setValue(prefix + "/assignees", mAssignees);
    settings.setValue(prefix + "/assigneeGroup", mAssignees.isEmpty() ? QString() : mAssigneeGroup);
    settings.setValue(prefix + "/countries", mCountries);
    settings.setValue(prefix + "/countryGroup", mCountries.isEmpty() ? QString() : mCountryGroup);
    settings.setValue(prefix + "/maxDateIndex", mMaxDateIndex);
    settings.setValue(prefix + "/modifiedBefore", mModifiedBefore);
    settings.setValue(prefix + "/modifiedAfter", mModifiedAfter);
    settings.setValue(prefix + "/showOpen", mShowOpen);
    settings.setValue(prefix + "/showClosed", mShowClosed);
}

void OpportunityFilterSettings::load(const QSettings &settings, const QString &prefix)
{
    mAssignees = settings.value(prefix + "/assignees").toStringList();
    mAssigneeGroup = settings.value(prefix + "/assigneeGroup").toString();
    mCountries = settings.value(prefix + "/countries").toStringList();
    mCountryGroup = settings.value(prefix + "/countryGroup").toString();
    mMaxDateIndex = settings.value(prefix + "/maxDateIndex").toInt();
    mModifiedBefore = settings.value(prefix + "/modifiedBefore").toDate();
    mModifiedAfter = settings.value(prefix + "/modifiedAfter").toDate();
    QVariant val = settings.value(prefix + "/showOpen");
    mShowOpen = val.isValid() ? val.toBool() : true;
    mShowClosed = settings.value(prefix + "/showClosed").toBool();
}
