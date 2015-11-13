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

#include "LuaStar.h"
#include "StarParser.h"
#include <Script2/QtValue.h>
#include <QFile>
#include <QBuffer>
#include <QtDebug>
using namespace Star;
using namespace Lua;

static int openDocument( lua_State* L )
{
	QFile in( QtValueBase::toString( L, 1, true ) );
	QByteArray coding = "UTF-8";
	if( lua_gettop(L) > 1 )
		coding = QtValueBase::toString( L, 2, true ).toAscii();
	bool newSyntax = false;
	if( lua_gettop(L) > 2 )
		newSyntax = lua_toboolean( L, 3 );
	bool errorException = true;
	if( lua_gettop(L) > 3 )
		errorException = lua_toboolean( L, 4 );
	if( !in.open(QIODevice::ReadOnly) && errorException )
		luaL_error( L, "cannot open file for reading '%s'", in.fileName().toLatin1().data() );
	StarParser* p = ValueBinding<StarParser>::create(L);
	p->setNewSyntax(newSyntax);
	if( !p->parse( &in, coding ) && errorException )
		luaL_error( L, "error parsing STAR file: %s", p->getError().toLatin1().data() );
	return 1;
}

static int parseDocument( lua_State* L )
{
	QByteArray star = QtValueBase::toString( L, 1, true ).toUtf8();
	bool newSyntax = false;
	if( lua_gettop(L) > 1 )
		newSyntax = lua_toboolean( L, 2 );
	bool errorException = true;
	if( lua_gettop(L) > 2 )
		errorException = lua_toboolean( L, 3 );
	QBuffer in( &star );
	in.open(QIODevice::ReadOnly);
	StarParser* p = ValueBinding<StarParser>::create(L);
	p->setNewSyntax(newSyntax);
	if( !p->parse( &in, "UTF-8" ) && errorException )
		luaL_error( L, "error parsing STAR file: %s", p->getError().toLatin1().data() );
	return 1;
}

static const luaL_reg starlib[] =
{
	{ "openDocument", openDocument },
	{ "parseDocument", parseDocument },
	{ 0, 0 }
};

static void pushValue( lua_State* L, const QVariant& v )
{
	if( v.canConvert<Star::StarParser::Loop::Table>() )
		lua_pushnil( L );
	else if( v.canConvert<Star::StarParser::Reference>() )
	{
		*ValueBinding<Star::StarParser::Reference>::create(L) = v.value<Star::StarParser::Reference>();
	}else if( v.type() == QVariant::Map )
	{
		QVariantMap map = v.value<QVariantMap>();
		lua_createtable( L, 0, map.size() );
		const int top = lua_gettop(L);
		QVariantMap::const_iterator i;
		for( i = map.begin(); i != map.end(); ++i )
		{
			*QtValue<QString>::create( L ) = i.key();
			pushValue( L, i.value() );
			lua_rawset( L, top );
		}
	}else if( v.type() == QVariant::List )
	{
		QVariantList list = v.value<QVariantList>();
		lua_createtable( L, list.size(), 0 );
		const int top = lua_gettop(L);
		for( int i = 0; i < list.size(); i++ )
		{
			pushValue( L, list[i] );
			lua_rawseti( L, top, i + 1 );
		}
	}else
		QtValueBase::pushVariant( L, v ); // eigentlich kommt hier nur noch QString oder string vor
}

static int getSource( lua_State* L )
{
	Star::StarParser::Reference* r = ValueBinding<Star::StarParser::Reference>::check( L, 1 );
	pushValue( L, r->d_source );
	return 1;
}

static int getBlock( lua_State* L )
{
	Star::StarParser::Reference* r = ValueBinding<Star::StarParser::Reference>::check( L, 1 );
	pushValue( L, r->d_block );
	return 1;
}

static int getFrame( lua_State* L )
{
	Star::StarParser::Reference* r = ValueBinding<Star::StarParser::Reference>::check( L, 1 );
	pushValue( L, r->d_frame );
	return 1;
}

static int getItem( lua_State* L )
{
	Star::StarParser::Reference* r = ValueBinding<Star::StarParser::Reference>::check( L, 1 );
	pushValue( L, r->d_item );
	return 1;
}

static int getKey( lua_State* L )
{
	Star::StarParser::Reference* r = ValueBinding<Star::StarParser::Reference>::check( L, 1 );
	pushValue( L, r->d_key );
	return 1;
}

static const luaL_reg starref[] =
{
	{ "getSource", getSource },
	{ "getBlock", getBlock },
	{ "getFrame", getFrame },
	{ "getItem", getItem },
	{ "getKey", getKey },
	{ 0, 0 }
};

static int getRowCount( lua_State* L )
{
	StarParser::Loop::Table* t = ValueBinding<StarParser::Loop::Table>::check( L, 1 );
	lua_pushinteger( L, t->count() );
	return 1;
}

static int getRow( lua_State* L )
{
	StarParser::Loop::Table* t = ValueBinding<StarParser::Loop::Table>::check( L, 1 );
	const int row = luaL_checkinteger( L, 2 ) - 1;
	if( row < 0 || row >= t->size() )
		luaL_argerror( L, 2, "row number out of range");
	const QVariantList& vl = t->at(row);
	lua_createtable( L, vl.size(), 0 );
	const int top = lua_gettop(L);
	for( int i = 0; i < vl.size(); i++ )
	{
		// Falls eine Subtable in der Row vorhanden ist, findet man sie als QVariant in der letzten Spalte
		if( vl[i].canConvert<Star::StarParser::Loop::Table>() )
			break;
		pushValue( L, vl[i] );
		lua_rawseti( L, top, i + 1 );
	}
	return 1;
}

static int getCell( lua_State* L )
{
	StarParser::Loop::Table* t = ValueBinding<StarParser::Loop::Table>::check( L, 1 );
	const int row = luaL_checkinteger( L, 2 ) - 1;
	if( row < 0 || row >= t->size() )
		luaL_argerror( L, 2, "row number out of range");
	const QVariantList& vl = t->at(row);
	const int col = luaL_checkinteger( L, 3 ) - 1;
	if( col < 0 || col >= vl.size() || vl[col].canConvert<Star::StarParser::Loop::Table>() )
		lua_pushnil(L);
	else
		pushValue( L, vl[col] );
	return 1;
}

static int getSubTable( lua_State* L )
{
	StarParser::Loop::Table* t = ValueBinding<StarParser::Loop::Table>::check( L, 1 );
	const int row = luaL_checkinteger( L, 2 ) - 1;
	if( row < 0 || row >= t->size() )
		luaL_argerror( L, 2, "row number out of range");
	const QVariantList& vl = t->at(row);
	if( vl.isEmpty() || !vl.last().canConvert<Star::StarParser::Loop::Table>() )
		lua_pushnil( L );
	else
		*ValueBinding<StarParser::Loop::Table>::create(L) = vl.last().value<Star::StarParser::Loop::Table>();
	return 1;
}

static const luaL_reg starlooptable[] =
{
	{ "getRowCount", getRowCount },
	{ "getRow", getRow },
	{ "getValue", getCell },
	{ "getTable", getSubTable },
	// obsolet
	{ "getCell", getCell },
	{ "getSubTable", getSubTable },
	{ 0, 0 }
};

static int getHeader( lua_State* L )
{
	StarParser::Loop* l = ValueBinding<StarParser::Loop>::check( L, 1 );
	int row = 0;
	if( lua_gettop(L) > 1 )
		row = luaL_checkinteger( L, 2 ) - 1;
	if( row >= 0 && row < l->d_headers.size() )
	{
		const StarParser::Loop::Header& h = l->d_headers[row];
		lua_createtable( L, h.size(), 0 );
		const int top = lua_gettop(L);
		for( int i = 0; i < h.size(); i++ )
		{
			*QtValue<QString>::create(L) = h[i];
			lua_rawseti( L, top, i + 1 );
		}
	}else
		luaL_error( L, "no header on level %d", row + 1 );
	return 1;
}

static int getHeaderCount( lua_State* L )
{
	StarParser::Loop* l = ValueBinding<StarParser::Loop>::check( L, 1 );
	lua_pushinteger( L, l->d_headers.size() );
	return 1;
}

static int getTable( lua_State* L )
{
	StarParser::Loop* l = ValueBinding<StarParser::Loop>::check( L, 1 );
	*ValueBinding<StarParser::Loop::Table>::create( L ) = l->d_table;
	return 1;
}

static const luaL_reg starloop[] =
{
	{ "getTable", getTable },
	{ "getHeader", getHeader },
	{ "getHeaderCount", getHeaderCount },
	{ 0, 0 }
};

static int getGlobalBlock( lua_State* L )
{
	StarParser* p = ValueBinding<StarParser>::check( L, 1 );
	StarParser::Block* b = ValueBinding<StarParser::Block>::create(L);
	*b = p->getGlobal();
	return 1;
}

static int getError( lua_State* L )
{
	StarParser* p = ValueBinding<StarParser>::check( L, 1 );
	lua_pushstring( L, p->getError().toLatin1() );
	return 1;
}

static int isValid( lua_State* L )
{
	StarParser* p = ValueBinding<StarParser>::check( L, 1 );
	lua_pushboolean( L, p->getError().isEmpty() );
	return 1;
}

static int isNewSyntax( lua_State* L )
{
	StarParser* p = ValueBinding<StarParser>::check( L, 1 );
	lua_pushboolean( L, p->isNewSyntax() );
	return 1;
}

static int getDataBlock( lua_State* L )
{
	StarParser* p = ValueBinding<StarParser>::check( L, 1 );
	const QString name = QtValueBase::toString( L, 2, true );
	StarParser::Blocks::const_iterator i = p->getBlocks().find( name );
	if( i == p->getBlocks().end() )
		lua_pushnil(L);
	else
		*ValueBinding<StarParser::Block>::create(L) = i.value();
	return 1;
}

static int getDataBlocks(lua_State* L )
{
	StarParser* p = ValueBinding<StarParser>::check( L, 1 );
	lua_createtable(L, 0, p->getBlocks().size() );
	const int table = lua_gettop(L);
	StarParser::Blocks::const_iterator i;
	for( i = p->getBlocks().begin(); i != p->getBlocks().end(); ++i )
	{
		*QtValue<QString>::create(L) = i.key();
		*ValueBinding<StarParser::Block>::create(L) = i.value();
		lua_rawset( L, table );
	}
	return 1;
}

static const luaL_reg starfile[] =
{
	{ "getGlobalBlock", getGlobalBlock },
	{ "getError", getError },
	{ "isNewSyntax", isNewSyntax },
	{ "getDataBlock", getDataBlock },
	{ "getDataBlocks", getDataBlocks },
	{ "isValid", isValid },
	{ 0, 0 }
};

static int getSaveFrames(lua_State* L )
{
	StarParser::Block* b = ValueBinding<StarParser::Block>::check( L, 1 );
	lua_createtable(L, 0, b->d_blocks.size() );
	const int table = lua_gettop(L);
	StarParser::Blocks::const_iterator i;
	for( i = b->d_blocks.begin(); i != b->d_blocks.end(); ++i )
	{
		*QtValue<QString>::create(L) = i.key();
		*ValueBinding<StarParser::Block>::create(L) = i.value();
		lua_rawset( L, table );
	}
	return 1;
}

static int getSaveFrame( lua_State* L )
{
	StarParser::Block* b = ValueBinding<StarParser::Block>::check( L, 1 );
	const QString name = QtValueBase::toString( L, 2, true );
	StarParser::Blocks::const_iterator i = b->d_blocks.find( name );
	if( i == b->d_blocks.end() )
		lua_pushnil(L);
	else
		*ValueBinding<StarParser::Block>::create(L) = i.value();
	return 1;
}

static int getValues(lua_State* L )
{
	StarParser::Block* b = ValueBinding<StarParser::Block>::check( L, 1 );
	lua_createtable(L, 0, b->d_items.size() );
	const int table = lua_gettop(L);
	QMap<QString,QVariant>::const_iterator i;
	for( i = b->d_items.begin(); i != b->d_items.end(); ++i )
	{
		*QtValue<QString>::create(L) = i.key();
		pushValue( L, i.value() );
		lua_rawset( L, table );
	}
	return 1;
}

static int getValue( lua_State* L )
{
	StarParser::Block* b = ValueBinding<StarParser::Block>::check( L, 1 );
	const QString name = QtValueBase::toString( L, 2, true );
	QMap<QString,QVariant>::const_iterator i = b->d_items.find( name );
	if( i == b->d_items.end() )
		lua_pushnil(L);
	else
		pushValue( L, i.value() );
	return 1;
}

static int getLoopCount( lua_State* L )
{
	StarParser::Block* b = ValueBinding<StarParser::Block>::check( L, 1 );
	lua_pushinteger( L, b->d_loops.size() );
	return 1;
}

static int getLoop( lua_State* L )
{
	StarParser::Block* b = ValueBinding<StarParser::Block>::check( L, 1 );
	int row = 0;
	if( lua_gettop( L ) > 1 )
		row = luaL_checkinteger( L, 2 ) - 1;
	if( row < 0 || row >= b->d_loops.size() )
		luaL_argerror( L, 2, "index out of range" );
	*ValueBinding<StarParser::Loop>::create(L) = b->d_loops[row];
	return 1;
}

static int isEmpty( lua_State* L )
{
	StarParser::Block* b = ValueBinding<StarParser::Block>::check( L, 1 );
	lua_pushboolean( L, b->d_blocks.isEmpty() && b->d_items.isEmpty() && b->d_loops.isEmpty() );
	return 1;
}

static const luaL_reg starblock[] =
{
	{ "getLoopCount", getLoopCount },
	{ "getLoop", getLoop },
	{ "getValues", getValues },
	{ "getValue", getValue },
	{ "getSaveFrames", getSaveFrames },
	{ "getSaveFrame", getSaveFrame },
	{ "isEmpty", isEmpty },
	{ 0, 0 }
};

void LuaStar::install(lua_State *L)
{
	StackTester test(L,0);
	luaL_register( L, "star", starlib );
	lua_pop(L,1); // lib
	ValueBinding<StarParser>::install( L, "StarFile", starfile, false );
	ValueBinding<StarParser::Block>::install( L, "StarBlock", starblock, false );
	ValueBinding<StarParser::Loop>::install( L, "StarLoop", starloop, false );
	ValueBinding<StarParser::Loop::Table>::install( L, "StarTable", starlooptable, false );
	ValueBinding<Star::StarParser::Reference>::install( L, "StarRef", starref, false );
}
