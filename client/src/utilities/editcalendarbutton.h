#ifndef EDITCALENDARBUTTON_H
#define EDITCALENDARBUTTON_H

#include <QToolButton>
#include <QCalendarWidget>

class EditCalendarButton : public QToolButton
{
    Q_OBJECT
public:
    explicit EditCalendarButton(QWidget *parent = 0);

    ~EditCalendarButton();

protected:
    virtual void mousePressEvent(QMouseEvent *e);

    friend class ContactDetails;
    friend class OpportunityDetails;
    friend class LeadDetails;
    friend class CampaignDetails;
private:
    inline QCalendarWidget *calendarWidget()
    {
        return mCalendar;
    }
    inline QDialog *calendarDialog()
    {
        return mDialog;
    }

    QCalendarWidget *mCalendar;
    QDialog *mDialog;

};

#endif /* EDITCALENDARBUTTON_H */

