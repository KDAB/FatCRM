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
#include "clientsettings.h"
#include "kdcrmutils.h"

#include <KAboutData>
#include <KDBusService>
#include <KDReportsReport.h>

#include <QApplication>
#include <KLocalizedString>
#include <KMessageBox>
#include <QCommandLineParser>
#include <QMimeDatabase>
#include <QSettings>

#include <stdio.h> // for stdout

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
    QCommandLineOption printToPdfOption("print-to-pdf", i18n("Print page after initial import and exit FatCRM"), "fileName");
    parser.addOption(printToPdfOption);
    QCommandLineOption printTypeOption("print-type", i18n("Print type, either 'Accounts', 'Opportunities' or 'Contacts'"), "printType", "Opportunities");
    parser.addOption(printTypeOption);
    QCommandLineOption loadSavedSearchOption("load-saved-search", i18n("Load saved search after startup"), "name");
    parser.addOption(loadSavedSearchOption);
    QCommandLineOption noOverlayOption("nooverlay", i18n("Do not display the overlay during initial data loading"));
    parser.addOption(noOverlayOption);
    QCommandLineOption configKeyOption("config-key", i18n("Internal feature: Return the value for setting <key>"), "key");
    parser.addOption(configKeyOption);
    QCommandLineOption configValueOption("set-config-value", i18n("Internal feature: Set the value for setting specified by 'config-key' parameter"), "value");
    parser.addOption(configValueOption);
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    if (parser.isSet(configKeyOption)) {
        QTextStream qout(stdout);
        QTextStream qerr(stderr);

        const QVariant currentValue = ClientSettings::self()->settings().value(parser.value(configKeyOption));

        if (parser.isSet(configValueOption)) {
            const QString key = parser.value(configKeyOption);

            // write config
            QVariant valueAsVariant(parser.value(configValueOption));
            if (!valueAsVariant.convert(static_cast<int>(currentValue.type()))) {
                qerr << "Failed to convert value to target type: " << currentValue.typeName() << endl;
                return 1;
            }

            ClientSettings::self()->settings().setValue(key, valueAsVariant);
            qerr << "Value for '" << key << "' set to: " << valueAsVariant.toString() << endl;
            return 0;
        }

        // else: read config
        qout << currentValue.toString() << endl;
        return 0;
    }

    QMimeDatabase db;
    if (!db.mimeTypeForName("application/x-vnd.kdab.crm.opportunity").isValid()) {
        KMessageBox::error(nullptr, i18n("Mimetype application/x-vnd.kdab.crm.opportunity not found, please check your FatCRM installation"));
        return 1;
    }

    KDBusService service(KDBusService::Multiple);

    KDCRMUtils::setupIconTheme();

    auto *mainWindow = new MainWindow(!parser.isSet(noOverlayOption));
    mainWindow->setAttribute(Qt::WA_DeleteOnClose);

    if (parser.isSet(printToPdfOption) || parser.isSet(loadSavedSearchOption)) {
        const QString fileName = parser.value(printToPdfOption);
        const DetailsType detailsType = stringToType(parser.value(printTypeOption));
        const QString loadSavedSearch = parser.value(loadSavedSearchOption);

        QObject::connect(mainWindow, &MainWindow::initialLoadingDone, [mainWindow, fileName, detailsType, loadSavedSearch, &app]() {
            QTextStream qout(stdout);
            QTextStream qerr(stderr);

            // first: load saved search if requested
            if (!loadSavedSearch.isEmpty()) {
                const bool loadingSucceeded = mainWindow->loadSavedSearch(loadSavedSearch);
                if (!loadingSucceeded) {
                    qerr << "Error: Failed to load saved search: " << loadSavedSearch << endl;
                    app.exit(1);
                    return;
                }
            }

            // second: print a PDF if requested and then exit the application
            if (!fileName.isEmpty()) {
                qerr << "Initial import done, now printing report as requested for: " << typeToString(detailsType) << endl;
                auto page = mainWindow->pageForType(detailsType);
                if (!page) {
                    qerr << "Error: No page for: " << typeToString(detailsType);
                    app.exit(1);
                    return;
                }

                auto report = page->generateReport(false);
                const bool exportSucceeded = report->exportToFile(fileName);
                if (!exportSucceeded) {
                    qerr << "Error: Printing failed." << endl;
                    app.exit(1);
                    return;
                }

                qerr << "Printing to file done: " << fileName << endl;
                app.exit(0);
            }
        });
    } else {
        mainWindow->show();
    }

    return app.exec();
}
