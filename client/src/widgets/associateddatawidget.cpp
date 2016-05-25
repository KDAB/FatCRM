#include "associateddatawidget.h"
#include "ui_associateddatawidget.h"

#include <QStringListModel>

AssociatedDataWidget::AssociatedDataWidget(QWidget *parent) :
    QWidget(parent),
    mUi(new Ui::AssociatedDataWidget)
{
    mUi->setupUi(this);
    connect(mUi->opportunitiesListView, SIGNAL(activated(QModelIndex)), this, SLOT(itemActivated(QModelIndex)));
    connect(mUi->contactsListView, SIGNAL(activated(QModelIndex)), this, SLOT(itemActivated(QModelIndex)));
}

AssociatedDataWidget::~AssociatedDataWidget()
{
    delete mUi;
}

void AssociatedDataWidget::hideOpportunityGui()
{
    mUi->opportunitiesLabel->hide();
    mUi->opportunitiesListView->hide();
}

void AssociatedDataWidget::setContactsModel(QStringListModel *model)
{
    mUi->contactsListView->setModel(model);

}

void AssociatedDataWidget::setOpportunitiesModel(QStringListModel *model)
{
    mUi->opportunitiesListView->setModel(model);
}

void AssociatedDataWidget::itemActivated(const QModelIndex &index)
{
    emit openItem(index.data(Qt::DisplayRole).toString());

}
