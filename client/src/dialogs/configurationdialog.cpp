#include "configurationdialog.h"
#include "ui_configurationdialog.h"
#include <QListView>
#include <QModelIndex>
#include <QDebug>

ConfigurationDialog::ConfigurationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigurationDialog)
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

void ConfigurationDialog::setAssigneeFilters(const ClientSettings::AssigneeFilters &assigneeFilters)
{
    m_assigneeFilters = assigneeFilters;
    ui->groupListWidget->setItems(m_assigneeFilters.groups());
}

ClientSettings::AssigneeFilters ConfigurationDialog::assigneeFilters() const
{
    return m_assigneeFilters;
}

void ConfigurationDialog::slotGroupListClicked(const QModelIndex &idx)
{
    m_currentFilterRow = idx.row();
    ui->usersListWidget->setEnabled(true);
    qDebug() << "m_currentFilterRow=" << m_currentFilterRow;
    ui->usersListWidget->setItems(m_assigneeFilters.items().at(m_currentFilterRow).users);
}

void ConfigurationDialog::slotGroupRemoved(const QString &group)
{
    Q_UNUSED(group);
    m_assigneeFilters.removeGroup(m_currentFilterRow);
    ui->groupListWidget->setEnabled(!m_assigneeFilters.items().isEmpty());
}

void ConfigurationDialog::slotGroupAdded(const QString &group)
{
    m_assigneeFilters.prependGroup(group);
}

void ConfigurationDialog::slotUsersChanged()
{
    qDebug() << "Updating group" << m_currentFilterRow << "with list" << ui->usersListWidget->items();
    m_assigneeFilters.updateGroup(m_currentFilterRow, ui->usersListWidget->items());
}