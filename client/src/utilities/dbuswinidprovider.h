#ifndef DBUSWINIDPROVIDER_H
#define DBUSWINIDPROVIDER_H

#include <QObject>

class QWidget;

class DBusWinIdProvider : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.akonaditray")

public:
    explicit DBusWinIdProvider(QWidget *referenceWindow);

    ~DBusWinIdProvider();

public Q_SLOTS:
    Q_SCRIPTABLE qlonglong getWinId();

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void tryRegister())
};

#endif
