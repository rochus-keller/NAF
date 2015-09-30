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

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextBlock>
#include <QTextBlockFormat>
#include <QTextCharFormat>
#include <QTextBlockUserData>
#include <QTextLayout>
#include <QString>
#include <QTextList>
#include <QTextDocument>
#include "QtlTextBlock.h"
#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;

GCC_IGNORE(-Wunused-variable);
GCC_IGNORE(-Wunused-but-set-variable);

int TextBlock::init( lua_State * L )
{
	QTextBlock* lhs = ValueInstaller2<QTextBlock>::check( L, 1 );
	*lhs = QTextBlock();
	return 0;
}

int TextBlock::begin ( lua_State * L ) //const iterator
{
	QTextBlock* lhs = ValueInstaller2<QTextBlock>::check( L, 1 );
	//ObjectPeer::pushPeer( L, lhs->begin(), true );
	return 1;
}
int TextBlock::blockFormat ( lua_State * L ) //const QTextBlockFormat
{
	QTextBlock* lhs = ValueInstaller2<QTextBlock>::check( L, 1 );
	QTextBlockFormat* res = ValueInstaller2<QTextBlockFormat>::check( L );
	*res = lhs->blockFormat();
	return 0;
}
int TextBlock::blockFormatIndex ( lua_State * L ) //const
{
	QTextBlock* lhs = ValueInstaller2<QTextBlock>::check( L, 1 );
	Util::push( L, lhs->blockFormatIndex() );
	return 1;
}
int TextBlock::charFormat ( lua_State * L ) // const QTextCharFormat
{
	QTextBlock* lhs = ValueInstaller2<QTextBlock>::check( L, 1 );
	QTextCharFormat* res = ValueInstaller2<QTextCharFormat>::create( L );
	*res = lhs->charFormat();
	return 1;
}
int TextBlock::charFormatIndex ( lua_State * L ) //const
{
	QTextBlock* lhs = ValueInstaller2<QTextBlock>::check( L, 1 );
	Util::push( L, lhs->charFormatIndex() );
	return 1;
}
int TextBlock::contains ( lua_State * L ) //bool ( int position )
{
	QTextBlock* lhs = ValueInstaller2<QTextBlock>::check( L, 1 );
	if ( Util::isNum( L, 2 ) )
	{
		Util::push( L, lhs->contains( Util::toInt( L, 2 ) ) );
	}
	else
	{
		Util::push( L, false );
	}
	return 1;
}
int TextBlock::document ( lua_State * L ) //const QTextDocument 
{
	QTextBlock* lhs = ValueInstaller2<QTextBlock>::check( L, 1 );
	const QTextDocument* res = ValueInstaller2<QTextDocument>::create( L );
	res = lhs->document();
	return 1;
}
int TextBlock::end ( lua_State * L ) //const iterator
{
	QTextBlock* lhs = ValueInstaller2<QTextBlock>::check( L, 1 );
//ObjectPeer::pushPeer( L, lhs->end(), true );
	return 1;
}
int TextBlock::isValid ( lua_State * L ) //bool 
{
	QTextBlock* lhs = ValueInstaller2<QTextBlock>::check( L, 1 );
	Util::push( L, lhs->isValid() );
	return 1;
}

int TextBlock::layout ( lua_State * L ) //const QTextLayout
{
	QTextBlock* lhs = ValueInstaller2<QTextBlock>::check( L, 1 );
	QTextLayout* res = ValueInstaller2<QTextLayout>::create( L );
	res = lhs->layout();
	return 1;
}
int TextBlock::length ( lua_State * L ) //int
{
	QTextBlock* lhs = ValueInstaller2<QTextBlock>::check( L, 1 );
	Util::push( L, lhs->length() );
	return 1;
}
int TextBlock::next ( lua_State * L ) //QTextBlock
{
	QTextBlock* lhs = ValueInstaller2<QTextBlock>::check( L, 1 );
	QTextBlock* res = ValueInstaller2<QTextBlock>::create( L );
	*res = lhs->next();
	return 1;
}
int TextBlock::position ( lua_State * L ) //int
{
	QTextBlock* lhs = ValueInstaller2<QTextBlock>::check( L, 1 );
	Util::push( L, lhs->position() );
	return 1;
}
int TextBlock::previous ( lua_State * L ) //QTextBlock
{
	QTextBlock* lhs = ValueInstaller2<QTextBlock>::check( L, 1 );
	QTextBlock* res = ValueInstaller2<QTextBlock>::create( L );
	*res = lhs->previous();
	return 1;
}
int TextBlock::setUserData ( lua_State * L ) //( QTextBlockUserData * data ) void
{
	QTextBlock* lhs = ValueInstaller2<QTextBlock>::check( L, 1 );
	QTextBlockUserData* data = ValueInstaller2<QTextBlockUserData>::check( L, 1 );
	lhs->setUserData( data );
	return 0;
}
int TextBlock::setUserState ( lua_State * L ) // ( int state ) void
{
	QTextBlock* lhs = ValueInstaller2<QTextBlock>::check( L, 1 );
	int state = 0;
	if ( Util::isNum( L, 2 ) )
	{
		state = Util::toInt( L, 2 );
	}
	lhs->setUserState(state);
	return 0;
}

int TextBlock::text ( lua_State * L ) //const QString
{
	QTextBlock* lhs = ValueInstaller2<QTextBlock>::check( L, 1 );
	//QString* res = ValueInstaller2<QString>::create( L );
	//*res = lhs->text();
	Util::push( L, lhs->text() );
	return 1;
}
int TextBlock::textList ( lua_State * L )// const QTextList
{
	QTextBlock* lhs = ValueInstaller2<QTextBlock>::check( L, 1 );
	ObjectPeer::pushPeer( L, lhs->textList(), true );
	return 1;
}
int TextBlock::userData ( lua_State * L ) // const QTextBlockUserData 
{
	QTextBlock* lhs = ValueInstaller2<QTextBlock>::check( L, 1 );
	QTextBlockUserData* res = ValueInstaller2<QTextBlockUserData>::create( L );
	res = lhs->userData();
	return 1;
}

int TextBlock::userState ( lua_State * L ) // const
{
	QTextBlock* lhs = ValueInstaller2<QTextBlock>::check( L, 1 );
	Util::push( L, lhs->userState() );
	return 1;
}
int TextBlock::__ne( lua_State * L ) // ( const QTextBlock & other ) bool
{
	QTextBlock* lhs = ValueInstaller2<QTextBlock>::check( L, 1 );
	QTextBlock* rhs = ValueInstaller2<QTextBlock>::check( L, 2 );
	Util::push( L, *lhs != *rhs );
	return 1;	
}
int TextBlock::__lt( lua_State * L ) // ( const QTextBlock & other ) bool
{
	QTextBlock* lhs = ValueInstaller2<QTextBlock>::check( L, 1 );
	QTextBlock* rhs = ValueInstaller2<QTextBlock>::check( L, 2 );
	Util::push( L, *lhs < *rhs );
	return 1;	
}
int TextBlock::operatorEq( lua_State * L ) // ( const QTextBlock & other ) QTextBlock 
{
	QTextBlock* lhs = ValueInstaller2<QTextBlock>::check( L, 1 );
	QTextBlock* other = ValueInstaller2<QTextBlock>::check( L, 2 );
	QTextBlock* res = ValueInstaller2<QTextBlock>::create( L );
	*res = lhs->operator =( *other );
	return 1;	
}
int TextBlock::__eq( lua_State * L ) // ( const QTextBlock & other ) bool
{
	QTextBlock* lhs = ValueInstaller2<QTextBlock>::check( L, 1 );
	QTextBlock* rhs = ValueInstaller2<QTextBlock>::check( L, 2 );
	Util::push( L, *lhs == *rhs );
	return 1;	
}

static const luaL_reg _TextBlock[] = 
{
	{ Util::s_init, TextBlock::init },
	{ "begin", TextBlock::begin },// not completed
	{ "blockFormatIndex", TextBlock::blockFormatIndex },
	{ "blockFormat", TextBlock::blockFormat },
	{ "charFormat", TextBlock::charFormat },
	{ "charFormatIndex", TextBlock::charFormatIndex },
	{ "contains", TextBlock::contains },
	{ "document", TextBlock::document },
	{ "end", TextBlock::end },// not completed
	{ "isValid", TextBlock::isValid },
	{ "layout", TextBlock::layout },
	{ "length", TextBlock::length },
	{ "next", TextBlock::next },
	{ "position", TextBlock::position },
	{ "previous", TextBlock::previous },
	{ "setUserData", TextBlock::setUserData },
	{ "setUserState", TextBlock::setUserState },
	{ "text", TextBlock::text },
	{ "textList", TextBlock::textList },
	{ "userData", TextBlock::userData },
	{ "userState", TextBlock::userState },
	{ "__ne", TextBlock::__ne },
	{ "__lt", TextBlock::__lt },
	{ "operatorEq", TextBlock::operatorEq },
	{ "__eq", TextBlock::__eq },
	{ 0, 0 }
};
void TextBlock::install(lua_State * L){
	ValueInstaller2<QTextBlock>::install( L, _TextBlock );
}
