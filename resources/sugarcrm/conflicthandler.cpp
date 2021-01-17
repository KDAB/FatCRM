/*
  This file is part of FatCRM, a desktop application for SugarCRM written by KDAB.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "conflicthandler.h"
#include "conflictresolvedialog.h"
#include "sugarcrmresource_debug.h"
#include <AkonadiCore/Collection>
#include <AkonadiCore/Item>
#include <AkonadiCore/ItemCreateJob>
#include <AkonadiCore/ItemModifyJob>
#include <AkonadiCore/Session>
using namespace Akonadi;

#include <KLocalizedString>
#include <KWindowSystem>
#include <kwindowsystem_version.h>

class ConflictHandler::Private
{
    ConflictHandler *const q;
public:
    Private(ConflictHandler *parent, ConflictType type)
        : q(parent), mType(type), mDiffInterface(nullptr), mWindowId(0),
          mSession(new Session(Session::defaultSession()->sessionId() + "-conflicthandler", parent))
    {
    }

public:
    ConflictType mType;

    Item mLocalItem;
    Item mRemoteItem;

    DifferencesAlgorithmInterface *mDiffInterface;
    WId mWindowId;
    QString mName;

    Session *mSession;

public:
    void resolve();
    void createDuplicate(const Item &item);

public: // slots
    void duplicateLocalItemResult(KJob *job);
};

void ConflictHandler::Private::resolve()
{
    ConflictResolveDialog dialog;

    if (mWindowId != 0) {
        // if we have a "parent" window id, use it
        // otherwise focus stealing prevention might put us behind it
#if KWINDOWSYSTEM_VERSION >= QT_VERSION_CHECK(5,62,0)
        dialog.setAttribute(Qt::WA_NativeWindow, true);
        KWindowSystem::setMainWindow(dialog.windowHandle(), mWindowId);
#else
        KWindowSystem::setMainWindow(&dialog, mWindowId);
#endif
    }

    if (mName.isEmpty()) {
        dialog.setWindowTitle(i18nc("@title:window", "Conflict Resolution"));
    } else {
        dialog.setWindowTitle(i18nc("@title:window", "%1: Conflict Resolution", mName));
    }

    dialog.setConflictingItems(mLocalItem, mRemoteItem);
    dialog.setDifferencesInterface(mDiffInterface);

    dialog.exec();

    ConflictHandler::ResolveStrategy solution = dialog.resolveStrategy();
    switch (solution) {
    case ConflictHandler::UseLocalItem:
        // update the local items remote revision and try again
        mLocalItem.setRemoteRevision(mRemoteItem.remoteRevision());
        emit q->updateOnBackend(mLocalItem);
        break;

    case ConflictHandler::UseOtherItem: {
        // use remote item to acknowledge change
        mRemoteItem.setId(mLocalItem.id());
        mRemoteItem.setRevision(mLocalItem.revision());
        emit q->commitChange(mRemoteItem);

        // commit does not update payload, so we modify as well
        auto *modifyJob = new ItemModifyJob(mRemoteItem, q);
        modifyJob->disableRevisionCheck();
        break;
    }
    case ConflictHandler::UseBothItems: {
        // use remote item to acknowledge change and duplicate local item
        mRemoteItem.setId(mLocalItem.id());
        mRemoteItem.setRevision(mLocalItem.revision());
        emit q->commitChange(mRemoteItem);

        // commit does not update payload, so we modify as well
        auto *modifyJob = new ItemModifyJob(mRemoteItem, q);
        modifyJob->disableRevisionCheck();

        createDuplicate(mLocalItem);
        break;
    }
    }
}

void ConflictHandler::Private::createDuplicate(const Item &item)
{
    qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "Creating duplicate of item: id=" << item.id() << ", remoteId=" << item.remoteId();
    Item duplicate(item);
    duplicate.setId(-1);
    duplicate.setRemoteId(QString());
    duplicate.setRemoteRevision(QString());

    ItemCreateJob *job = new ItemCreateJob(duplicate, item.parentCollection(), mSession);
    connect(job, SIGNAL(result(KJob*)), q, SLOT(duplicateLocalItemResult(KJob*)));
}

void ConflictHandler::Private::duplicateLocalItemResult(KJob *job)
{
    auto *createJob = qobject_cast<ItemCreateJob *>(job);
    Q_ASSERT(createJob != nullptr);

    const Item item = createJob->item();

    if (createJob->error() != 0) {
        qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << "Duplicating local item" << item.id()
                   << ", collection" << item.parentCollection().name()
                   << "for conflict resolution failed on local create: "
                   << "error=" << createJob->error() << "message=" << createJob->errorText();
    } else {
        qCDebug(FATCRM_SUGARCRMRESOURCE_LOG) << "Duplicating local item" << item.id()
                 << ", collection" << item.parentCollection().name()
                 << "for conflict resolution succeeded:";
    }
}

ConflictHandler::ConflictHandler(ConflictType type, QObject *parent)
    : QObject(parent), d(new Private(this, type))
{
}

ConflictHandler::~ConflictHandler()
{
    delete d;
}

void ConflictHandler::setConflictingItems(const Item &changedItem, const Item &conflictingItem)
{
    d->mLocalItem = changedItem;
    d->mRemoteItem = conflictingItem;
}

void ConflictHandler::setDifferencesInterface(DifferencesAlgorithmInterface *interface)
{
    Q_ASSERT(interface != nullptr);
    d->mDiffInterface = interface;
}

void ConflictHandler::setParentName(const QString &name)
{
    d->mName = name;
}

void ConflictHandler::setParentWindowId(WId windowId)
{
    d->mWindowId = windowId;
}

void ConflictHandler::start()
{
    Q_ASSERT(d->mType == BackendConflict);

    d->resolve();
}
#include "moc_conflicthandler.cpp"
