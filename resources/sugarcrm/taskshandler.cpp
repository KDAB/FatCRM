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
#include "sugarcrmresource_debug.h"
#include "sugarsession.h"
#include "sugarsoap.h"
using namespace KDSoapGenerated;

#include "kdcrmdata/kdcrmfields.h"
#include "kdcrmdata/kdcrmutils.h"

#include <KCalCore/Todo>
using namespace KCalCore;

#include <AkonadiCore/abstractdifferencesreporter.h>
#include <AkonadiCore/Collection>

#include <KDateTime>
#include <KLocalizedString>

#include <QHash>

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
    todo.setCreated( KDateTime::fromString(value, QStringLiteral("%Y-%m-%d %H:%M:%S")) );
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

static QString getAssignedUserName( const KCalCore::Todo &todo )
{
    return todo.customProperty( "SugarCRM", "X-AssignedUserName" );
}

static void setAssignedUserName( const QString &value, KCalCore::Todo &todo )
{
    todo.setCustomProperty( "SugarCRM", "X-AssignedUserName", value );
}

static QString getStatus( const KCalCore::Todo &todo )
{
    if ( todo.status() == KCalCore::Todo::StatusConfirmed ) {
        return QStringLiteral( "Not Started" );
    } else if ( todo.status() == KCalCore::Todo::StatusInProcess ) {
        return QStringLiteral( "In Progress" );
    } else if ( todo.status() == KCalCore::Todo::StatusCompleted ) {
        return QStringLiteral( "Completed" );
    } else if ( todo.status() == KCalCore::Todo::StatusNeedsAction ) {
        return QStringLiteral( "Pending Input" );
    } else if ( todo.customStatus() == QLatin1String("StatusDeferred") ) {
        return QStringLiteral( "Deferred" );
    } else {
        return QStringLiteral( "None" );
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
        todo.setCustomStatus( QStringLiteral( "StatusDeferred" ) );
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
    return dateDue.toString( QStringLiteral("%Y-%m-%d %H:%M:%S") );
}

static void setDateDue( const QString &value, KCalCore::Todo &todo )
{
    todo.setDtDue( KDateTime::fromString( value, QStringLiteral("%Y-%m-%d %H:%M:%S"), nullptr, true ) );
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
    return dateStart.toString( QStringLiteral("%Y-%m-%d %H:%M:%S") );
}

static void setDateStart( const QString &value, KCalCore::Todo &todo )
{
    todo.setDtStart( KDateTime::fromString( value, QStringLiteral("%Y-%m-%d %H:%M:%S"), nullptr, true ) );
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
        return QStringLiteral( "High" );
    } else if ( todo.priority() == 5 ) {
        return QStringLiteral( "Medium" );
    } else if ( todo.priority() == 9 ) {
        return QStringLiteral( "Low" );
    } else {
        return QStringLiteral( "None" );
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
    : ModuleHandler( QStringLiteral( "Tasks" ), session ),
      mAccessors( new AccessorHash )
{
    mAccessors->insert( KDCRMFields::id(),
                        new TaskAccessorPair( getId, setId, QString() ) );
    mAccessors->insert( KDCRMFields::name(),
                        new TaskAccessorPair( getSummary, setSummary, i18nc( "@item:intable TODO title", "Title" ) ) );
    mAccessors->insert( KDCRMFields::dateEntered(),
                        new TaskAccessorPair( getDateEntered, setDateEntered, i18nc( "@item:intable", "Creation Date" ) ) );
    mAccessors->insert( KDCRMFields::dateModified(),
                        new TaskAccessorPair( getDateModified, setDateModified, i18nc( "@item:intable", "Modification Date" ) ) );
    mAccessors->insert( KDCRMFields::modifiedUserId(),
                        new TaskAccessorPair( getModifiedUserId, setModifiedUserId, QString() ) );
    mAccessors->insert( KDCRMFields::createdBy(),
                        new TaskAccessorPair( getCreatedBy, setCreatedBy, QString() ) );
    mAccessors->insert( KDCRMFields::description(),
                        new TaskAccessorPair( getDescription, setDescription, i18nc( "@item:intable", "Description" ) ) );
    mAccessors->insert( KDCRMFields::deleted(),
                        new TaskAccessorPair( getDeleted, setDeleted, QString() ) );
    mAccessors->insert( KDCRMFields::assignedUserId(),
                        new TaskAccessorPair( getAssignedUserId, setAssignedUserId, QString() ) );
    mAccessors->insert( KDCRMFields::assignedUserName(),
                        new TaskAccessorPair( getAssignedUserName, setAssignedUserName, QString() ) );
    mAccessors->insert( KDCRMFields::status(),
                        new TaskAccessorPair( getStatus, setStatus, i18nc( "@item:intable", "Status" ) ) );
    mAccessors->insert( KDCRMFields::dateDueFlag(),
                        new TaskAccessorPair( getDateDueFlag, setDateDueFlag, QString() ) );
    mAccessors->insert( KDCRMFields::dateDue(),
                        new TaskAccessorPair( getDateDue, setDateDue, i18nc( "@item:intable", "Due Date" ) ) );
    mAccessors->insert( KDCRMFields::dateStartFlag(),
                        new TaskAccessorPair( getDateStartFlag, setDateStartFlag, QString() ) );
    mAccessors->insert( KDCRMFields::dateStart(),
                        new TaskAccessorPair( getDateStart, setDateStart, i18nc( "@item:intable", "Start Date" ) ) );
    mAccessors->insert( KDCRMFields::parentType(),
                        new TaskAccessorPair( getParentType, setParentType, QString() ) );
    mAccessors->insert( KDCRMFields::parentId(),
                        new TaskAccessorPair( getParentId, setParentId, QString() ) );
    mAccessors->insert( KDCRMFields::contactId(),
                        new TaskAccessorPair( getContactId, setContactId, QString() ) );
    mAccessors->insert( KDCRMFields::priority(),
                        new TaskAccessorPair( getPriority, setPriority, i18nc( "@item:intable", "Priority" ) ) );
}

TasksHandler::~TasksHandler()
{
    qDeleteAll( *mAccessors );
    delete mAccessors;
}

Akonadi::Collection TasksHandler::handlerCollection() const
{
    Akonadi::Collection taskCollection;
    taskCollection.setContentMimeTypes( QStringList() << KCalCore::Todo::todoMimeType() );
    taskCollection.setName( i18nc( "@item folder name", "Tasks" ) );
    taskCollection.setRights( Akonadi::Collection::CanChangeItem |
                              Akonadi::Collection::CanCreateItem |
                              Akonadi::Collection::CanDeleteItem );

    return taskCollection;
}

QString TasksHandler::orderByForListing() const
{
    return QStringLiteral("tasks.id");
}

QStringList TasksHandler::supportedSugarFields() const
{
    return sugarFieldsFromCrmFields(mAccessors->keys());
}

QStringList TasksHandler::supportedCRMFields() const
{
    return mAccessors->keys();
}

bool TasksHandler::setEntry( const Akonadi::Item &item )
{
    if ( !item.hasPayload<KCalCore::Todo::Ptr>() ) {
        qCCritical(FATCRM_SUGARCRMRESOURCE_LOG) << "item (id=" << item.id() << ", remoteId=" << item.remoteId()
                 << ", mime=" << item.mimeType() << ") is missing Todo payload";
        return false;
    }

    QList<TNS__Name_value> itemList;

    // if there is an id add it, otherwise skip this field
    // no id will result in the contact being added
    if ( !item.remoteId().isEmpty() ) {
        TNS__Name_value field;
        field.setName( QStringLiteral( "id" ) );
        field.setValue( item.remoteId() );

        itemList << field;
    }

    const KCalCore::Todo::Ptr todo = item.payload<KCalCore::Todo::Ptr>();
    AccessorHash::const_iterator it    = mAccessors->constBegin();
    AccessorHash::const_iterator endIt = mAccessors->constEnd();
    for ( ; it != endIt; ++it ) {
        // check if this is a read-only field
        if ( (*it)->getter == nullptr ) {
            continue;
        }
        TNS__Name_value field;
        field.setName(sugarFieldFromCrmField(it.key()));
        field.setValue(KDCRMUtils::encodeXML((*it)->getter(*todo)));

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
        qCWarning(FATCRM_SUGARCRMRESOURCE_LOG) << "Tasks entry for id=" << entry.id() << "has no values";
        return item;
    }

    item.setRemoteId( entry.id() );
    item.setParentCollection( parentCollection );
    item.setMimeType( KCalCore::Todo::todoMimeType() );

    KCalCore::Todo::Ptr todo( new KCalCore::Todo );
    todo->setUid( entry.id() );

    Q_FOREACH( const TNS__Name_value &namedValue, valueList ) {
        const QString crmFieldName = sugarFieldToCrmField(namedValue.name());
        const AccessorHash::const_iterator accessIt = mAccessors->constFind(crmFieldName);
        if ( accessIt == mAccessors->constEnd() ) {
            // no accessor for field
            continue;
        }

        (*accessIt)->setter(KDCRMUtils::decodeXML(namedValue.value()), *todo );
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
        if ( (*it)->getter == nullptr ) {
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
