/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <AkonadiAgentBase/ResourceBase>

#include <QStringList>

class ConflictHandler;
class KJob;
class ResourceDebugInterface;
class ModuleDebugInterface;
class ModuleHandler;
class SugarSession;
class LoginJob;
class SugarJob;
class PasswordHandler;

template <typename U, typename V> class QHash;

struct HandleDeletedItemsArg
{
    Akonadi::Collection collection;
    Akonadi::Item::List deletedItems;
    ModuleHandler *module;
};


class SugarCRMResource : public Akonadi::ResourceBase, public Akonadi::AgentBase::ObserverV2
{
    friend class ItemTransferInterface;
    friend class ModuleDebugInterface;
    friend class ResourceDebugInterface;

    Q_OBJECT

public:
    explicit SugarCRMResource(const QString &id);
    ~SugarCRMResource() override;

public Q_SLOTS:
    void configure(WId windowId) override;

private:
    PasswordHandler *mPasswordHandler;
    SugarSession *mSession;
    SugarJob *mCurrentJob; // do we ever run two jobs in parallel? in that case make it a list
    LoginJob *mLoginJob; // this one can happen in parallel, e.g. start listjob, setonline(false), setonline(true) -> LoginJob is created, and only afterwards the listjob finishes.

    ResourceDebugInterface *mDebugInterface;

    typedef QHash<QString, ModuleHandler *> ModuleHandlerHash;
    ModuleHandlerHash *mModuleHandlers;
    typedef QHash<QString, ModuleDebugInterface *> ModuleDebugInterfaceHash;
    ModuleDebugInterfaceHash *mModuleDebugInterfaces;

    ConflictHandler *mConflictHandler;
    int mTotalItems;
    bool mOnline;

private:
    void aboutToQuit() override;
    void doSetOnline(bool online) override;

    void itemAdded(const Akonadi::Item &item, const Akonadi::Collection &collection) override;
    void itemChanged(const Akonadi::Item &item, const QSet<QByteArray> &parts) override;
    void itemRemoved(const Akonadi::Item &item) override;

private Q_SLOTS:
    void retrieveCollections() override;
    void retrieveItems(const Akonadi::Collection &col) override;
    bool retrieveItem(const Akonadi::Item &item, const QSet<QByteArray> &parts) override;

    void startExplicitLogin();
    void explicitLoginResult(KJob *job);

    void listModulesResult(KJob *job);

    void slotTotalItems(int count);
    void slotProgress(int count);
    void slotItemsReceived(const Akonadi::Item::List &items, bool isUpdateJob);
    void listEntriesResult(KJob *job);

    void handleDeletedItems(const QVariant &val);
    void slotDeleteEntriesResult(KJob*);

    void createEntryResult(KJob *job);

    void deleteEntryResult(KJob *job);

    void fetchEntryResult(KJob *job);

    void updateEntryResult(KJob *job);

    void commitChange(const Akonadi::Item &item);

    void updateOnBackend(const Akonadi::Item &item);

private:
    void updateItem(const Akonadi::Item &item, ModuleHandler *handler);
    void createModuleHandlers(const QStringList &availableModules);
    bool hasInvalidSessionError(KJob *job);

    // very similar to imapresource's ActionIfNoSession
    enum ActionOnError { CancelTaskOnError, DeferTaskOnError };
    bool handleError(KJob *job, ActionOnError action);
};

Q_DECLARE_METATYPE(HandleDeletedItemsArg)

#endif
