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

#ifndef ENUMS_H
#define ENUMS_H

#include <QString>
#include <QMetaType>

enum DetailsType {
    Account,
    Opportunity,
    Lead,
    Contact,
    Campaign,
    MaxType = Campaign
};

QString typeToString(DetailsType type);
QString typeToTranslatedString(DetailsType type);

enum ReferencedDataType {
    AccountRef,        // account.id()     => account.name()
    AssignedToRef,     // user id          => user name
    //CampaignRef,       // campaign.id()    => campaign.name()
    ContactRef        // contact id       => full name   (used for "reports to" combo)
};

enum MaxNextStepDate {
    NoDate,
    OneMonthAgo,
    Today,
    EndOfThisWeek,
    EndOfThisMonth,
    EndOfThisYear,
    CustomDate
};

QString maxNextStepDateToString(MaxNextStepDate date);
MaxNextStepDate maxNextStepDateFromString(const QString &str);

Q_DECLARE_METATYPE(DetailsType);
#endif /* ENUMS_H */

