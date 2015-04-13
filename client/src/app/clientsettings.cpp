#include "clientsettings.h"

#include <QWidget>
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

void ClientSettings::sync()
{
    m_settings->sync();
}

void ClientSettings::setShowDetails(const QString &typeString, bool on)
{
    m_settings->setValue("showDetails/" + typeString, on);
}

bool ClientSettings::showDetails(const QString &typeString) const
{
    const QVariant value = m_settings->value("showDetails/" + typeString);
    return value.isValid() ? value.toBool() : true;
}

void ClientSettings::setFullUserName(const QString &name)
{
    m_settings->setValue("fullUserName", name);
}

QString ClientSettings::fullUserName() const
{
    return m_settings->value("fullUserName").toString();
}

void ClientSettings::saveWindowSize(const QString &windowId, QWidget *window)
{
    m_settings->setValue("windowSize/" + windowId, window->size());
}

void ClientSettings::restoreWindowSize(const QString &windowId, QWidget *window) const
{
    const QSize sz = m_settings->value("windowSize/" + windowId).toSize();
    if (sz.isValid()) {
        window->resize(sz);
    }
}

void ClientSettings::setAssigneeFilters(const ClientSettings::AssigneeFilters &filters)
{
    m_settings->setValue("assigneeFilters", filters.toString());
    emit assigneeFiltersChanged();
}

ClientSettings::AssigneeFilters ClientSettings::assigneeFilters() const
{
    ClientSettings::AssigneeFilters ret;
    ret.loadFromString(m_settings->value("assigneeFilters").toString());
    return ret;
}

QString ClientSettings::AssigneeFilters::toString() const
{
    QString ret;
    for (int i = 0 ; i < m_items.count() ; ++i) {
        const Item& item = m_items.at(i);
        ret += item.group + ';';
        ret += item.users.join(QString(QLatin1Char(';')));
        if (i + 1 < m_items.count())
            ret += '|';
    }
    return ret;
}

void ClientSettings::AssigneeFilters::loadFromString(const QString &str)
{
    Q_FOREACH(const QString &itemStr, str.split('|')) {
        const QStringList lst = itemStr.split(';', QString::SkipEmptyParts);
        Item item;
        item.group = lst.first();
        item.users = lst.mid(1);
        m_items.append(item);
    }
}

void ClientSettings::AssigneeFilters::removeGroup(int row)
{
    m_items.remove(row);
}

void ClientSettings::AssigneeFilters::prependGroup(const QString &group)
{
    Item item;
    item.group = group;
    m_items.prepend(item);
}

void ClientSettings::AssigneeFilters::updateGroup(int row, const QStringList &users)
{
    m_items[row].users = users;
}

QStringList ClientSettings::AssigneeFilters::groups() const
{
    QStringList ret;
    Q_FOREACH(const Item &item, m_items) {
        ret.append(item.group);
    }
    return ret;
}
