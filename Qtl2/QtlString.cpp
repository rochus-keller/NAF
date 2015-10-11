/*
 * Copyright 2000-2015 Rochus Keller <mailto:rkeller@nmr.ch>
 *
 * This file is part of the CARA (Computer Aided Resonance Assignment,
 * see <http://cara.nmr.ch/>) NMR Application Framework (NAF) library.
 *
 * The following is the license that applies to this copy of the
 * library. For a license to use the library under conditions
 * other than those described here, please email to rkeller@nmr.ch.
 *
 * GNU General Public License Usage
 * This file may be used under the terms of the GNU General Public
 * License (GPL) versions 2.0 or 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in
 * the packaging of this file. Please review the following information
 * to ensure GNU General Public Licensing requirements will be met:
 * http://www.fsf.org/licensing/licenses/info/GPLv2.html and
 * http://www.gnu.org/copyleft/gpl.html.
 */

#include "QtlString.h"
#include <Script2/QtValue.h>
#include <Script/Util.h>
#include <QStringList>
using namespace Qtl;
using namespace Lua;

static int init(lua_State * L)
{
    QString* obj = QtValue<QString>::check( L, 1 );
	if( Util::isStr( L, 2 ) )
		*obj = QString::fromLatin1( Util::toStr( L, 2 ) );
    else if( QString* str = QtValue<QString>::cast( L, 2 ) )
		*obj = *str;
	else if( Util::isNum( L, 2 ) )
        *obj = QString( Util::toInt( L, 2 ), *QtValue<QChar>::check( L, 3 ) );
    else if( QChar* ch = QtValue<QChar>::cast( L, 2 ) )
		*obj = QString( *ch );
	else
		Util::error( L, "invalid argument" );
	return 0;
}

static int append( lua_State * L)
{
    QString* obj = QtValue<QString>::check( L, 1 );
    if( QtValueBase::isString( L, 2 ) )
        obj->append( QtValueBase::toString( L, 2 ) );
    else if( QChar* ch = QtValue<QChar>::cast( L, 2 ) )
		obj->append( *ch );
	else
		Util::error( L, "invalid argument" );
	lua_pushvalue( L, 1 );
	return 1;
}

static int toLatin1( lua_State * L)
{
    QString* obj = QtValue<QString>::check( L, 1 );
    Util::push( L, obj->toLatin1() );
	return 1;
}

static int arg( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	if( QtValueBase::isString( L, 2 ) )
	{
		if( Util::top( L ) == 2 )
			obj->arg( QtValueBase::toString( L, 2 ) );
		else if( Util::isNum( L, 3 ) )
		{
			if( QChar* c = QtValue<QChar>::cast( L, 4 ) )
				obj->arg( QtValueBase::toString( L, 2 ), Util::toInt( L, 3 ), *c );
			else
				obj->arg( QtValueBase::toString( L, 2 ), Util::toInt( L, 3 ) );
		}else
		{
			switch( Util::top(L) )
			{
			case 3:
				obj->arg( QtValueBase::toString( L, 2 ), QtValueBase::toString( L, 3 ) );
				break;
			case 4:
				obj->arg( QtValueBase::toString( L, 2 ), QtValueBase::toString( L, 3 ), QtValueBase::toString( L, 4 ) );
				break;
			case 5:
				obj->arg( QtValueBase::toString( L, 2 ), QtValueBase::toString( L, 3 ), QtValueBase::toString( L, 4 ),
						  QtValueBase::toString( L, 5 ) );
				break;
			case 6:
				obj->arg( QtValueBase::toString( L, 2 ), QtValueBase::toString( L, 3 ), QtValueBase::toString( L, 4 ),
						  QtValueBase::toString( L, 5 ), QtValueBase::toString( L, 6 ) );
				break;
			case 7:
				obj->arg( QtValueBase::toString( L, 2 ), QtValueBase::toString( L, 3 ), QtValueBase::toString( L, 4 ),
						  QtValueBase::toString( L, 5 ), QtValueBase::toString( L, 6 ), QtValueBase::toString( L, 7 ) );
				break;
			default:
				luaL_error( L, "arguments not supported" );
				break;
			}
		}

	}else if( Util::isNum( L, 2 ) )
	{
		if( Util::isInt( L, 2 ) )
		{
			switch( Util::top(L) )
			{
			case 2:
				obj->arg( Util::toInt(L, 2) );
				break;
			case 3:
				obj->arg( Util::toInt(L, 2), Util::toInt(L, 3) );
				break;
			case 4:
				obj->arg( Util::toInt(L, 2), Util::toInt(L, 3), Util::toInt(L, 4 ) );
				break;
			case 5:
				obj->arg( Util::toInt(L, 2), Util::toInt(L, 3), Util::toInt(L, 4 ), *QtValue<QChar>::check( L, 5 ) );
				break;
			}
		}else
		{
			switch( Util::top(L) )
			{
			case 2:
				obj->arg( Util::toDbl(L, 2) );
				break;
			case 3:
				obj->arg( Util::toDbl(L, 2), Util::toInt(L, 3) );
				break;
			case 4:
				obj->arg( Util::toDbl(L, 2), Util::toInt(L, 3), Util::toChar(L, 4 ) );
				break;
			case 5:
				obj->arg( Util::toDbl(L, 2), Util::toInt(L, 3), Util::toChar(L, 4 ), Util::toInt(L, 5) );
				break;
			case 6:
				obj->arg( Util::toDbl(L, 2), Util::toInt(L, 3), Util::toChar(L, 4 ), Util::toInt(L, 5), *QtValue<QChar>::check( L, 5 ) );
				break;
			}
		}
	}else if( QChar* c = QtValue<QChar>::cast( L, 2 ) )
	{
		switch( Util::top(L) )
		{
		case 2:
			obj->arg( *c );
			break;
		case 3:
			obj->arg( *c, Util::toInt(L, 3) );
			break;
		case 4:
			obj->arg( *c, Util::toInt(L, 3), *QtValue<QChar>::check( L, 4 ) );
			break;
		}
	}
	lua_pushvalue( L, 1 );
	return 1;
}

static int at( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	*QtValue<QChar>::create( L ) = obj->at( Util::toInt( L, 2 ) );
	return 1;
}

static int capacity( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	Util::push( L, obj->capacity() );
	return 1;
}

static int chop( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	obj->chop( Util::toInt(L,2) );
	return 0;
}

static int clear( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	obj->clear();
	return 0;
}

static Qt::CaseSensitivity toCs( lua_State * L, int n )
{
	Qt::CaseSensitivity cs = Qt::CaseSensitive;
	if( Util::top( L ) < n )
		return cs;
	switch( Util::toInt( L, n ) )
	{
	case Qt::CaseInsensitive:
		cs = Qt::CaseInsensitive;
		break;
	case Qt::CaseSensitive:
		cs = Qt::CaseSensitive;
		break;
	default:
		luaL_argerror( L, n, "expecting an integer value in Qt::CaseSensitivity");
	}
	return cs;
}

static int compare( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	Qt::CaseSensitivity cs = Qt::CaseSensitive;
	if( Util::top( L ) > 2 )
		cs = toCs( L, 3 );
	Util::push( L, obj->compare( QtValueBase::toString( L, 2 ), cs ) );
	return 1;
}

static int contains( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	if( QtValueBase::isString( L, 2 ) )
	{
		Util::push( L, obj->compare( QtValueBase::toString( L, 2 ), toCs( L, 3 ) ) );
	}else if( QChar* c = QtValue<QChar>::cast( L, 2 ) )
	{
		Util::push( L, obj->compare( *c, toCs( L, 3 ) ) );
	}else
		luaL_error( L, "arguments not supported" );
	// TODO: QRegExp
	return 1;
}

static int count( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	if( Util::top( L ) == 1 )
		Util::push( L, obj->count() );
	else if( QtValueBase::isString( L, 2 ) )
	{
		Util::push( L, obj->count( QtValueBase::toString( L, 2 ), toCs( L, 3 ) ) );
	}else if( QChar* c = QtValue<QChar>::cast( L, 2 ) )
	{
		Util::push( L, obj->count( *c, toCs( L, 3 ) ) );
	}else
		luaL_error( L, "arguments not supported" );
	// TODO: QRegExp
	return 1;
}

static int endsWith( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	if( QtValueBase::isString( L, 2 ) )
	{
		Util::push( L, obj->endsWith( QtValueBase::toString( L, 2 ), toCs( L, 3 ) ) );
	}else if( QChar* c = QtValue<QChar>::cast( L, 2 ) )
	{
		Util::push( L, obj->endsWith( *c, toCs( L, 3 ) ) );
	}else
		luaL_error( L, "arguments not supported" );
	return 1;
}

static int fill( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	int size = -1;
	if( Util::top(L) > 2 )
		size = Util::toInt( L, 3 );
	obj->fill( *QtValue<QChar>::check( L, 2 ), size );
	lua_pushvalue( L, 1 );
	return 1;
}

static int indexOf( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	int from = 0;
	if( Util::top(L) > 2 )
		from = Util::toInt(L,3);
	if( QtValueBase::isString( L, 2 ) )
	{
		Util::push( L, obj->indexOf( QtValueBase::toString( L, 2 ), from, toCs( L, 4 ) ) );
	}else if( QChar* c = QtValue<QChar>::cast( L, 2 ) )
	{
		Util::push( L, obj->indexOf( *c, from, toCs( L, 4 ) ) );
	}else
		luaL_error( L, "arguments not supported" );
	// TODO: QRegExp
	return 1;
}

static int insert( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	if( QtValueBase::isString( L, 3 ) )
	{
		Util::push( L, obj->insert( Util::toInt( L, 2 ), QtValueBase::toString( L, 3 ) ) );
	}else if( QChar* c = QtValue<QChar>::cast( L, 2 ) )
	{
		Util::push( L, obj->insert( Util::toInt( L, 2 ), *c ) );
	}else
		luaL_error( L, "arguments not supported" );
	lua_pushvalue( L, 1 );
	return 1;
}

static int isEmpty( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	Util::push( L, obj->isEmpty() );
	return 1;
}

static int isNull( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	Util::push( L, obj->isNull() );
	return 1;
}

static int lastIndexOf( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	int from = -1;
	if( Util::top(L) > 2 )
		from = Util::toInt(L,3);
	if( QtValueBase::isString( L, 2 ) )
	{
		Util::push( L, obj->lastIndexOf( QtValueBase::toString( L, 2 ), from, toCs( L, 4 ) ) );
	}else if( QChar* c = QtValue<QChar>::cast( L, 2 ) )
	{
		Util::push( L, obj->lastIndexOf( *c, from, toCs( L, 4 ) ) );
	}else
		luaL_error( L, "arguments not supported" );
	// TODO: QRegExp
	return 1;
}

static int left( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	*QtValue<QString>::create(L) = obj->left( Util::toInt( L, 2 ) );
	return 1;
}

static int leftJustified( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	QChar fill = QLatin1Char( ' ' );
	if( Util::top( L ) > 2 )
		fill = *QtValue<QChar>::check( L, 3 );
	bool truncate = false;
	if( Util::top( L ) > 3 )
		truncate = Util::toBool( L, 4 );
	*QtValue<QString>::create(L) = obj->leftJustified( Util::toInt( L, 2 ), fill, truncate );
	return 1;
}

static int length( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	Util::push( L, obj->length() );
	return 1;
}

static int localeAwareCompare( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	Util::push( L, obj->localeAwareCompare( QtValueBase::toString( L, 2 ) ) );
	return 1;
}

static int mid( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	int n = -1;
	if( Util::top(L) > 2 )
		n = Util::toInt( L, 3 );
	*QtValue<QString>::create(L) = obj->mid( Util::toInt( L, 2 ), n );
	return 1;
}

static int normalized( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	QString::NormalizationForm mode;
	const int i2 = Util::toInt(L, 2);
	switch( i2 )
	{
	case QString::NormalizationForm_D:
	case QString::NormalizationForm_C:
	case QString::NormalizationForm_KD:
	case QString::NormalizationForm_KC:
		mode = (QString::NormalizationForm)i2;
		break;
	default:
		luaL_argerror( L, 2, "expecting integer value in QString::NormalizationForm" );
	}

	if( Util::top(L) > 2 )
	{
		QChar::UnicodeVersion version;
		const int i3 = Util::toInt(L, 2);
		switch( i3 )
		{
		case QChar::Unicode_Unassigned:
		case QChar::Unicode_1_1:
		case QChar::Unicode_2_0:
		case QChar::Unicode_2_1_2:
		case QChar::Unicode_3_0:
		case QChar::Unicode_3_1:
		case QChar::Unicode_3_2:
		case QChar::Unicode_4_0:
		case QChar::Unicode_4_1:
		case QChar::Unicode_5_0:
			version = (QChar::UnicodeVersion)i3;
			break;
		default:
			luaL_argerror( L, 2, "expecting integer value in QChar::UnicodeVersion" );
		}
		*QtValue<QString>::create(L) = obj->normalized( mode, version );
	}else
		*QtValue<QString>::create(L) = obj->normalized( mode );
	return 1;
}

static int prepend( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	if( QtValueBase::isString( L, 2 ) )
		obj->prepend( QtValueBase::toString( L, 2 ) );
	else if( QChar* ch = QtValue<QChar>::cast( L, 2 ) )
		obj->prepend( *ch );
	else
		Util::error( L, "invalid argument" );
	lua_pushvalue( L, 1 );
	return 1;
}

static int remove( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	if( Util::isNum( L, 2 ) )
		obj->remove( Util::toInt( L, 2 ), Util::toInt( L, 3 ) );
	else if( QtValueBase::isString( L, 2 ) )
		obj->remove( QtValueBase::toString( L, 2 ), toCs( L, 3) );
	else if( QChar* ch = QtValue<QChar>::cast( L, 2 ) )
		obj->remove( *ch, toCs( L, 3) );
	else
		Util::error( L, "invalid argument" );
	// TODO: QRegExp
	lua_pushvalue( L, 1 );
	return 1;
}

static int replace( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	if( Util::isNum( L, 2 ) )
	{
		if( QtValueBase::isString( L, 4 ) )
			obj->replace( Util::toInt( L, 2 ), Util::toInt( L, 3 ), QtValueBase::isString( L, 4 ) );
		else
			obj->replace( Util::toInt( L, 2 ), Util::toInt( L, 3 ), *QtValue<QChar>::check( L, 4 ) );
	}else if( QtValueBase::isString( L, 2 ) )
		obj->replace( QtValueBase::toString( L, 2 ), QtValueBase::toString( L, 3 ), toCs( L, 4) );
	else if( QChar* ch = QtValue<QChar>::cast( L, 2 ) )
	{
		if( QtValueBase::isString( L, 3 ) )
			obj->replace( *ch, QtValueBase::toString( L, 3 ), toCs( L, 4) );
		else
			obj->replace( *ch, *QtValue<QChar>::check( L, 3 ), toCs( L, 4) );
	}else
		Util::error( L, "invalid argument" );
	// TODO: QRegExp
	lua_pushvalue( L, 1 );
	return 1;
}

static int reserve( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	obj->reserve( Util::toInt( L, 2 ) );
	return 0;
}

static int resize( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	obj->resize( Util::toInt( L, 2 ) );
	return 0;
}

static int right( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	*QtValue<QString>::create(L) = obj->right( Util::toInt( L, 2 ) );
	return 1;
}

static int rightJustified( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	QChar fill = QLatin1Char( ' ' );
	if( Util::top( L ) > 2 )
		fill = *QtValue<QChar>::check( L, 3 );
	bool truncate = false;
	if( Util::top( L ) > 3 )
		truncate = Util::toBool( L, 4 );
	*QtValue<QString>::create(L) = obj->rightJustified( Util::toInt( L, 2 ), fill, truncate );
	return 1;
}

static int section( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	int end = -1;
	if( Util::top(L) > 3 )
		end = Util::toInt( L, 4 );
	QString::SectionFlags flags = QString::SectionDefault;
	if( Util::top(L) > 4 )
		flags &= Util::toInt( L, 5 );
	QString res;
	if( QtValueBase::isString( L, 2 ) )
		res = obj->section( QtValueBase::toString( L, 2 ), Util::toInt(L, 3), end, flags );
	else if( QChar* ch = QtValue<QChar>::cast( L, 2 ) )
		res = obj->section( *ch, Util::toInt(L, 3), end, flags );
	else
		Util::error( L, "invalid argument" );
	// TODO: QRegExp
	*QtValue<QString>::create(L) = res;
	return 1;
}

static int setNum( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	if( Util::isStr( L, 3 ) )
	{
		int precision = 6;
		if( Util::top(L) > 3 )
			precision = Util::toInt( L, 4 );
		obj->setNum( Util::toDbl( L, 2 ), Util::toChar( L, 3 ), precision );
	}else if( Util::isInt( L, 2 ) )
	{
		int base = 10;
		if( Util::top(L) > 2 )
			base = Util::toInt( L, 3 );
		obj->setNum( Util::toInt( L, 2 ), base );
	}else
		obj->setNum( Util::toDbl( L, 2 ) );
	lua_pushvalue( L, 1 );
	return 1;
}

static int simplified( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	*QtValue<QString>::create(L) = obj->simplified();
	return 1;
}

static int size( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	Util::push( L, obj->size() );
	return 1;
}

static int split( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	QString::SplitBehavior behavior = QString::KeepEmptyParts;
	if( Util::top(L) > 2 )
	{
		switch( Util::toInt(L,3) )
		{
		case QString::KeepEmptyParts:
		case QString::SkipEmptyParts:
			behavior = (QString::SplitBehavior)Util::toInt(L,3);
			break;
		default:
			luaL_argerror( L, 2, "expecting integer value in QString::SplitBehavior" );
		}
	}
	QStringList res;
	if( QtValueBase::isString( L, 2 ) )
		res = obj->split( QtValueBase::toString( L, 2 ), behavior, toCs( L, 4 ) );
	else if( QChar* ch = QtValue<QChar>::cast( L, 2 ) )
		res = obj->split( *ch, behavior, toCs( L, 4 ) );
	else
		Util::error( L, "invalid argument" );
	// TODO: QRegExp
	*QtValue<QStringList>::create(L) = res;
	return 1;
}

static int squeeze( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	obj->squeeze();
	return 0;
}

static int startsWith( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	if( QtValueBase::isString( L, 2 ) )
	{
		Util::push( L, obj->startsWith( QtValueBase::toString( L, 2 ), toCs( L, 3 ) ) );
	}else if( QChar* c = QtValue<QChar>::cast( L, 2 ) )
	{
		Util::push( L, obj->startsWith( *c, toCs( L, 3 ) ) );
	}else
		luaL_error( L, "arguments not supported" );
	return 1;
}

static int toCaseFolded( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	*QtValue<QString>::create(L) = obj->toCaseFolded();
	return 1;
}

static int toDouble( lua_State * L) // auch toFloat
{
	QString* obj = QtValue<QString>::check( L, 1 );
	bool ok = false;
	Util::push( L, obj->toDouble( &ok ) );
	Util::push( L, ok );
	return 2;
}

enum Type { Int, Long, LongLong, Short, UInt, ULong, ULongLong, UShort };
template<Type type>
static int toNumber( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	bool ok = false;
	int base = 10;
	if( Util::top(L) > 1 )
		base = Util::toInt(L,2);
	bool res = false;
	switch(type)
	{
	case Int:
		obj->toInt( &ok, base );
		break;
	case Long:
		obj->toLong( &ok, base );
		break;
	case LongLong:
		obj->toLongLong( &ok, base );
		break;
	case Short:
		obj->toShort( &ok, base );
		break;
	case UInt:
		obj->toUInt( &ok, base );
		break;
	case ULong:
		obj->toULong( &ok, base );
		break;
	case ULongLong:
		obj->toULongLong( &ok, base );
		break;
	case UShort:
		obj->toUShort( &ok, base );
		break;
	}
	Util::push( L, res );
	Util::push( L, ok );
	return 2;
}

static int toLocal8Bit( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	*QtValue<QString>::create(L) = obj->toLocal8Bit();
	return 1;
}

static int toLower( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	*QtValue<QString>::create(L) = obj->toLower();
	return 1;
}

static int toUpper( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	*QtValue<QString>::create(L) = obj->toUpper();
	return 1;
}

static int toUtf8( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	*QtValue<QString>::create(L) = obj->toUtf8();
	return 1;
}

static int trimmed( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	*QtValue<QString>::create(L) = obj->trimmed();
	return 1;
}

static int truncate( lua_State * L)
{
	QString* obj = QtValue<QString>::check( L, 1 );
	obj->truncate( Util::toInt( L, 2 ) );
	return 0;
}

static int fromLatin1( lua_State * L)
{
	*QtValue<QString>::create(L) = QString::fromLatin1( Util::toStr( L, 1 ) );
	return 1;
}

static int fromLocal8Bit( lua_State * L)
{
	*QtValue<QString>::create(L) = QString::fromLocal8Bit( Util::toStr( L, 1 ) );
	return 1;
}

static int number( lua_State * L)
{
	QString* obj = QtValue<QString>::create(L);
	if( Util::isStr( L, 3 ) )
	{
		int precision = 6;
		if( Util::top(L) > 2 )
			precision = Util::toInt( L, 3 );
		obj->setNum( Util::toDbl( L, 1 ), Util::toChar( L, 2 ), precision );
	}else if( Util::isInt( L, 1 ) )
	{
		int base = 10;
		if( Util::top(L) > 1 )
			base = Util::toInt( L, 2 );
		obj->setNum( Util::toInt( L, 1 ), base );
	}else
		obj->setNum( Util::toDbl( L, 1 ) );
	return 1;
}

// TODO: operators

static const luaL_reg _lib[] = 
{
	{ "number", number },
	{ "fromLocal8Bit", fromLocal8Bit },
	{ "fromLatin1", fromLatin1 },
	{ "fromAscii", fromLatin1 },
	{ "toAscii", toLatin1 },
	{ "toLatin1", toLatin1 },
	{ "append", append },
	{ "arg", arg },
	{ "arg", at },
	{ "capacity", capacity },
	{ "chop", chop },
	{ "clear", clear },
	{ "compare", compare },
	{ "contains", contains },
	{ "count", count },
	{ "count", endsWith },
	{ "fill", fill },
	{ "indexOf", indexOf },
	{ "insert", insert },
	{ "isEmpty", isEmpty },
	{ "isNull", isNull },
	{ "lastIndexOf", lastIndexOf },
	{ "left", left },
	{ "leftJustified", leftJustified },
	{ "length", length },
	{ "localeAwareCompare", localeAwareCompare },
	{ "mid", mid },
	{ "normalized", normalized },
	{ "prepend", prepend },
	{ "remove", remove },
	{ "replace", replace },
	{ "reserve", reserve },
	{ "resize", resize },
	{ "right", right },
	{ "rightJustified", rightJustified },
	{ "section", section },
	{ "setNum", setNum },
	{ "simplified", simplified },
	{ "size", size },
	{ "split", split },
	{ "squeeze", squeeze },
	{ "startsWith", startsWith },
	{ "toCaseFolded", toCaseFolded },
	{ "toDouble", toDouble },
	{ "toFloat", toDouble },
	{ "toLocal8Bit", toLocal8Bit },
	{ "toLower", toLower },
	{ "toUpper", toUpper },
	{ "toUtf8", toUtf8 },
	{ "trimmed", trimmed },
	{ "truncate", truncate },
	{ "toInt", toNumber<Int>  },
	{ "toLong", toNumber<Long>  },
	{ "toLongLong", toNumber<LongLong>  },
	{ "toShort", toNumber<Short>  },
	{ "toUInt", toNumber<UInt>  },
	{ "toULong", toNumber<ULong>  },
	{ "toULongLong", toNumber<ULongLong>  },
	{ "toUShort", toNumber<UShort>  },
	{ "init", init },
	{ 0, 0 }
};

void String::install(lua_State * L){
    QtValue<QString>::install( L, "QString", _lib );
    QtValue<QString>::addMetaMethod( L, "__tostring", toLatin1 );
}
