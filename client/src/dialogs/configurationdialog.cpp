#include "configurationdialog.h"
#include "editlistdialog.h"
#include "ui_configurationdialog.h"
#include <QListView>
#include <QModelIndex>
#include <QDebug>

ConfigurationDialog::ConfigurationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigurationDialog),
    m_currentFilterRow(-1),
    m_currentCountryGroupRow(-1)
{
    ui->setupUi(this);

    connect(ui->groupListWidget->listView(), SIGNAL(clicked(QModelIndex)),
            this, SLOT(slotGroupListClicked(QModelIndex)));
    connect(ui->groupListWidget, SIGNAL(removed(QString)),
            this, SLOT(slotGroupRemoved(QString)));
    connect(ui->groupListWidget, SIGNAL(added(QString)),
            this, SLOT(slotGroupAdded(QString)));
    connect(ui->usersListWidget, SIGNAL(changed()),
            this, SLOT(slotUsersChanged()));
    ui->usersListWidget->setEnabled(false);

    connect(ui->countryListWidget->listView(), SIGNAL(clicked(QModelIndex)),
            this, SLOT(slotCountryListClicked(QModelIndex)));
    connect(ui->countryListWidget, SIGNAL(removed(QString)),
            this, SLOT(slotCountryRemoved(QString)));
    connect(ui->countryListWidget, SIGNAL(added(QString)),
            this, SLOT(slotCountryAdded(QString)));
    connect(ui->editSelectedCountryFilter, SIGNAL(clicked()),
            this, SLOT(slotEditCountryGroup()));
    ui->editSelectedCountryFilter->setEnabled(false);
}

ConfigurationDialog::~ConfigurationDialog()
{
    delete ui;
}

void ConfigurationDialog::setFullUserName(const QString &fullUserName)
{
    ui->fullName->setText(fullUserName);
}

QString ConfigurationDialog::fullUserName() const
{
    return ui->fullName->text();
}

void ConfigurationDialog::setAssigneeFilters(const ClientSettings::GroupFilters &assigneeFilters)
{
    m_assigneeFilters = assigneeFilters;
    ui->groupListWidget->setItems(m_assigneeFilters.groupNames());
}

ClientSettings::GroupFilters ConfigurationDialog::assigneeFilters() const
{
    return m_assigneeFilters;
}

void ConfigurationDialog::setCountryFilters(const ClientSettings::GroupFilters &countryFilters)
{
    m_countryFilters = countryFilters;
    ui->countryListWidget->setItems(m_countryFilters.groupNames());
}

ClientSettings::GroupFilters ConfigurationDialog::countryFilters() const
{
    return m_countryFilters;
}

void ConfigurationDialog::slotGroupListClicked(const QModelIndex &idx)
{
    m_currentFilterRow = idx.row();
    ui->usersListWidget->setEnabled(true);
    qDebug() << "m_currentFilterRow=" << m_currentFilterRow;
    ui->usersListWidget->setItems(m_assigneeFilters.groups().at(m_currentFilterRow).entries);
}

void ConfigurationDialog::slotGroupRemoved(const QString &group)
{
    Q_UNUSED(group);
    m_assigneeFilters.removeGroup(m_currentFilterRow);
    ui->groupListWidget->setEnabled(!m_assigneeFilters.groups().isEmpty());
}

void ConfigurationDialog::slotGroupAdded(const QString &group)
{
    m_assigneeFilters.prependGroup(group);
}

void ConfigurationDialog::slotCountryListClicked(const QModelIndex &idx)
{
    m_currentCountryGroupRow = idx.row();
    ui->editSelectedCountryFilter->setEnabled(true);
}

void ConfigurationDialog::slotCountryRemoved(const QString &country)
{
    Q_UNUSED(country);
    m_countryFilters.removeGroup(m_currentCountryGroupRow);
}

void ConfigurationDialog::slotCountryAdded(const QString &country)
{
    m_countryFilters.prependGroup(country);
}

void ConfigurationDialog::slotUsersChanged()
{
    qDebug() << "Updating group" << m_currentFilterRow << "with list" << ui->usersListWidget->items();
    m_assigneeFilters.updateGroup(m_currentFilterRow, ui->usersListWidget->items());
}

void ConfigurationDialog::slotEditCountryGroup()
{
    EditListDialog dialog(this);
    dialog.setItems(m_countryFilters.groups().at(m_currentCountryGroupRow).entries);
    if (dialog.exec()) {
        m_countryFilters.updateGroup(m_currentCountryGroupRow, dialog.items());
    }
}
