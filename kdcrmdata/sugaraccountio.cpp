#include "sugaraccountio.h"
#include "sugaraccount.h"

#include <QtCore/QIODevice>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>

bool SugarAccountIO::readSugarAccount( QIODevice *device, SugarAccount &account )
{
    if ( device == 0 || !device->isReadable() )
        return false;

    account = SugarAccount();

    QTextStream reader( device );
    while ( !reader.atEnd() )
        qDebug() << reader.readLine();

    // write into the account
}

bool SugarAccountIO::writeSugarAccount(  const SugarAccount &account, QIODevice *device )
{
    QTextStream writer( device );
}

