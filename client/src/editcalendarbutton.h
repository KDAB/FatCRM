#ifndef EDITCALENDARBUTTON_H
#define EDITCALENDARBUTTON_H

#include <QToolButton>
#include <QCalendarWidget>


class EditCalendarButton : public QToolButton
{
    Q_OBJECT
public:
    explicit EditCalendarButton( QWidget *parent = 0 );

    ~EditCalendarButton();

protected:
    virtual void mousePressEvent( QMouseEvent* e );

    friend class ContactDetails;
    friend class OpportunityDetails;
    friend class LeadDetails;
private:
    inline QCalendarWidget* calendarWidget() { return mCalendar; }

    QCalendarWidget *mCalendar;
};


#endif /* EDITCALENDARBUTTON_H */

