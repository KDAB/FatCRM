#include "editcalendarbutton.h"

#include <QMouseEvent>
#include <QDialog>
#include <QVBoxLayout>

EditCalendarButton::EditCalendarButton( QWidget *parent )
    : QToolButton( parent ),
      mCalendar(new QCalendarWidget()),
      mDialog( new QDialog )
{
    setText( tr( "&Edit" ) );
    mDialog->setWindowTitle( tr( "Calendar" ) );
    QVBoxLayout *dlgLayout = new QVBoxLayout;
    dlgLayout->addWidget( mCalendar );
    mDialog->setLayout( dlgLayout );
}

EditCalendarButton::~EditCalendarButton()
{

}


void EditCalendarButton::mousePressEvent( QMouseEvent* e )
{
    if ( mDialog->isVisible() )
        mDialog->close();
    else {
        mDialog->move( e->globalPos() );
        mDialog->show();
    }
}
