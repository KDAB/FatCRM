/****************************************************************************
** Copyright (C) 2010, 2011 Klaralvdalens Datakonsult AB.  All rights reserved.
** Copyright (C) 2011-2012 Eric Newberry <ericnewberry@mirametrics.com>. All Rights Reserved.
** Copyright (C) 2012, Kevin Krammer <kevin.krammer@gmx.at>
**
** This file is part of the Akonadi Resource for SugarCRM.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 and version 3 as published by the
** Free Software Foundation and appearing in the file LICENSE.GPL included.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** Contact info@kdab.com if any conditions of this licensing are not
** clear to you.
**
**********************************************************************/

#include "taskshandler.h"

#include "sugarsession.h"
#include "sugarsoap.h"

#include <kcalcore/todo.h>

#include <akonadi/collection.h>
#include <akonadi/abstractdifferencesreporter.h>

#include <kdatetime.h>

#include <KLocale>

#include <QHash>

using namespace KCalCore;
using namespace KDSoapGenerated;

static QString getId( const KCalCore::Todo &todo )
{
    return todo.customProperty( "SugarCRM", "X-Id" );
}

static void setId( const QString &value, KCalCore::Todo &todo )
{
    todo.setCustomProperty( "SugarCRM", "X-Id", value );
}

static QString getDateEntered( const KCalCore::Todo &todo )
{
    return todo.created().toString();
}

static void setDateEntered( const QString &value, KCalCore::Todo &todo )
{
    todo.setCreated( KDateTime::fromString(value, "%Y-%m-%d %H:%M:%S") );
}

static QString getDateModified( const KCalCore::Todo &todo )
{
    return todo.customProperty( "SugarCRM", "X-DateModified" );
}

static void setDateModified( const QString &value, KCalCore::Todo &todo )
{
    todo.setCustomProperty( "SugarCRM", "X-DateModified", value );
}

static QString getModifiedUserId( const KCalCore::Todo &todo )
{
    return todo.customProperty( "SugarCRM", "X-ModifiedUserId" );
}

static void setModifiedUserId( const QString &value, KCalCore::Todo &todo )
{
    todo.setCustomProperty( "SugarCRM", "X-ModifiedUserId", value );
}

static QString getCreatedBy( const KCalCore::Todo &todo )
{
    return todo.customProperty( "SugarCRM", "X-CreatedBy" );
}

static void setCreatedBy( const QString &value, KCalCore::Todo &todo )
{
    todo.setCustomProperty( "SugarCRM", "X-CreatedBy", value );
}

static QString getDescription( const KCalCore::Todo &todo )
{
    return todo.description();
}

static void setDescription( const QString &value, KCalCore::Todo &todo )
{
    todo.setDescription( value );
}

static QString getSummary( const KCalCore::Todo &todo )
{
    return todo.summary();
}

static void setSummary( const QString &value, KCalCore::Todo &todo )
{
    todo.setSummary( value );
}

static QString getDeleted( const KCalCore::Todo &todo )
{
    return todo.customProperty( "SugarCRM", "X-Deleted" );
}

static void setDeleted( const QString &value, KCalCore::Todo &todo )
{
    todo.setCustomProperty( "SugarCRM", "X-Deleted", value );
}

static QString getAssignedUserId( const KCalCore::Todo &todo )
{
    return todo.customProperty( "SugarCRM", "X-AssignedUserId" );
}

static void setAssignedUserId( const QString &value, KCalCore::Todo &todo )
{
    todo.setCustomProperty( "SugarCRM", "X-AssignedUserId", value );
}

static QString getStatus( const KCalCore::Todo &todo )
{
    if ( todo.status() == KCalCore::Todo::StatusConfirmed ) {
        return QLatin1String( "Not Started" );
    } else if ( todo.status() == KCalCore::Todo::StatusInProcess ) {
        return QLatin1String( "In Progress" );
    } else if ( todo.status() == KCalCore::Todo::StatusCompleted ) {
        return QLatin1String( "Completed" );
    } else if ( todo.status() == KCalCore::Todo::StatusNeedsAction ) {
        return QLatin1String( "Pending Input" );
    } else if ( todo.customStatus() == "StatusDeferred" ) {
        return QLatin1String( "Deferred" );
    } else {
        return QLatin1String( "None" );
    }
}

static void setStatus( const QString &value, KCalCore::Todo &todo )
{
    if ( value == QLatin1String( "Not Started" ) ) {
        todo.setStatus( KCalCore::Todo::StatusConfirmed );
    } else if ( value == QLatin1String( "In Progress" ) ) {
        todo.setStatus( KCalCore::Todo::StatusInProcess );
    } else if ( value == QLatin1String( "Completed" ) ) {
        todo.setStatus( KCalCore::Todo::StatusCompleted );
    } else if ( value == QLatin1String( "Pending Input" ) ) {
        todo.setStatus( KCalCore::Todo::StatusNeedsAction );
    } else if ( value == QLatin1String( "Deferred" ) ) {
        todo.setCustomStatus( QLatin1String( "StatusDeferred" ) );
    } else {
        todo.setStatus( KCalCore::Todo::StatusNone );
    }
}

static QString getDateDueFlag( const KCalCore::Todo &todo )
{
    return todo.customProperty( "SugarCRM", "X-DateDueFlag" );
}

static void setDateDueFlag( const QString &value, KCalCore::Todo &todo )
{
    todo.setCustomProperty( "SugarCRM", "X-DateDueFlag", value );
}

static QString getDateDue( const KCalCore::Todo &todo )
{
    KDateTime dateDue = todo.dtDue();
    return dateDue.toString( "%Y-%m-%d %H:%M:%S" );
}

static void setDateDue( const QString &value, KCalCore::Todo &todo )
{
    todo.setDtDue( KDateTime::fromString( value, "%Y-%m-%d %H:%M:%S", 0, true ) );
}

static QString getDateStartFlag( const KCalCore::Todo &todo )
{
    return todo.customProperty( "SugarCRM", "X-DateStartFlag" );
}

static void setDateStartFlag( const QString &value, KCalCore::Todo &todo )
{
    todo.setCustomProperty( "SugarCRM", "X-DateStartFlag", value );
}

static QString getDateStart( const KCalCore::Todo &todo )
{
    KDateTime dateStart = todo.dtStart();
    return dateStart.toString( "%Y-%m-%d %H:%M:%S" );
}

static void setDateStart( const QString &value, KCalCore::Todo &todo )
{
    todo.setDtStart( KDateTime::fromString( value, "%Y-%m-%d %H:%M:%S", 0, true ) );
}

static QString getParentType( const KCalCore::Todo &todo )
{
    return todo.customProperty( "SugarCRM", "X-ParentType" );
}

static void setParentType( const QString &value, KCalCore::Todo &todo )
{
    todo.setCustomProperty( "SugarCRM", "X-ParentType", value );
}

static QString getParentId( const KCalCore::Todo &todo )
{
    return todo.customProperty( "SugarCRM", "X-ParentId" );
}

static void setParentId( const QString &value, KCalCore::Todo &todo )
{
    todo.setCustomProperty( "SugarCRM", "X-ParentId", value );
}

static QString getContactId( const KCalCore::Todo &todo )
{
    return todo.customProperty( "SugarCRM", "X-ContactId" );
}

static void setContactId( const QString &value, KCalCore::Todo &todo )
{
    todo.setCustomProperty( "SugarCRM", "X-ContactId", value );
}

static QString getPriority( const KCalCore::Todo &todo )
{
    if ( todo.priority() == 1 ) {
	return QLatin1String( "High" );
    } else if ( todo.priority() == 5 ) {
	return QLatin1String( "Medium" );
    } else if ( todo.priority() == 9 ) {
	return QLatin1String( "Low" );
    } else {
	return QLatin1String( "None" );
    }
}

static void setPriority( const QString &value, KCalCore::Todo &todo )
{
    if ( value == QLatin1String( "High" ) ) {
	todo.setPriority( 1 );
    } else if ( value == QLatin1String( "Medium" ) ) {
	todo.setPriority( 5 );
    } else if ( value == QLatin1String( "Low" ) ) {
	todo.setPriority( 9 );
    } else {
	todo.setPriority( 0 );
    }
}

TasksHandler::TasksHandler( SugarSession *session )
    : ModuleHandler( QLatin1String( "Tasks" ), session ),
      mAccessors( new AccessorHash )
{
    mAccessors->insert( QLatin1String( "id" ),
                        new TaskAccessorPair( getId, setId, QString() ) );
    mAccessors->insert( QLatin1String( "name" ),
                        new TaskAccessorPair( getSummary, setSummary, i18nc( "@item:intable TODO title", "Title" ) ) );
    mAccessors->insert( QLatin1String( "date_entered" ),
                        new TaskAccessorPair( getDateEntered, setDateEntered, i18nc( "@item:intable", "Creation Date" ) ) );
    mAccessors->insert( QLatin1String( "date_modififed" ),
                        new TaskAccessorPair( getDateModified, setDateModified, i18nc( "@item:intable", "Modification Date" ) ) );
    mAccessors->insert( QLatin1String( "modified_user_id" ),
                        new TaskAccessorPair( getModifiedUserId, setModifiedUserId, QString() ) );
    mAccessors->insert( QLatin1String( "created_by" ),
                        new TaskAccessorPair( getCreatedBy, setCreatedBy, QString() ) );
    mAccessors->insert( QLatin1String( "description" ),
                        new TaskAccessorPair( getDescription, setDescription, i18nc( "@item:intable", "Description" ) ) );
    mAccessors->insert( QLatin1String( "deleted" ),
                        new TaskAccessorPair( getDeleted, setDeleted, QString() ) );
    mAccessors->insert( QLatin1String( "assigned_user_id" ),
                        new TaskAccessorPair( getAssignedUserId, setAssignedUserId, QString() ) );
    mAccessors->insert( QLatin1String( "status" ),
                        new TaskAccessorPair( getStatus, setStatus, i18nc( "@item:intable", "Status" ) ) );
    mAccessors->insert( QLatin1String( "date_due_flag" ),
                        new TaskAccessorPair( getDateDueFlag, setDateDueFlag, QString() ) );
    mAccessors->insert( QLatin1String( "date_due" ),
                        new TaskAccessorPair( getDateDue, setDateDue, i18nc( "@item:intable", "Due Date" ) ) );
    mAccessors->insert( QLatin1String( "date_start_flag" ),
                        new TaskAccessorPair( getDateStartFlag, setDateStartFlag, QString() ) );
    mAccessors->insert( QLatin1String( "date_start" ),
                        new TaskAccessorPair( getDateStart, setDateStart, i18nc( "@item:intable", "Start Date" ) ) );
    mAccessors->insert( QLatin1String( "parent_type" ),
                        new TaskAccessorPair( getParentType, setParentType, QString() ) );
    mAccessors->insert( QLatin1String( "parent_id" ),
                        new TaskAccessorPair( getParentId, setParentId, QString() ) );
    mAccessors->insert( QLatin1String( "contact_id" ),
                        new TaskAccessorPair( getContactId, setContactId, QString() ) );
    mAccessors->insert( QLatin1String( "priority" ),
                        new TaskAccessorPair( getPriority, setPriority, i18nc( "@item:intable", "Priority" ) ) );
}

TasksHandler::~TasksHandler()
{
    qDeleteAll( *mAccessors );
    delete mAccessors;
}

QStringList TasksHandler::supportedFields() const
{
    return mAccessors->keys();
}

Akonadi::Collection TasksHandler::collection() const
{
    Akonadi::Collection taskCollection;
    taskCollection.setRemoteId( moduleName() );
    taskCollection.setContentMimeTypes( QStringList() << KCalCore::Todo::todoMimeType() );
    taskCollection.setName( i18nc( "@item folder name", "Tasks" ) );
    taskCollection.setRights( Akonadi::Collection::CanChangeItem |
                              Akonadi::Collection::CanCreateItem |
                              Akonadi::Collection::CanDeleteItem );

    return taskCollection;
}

void TasksHandler::listEntries( const ListEntriesScope &scope )
{
    const QString query = scope.query( QLatin1String( "tasks" ) );
    const QString orderBy = QLatin1String( "tasks.id" );
    const int offset = scope.offset();
    const int maxResults = 100;
    const int fetchDeleted = scope.deleted();

    TNS__Select_fields selectedFields;
    selectedFields.setItems( mAccessors->keys() );

    soap()->asyncGet_entry_list( sessionId(), moduleName(), query, orderBy, offset, selectedFields, maxResults, fetchDeleted );
}

bool TasksHandler::setEntry( const Akonadi::Item &item )
{
    if ( !item.hasPayload<KCalCore::Todo::Ptr>() ) {
        kError() << "item (id=" << item.id() << ", remoteId=" << item.remoteId()
                 << ", mime=" << item.mimeType() << ") is missing Todo payload";
        return false;
    }

    QList<TNS__Name_value> itemList;

    // if there is an id add it, otherwise skip this field
    // no id will result in the contact being added
    if ( !item.remoteId().isEmpty() ) {
        TNS__Name_value field;
        field.setName( QLatin1String( "id" ) );
        field.setValue( item.remoteId() );

        itemList << field;
    }

    const KCalCore::Todo::Ptr todo = item.payload<KCalCore::Todo::Ptr>();
    AccessorHash::const_iterator it    = mAccessors->constBegin();
    AccessorHash::const_iterator endIt = mAccessors->constEnd();
    for ( ; it != endIt; ++it ) {
        // check if this is a read-only field
        if ( (*it)->getter == 0 ) {
            continue;
        }
        TNS__Name_value field;
        field.setName( it.key() );
        field.setValue( (*it)->getter( *todo ) );

        itemList << field;
    }

    TNS__Name_value_list valueList;
    valueList.setItems( itemList );
    soap()->asyncSet_entry( sessionId(), moduleName(), valueList );

    return true;
}

Akonadi::Item TasksHandler::itemFromEntry( const TNS__Entry_value &entry, const Akonadi::Collection &parentCollection )
{
    Akonadi::Item item;

    const QList<TNS__Name_value> valueList = entry.name_value_list().items();
    if ( valueList.isEmpty() ) {
        kWarning() << "Tasks entry for id=" << entry.id() << "has no values";
        return item;
    }

    item.setRemoteId( entry.id() );
    item.setParentCollection( parentCollection );
    item.setMimeType( KCalCore::Todo::todoMimeType() );

    KCalCore::Todo::Ptr todo( new KCalCore::Todo );
    todo->setUid( entry.id() );

    Q_FOREACH( const TNS__Name_value &namedValue, valueList ) {
        const AccessorHash::const_iterator accessIt = mAccessors->constFind( namedValue.name() );
        if ( accessIt == mAccessors->constEnd() ) {
            // no accessor for field
            continue;
        }

        (*accessIt)->setter( namedValue.value(), *todo );
    }    
    
    item.setPayload<KCalCore::Todo::Ptr>( todo );
    item.setRemoteRevision( getDateModified( *todo ) );

    return item;
}

void TasksHandler::compare( Akonadi::AbstractDifferencesReporter *reporter,
                               const Akonadi::Item &leftItem, const Akonadi::Item &rightItem )
{
    Q_ASSERT( leftItem.hasPayload<KCalCore::Todo::Ptr>() );
    Q_ASSERT( rightItem.hasPayload<KCalCore::Todo::Ptr>() );

    const KCalCore::Todo::Ptr leftTask = leftItem.payload<KCalCore::Todo::Ptr>();
    const KCalCore::Todo::Ptr rightTask = rightItem.payload<KCalCore::Todo::Ptr>();

    const QString modifiedBy = mSession->userName();
    const QString modifiedOn = formatDate( getDateModified( *rightTask ) );

    reporter->setLeftPropertyValueTitle( i18nc( "@title:column", "Local Task" ) );
    reporter->setRightPropertyValueTitle(
        i18nc( "@title:column", "Serverside Task: modified by %1 on %2",
               modifiedBy, modifiedOn ) );

    AccessorHash::const_iterator it    = mAccessors->constBegin();
    AccessorHash::const_iterator endIt = mAccessors->constEnd();
    for ( ; it != endIt; ++it ) {
        // check if this is a read-only field
        if ( (*it)->getter == 0 ) {
            continue;
        }

        QString leftValue = (*it)->getter( *leftTask );
        QString rightValue = (*it)->getter( *rightTask );

        QString diffName = (*it)->diffName;
        if ( diffName.isEmpty() ) {
            // TODO might need override for Status and Priority
            // internal field, skip
            continue;
        }

        if ( leftValue.isEmpty() && rightValue.isEmpty() ) {
            continue;
        }

        if ( leftValue.isEmpty() ) {
            reporter->addProperty( Akonadi::AbstractDifferencesReporter::AdditionalRightMode,
                                   diffName, leftValue, rightValue );
        } else if ( rightValue.isEmpty() ) {
            reporter->addProperty( Akonadi::AbstractDifferencesReporter::AdditionalLeftMode,
                                   diffName, leftValue, rightValue );
        } else if ( leftValue != rightValue ) {
            reporter->addProperty( Akonadi::AbstractDifferencesReporter::ConflictMode,
                                   diffName, leftValue, rightValue );
        }
    }
}
