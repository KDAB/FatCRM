#include "conflictresolvedialog.h"

#include <akonadi/differencesalgorithminterface.h>
#include <akonadi/item.h>

#include <KTextBrowser>

using namespace Akonadi;

class ConflictResolveDialog::Private
{
    ConflictResolveDialog *const q;
public:
    explicit Private( ConflictResolveDialog *parent )
        : q( parent ), mDiffInterface( 0 ), mView( 0 )
    {
    }

public:
    Item mLocalItem;
    Item mOtherItem;

    DifferencesAlgorithmInterface *mDiffInterface;

    KTextBrowser *mView;
};

ConflictResolveDialog::ConflictResolveDialog( QWidget *parent )
    : KDialog( parent ), d( new Private( this ) )
{
}

ConflictResolveDialog::~ConflictResolveDialog()
{
    delete d;
}

void ConflictResolveDialog::setConflictingItems( const Item &localItem, const Item &otherItem )
{
    d->mLocalItem = localItem;
    d->mOtherItem = otherItem;
}

void ConflictResolveDialog::setDifferencesInterface( DifferencesAlgorithmInterface *interface )
{
    d->mDiffInterface = interface;
}

ConflictHandler::ResolveStrategy ConflictResolveDialog::resolveStrategy() const
{
    return ConflictHandler::UseLocalItem;
}


#include "conflictresolvedialog.moc"
