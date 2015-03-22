#include "sugarclient.h"

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <KLocale>

static const char description[] = I18N_NOOP("A Simple SugarCRM Client");
static const char version[] = "0.1";

int main(int argc, char **argv)
{
    KAboutData about("sugarclient", 0, ki18n("SugarClient"),
                     version, ki18n(description),
                     KAboutData::License_Custom, ki18n("(C) 2010 KDAB"),
                     KLocalizedString(), 0, "info@kdab.com");

    KCmdLineArgs::init(argc, argv, &about);
    KApplication app;
    SugarClient *sugarClt = new SugarClient;
    sugarClt->show();
    return app.exec();
}

