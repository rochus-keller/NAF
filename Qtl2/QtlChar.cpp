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

#include <QChar>
#include "QtlChar.h"
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;
int Char::category(lua_State * L) // const : Category
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	Lua::Util::push( L, lhs->category() );
	return 1;
}
int Char::cell(lua_State * L) // const : uchar
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushnumber( L, lhs->cell() );
	return 1;
}
int Char::combiningClass(lua_State * L) // const : unsigned char
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	Lua::Util::push( L, lhs->combiningClass() );
	return 1;
}
int Char::init(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	if( Util::isNum( L, 2 ) )
	{
		*lhs = QChar( Util::toInt( L, 2 ) );
	}
	else if( Util::isStr( L, 2)  )
	{
		QByteArray str = Util::toStr( L, 2);
		if( str.length() != 1 )
			luaL_error( L, "QChar:init expecting string with length 1" );
		*lhs = QChar( str[0] );
	}
	return 0;
}

static int decomposition(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	*QtValue<QString>::create( L ) = lhs->decomposition();
	return 1;
}
static int decompositionTag(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushnumber( L, lhs->decompositionTag() );
	return 1;
}
static int digitValue(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushnumber( L, lhs-> digitValue() );
	return 1;
}
static int direction(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushnumber( L, lhs-> direction() );
	return 1;
}
static int hasMirrored(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushboolean( L, lhs-> hasMirrored() );
	return 1;
}
static int isDigit(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushboolean( L, lhs-> isDigit() );
	return 1;
}
static int isHighSurrogate(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushboolean( L, lhs-> isHighSurrogate() );
	return 1;
}
static int isLetter(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushboolean( L, lhs-> isLetter() );
	return 1;
}
static int isLetterOrNumber(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushboolean( L, lhs-> isLetterOrNumber() );
	return 1;
}
static int isLowSurrogate(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushboolean( L, lhs-> isLowSurrogate() );
	return 1;
}
static int isLower(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushboolean( L, lhs-> isLower() );
	return 1;
}
static int isMark(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushboolean( L, lhs-> isMark() );
	return 1;
}
static int isNull(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushboolean( L, lhs-> isNull() );
	return 1;
}
static int isNumber(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushboolean( L, lhs-> isNumber() );
	return 1;
}
static int isPrint(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushboolean( L, lhs-> isPrint() );
	return 1;
}
static int isPunct(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushboolean( L, lhs-> isPunct() );
	return 1;
}
static int isSpace(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushboolean( L, lhs-> isSpace() );
	return 1;
}
static int isSymbol(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushboolean( L, lhs-> isSymbol() );
	return 1;
}
static int isTitleCase(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushboolean( L, lhs-> isTitleCase() );
	return 1;
}
static int isUpper(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushboolean( L, lhs-> isUpper() );
	return 1;
}
static int joining(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushnumber( L, lhs-> joining() );
	return 1;
}
static int  mirroredChar(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	*QtValue<QChar>::create( L ) = lhs-> mirroredChar();
	return 1;
}
static int row(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushnumber( L, lhs-> row() );
	return 1;
}
static int toAscii (lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushstring( L, QByteArray( 1, lhs-> toAscii ()) );
	return 1;
}
static int   toCaseFolded(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	*QtValue<QChar>::create( L ) = lhs->  toCaseFolded();
	return 1;
}
static int  toLatin1 (lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushstring( L, QByteArray( 1, lhs->  toLatin1 ()) );
	return 1;
}
static int  toLower(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	*QtValue<QChar>::create( L ) = lhs-> toLower();
	return 1;
}
static int toTitleCase (lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	*QtValue<QChar>::create( L ) = lhs->  toTitleCase ();
	return 1;
}
static int toUpper  (lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	*QtValue<QChar>::create( L ) = lhs->  toUpper  ();
	return 1;
}
static int unicode(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushnumber( L, lhs-> unicode() );
	return 1;
}
static int  unicodeVersion(lua_State * L)
{
	QChar* lhs = QtValue<QChar>::check( L, 1 );
	lua_pushnumber( L, lhs->  unicodeVersion() );
	return 1;
}
static const luaL_reg _charlib[] = 
{
	{ "init", Char::init },
	{ "category", Char::category },
	{ "cell", Char::cell },
	{ "combiningClass", Char::combiningClass },
	{ "decomposition", decomposition },
	{ "decompositionTag", decompositionTag },
	{ "digitValue", digitValue },
	{ "direction", direction },
	{ "isDigit", isDigit },
	{ "isHighSurrogate", isHighSurrogate },
	{ "isLetter", isLetter },
	{ "isLetterOrNumber", isLetterOrNumber },
	{ "isLowSurrogate", isLowSurrogate },
	{ "isLower", isLower },
	{ "isMark", isMark },
	{ "isNull", isNull },
	{ "isNumber", isNumber },
	{ "isPrint", isPrint },
	{ "isPunct", isPunct },
	{ "isSpace", isSpace },
	{ "isSymbol", isSymbol },
	{ "isTitleCase", isTitleCase },
	{ "isUpper", isUpper },
	{ "hasMirrored", hasMirrored },
	{ "joining", joining },
	{ "mirroredChar", mirroredChar },
	{ "row", row },
	{ "toAscii ", toAscii  },
	{ " toCaseFolded",  toCaseFolded },
	{ "toLatin1", toLatin1 },
	{ "toLower", toLower },
	{ "toTitleCase", toTitleCase },
	{ "toUpper", toUpper },
	{ "unicode", unicode },
	{ "unicodeVersion",  unicodeVersion },
	{ 0, 0 }
};
void Char::install(lua_State * L){
    QtValue<QChar>::install( L, "QChar", _charlib );
}
