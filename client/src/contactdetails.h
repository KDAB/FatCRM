#ifndef CONTACTDETAILS_H
#define CONTACTDETAILS_H

#include "ui_contactdetails.h"

#include <akonadi/item.h>

#include <QtGui/QWidget>


class ContactDetails : public QWidget
{
    Q_OBJECT
public:
    explicit ContactDetails( QWidget *parent = 0 );

    ~ContactDetails();

    void setItem( const Akonadi::Item &item );
    void clearFields();

private:
    Ui_ContactDetails mUi;
};

#endif /* CONTACTDETAILS_H */

