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

#ifndef SUGARCRMRESOURCE_H
#define SUGARCRMRESOURCE_H

#include <Akonadi/ResourceBase>

#include <QStringList>

class ConflictHandler;
class KJob;
class ResourceDebugInterface;
class ModuleDebugInterface;
class ModuleHandler;
class SugarSession;
class LoginJob;
class SugarJob;

template <typename U, typename V> class QHash;

class SugarCRMResource : public Akonadi::ResourceBase, public Akonadi::AgentBase::Observer
{
    friend class ModuleDebugInterface;
    friend class ResourceDebugInterface;

    Q_OBJECT

public:
    SugarCRMResource(const QString &id);
    ~SugarCRMResource();

public Q_SLOTS:
    virtual void configure(WId windowId);

private:
    SugarSession *mSession;
    SugarJob *mCurrentJob; // do we ever run two jobs in parallel? in that case make it a list
    LoginJob *mLoginJob; // this one can happen in parallel, e.g. start listjob, setonline(false), setonline(true) -> LoginJob is created, and only afterwards the listjob finishes.

    ResourceDebugInterface *mDebugInterface;

    typedef QHash<QString, ModuleHandler *> ModuleHandlerHash;
    ModuleHandlerHash *mModuleHandlers;
    typedef QHash<QString, ModuleDebugInterface *> ModuleDebugInterfaceHash;
    ModuleDebugInterfaceHash *mModuleDebugInterfaces;

    ConflictHandler *mConflictHandler;
    bool mOnline;

private:
    void aboutToQuit() Q_DECL_OVERRIDE;
    void doSetOnline(bool online) Q_DECL_OVERRIDE;

    void itemAdded(const Akonadi::Item &item, const Akonadi::Collection &collection) Q_DECL_OVERRIDE;
    void itemChanged(const Akonadi::Item &item, const QSet<QByteArray> &parts) Q_DECL_OVERRIDE;
    void itemRemoved(const Akonadi::Item &item) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void retrieveCollections() Q_DECL_OVERRIDE;
    void retrieveItems(const Akonadi::Collection &col) Q_DECL_OVERRIDE;
    bool retrieveItem(const Akonadi::Item &item, const QSet<QByteArray> &parts) Q_DECL_OVERRIDE;

    void startExplicitLogin();
    void explicitLoginResult(KJob *job);

    void listModulesResult(KJob *job);

    void slotTotalItems(int count);
    void itemsReceived(const Akonadi::Item::List &items);
    void deletedReceived(const Akonadi::Item::List &items);
    void listEntriesResult(KJob *job);

    void createEntryResult(KJob *job);

    void deleteEntryResult(KJob *job);

    void fetchEntryResult(KJob *job);

    void updateEntryResult(KJob *job);

    void commitChange(const Akonadi::Item &item);

    void updateOnBackend(const Akonadi::Item &item);

private:
    void updateItem(const Akonadi::Item &item, ModuleHandler *handler);
    void createModuleHandlers(const QStringList &availableModules);

    bool handleLoginError(KJob *job);
};

#endif
