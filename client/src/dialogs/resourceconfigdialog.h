#ifndef RESOURCECONFIGDIALOG_H
#define RESOURCECONFIGDIALOG_H

#include <QDialog>

class KJob;

class ResourceConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ResourceConfigDialog( QWidget *parent = 0 );

    ~ResourceConfigDialog();

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT( d, void updateButtonStates() )
    Q_PRIVATE_SLOT( d, void addResource() )
    Q_PRIVATE_SLOT( d, void configureResource() )
    Q_PRIVATE_SLOT( d, void removeResource() )
    Q_PRIVATE_SLOT( d, void resourceCreateResult( KJob* ) )
};

#endif
