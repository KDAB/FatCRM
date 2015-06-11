#ifndef OPPORTUNITYFILTERSETTINGS_H
#define OPPORTUNITYFILTERSETTINGS_H

#include <QStringList>
#include <QDate>

/**
 * Filter oportunities by assignees OR by country.
 * And with a possible max date.
 * And either open, or closed, or both.
 * And a possible range of "modified date".
 */
class OpportunityFilterSettings
{
public:
    OpportunityFilterSettings();

    void setAssignees(const QStringList &assignees, const QString &assigneeGroup);
    QStringList assignees() const { return mAssignees; }
    QString assigneeGroup() const { return mAssigneeGroup; }
    void setCountries(const QStringList &countries, const QString &countryGroup);
    QStringList countries() const { return mCountries; }
    QString countryGroup() const { return mCountryGroup; }
    void setMaxDate(const QDate &maxDate);
    QDate maxDate() const { return mMaxDate; }
    void setModifiedAfter(const QDate &modifiedAfter);
    QDate modifiedAfter() const { return mModifiedAfter; }
    void setModifiedBefore(const QDate &modifiedBefore);
    QDate modifiedBefore() const { return mModifiedBefore; }
    void setShowOpenClosed(bool showOpen, bool showClosed);
    bool showOpen() const { return mShowOpen; }
    bool showClosed() const { return mShowClosed; }

    QString filterDescription() const;

private:
    QStringList mAssignees; // no filtering if empty
    QStringList mCountries; // no filtering if empty
    QString mAssigneeGroup; // user-visible description for <assignee>
    QString mCountryGroup; // user-visible description for <countries>
    QDate mMaxDate;
    QDate mModifiedBefore, mModifiedAfter;
    bool mShowOpen;
    bool mShowClosed;
};

#endif // OPPORTUNITYFILTERSETTINGS_H
