/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "itemstreemodel.h"
#include "resourceconfigdialog.h"
#include "sugaraccount.h"
#include "sugaropportunity.h"

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>

#include <Akonadi/Collection>
#include <Akonadi/CollectionFetchJob>
#include <Akonadi/CollectionFetchScope>
#include <Akonadi/ChangeRecorder>
#include <Akonadi/ItemFetchScope>
#include <Akonadi/EntityTreeView>
#include <Akonadi/EntityMimeTypeFilterModel>
#include <Akonadi/Session>
using namespace Akonadi;

#include <QTreeView>
#include <QTimer>

class Controller : public QObject
{
    Q_OBJECT
public:
    Controller(QTreeView *tree, ChangeRecorder *changeRecorder)
        : mChangeRecorder(changeRecorder),
          mTreeView(tree)
    {
        CollectionFetchJob *fetchJob = new CollectionFetchJob(Collection::root(), CollectionFetchJob::Recursive,
                                                              changeRecorder->session());
        fetchJob->fetchScope().setContentMimeTypes(QStringList() << SugarOpportunity::mimeType());
       // fetchJob->fetchScope().setAncestorRetrieval(Akonadi::CollectionFetchScope::All); // needed?

        connect(fetchJob, SIGNAL(result(KJob*)),
                this, SLOT(rootFetchJobDone(KJob*)));
    }
private Q_SLOTS:
    void rootFetchJobDone(KJob *job) {
        if (job->error()) {
            kWarning() << job->errorString();
            return;
        }
        CollectionFetchJob *collectionJob = qobject_cast<CollectionFetchJob *>(job);
        const Collection::List list = collectionJob->collections();

        kDebug() << list.count();
        kDebug() << list.first().name();
        mChangeRecorder->setCollectionMonitored(list.first(), true);

        ItemsTreeModel *model = new ItemsTreeModel(Opportunity, mChangeRecorder);
        mTreeView->setModel(model);


        //
    #if 1
        Akonadi::EntityMimeTypeFilterModel *filterModel = new Akonadi::EntityMimeTypeFilterModel(this);
        filterModel->setSourceModel(model);
        filterModel->addMimeTypeInclusionFilter(SugarOpportunity::mimeType());
        filterModel->setHeaderGroup(Akonadi::EntityTreeModel::ItemListHeaders);

        Akonadi::EntityTreeView *view = new Akonadi::EntityTreeView;
        view->setModel(filterModel);
        view->resize(900, 900);
        view->show();
    #endif

    }

private:
    ChangeRecorder *mChangeRecorder;
    QTreeView *mTreeView;
};

int main(int argc, char **argv)
{
    KAboutData about("itemstreemodeltest", 0, ki18n("ItemsTreeModel test"),
                     "0.1", ki18n("Interactive test program for ItemsTreeModel"),
                     KAboutData::License_GPL_V2, ki18n("(C) 2015 KDAB"),
                     KLocalizedString(), 0, "david.faure@kdab.com");

    KCmdLineArgs::init(argc, argv, &about);
    KApplication app;

    Akonadi::ChangeRecorder recorder;

    recorder.setMimeTypeMonitored(SugarOpportunity::mimeType());
    recorder.itemFetchScope().fetchFullPayload(true);

    QTreeView *tree = new QTreeView(0);
    tree->resize(900, 900);
    tree->show();

    Controller controller(tree, &recorder);

    return app.exec();
}

#include "itemstreemodeltest.moc"
