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

#include "mainwindow.h"
#include <config-fatcrm-version.h>

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>
#include <KMessageBox>
#include <KMimeType>

static const char description[] = I18N_NOOP("A Simple SugarCRM Client");
static const char version[] = FATCRM_VERSION_STRING;

int main(int argc, char **argv)
{
    KAboutData about("fatcrm", 0, ki18n("FatCRM"),
                     version, ki18n(description),
                     KAboutData::License_GPL_V2, ki18n("(C) 2010-2016 KDAB"),
                     KLocalizedString(), 0, "info@kdab.com");

    KCmdLineArgs::init(argc, argv, &about);
    KApplication app;

    if (!KMimeType::mimeType("application/x-vnd.kdab.crm.opportunity")) {
        KMessageBox::error(0, i18n("Mimetype application/x-vnd.kdab.crm.opportunity not found, please check your FatCRM installation"));
        return 1;
    }

    MainWindow *window = new MainWindow;
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->show();
    return app.exec();
}
