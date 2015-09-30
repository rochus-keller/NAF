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
#include "QtlTableWidgetSelectionRange.h"
#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;

int TableWidgetSelectionRange::init( lua_State * L )
{
	QTableWidgetSelectionRange* lhs = ValueInstaller2<QTableWidgetSelectionRange>::check( L, 1 );
	if (Util::isNil( L, 2 ))
	{
		*lhs = QTableWidgetSelectionRange();
	}
	else if (Util::isNum( L, 2 ))
	{
		int top = Util::toInt( L, 2 );
		int left = Util::toInt( L, 3 );
		int bottom = Util::toInt( L, 4 );
		int right = Util::toInt( L, 5 );
		*lhs = QTableWidgetSelectionRange( top, left, bottom, right );
	}
	else 
	{
		lhs->~QTableWidgetSelectionRange();
	}
	return 0;
}
int TableWidgetSelectionRange::bottomRow ( lua_State * L ) //  const
{
	QTableWidgetSelectionRange* lhs = ValueInstaller2<QTableWidgetSelectionRange>::check( L, 1 );
	Util::push( L, lhs->bottomRow() );
	return 1;
}
int TableWidgetSelectionRange::columnCount ( lua_State * L ) //  const
{
	QTableWidgetSelectionRange* lhs = ValueInstaller2<QTableWidgetSelectionRange>::check( L, 1 );
	Util::push( L, (lhs->leftColumn() - lhs->rightColumn() + 1 ) );
	return 1;
}
int TableWidgetSelectionRange::leftColumn ( lua_State * L ) //  const
{
	QTableWidgetSelectionRange* lhs = ValueInstaller2<QTableWidgetSelectionRange>::check( L, 1 );
	Util::push( L, lhs->leftColumn() );
	return 1;
}
int TableWidgetSelectionRange::rightColumn  ( lua_State * L ) //  const
{
	QTableWidgetSelectionRange* lhs = ValueInstaller2<QTableWidgetSelectionRange>::check( L, 1 );
	Util::push( L, lhs->rightColumn() );
	return 1;
}
int TableWidgetSelectionRange::rowCount ( lua_State * L ) //  const
{
	QTableWidgetSelectionRange* lhs = ValueInstaller2<QTableWidgetSelectionRange>::check( L, 1 );
	Util::push( L, (lhs->bottomRow() - lhs->topRow() + 1 ) );
	return 1;
}
int TableWidgetSelectionRange::topRow ( lua_State * L ) //  const
{
	QTableWidgetSelectionRange* lhs = ValueInstaller2<QTableWidgetSelectionRange>::check( L, 1 );
	Util::push( L, lhs->topRow() );
	return 1;
}
static const luaL_reg _TableWidgetSelectionRange[] = 
{
	{ Util::s_init, TableWidgetSelectionRange::init },
	{ "bottomRow", TableWidgetSelectionRange::bottomRow },
	{ "columnCount", TableWidgetSelectionRange::columnCount },
	{ "leftColumn", TableWidgetSelectionRange::leftColumn },
	{ "rightColumn", TableWidgetSelectionRange::rightColumn },
	{ "rowCount", TableWidgetSelectionRange::rowCount },
	{ "topRow", TableWidgetSelectionRange::topRow },
	{ 0, 0 }
};
void TableWidgetSelectionRange::install(lua_State * L){
	//ObjectInstaller<QTableWidgetSelectionRange>::install( L, _TableWidgetSelectionRange, ObjectInstaller<QObject>::className());
	ValueInstaller2<QTableWidgetSelectionRange>::install( L, _TableWidgetSelectionRange, "QTableWidgetSelectionRange" );
}
