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

#include <QTextOption>
#include <QFlags>
#include <QPainter>
#include "QtlTextOption.h"
#include <Script/ValueInstaller.h>
#include "Variant.h"
#include "Enums.h"

#include <Script/ObjectInstaller.h>

using namespace Qtl;
using namespace Lua;

GCC_IGNORE(-Wunused-variable);
GCC_IGNORE(-Wunused-but-set-variable);

int TextOption::alignment( lua_State * L ) // const Qt::Alignment 
{
	QTextOption* lhs = ValueInstaller2<QTextOption>::check( L, 1 );
	Qt::Alignment alignment=(Qt::Alignment) lhs->alignment();
	Util::push( L, alignment);
	return 1;
}
int TextOption::flags ( lua_State * L ) //const Flags
{
	QTextOption* lhs = ValueInstaller2<QTextOption>::check( L, 1 );
	lhs->flags();
	return 0;
}
int TextOption::setAlignment ( lua_State * L ) //( Qt::Alignment alignment )void
{
	QTextOption* lhs = ValueInstaller2<QTextOption>::check( L, 1 );
	Enums enums(L);
	Qt::Alignment alignment=(Qt::Alignment)enums.Alignment( 2 );
	lhs->setAlignment( alignment );
	return 0;
}
int TextOption::setFlags ( lua_State * L ) // ( Flags flags )void
{
	QTextOption* lhs = ValueInstaller2<QTextOption>::check( L, 1 );
	//Enums enums( L );
	//QFlags::Flags ll = (QFlags::QFlags)enums.Flags( 2 );
	//lhs->setFlags( QFlag::QFlag( Util::toInt(L, 2 ) ) );
	return 0;
}
int TextOption::setTextDirection ( lua_State * L ) // ( Qt::LayoutDirection direction ) void
{
//Qt::LayoutDirection
	QTextOption* lhs = ValueInstaller2<QTextOption>::check( L, 1 );
	Enums enums(L);
	Qt::LayoutDirection direction =(Qt::LayoutDirection)enums.LayoutDirection( 2 );
	lhs->setTextDirection( direction );
	return 0;
}
int TextOption::setTabStop( lua_State * L ) // ( qreal tabStop )void
{
	QTextOption* lhs = ValueInstaller2<QTextOption>::check( L, 1 );
	qreal tabStop = Util::toDbl( L, 2 );
	lhs->setTabStop( tabStop );
	return 0;
}

int TextOption::setUseDesignMetrics( lua_State * L ) //  ( bool enable )void
{
	QTextOption* lhs = ValueInstaller2<QTextOption>::check( L, 1 );
	lhs->setUseDesignMetrics( Util::toBool( L, 2 ) );
	return 0;
}
int TextOption::tabStop ( lua_State * L ) //  const qreal
{
	QTextOption* lhs = ValueInstaller2<QTextOption>::check( L, 1 );
	Util::push( L, lhs->tabStop() );
	return 0;
}
int TextOption::useDesignMetrics ( lua_State * L ) //const bool
{
	QTextOption* lhs = ValueInstaller2<QTextOption>::check( L, 1 );
	Util::push( L, lhs->useDesignMetrics() );
	return 0;
}

int TextOption::setWrapMode ( lua_State * L ) //( WrapMode mode ) void
{
	QTextOption* lhs = ValueInstaller2<QTextOption>::check( L, 1 );
	Enums enums(L);
	QTextOption::WrapMode mode = ( QTextOption::WrapMode )enums.wrapMode( 2 );
	lhs->setWrapMode( mode );
	return 0;
}


int TextOption::textDirection ( lua_State * L ) //const Qt::LayoutDirection  
{
	QTextOption* lhs = ValueInstaller2<QTextOption>::check( L, 1 );
	Util::push( L, lhs->textDirection() );
	return 0;
}
int TextOption::wrapMode ( lua_State * L ) //const 
{
	QTextOption* lhs = ValueInstaller2<QTextOption>::check( L, 1 );
	Util::push( L, lhs->wrapMode() );
	return 0;
}
int TextOption::operatorEq ( lua_State * L ) //( const QTextOption & other )QTextOption
{
	QTextOption* lhs = ValueInstaller2<QTextOption>::check( L, 1 );
	QTextOption* other = ValueInstaller2<QTextOption>::check( L, 2 );
	QTextOption* res = ValueInstaller2<QTextOption>::create( L );
	res =  other ;
	return 1;
}
int TextOption::init(lua_State * L)
{
	QTextOption* lhs = ValueInstaller2<QTextOption>::check( L, 1 );
	if( Util::isNum(L, 2) )
	{
		Enums enums(L);
		Qt::Alignment f=(Qt::Alignment)enums.Alignment( 2 );
		*lhs = QTextOption( f );
	}else if( QTextOption* p = ValueInstaller2<QTextOption>::cast( L, 2 ) )
	{
		*lhs = QTextOption( *p );
	}
	else if ( Util::isNil( L, 2 ) )
	{
		*lhs = QTextOption();
	}
	else
	{
		lhs->~QTextOption();
	}
	return 0;
}
static const luaL_reg _TextOption[] = 
{
	{ "alignment", TextOption::alignment },
	{ "flags", TextOption::flags},	
	{ "setAlignment", TextOption::setAlignment},
	{ "setFlags", TextOption::setFlags},// check
//void setTabArray ( QList<qreal> tabStops )
	{ "setTabStop", TextOption::setTabStop},
	{ "setTextDirection", TextOption::setTextDirection},
	{ "setUseDesignMetrics", TextOption::setUseDesignMetrics},
	{ "setWrapMode", TextOption::setWrapMode},
//QList<qreal> tabArray () const
	{ "tabStop", TextOption::tabStop},
	{ "textDirection", TextOption::textDirection},
	{ "useDesignMetrics", TextOption::useDesignMetrics},
	{ "wrapMode", TextOption::wrapMode},
	{ "operatorEq", TextOption::operatorEq },
	{ Util::s_init, TextOption::init },
	{ 0, 0 }
};

void TextOption::install(lua_State * L){
	ValueInstaller2<QTextOption>::install( L, _TextOption, "QTextOption" );
	//ValueInstaller2<QPointF>::addMethodsToMeta( L, _PointMeta );
}
