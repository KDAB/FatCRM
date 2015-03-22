#include <resourceconfigdialog.h>

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <KLocale>

int main(int argc, char **argv)
{
    KAboutData about("sugarclient", 0, ki18n("SugarClient"),
                     "0.1", ki18n("Resource Config Dialog test"),
                     KAboutData::License_Custom, ki18n("(C) 2015 KDAB"),
                     KLocalizedString(), 0, "david.faure@kdab.com");

    KCmdLineArgs::init(argc, argv, &about);
    KApplication app;
    ResourceConfigDialog *dlg = new ResourceConfigDialog;
    dlg->show();
    return app.exec();
}



