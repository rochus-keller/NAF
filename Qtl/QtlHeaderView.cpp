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

#include <QHeaderView>
#include <QPoint>
#include "QtlHeaderView.h"
#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;
int HeaderView::count(lua_State * L) // const : int
{
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	Util::push( L, obj->count() );
	return 1;
}
int HeaderView::hiddenSectionCount(lua_State * L) // const : int
{
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	Util::push( L, obj->hiddenSectionCount() );
	return 1;
}
int HeaderView::hideSection(lua_State * L) // ( int logicalIndex )
{
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	obj->hideSection( Util::toInt( L, 2 ) );
	return 0;
}
int HeaderView::isClickable(lua_State * L) // const : bool
{
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	Util::push( L, obj->isClickable() );
	return 1;
}
int HeaderView::isMovable(lua_State * L) // const : bool
{
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	Util::push( L, obj->isMovable() );
	return 1;
}
int HeaderView::isSectionHidden(lua_State * L) // ( int logicalIndex ) const : bool
{
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	Util::push( L, obj->isSectionHidden( Util::toInt( L, 2) ) );
	return 1;
}
int HeaderView::length(lua_State * L) // const : int
{
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	Util::push( L, obj->length() );
	return 1;
}
int HeaderView::logicalIndex(lua_State * L) // ( int visualIndex ) const : int
{
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	Util::push( L, obj->logicalIndex( Util::toInt( L, 2) ) );
	return 1;
}
int HeaderView::logicalIndexAt(lua_State * L) // ( int position ) const : int
// ( const QPoint & pos ) const : int
// ( int x, int y ) const : int
{
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	if( QPointF* r = ValueInstaller2<QPointF>::cast( L, 2 ) )
		Util::push( L, obj->logicalIndexAt( r->toPoint() ) );
	else if( Util::isNum( L, 3 ) )
		Util::push( L, obj->logicalIndexAt( Util::toInt( L, 2), Util::toInt( L, 3) ) );
	else
		Util::push( L, obj->logicalIndexAt( Util::toInt( L, 2) ) );
	return 1;
}
int HeaderView::moveSection(lua_State * L) // ( int from, int to )
{
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	obj->moveSection( Util::toInt( L, 2), Util::toInt( L, 3) );
	return 1;
}
int HeaderView::offset(lua_State * L) // const : int
{
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	Util::push( L, obj->offset() );
	return 1;
}
int HeaderView::orientation(lua_State * L) // const : Qt::Orientation 
{
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	Util::push( L, obj->orientation() );
	return 1;
}
int HeaderView::resizeMode(lua_State * L) // ( int logicalIndex ) const : ResizeMode 
{
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	Util::push( L, obj->resizeMode( Util::toInt( L, 2) ) );
	return 1;
}
int HeaderView::resizeSection(lua_State * L) // ( int logicalIndex, int size )
{
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	obj->resizeSection( Util::toInt( L, 2), Util::toInt( L, 3) );
	return 0;
}
int HeaderView::resizeSections(lua_State * L) // ( QHeaderView::ResizeMode mode )
{	
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	QHeaderView::ResizeMode f;
	int fInt = Util::toInt( L, 2 );	
	if( fInt >=0 && fInt <= 3 )
	{
		f = (QHeaderView::ResizeMode) fInt;
		obj->resizeSections( f ) ;
	}
	else
		Lua::Util::error( L, "expecting a valid QHeaderView::ResizeMode" );
	return 0;
}
int HeaderView::sectionPosition(lua_State * L) // ( int logicalIndex ) const : int
{	
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	Util::push( L, obj->sectionPosition( Util::toInt( L, 2) ) );
	return 1;
}
int HeaderView::sectionSize(lua_State * L) // ( int logicalIndex ) const : int
{	
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	Util::push( L, obj->sectionSize( Util::toInt( L, 2) ) );
	return 1;
}
int HeaderView::sectionSizeHint(lua_State * L) // ( int logicalIndex ) const : int
{	
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	Util::push( L, obj->sectionSizeHint( Util::toInt( L, 2) ) );
	return 1;
}
int HeaderView::sectionViewportPosition(lua_State * L) // ( int logicalIndex ) const : int
{	
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	Util::push( L, obj->sectionViewportPosition( Util::toInt( L, 2) ) );
	return 1;
}
int HeaderView::sectionsHidden(lua_State * L) // const : bool 
{	
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	Util::push( L, obj->sectionsHidden() );
	return 1;
}
int HeaderView::sectionsMoved(lua_State * L) // const : bool
{	
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	Util::push( L, obj->sectionsMoved() );
	return 1;
}
int HeaderView::setClickable(lua_State * L) // ( bool clickable )
{	
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	obj->setClickable( Util::toBool( L, 2 ) );
	return 0;
}
int HeaderView::setMovable(lua_State * L) // ( bool movable )
{	
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	obj->setMovable( Util::toBool( L, 2 ) );
	return 0;
}
int HeaderView::setResizeMode(lua_State * L) // ( ResizeMode mode )
{	
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	QHeaderView::ResizeMode f;
	if( Util::isNum( L, 3 ) )
	{
		int fInt = Util::toInt( L, 3 );	
		if( fInt >=0 && fInt <= 3 )
		{
			f = (QHeaderView::ResizeMode) fInt;
			obj->setResizeMode( Util::toInt( L, 2), f ) ;
		}
		else
			Lua::Util::error( L, "expecting a valid QHeaderView::ResizeMode" );
	}
	else
	{
		int fInt = Util::toInt( L, 2 );	
		if( fInt >=0 && fInt <= 3 )
		{
			f = (QHeaderView::ResizeMode) fInt;
			obj->setResizeMode( f ) ;
		}
		else
			Lua::Util::error( L, "expecting a valid QHeaderView::ResizeMode" );
	}
	return 0;
}
int HeaderView::setSectionHidden(lua_State * L) // ( int logicalIndex, bool hide )
{
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	obj->setSectionHidden( Util::toInt( L, 2 ), Util::toBool( L, 3 ) );
	return 0;
}
int HeaderView::setSortIndicator(lua_State * L) // ( int logicalIndex, Qt::SortOrder order )
{
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	Qt::SortOrder f;
	int fInt = Util::toInt( L, 3 );	
	if( fInt ==0 || fInt == 1 )
	{
		f = (Qt::SortOrder) fInt;
		obj->setSortIndicator( Util::toInt( L, 2 ), f ) ;
	}
	else
		Lua::Util::error( L, "expecting a valid Qt::SortOrder" );
	return 0;
}
int HeaderView::showSection(lua_State * L) // ( int logicalIndex )
{
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	obj->showSection( Util::toInt( L, 2 ) );
	return 0;
}
int HeaderView::sizeHint(lua_State * L) // const : QSize 
{
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	QSizeF* res = ValueInstaller2<QSizeF>::create( L );
	*res = obj->sizeHint();
	return 1;
}
int HeaderView::sortIndicatorOrder(lua_State * L) // const : Qt::SortOrder 
{
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	Util::push( L, obj->sortIndicatorOrder() );
	return 1;
}
int HeaderView::sortIndicatorSection(lua_State * L) // const : int
{
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	Util::push( L, obj->sortIndicatorSection() );
	return 1;
}
int HeaderView::stretchSectionCount(lua_State * L) // const : int
{
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	Util::push( L, obj->stretchSectionCount() );
	return 1;
}
int HeaderView::swapSections(lua_State * L) // ( int first, int second )
{
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	obj->swapSections( Util::toInt( L, 2 ), Util::toInt( L, 3 ) );
	return 0;
}
int HeaderView::visualIndex(lua_State * L) // ( int logicalIndex ) const : int
{
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	Util::push( L, obj->visualIndex( Util::toInt( L, 2) ) );
	return 1;
}
int HeaderView::visualIndexAt(lua_State * L) // ( int position ) const : int
{
	QHeaderView* obj = ObjectHelper<QHeaderView>::check( L, 1);
	Util::push( L, obj->visualIndexAt( Util::toInt( L, 2) ) );
	return 1;
}
static const luaL_reg _HeaderView[] = 
{
	{ "count", HeaderView::count },
	{ "hiddenSectionCount", HeaderView::hiddenSectionCount },
	{ "hideSection", HeaderView::hideSection },
	{ "isClickable", HeaderView::isClickable },
	{ "isMovable", HeaderView::isMovable },
	{ "isSectionHidden", HeaderView::isSectionHidden },
	{ "length", HeaderView::length },
	{ "logicalIndex", HeaderView::logicalIndex },
	{ "logicalIndexAt", HeaderView::logicalIndexAt },
	{ "moveSection", HeaderView::moveSection },
	{ "offset", HeaderView::offset },
	{ "orientation", HeaderView::orientation },
	{ "resizeMode", HeaderView::resizeMode },
	{ "resizeSection", HeaderView::resizeSection },
	{ "resizeSections", HeaderView::resizeSections },
	{ "sectionPosition", HeaderView::sectionPosition },
	{ "sectionSize", HeaderView::sectionSize },
	{ "sectionSizeHint", HeaderView::sectionSizeHint },
	{ "sectionViewportPosition", HeaderView::sectionViewportPosition },
	{ "sectionsHidden", HeaderView::sectionsHidden },
	{ "sectionsMoved", HeaderView::sectionsMoved },
	{ "setClickable", HeaderView::setClickable },
	{ "setMovable", HeaderView::setMovable },
	{ "setResizeMode", HeaderView::setResizeMode },
	{ "setSectionHidden", HeaderView::setSectionHidden },
	{ "setSortIndicator", HeaderView::setSortIndicator },
	{ "showSection", HeaderView::showSection },
	{ "sizeHint" , HeaderView::sizeHint },
	{ "sortIndicatorOrder", HeaderView::sortIndicatorOrder },
	{ "sortIndicatorSection", HeaderView::sortIndicatorSection },
	{ "swapSections", HeaderView::swapSections },
	{ "visualIndex", HeaderView::visualIndex },
	{ 0, 0 }
};
void HeaderView::install(lua_State * L){
	ObjectInstaller<QHeaderView, AbstractObjectFactory>::install( L, _HeaderView, ObjectInstaller<QAbstractItemView>::className());
}
