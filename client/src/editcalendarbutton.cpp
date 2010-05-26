#include "editcalendarbutton.h"

#include <QMouseEvent>

EditCalendarButton::EditCalendarButton( QWidget *parent )
    : QToolButton( parent ), mCalendar(new QCalendarWidget())
{
    setText( tr( "&Edit" ) );
}

EditCalendarButton::~EditCalendarButton()
{
    delete mCalendar;
}


void EditCalendarButton::mousePressEvent( QMouseEvent* e )
{
    if ( mCalendar->isVisible() )
        mCalendar->hide();
    else {
        mCalendar->move( e->globalPos() );
        mCalendar->show();
        mCalendar->raise();
    }
}
