#ifndef CLIENTSETTINGS_H
#define CLIENTSETTINGS_H

class QSettings;
class QString;

class ClientSettings
{
public:
    static ClientSettings *self();

    void setFullUserName(const QString &name);
    QString fullUserName() const;

    ClientSettings();
    ~ClientSettings();
private:
    QSettings *m_settings;
};

#endif // CLIENTSETTINGS_H
