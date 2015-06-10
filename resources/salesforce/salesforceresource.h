/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef SALESFORCERESOURCE_H
#define SALESFORCERESOURCE_H

#include <akonadi/resourcebase.h>

#include <QStringList>

class SalesforceModuleHandler;
class KDSoapMessage;
class SforceService;

class TNS__DeleteResponse;
class TNS__DescribeGlobalResponse;
class TNS__DescribeSObjectsResponse;
class TNS__LoginResponse;
class TNS__QueryMoreResponse;
class TNS__QueryResponse;
class TNS__UpsertResponse;

template <typename U, typename V> class QHash;

class SalesforceResource : public Akonadi::ResourceBase, public Akonadi::AgentBase::Observer
{
    friend class ModuleDebugInterface;
    friend class ResourceDebugInterface;

    Q_OBJECT

public:
    explicit SalesforceResource(const QString &id);
    ~SalesforceResource();

public Q_SLOTS:
    virtual void configure(WId windowId);

protected:
    SforceService *mSoap;

    QString mSessionId;

    Akonadi::Collection mTopLevelCollection;

    Akonadi::Item mPendingItem;

    QStringList mAvailableModules;
    typedef QHash<QString, SalesforceModuleHandler *> ModuleHandlerHash;
    ModuleHandlerHash *mModuleHandlers;

protected:
    void aboutToQuit();
    void doSetOnline(bool online);

    void doLogin();

    void itemAdded(const Akonadi::Item &item, const Akonadi::Collection &collection);
    void itemChanged(const Akonadi::Item &item, const QSet<QByteArray> &parts);
    void itemRemoved(const Akonadi::Item &item);

    void connectSoapProxy();

protected Q_SLOTS:
    void retrieveCollections();
    void retrieveItems(const Akonadi::Collection &col);
    bool retrieveItem(const Akonadi::Item &item, const QSet<QByteArray> &parts);

    void loginDone(const TNS__LoginResponse &callResult);
    void loginError(const KDSoapMessage &fault);

    void getEntryListDone(const TNS__QueryResponse &callResult);
    void getEntryListDone(const TNS__QueryMoreResponse &callResult);
    void getEntryListError(const KDSoapMessage &fault);

    void setEntryDone(const TNS__UpsertResponse &callResult);
    void setEntryError(const KDSoapMessage &fault);

    void deleteEntryDone(const TNS__DeleteResponse &callResult);
    void deleteEntryError(const KDSoapMessage &fault);

    void describeGlobalDone(const TNS__DescribeGlobalResponse &callResult);
    void describeGlobalError(const KDSoapMessage &fault);

    void describeSObjects(const QStringList &objects);
    void describeSObjectsDone(const TNS__DescribeSObjectsResponse &callResult);
    void describeSObjectsError(const KDSoapMessage &fault);
};

#endif
