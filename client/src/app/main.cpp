/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "mainwindow.h"
#include <config-fatcrm-version.h>

#include <kdcrmutils.h>

#include <KAboutData>
#include <KDBusService>

#include <QApplication>
#include <KLocalizedString>
#include <KMessageBox>
#include <QCommandLineParser>
#include <QMimeDatabase>

static const char description[] = I18N_NOOP("A Simple SugarCRM Client");
static const char version[] = FATCRM_VERSION_STRING;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("fatcrm");

    KAboutData aboutData(QStringLiteral("fatcrm"), i18n("FatCRM"),
                     version, i18n(description),
                     KAboutLicense::GPL_V2, i18n("(C) 2010-2018 KDAB"),
                     QString(), QStringLiteral("info@kdab.com"));

    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    parser.addVersionOption();
    parser.addHelpOption();
    QCommandLineOption noOverlayOption("nooverlay", i18n("Do not display the overlay during initial data loading"));
    parser.addOption(noOverlayOption);
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    QMimeDatabase db;
    if (!db.mimeTypeForName("application/x-vnd.kdab.crm.opportunity").isValid()) {
        KMessageBox::error(nullptr, i18n("Mimetype application/x-vnd.kdab.crm.opportunity not found, please check your FatCRM installation"));
        return 1;
    }

    KDBusService service(KDBusService::Multiple);

    KDCRMUtils::setupIconTheme();

    auto *window = new MainWindow(!parser.isSet(noOverlayOption));
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->show();
    return app.exec();
}
