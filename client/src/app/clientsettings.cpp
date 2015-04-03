#include "clientsettings.h"

#include <QSettings>

Q_GLOBAL_STATIC(ClientSettings, s_self);

ClientSettings *ClientSettings::self()
{
    return s_self();
}

ClientSettings::ClientSettings()
    : m_settings(new QSettings("KDAB", "FatCRM"))
{
}

ClientSettings::~ClientSettings()
{
    delete m_settings;
}

void ClientSettings::setFullUserName(const QString &name)
{
    m_settings->setValue("fullUserName", name);
}

QString ClientSettings::fullUserName() const
{
    return m_settings->value("fullUserName").toString();
}
