#include <resourceconfigdialog.h>

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <KLocale>

#include <QTreeView>
#include <QTimer>
#include <Akonadi/Collection>
#include <Akonadi/CollectionFetchJob>
#include <Akonadi/ChangeRecorder>
#include <Akonadi/ItemFetchScope>
#include <Akonadi/EntityTreeView>
#include <Akonadi/EntityMimeTypeFilterModel>
#include <Akonadi/Session>
#include <itemstreemodel.h>
#include <sugaraccount.h>
#include <sugaropportunity.h>

using namespace Akonadi;

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
    KAboutData about("sugarclient", 0, ki18n("SugarClient"),
                     "0.1", ki18n("Resource Config Dialog test"),
                     KAboutData::License_Custom, ki18n("(C) 2015 KDAB"),
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
