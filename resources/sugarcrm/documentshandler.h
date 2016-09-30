/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Tobias Koenig <tobias.koenig@kdab.com>

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

#ifndef DOCUMENTSHANDLER_H
#define DOCUMENTSHANDLER_H

#include "modulehandler.h"
#include "kdcrmdata/sugardocument.h"

class DocumentAccessorPair;

template <typename U, typename V> class QHash;

class DocumentsHandler : public ModuleHandler
{
public:
    explicit DocumentsHandler(SugarSession *session);

    ~DocumentsHandler();

    Akonadi::Collection handlerCollection() const Q_DECL_OVERRIDE;

    bool setEntry(const Akonadi::Item &item) override;

    QString orderByForListing() const Q_DECL_OVERRIDE;
    QStringList supportedSugarFields() const Q_DECL_OVERRIDE;
    QStringList supportedCRMFields() const Q_DECL_OVERRIDE;

    int expectedContentsVersion() const Q_DECL_OVERRIDE;

    bool needsExtraInformation() const Q_DECL_OVERRIDE;
    void getExtraInformation(Akonadi::Item::List &items) Q_DECL_OVERRIDE;
    Akonadi::Item itemFromEntry(const KDSoapGenerated::TNS__Entry_value &entry, const Akonadi::Collection &parentCollection) Q_DECL_OVERRIDE;

    void compare(Akonadi::AbstractDifferencesReporter *reporter,
                 const Akonadi::Item &leftItem, const Akonadi::Item &rightItem) Q_DECL_OVERRIDE;

private:
    SugarDocument::AccessorHash mAccessors;
};

#endif
