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
    void enableFields();
    void disableFields();
    void setModifyFlag();

Q_SIGNALS:
    void saveContact( const Akonadi::Item &item );
    void modifyContact( const Akonadi::Item &item );

private:
    void initialize();

    bool mModifyFlag;
    Ui_ContactDetails mUi;

private Q_SLOTS:
    void slotEnableSaving();
    void slotSaveContact();
};

#endif /* CONTACTDETAILS_H */

