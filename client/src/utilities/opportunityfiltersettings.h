#ifndef OPPORTUNITYFILTERSETTINGS_H
#define OPPORTUNITYFILTERSETTINGS_H

#include <QStringList>
#include <QDate>
class QSettings;

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
    // default copy ctor and operator= are wanted.

    void setAssignees(const QStringList &assignees, const QString &assigneeGroup);
    QStringList assignees() const { return mAssignees; }
    QString assigneeGroup() const { return mAssigneeGroup; }
    void setCountries(const QStringList &countries, const QString &countryGroup);
    QStringList countries() const { return mCountries; }
    QString countryGroup() const { return mCountryGroup; }
    void setMaxDate(const QDate &maxDate, int comboIndex);
    QDate maxDate() const { return mMaxDate; }
    int maxDateIndex() const { return mMaxDateIndex; }
    void setModifiedAfter(const QDate &modifiedAfter);
    QDate modifiedAfter() const { return mModifiedAfter; }
    void setModifiedBefore(const QDate &modifiedBefore);
    QDate modifiedBefore() const { return mModifiedBefore; }
    void setShowOpenClosed(bool showOpen, bool showClosed);
    bool showOpen() const { return mShowOpen; }
    bool showClosed() const { return mShowClosed; }

    QString filterDescription() const;

    void save(QSettings &settings, const QString &prefix) const;
    void load(const QSettings &settings, const QString &prefix);

private:
    QStringList mAssignees; // no filtering if empty
    QStringList mCountries; // no filtering if empty
    QString mAssigneeGroup; // user-visible description for <assignee>
    QString mCountryGroup; // user-visible description for <countries>
    QDate mMaxDate;
    QDate mModifiedBefore, mModifiedAfter;
    int mMaxDateIndex;
    bool mShowOpen;
    bool mShowClosed;
};

#endif // OPPORTUNITYFILTERSETTINGS_H
