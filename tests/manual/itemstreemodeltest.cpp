#include <resourceconfigdialog.h>

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <KLocale>

#include <QTreeView>
#include <Akonadi/ChangeRecorder>
#include <Akonadi/ItemFetchScope>
#include <itemstreemodel.h>
#include <sugaraccount.h>

int main(int argc, char **argv)
{
    KAboutData about("sugarclient", 0, ki18n("SugarClient"),
                     "0.1", ki18n("Resource Config Dialog test"),
                     KAboutData::License_Custom, ki18n("(C) 2015 KDAB"),
                     KLocalizedString(), 0, "david.faure@kdab.com");

    KCmdLineArgs::init(argc, argv, &about);
    KApplication app;

    Akonadi::ChangeRecorder recorder;
    recorder.setMimeTypeMonitored(SugarAccount::mimeType());
    recorder.itemFetchScope().fetchFullPayload(true);

    ItemsTreeModel *model = new ItemsTreeModel(Account, &recorder);

    QTreeView *tree = new QTreeView;
    tree->setModel(model);
    tree->resize(900, 900);
    tree->show();

    return app.exec();
}



