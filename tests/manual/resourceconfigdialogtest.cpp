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

#include "resourceconfigdialog.h"
#include <config-fatcrm-version.h>

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>

int main(int argc, char **argv)
{
    KAboutData about("resourceconfigdialogtest", 0, ki18n("ResourceConfigDialog test"),
                     FATCRM_VERSION_STRING, ki18n("Resource Config Dialog test program"),
                     KAboutData::License_GPL_V2, ki18n("(C) 2015-2016 KDAB"),
                     KLocalizedString(), 0, "david.faure@kdab.com");

    KCmdLineArgs::init(argc, argv, &about);
    KApplication app;
    ResourceConfigDialog *dlg = new ResourceConfigDialog;
    dlg->show();
    return app.exec();
}
