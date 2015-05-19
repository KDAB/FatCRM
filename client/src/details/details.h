#ifndef DETAILS_H
#define DETAILS_H

#include "enums.h"

#include <akonadi/item.h>

#include <QtGui>

class NotesRepository;

class Details : public QWidget
{
    Q_OBJECT

public:
    explicit Details(DetailsType type, QWidget *parent = 0);

    ~Details();

    virtual QMap<QString, QString> data(const Akonadi::Item &item) const = 0;
    virtual void updateItem(Akonadi::Item &item, const QMap<QString, QString> &data) const = 0;

    void setData(const QMap<QString, QString> &data, QWidget *createdModifiedContainer);
    const QMap<QString, QString> getData() const;
    void clear();

    void setResourceIdentifier(const QByteArray &ident, const QString &baseUrl);
    virtual void setNotesRepository(NotesRepository *notesRepo) { Q_UNUSED(notesRepo); }

    DetailsType type() const
    {
        return mType;
    }

    QString windowTitle() const;
    void assignToMe();

Q_SIGNALS:
    void modified();

protected:
    QByteArray resourceIdentifier() const { return mResourceIdentifier; }
    QString resourceBaseUrl() const { return mResourceBaseUrl; }

    QStringList industryItems() const;
    QStringList sourceItems() const;
    QStringList stageItems() const;
    QStringList salutationItems() const;

    virtual void setDataInternal(const QMap<QString, QString> &) const {}
    QString id() const;

private Q_SLOTS:
    void doConnects();

private:
    QString currentAccountId() const;
    QString currentAssignedToId() const;
    QString currentCampaignId() const;
    QString currentReportsToId() const;

    const DetailsType mType;
    QByteArray mResourceIdentifier;
    QString mResourceBaseUrl;
    QStringList mKeys;
};
#endif /* DETAILS_H */

