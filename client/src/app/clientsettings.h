#ifndef CLIENTSETTINGS_H
#define CLIENTSETTINGS_H

class QSettings;
#include <QSize>
#include <QStringList>
#include <QVector>

class ClientSettings : public QObject
{
    Q_OBJECT
public:
    static ClientSettings *self();

    void sync();

    void setShowDetails(const QString &typeString, bool on);
    bool showDetails(const QString &typeString) const;

    void setFullUserName(const QString &name);
    QString fullUserName() const;

    void saveWindowSize(const QString &windowId, QWidget *window);
    void restoreWindowSize(const QString &windowId, QWidget *window) const;

    void setVisibleColumns(const QString &viewId, const QStringList &names);
    QStringList visibleColumns(const QString &viewId, const QStringList &defaultColumns) const;

    class AssigneeFilters
    {
    public:
        struct Item {
            QString group;
            QStringList users;
        };

        QString toString() const;
        void loadFromString(const QString &str);
        const QVector<Item>& items() const { return m_items; }
        void removeGroup(int row);
        void prependGroup(const QString &group);
        void updateGroup(int row, const QStringList &users);
        QStringList groups() const;
    private:
        QVector<Item> m_items;
    };
    void setAssigneeFilters(const AssigneeFilters &filters);
    AssigneeFilters assigneeFilters() const;

    ClientSettings();
    ~ClientSettings();

Q_SIGNALS:
    void assigneeFiltersChanged();

private:
    QSettings *m_settings;
};

#endif // CLIENTSETTINGS_H
