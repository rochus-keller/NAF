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
#include <QTextBlockFormat>
#include "QtlTextBlockFormat.h"
#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;
#include "Qtl/Enums.h"

    
int TextBlockFormat::init ( lua_State * L )//  QTextBlockFormat ()
{
	QTextBlockFormat* obj = ValueInstaller2<QTextBlockFormat>::check( L, 1 );
	*obj = QTextBlockFormat();
	return 0;
}
int TextBlockFormat::alignment ( lua_State * L )// const Qt::Alignment 
{
	QTextBlockFormat* obj = ValueInstaller2<QTextBlockFormat>::check( L, 1 );
	Util::push( L, obj->alignment() );
	return 1;
}
int TextBlockFormat::bottomMargin ( lua_State * L )// const qreal  
{
	QTextBlockFormat* obj = ValueInstaller2<QTextBlockFormat>::check( L, 1 );
	Util::push( L, obj->bottomMargin() );
	return 1;
}
int TextBlockFormat::indent ( lua_State * L )// int
{
	QTextBlockFormat* obj = ValueInstaller2<QTextBlockFormat>::check( L, 1 );
	Util::push( L, obj->indent() );
	return 1;
}
int TextBlockFormat::isValid ( lua_State * L )// bool
{
	QTextBlockFormat* obj = ValueInstaller2<QTextBlockFormat>::check( L, 1 );
	Util::push( L, obj->isValid() );
	return 1;
}
int TextBlockFormat::leftMargin ( lua_State * L )// const qreal  
{
	QTextBlockFormat* obj = ValueInstaller2<QTextBlockFormat>::check( L, 1 );
	Util::push( L, obj->leftMargin() );
	return 1;
}
int TextBlockFormat::nonBreakableLines ( lua_State * L ) //bool
{
	QTextBlockFormat* obj = ValueInstaller2<QTextBlockFormat>::check( L, 1 );
	Util::push( L, obj->nonBreakableLines() );
	return 1;
}
int TextBlockFormat::pageBreakPolicy ( lua_State * L )// const PageBreakFlags
{
	QTextBlockFormat* obj = ValueInstaller2<QTextBlockFormat>::check( L, 1 );
	Util::push( L, obj->pageBreakPolicy() );
	return 1;
}
int TextBlockFormat::rightMargin ( lua_State * L )// const qreal  
{
	QTextBlockFormat* obj = ValueInstaller2<QTextBlockFormat>::check( L, 1 );
	Util::push( L, obj->rightMargin() );
	return 1;
}
int TextBlockFormat::setAlignment ( lua_State * L ) //  void ( Qt::Alignment alignment )
{
	QTextBlockFormat* obj = ValueInstaller2<QTextBlockFormat>::check( L, 1 );
	Enums enums(L);
	Qt::Alignment f=(Qt::Alignment)enums.Alignment( 2 );
	obj->setAlignment( f );
	return 0;
}
int TextBlockFormat::setBottomMargin ( lua_State * L )// ( qreal margin ) void
{
	QTextBlockFormat* obj = ValueInstaller2<QTextBlockFormat>::check( L, 1 );
	qreal margin = Util::toDbl( L, 2 );
	obj->setBottomMargin( margin );
	return 0;
}
int TextBlockFormat::setIndent ( lua_State * L )// ( int indentation ) void
{
	QTextBlockFormat* obj = ValueInstaller2<QTextBlockFormat>::check( L, 1 );
	int indentation = 0;
	if ( Util::isNum( L, 2 ) )
	{
		indentation = Util::toInt( L, 2 );
	}
	obj->setIndent( indentation );
	return 0;
}
int TextBlockFormat::setLeftMargin ( lua_State * L )// ( qreal margin ) void
{
	QTextBlockFormat* obj = ValueInstaller2<QTextBlockFormat>::check( L, 1 );
	qreal margin = Util::toDbl( L, 2 );
	obj->setLeftMargin( margin );
	return 0;
}
int TextBlockFormat::setNonBreakableLines ( lua_State * L ) //( bool b ) void
{
	QTextBlockFormat* obj = ValueInstaller2<QTextBlockFormat>::check( L, 1 );
	if ( Util::toBool( L, 2 ) == true )
	{
		obj->setNonBreakableLines( Util::toBool( L, 2 ) );
	}
	return 0;
}
int TextBlockFormat::setPageBreakPolicy ( lua_State * L )// ( PageBreakFlags policy ) void
{
	QTextBlockFormat* obj = ValueInstaller2<QTextBlockFormat>::check( L, 1 );
	Enums enums(L);
	QTextFormat::PageBreakFlags policy = (QTextFormat::PageBreakFlags)enums.PageBreakFlags( 2 );
	obj->setPageBreakPolicy( policy );
	return 0;
}
int TextBlockFormat::setRightMargin ( lua_State * L )// ( qreal margin ) void
{
	QTextBlockFormat* obj = ValueInstaller2<QTextBlockFormat>::check( L, 1 );
	qreal margin = Util::toDbl( L, 2 );
	obj->setRightMargin( margin );
	return 0;
}
int TextBlockFormat::setTextIndent ( lua_State * L )// ( qreal indent ) void
{
	QTextBlockFormat* obj = ValueInstaller2<QTextBlockFormat>::check( L, 1 );
	qreal indent = Util::toDbl( L, 2 );
	obj->setTextIndent( indent );
	return 0;
}

int TextBlockFormat::setTopMargin ( lua_State * L )// ( qreal margin ) void
{
	QTextBlockFormat* obj = ValueInstaller2<QTextBlockFormat>::check( L, 1 );
	qreal margin = Util::toDbl( L, 2 );
	obj->setTopMargin( margin );
	return 0;
}
int TextBlockFormat::textIndent ( lua_State * L )// const qreal  
{
	QTextBlockFormat* obj = ValueInstaller2<QTextBlockFormat>::check( L, 1 );
	Util::push( L, obj->textIndent() );
	return 1;
}
int TextBlockFormat::topMargin ( lua_State * L )// const qreal  
{
	QTextBlockFormat* obj = ValueInstaller2<QTextBlockFormat>::check( L, 1 );
	Util::push( L, obj->topMargin() );
	return 1;
}

static const luaL_reg _TextBlockFormat[] = 
{
	  {"alignment" , TextBlockFormat::alignment },
      {"bottomMargin" , TextBlockFormat::bottomMargin },
      {"indent" , TextBlockFormat::indent },
      {"isValid" , TextBlockFormat::isValid },
      {"leftMargin" , TextBlockFormat::leftMargin },
      {"nonBreakableLines" , TextBlockFormat::nonBreakableLines },
      {"pageBreakPolicy" , TextBlockFormat::pageBreakPolicy },
      {"rightMargin" , TextBlockFormat::rightMargin },
      {"setAlignment" , TextBlockFormat::setAlignment },
      {"setBottomMargin" , TextBlockFormat::setBottomMargin },
      {"setIndent" , TextBlockFormat::setIndent },
      {"setLeftMargin" , TextBlockFormat::setLeftMargin },
      {"setNonBreakableLines" , TextBlockFormat::setNonBreakableLines },
      {"setPageBreakPolicy" , TextBlockFormat::setPageBreakPolicy },
      {"setRightMargin" , TextBlockFormat::setRightMargin },
      {"setTextIndent" , TextBlockFormat::setTextIndent },
      {"setTopMargin" , TextBlockFormat::setTopMargin },
      {"textIndent" , TextBlockFormat::textIndent },
      {"topMargin" , TextBlockFormat::topMargin },
	{ 0, 0 }
};
void TextBlockFormat::install(lua_State * L){
	ValueInstaller2<QTextBlockFormat>::install( L, _TextBlockFormat, "QTextBlockFormat" );
}
