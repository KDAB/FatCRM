#ifndef RESOURCECONFIGDIALOG_H
#define RESOURCECONFIGDIALOG_H

#include <QDialog>

namespace Akonadi
{
class AgentInstance;
}

class KJob;

class ResourceConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ResourceConfigDialog(QWidget *parent = 0);

    ~ResourceConfigDialog();

Q_SIGNALS:
    void resourceSelected(const Akonadi::AgentInstance &resource);

public Q_SLOTS:
    void resourceSelectionChanged(const Akonadi::AgentInstance &resource);

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void updateButtonStates())
    Q_PRIVATE_SLOT(d, void addResource())
    Q_PRIVATE_SLOT(d, void configureResource())
    Q_PRIVATE_SLOT(d, void syncResources())
    Q_PRIVATE_SLOT(d, void removeResource())
    Q_PRIVATE_SLOT(d, void resourceCreateResult(KJob *))
    Q_PRIVATE_SLOT(d, void applyResourceSelection())
    Q_PRIVATE_SLOT(d, void agentInstanceChanged(const Akonadi::AgentInstance &))
};

#endif
