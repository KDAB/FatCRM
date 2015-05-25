/*
  Copyright (c) 2009 Tobias Koenig <tokoe@kde.org>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
*/

#include "qcsvreader.h"

#include <QtCore/QStringList>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>
#include <QDebug>

#include <KLocalizedString>

QCsvBuilderInterface::~QCsvBuilderInterface()
{
}

class QCsvReader::Private
{
public:
    Private( QCsvBuilderInterface *builder )
        : mBuilder( builder ), mNotTerminated( true )
    {
        mTextQuote = QLatin1Char( '"' );
        mDelimiter = QLatin1Char( ' ' );
        mStartRow = 0;
        mCodec = QTextCodec::codecForLocale();
    }

    void emitBeginLine( uint row );
    void emitEndLine( uint row );
    void emitField( const QString &data, int row, int column );

    QCsvBuilderInterface *mBuilder;
    QTextCodec *mCodec;
    QChar mTextQuote;
    QChar mDelimiter;

    uint mStartRow;
    bool mNotTerminated;
};

void QCsvReader::Private::emitBeginLine( uint row )
{
    if ( ( row - mStartRow ) > 0 ) {
        mBuilder->beginLine();
    }
}

void QCsvReader::Private::emitEndLine( uint row )
{
    if ( ( row - mStartRow ) > 0 ) {
        mBuilder->endLine();
    }
}

void QCsvReader::Private::emitField( const QString &data, int row, int column )
{
    if ( ( row - mStartRow ) > 0 ) {
        mBuilder->field( data, row - mStartRow - 1, column - 1 );
    }
}

QCsvReader::QCsvReader( QCsvBuilderInterface *builder )
    : d( new Private( builder ) )
{
    Q_ASSERT( builder );
}

QCsvReader::~QCsvReader()
{
    delete d;
}

bool QCsvReader::read( QIODevice *device )
{
    enum State {
        StartLine,
        QuotedField,
        QuotedFieldEnd,
        NormalField,
        EmptyField
    };

    int row, column;

    QString field;
    QChar input;
    State currentState = StartLine;

    row = column = 1;

    d->mBuilder->begin();

    if ( !device->isOpen() ) {
        d->emitBeginLine( row );
        d->mBuilder->error( i18n("Device is not open") );
        d->emitEndLine( row );
        d->mBuilder->end();
        return false;
    }

    QTextStream inputStream( device );
    inputStream.setCodec( d->mCodec );

    /**
   * We use the following state machine to parse CSV:
   *
   * digraph {
   *   StartLine -> StartLine [label="\\r\\n"]
   *   StartLine -> QuotedField [label="Quote"]
   *   StartLine -> EmptyField [label="Delimiter"]
   *   StartLine -> NormalField [label="Other Char"]
   *
   *   QuotedField -> QuotedField [label="\\r\\n"]
   *   QuotedField -> QuotedFieldEnd [label="Quote"]
   *   QuotedField -> QuotedField [label="Delimiter"]
   *   QuotedField -> QuotedField [label="Other Char"]
   *
   *   QuotedFieldEnd -> StartLine [label="\\r\\n"]
   *   QuotedFieldEnd -> QuotedField [label="Quote"]
   *   QuotedFieldEnd -> EmptyField [label="Delimiter"]
   *   QuotedFieldEnd -> EmptyField [label="Other Char"]
   *
   *   EmptyField -> StartLine [label="\\r\\n"]
   *   EmptyField -> QuotedField [label="Quote"]
   *   EmptyField -> EmptyField [label="Delimiter"]
   *   EmptyField -> NormalField [label="Other Char"]
   *
   *   NormalField -> StartLine [label="\\r\\n"]
   *   NormalField -> NormalField [label="Quote"]
   *   NormalField -> EmptyField [label="Delimiter"]
   *   NormalField -> NormalField [label="Other Char"]
   * }
   */

    while ( !inputStream.atEnd() && d->mNotTerminated ) {
        inputStream >> input;

        switch ( currentState ) {
        case StartLine:
            if ( input == QLatin1Char( '\r' ) || input == QLatin1Char( '\n' ) ) {
                currentState = StartLine;
            } else if ( input == d->mTextQuote ) {
                d->emitBeginLine( row );
                currentState = QuotedField;
            } else if ( input == d->mDelimiter ) {
                d->emitBeginLine( row );
                d->emitField( field, row, column );
                column++;
                currentState = EmptyField;
            } else {
                d->emitBeginLine( row );
                field.append( input );
                currentState = NormalField;
            }
            break;
        case QuotedField:
            if ( input == QLatin1Char( '\r' ) || input == QLatin1Char( '\n' ) ) {
                field.append( input );
                currentState = QuotedField;
            } else if ( input == d->mTextQuote ) {
                currentState = QuotedFieldEnd;
            } else if ( input == d->mDelimiter ) {
                field.append( input );
                currentState = QuotedField;
            } else {
                field.append( input );
                currentState = QuotedField;
            }
            break;
        case QuotedFieldEnd:
            if ( input == QLatin1Char( '\r' ) || input == QLatin1Char( '\n' ) ) {
                d->emitField( field, row, column );
                field.clear();
                d->emitEndLine( row );
                column = 1;
                row++;
                currentState = StartLine;
            } else if ( input == d->mTextQuote ) {
                field.append( input );
                currentState = QuotedField;
            } else if ( input == d->mDelimiter ) {
                d->emitField( field, row, column );
                field.clear();
                column++;
                currentState = EmptyField;
            } else {
                d->emitField( field, row, column );
                field.clear();
                column++;
                field.append( input );
                currentState = EmptyField;
            }
            break;
        case NormalField:
            if ( input == QLatin1Char( '\r' ) || input == QLatin1Char( '\n' ) ) {
                d->emitField( field, row, column );
                field.clear();
                d->emitEndLine( row );
                row++;
                column = 1;
                currentState = StartLine;
            } else if ( input == d->mTextQuote ) {
                field.append( input );
                currentState = NormalField;
            } else if ( input == d->mDelimiter ) {
                d->emitField( field, row, column );
                field.clear();
                column++;
                currentState = EmptyField;
            } else {
                field.append( input );
                currentState = NormalField;
            }
            break;
        case EmptyField:
            if ( input == QLatin1Char( '\r' ) || input == QLatin1Char( '\n' ) ) {
                d->emitField( QString(), row, column );
                field.clear();
                d->emitEndLine( row );
                column = 1;
                row++;
                currentState = StartLine;
            } else if ( input == d->mTextQuote ) {
                currentState = QuotedField;
            } else if ( input == d->mDelimiter ) {
                d->emitField( QString(), row, column );
                column++;
                currentState = EmptyField;
            } else {
                field.append( input );
                currentState = NormalField;
            }
            break;
        }
    }

    if ( currentState != StartLine ) {
        if ( field.length() > 0 ) {
            d->emitField( field, row, column );
            ++row;
            field.clear();
        }
        d->emitEndLine( row );
    }

    d->mBuilder->end();

    return true;
}

void QCsvReader::setTextQuote( const QChar &textQuote )
{
    d->mTextQuote = textQuote;
}

QChar QCsvReader::textQuote() const
{
    return d->mTextQuote;
}

void QCsvReader::setDelimiter( const QChar &delimiter )
{
    d->mDelimiter = delimiter;
}

QChar QCsvReader::delimiter() const
{
    return d->mDelimiter;
}

void QCsvReader::setStartRow( uint startRow )
{
    d->mStartRow = startRow;
}

uint QCsvReader::startRow() const
{
    return d->mStartRow;
}

void QCsvReader::setTextCodec( QTextCodec *textCodec )
{
    d->mCodec = textCodec;
}

QTextCodec *QCsvReader::textCodec() const
{
    return d->mCodec;
}

void QCsvReader::terminate()
{
    d->mNotTerminated = false;
}

class QCsvStandardBuilder::Private
{
public:
    Private()
    {
        init();
    }

    void init();

    QString mLastErrorString;
    uint mRowCount;
    uint mColumnCount;
    QList<QStringList> mRows;
};

void QCsvStandardBuilder::Private::init()
{
    mRows.clear();
    mRowCount = 0;
    mColumnCount = 0;
    mLastErrorString.clear();
}

QCsvStandardBuilder::QCsvStandardBuilder()
    : d( new Private )
{
}

QCsvStandardBuilder::~QCsvStandardBuilder()
{
    delete d;
}

QString QCsvStandardBuilder::lastErrorString() const
{
    return d->mLastErrorString;
}

uint QCsvStandardBuilder::rowCount() const
{
    return d->mRowCount;
}

uint QCsvStandardBuilder::columnCount() const
{
    return d->mColumnCount;
}

QString QCsvStandardBuilder::data( uint row, uint column ) const
{
    if ( row > d->mRowCount || column > d->mColumnCount || column >= (uint)d->mRows[ row ].count() ) {
        return QString();
    }

    //qDebug() << "mRows[" << row << "][" << column << "] = " << d->mRows[ row ][ column ];
    return d->mRows[ row ][ column ];
}

void QCsvStandardBuilder::begin()
{
    d->init();
}

void QCsvStandardBuilder::beginLine()
{
    d->mRows.append( QStringList() );
    d->mRowCount++;
}

void QCsvStandardBuilder::field( const QString &data, uint row, uint column )
{
    const uint size = d->mRows[ row ].size();
    if ( column >= size ) {
        for ( uint i = column; i < size + 1; ++i ) {
            d->mRows[ row ].append( QString() );
        }
    }

    d->mRows[ row ][ column ] = data;
    //qDebug() << "mRows[" << row << "][" << column << "] = " << data;

    d->mColumnCount = qMax( d->mColumnCount, column + 1 );
}

void QCsvStandardBuilder::endLine()
{
}

void QCsvStandardBuilder::end()
{
}

void QCsvStandardBuilder::error( const QString &errorMsg )
{
    d->mLastErrorString = errorMsg;
}

