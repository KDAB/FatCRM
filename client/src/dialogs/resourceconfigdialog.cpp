/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: David Faure <david.faure@kdab.com>
           Michel Boyer de la Giroday <michel.giroday@kdab.com>
           Kevin Krammer <kevin.krammer@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "resourceconfigdialog.h"

#include "ui_resourceconfigdialog.h"

#include <Akonadi/AgentFilterProxyModel>
#include <Akonadi/AgentInstance>
#include <Akonadi/AgentInstanceCreateJob>
#include <Akonadi/AgentInstanceWidget>
#include <Akonadi/AgentManager>
#include <Akonadi/AgentType>
#include <Akonadi/AgentTypeDialog>
#include <Akonadi/AgentTypeModel>
#include <Akonadi/Control>
using namespace Akonadi;

#include <KDebug>

#include <QAbstractItemView>
#include <QItemSelectionModel>
#include <QMessageBox>

class ResourceConfigDialog::Private
{
    ResourceConfigDialog *const q;

public:
    Private(ResourceConfigDialog *parent) : q(parent), mApplyButton(0) {}

public:
    Ui::ResourceConfigDialog mUi;
    QPushButton *mApplyButton;

    AgentInstance mSelectedResource;
    AgentInstance mCurrentResource;

public: // slots
    void updateButtonStates();
    void addResource();
    void configureResource();
    void syncResources();
    void removeResource();
    void resourceCreateResult(KJob *job);
    void applyResourceSelection();
    void agentInstanceChanged(const AgentInstance &agent);
};

void ResourceConfigDialog::Private::updateButtonStates()
{
    bool canConfigure = false;
    bool canDelete = false;

    mCurrentResource = AgentInstance();

    const QList<AgentInstance> selectedResources = mUi.resources->selectedAgentInstances();
    switch (selectedResources.count()) {
    case 0:
        break;

    case 1:
        mCurrentResource = selectedResources.at(0);
        canConfigure = !mCurrentResource.type().capabilities().contains(QLatin1String("NoConfig"));
        canDelete = true;
        break;

    default:
        canDelete = true;
        break;
    }

    mUi.configureResource->setEnabled(canConfigure);
    mUi.removeResource->setEnabled(canDelete);
    mUi.syncResource->setEnabled(selectedResources.count() > 0);
    mApplyButton->setEnabled(mCurrentResource.isValid());
}

void ResourceConfigDialog::Private::addResource()
{
    AgentTypeDialog dialog(q);

    AgentFilterProxyModel *filterModel = dialog.agentFilterProxyModel();
    filterModel->addCapabilityFilter(QLatin1String("KDCRM"));

    if (dialog.exec() == QDialog::Accepted) {
        const AgentType resourceType = dialog.agentType();
        if (resourceType.isValid()) {
            AgentInstanceCreateJob *job = new AgentInstanceCreateJob(resourceType);
            job->configure(q);
            connect(job, SIGNAL(result(KJob*)), q, SLOT(resourceCreateResult(KJob*)));
            job->start();
        }
    }
}

void ResourceConfigDialog::Private::configureResource()
{
    if (mCurrentResource.isValid()) {
        mCurrentResource.configure(q);
    }
}

void ResourceConfigDialog::Private::syncResources()
{
    const QList<AgentInstance> selectedResources = mUi.resources->selectedAgentInstances();
    Q_FOREACH (AgentInstance resource, selectedResources) {
        resource.synchronize();
    }
}

void ResourceConfigDialog::Private::removeResource()
{
    const QList<AgentInstance> selectedResources = mUi.resources->selectedAgentInstances();
    Q_FOREACH (const AgentInstance &resource, selectedResources) {
        if (resource == mSelectedResource) {
            mSelectedResource = AgentInstance();
        }
        if (resource == mCurrentResource) {
            mCurrentResource = AgentInstance();
            emit q->resourceSelected(mCurrentResource);
        }
        AgentManager::self()->removeInstance(resource);
    }
}

void ResourceConfigDialog::Private::resourceCreateResult(KJob *job)
{
    kDebug() << "error=" << job->error() << "string=" << job->errorString();
    if (job->error() != 0) {
        QMessageBox::critical(q, i18nc("@title:window", "Failed to create CRM connector"),
                              job->errorString());
    }
}

void ResourceConfigDialog::Private::applyResourceSelection()
{
    q->accept();
    emit q->resourceSelected(mCurrentResource);
}

void ResourceConfigDialog::Private::agentInstanceChanged(const AgentInstance &agent)
{
    if (agent == mCurrentResource) {
        mCurrentResource = agent;
    }
}

ResourceConfigDialog::ResourceConfigDialog(QWidget *parent)
    : QDialog(parent), d(new Private(this))
{
    d->mUi.setupUi(this);

    Akonadi::Control::widgetNeedsAkonadi(this);

    AgentFilterProxyModel *filterModel = d->mUi.resources->agentFilterProxyModel();
    filterModel->addCapabilityFilter(QLatin1String("KDCRM"));

    // Remove this and use filterModel on the last line when everyone has kdepimlibs >= 4.14.7
#if 1
    WorkaroundFilterProxyModel *workaround = new WorkaroundFilterProxyModel(this);
    workaround->setSourceModel(filterModel);
    d->mUi.resources->view()->setModel(workaround);
#endif

    connect(d->mUi.resources->view()->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(updateButtonStates()));

    connect(d->mUi.addResource, SIGNAL(clicked()), this, SLOT(addResource()));
    connect(d->mUi.configureResource, SIGNAL(clicked()), this, SLOT(configureResource()));
    connect(d->mUi.syncResource, SIGNAL(clicked()), this, SLOT(syncResources()));
    connect(d->mUi.removeResource, SIGNAL(clicked()), this, SLOT(removeResource()));

    d->mApplyButton = d->mUi.buttonBox->button(QDialogButtonBox::Apply);
    d->mApplyButton->setText(i18nc("@action:button", "Select for work"));
    connect(d->mApplyButton, SIGNAL(clicked()), this, SLOT(applyResourceSelection()));
    connect(d->mUi.resources->view(), SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(applyResourceSelection()));

    connect(AgentManager::self(), SIGNAL(instanceNameChanged(Akonadi::AgentInstance)),
            SLOT(agentInstanceChanged(Akonadi::AgentInstance)));
    connect(AgentManager::self(), SIGNAL(instanceOnline(Akonadi::AgentInstance,bool)),
            SLOT(agentInstanceChanged(Akonadi::AgentInstance)));

    d->updateButtonStates();
}

ResourceConfigDialog::~ResourceConfigDialog()
{
    delete d;
}

void ResourceConfigDialog::resourceSelectionChanged(const AgentInstance &resource)
{
    d->mSelectedResource = resource;

    QItemSelectionModel *selectionModel = d->mUi.resources->view()->selectionModel();
    if (!resource.isValid()) {
        selectionModel->clear();
        return;
    }

    AgentFilterProxyModel *filterModel = d->mUi.resources->agentFilterProxyModel();
    for (int i = 0; i < filterModel->rowCount(QModelIndex()); ++i) {
        const QModelIndex index = filterModel->index(i, 0, QModelIndex());
        if (index.isValid()) {
            const QVariant data = filterModel->data(index);
            if (data.isValid()) {
                const AgentInstance instance = data.value<AgentInstance>();
                if (instance.isValid() && instance.identifier() == resource.identifier()) {
                    selectionModel->select(index, QItemSelectionModel::ClearAndSelect);
                    return;
                }
            }
        }
    }
}

bool WorkaroundFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &) const
{
    const QModelIndex index = sourceModel()->index(source_row, 0);
    const QStringList capabilities = index.data(Akonadi::AgentTypeModel::CapabilitiesRole).toStringList();
    return capabilities.contains("KDCRM");
}

#include "resourceconfigdialog.moc"
