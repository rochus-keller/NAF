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

#include <QWidget>
#include "QtlWidget.h"
#include "QtlFontInfo.h"
#include "QtlFontMetrics.h"
#include <QAction>
#include <QLayout>
#include <QList>
#include <QPalette>
#include <QFontInfo>
#include <QFontMetrics>
#include <QStyle>
#include <QPaintEngine>
#include <QString>
#include <QInputContext>
#include <QBitmap>
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script/Util.h>
#include "Variant.h"
using namespace Qtl;
using namespace Lua;

GCC_IGNORE(-Wunused-variable);
GCC_IGNORE(-Wunused-but-set-variable);

int Widget::actions(lua_State * L) // const : QList<QAction *>
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	QList<QAction *> r = obj->actions();
	lua_newtable( L );
	int t = lua_gettop( L );
	for( int i = 0; i < r.size(); ++i )
	{
		Util::push( L, i + 1 );
        QtObject<QAction>::create( L, r[i] );
        lua_rawset( L, t );
	}
	return 1;
}
int Widget::activateWindow(lua_State * L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	obj->activateWindow();
	return 0;
}
int Widget::addAction(lua_State * L) // ( QAction * action )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QAction* rhs = QtObject<QAction>::check( L, 2);
	obj->addAction(rhs);
	return 0;
}
int Widget::addActions(lua_State * L) // ( const QAction action, QAction action ...)
// table {QAction action, QAction action ...}
{	
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	int size=0;
	if (lua_istable(L,2))
	{
		QList<QAction *> res;
		 int i=0;
		 lua_pushnil(L);
		 while (lua_next(L, 2) != 0)
		 {
			
            QAction* r = QtObject<QAction>::check( L, -1);
			res.append(r);
			i++;
			lua_pop(L, 1);
		 }
		obj->addActions(res);
	}
    else if(QAction* r = QtObject<QAction>::cast( L, 2 ))
	{
		size=Util::top(L)-1;
		QList<QAction *> res;
		//rect_array=new QRect[size];
		for(int i = 2; i <= size+1; ++i)
		{
            QAction* r = QtObject<QAction>::check( L, i );
			res.append(r);
			//rect_array[i-2]=r->toRect();	
		}
		obj->addActions(res);
	}
	return 0;
}
int Widget::adjustSize(lua_State * L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	obj->adjustSize();
	return 0;
}
int Widget::backgroundRole(lua_State * L) // const : QPalette::ColorRole
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	Lua::Util::push( L, obj->backgroundRole() );
	return 1;
}
int Widget::childAt(lua_State * L) //( int x, int y ) const : QWidget
// ( const QPoint & p ) const : QWidget
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	if( Util::isNum(L, 2) )
        QtObject<QWidget>::create( L, obj->childAt( Util::toInt( L, 2 ), Util::toInt( L, 3 ) ) );
    else if( QPointF* r = QtValue<QPointF>::cast( L, 2 ) )
        QtObject<QWidget>::create( L, obj->childAt( r->toPoint() ));
	return 1;
}
int Widget::clearFocus(lua_State * L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	obj->clearFocus();
	return 0;
}
int Widget::clearMask(lua_State * L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	obj->clearMask();
	return 0;
}
int Widget::contentsRect(lua_State * L) // const : QRect
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QRectF* res = QtValue<QRectF>::create( L );
	*res=obj->contentsRect();
	return 1;
}
int Widget::ensurePolished(lua_State * L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	obj->ensurePolished();
	return 0;
}
int Widget::focusProxy(lua_State * L) // const : QWidget
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QtObject<QWidget>::create( L, obj->focusProxy());
	return 1;
}
int Widget::focusWidget(lua_State * L) // const : QWidget
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QtObject<QWidget>::create( L, obj->focusWidget());
	return 1;
}
int Widget::fontInfo(lua_State * L) // const : QFontInfo
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QFontInfo* res = QtValue<MyFontInfo>::create( L );
	*res = 	obj->fontInfo();
	return 1;
}
int Widget::fontMetrics(lua_State * L) // const : QFontMetrics 
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QFontMetricsF* res = QtValue<MyFontMetrics>::create( L );
	*res = 	obj->fontMetrics();
	return 1;
}
int Widget::foregroundRole(lua_State * L) // const : QPalette::ColorRole
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	Lua::Util::push( L, obj->foregroundRole() );
	return 1;
}
int Widget::getContentsMargins(lua_State * L) // ( int * left, int * top, int * right, int * bottom ) 
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	int left,top,right,bottom;
	obj->getContentsMargins(&left,&top,&right,&bottom);
	Util::push(L,left);
	Util::push(L,top);
	Util::push(L,right);
	Util::push(L,bottom);
	return 4;
}
int Widget::grabKeyboard(lua_State * L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	obj->grabKeyboard();
	return 0;
}
int Widget::grabMouse(lua_State * L) // ( const QCursor & cursor )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    if(QCursor* p = QtValue<QCursor>::cast( L, 2 ))
		obj->grabMouse( *p );
	else
		obj->grabMouse();
	return 0;
}
int Widget::grabShortcut(lua_State * L) // ( const QKeySequence & key, Qt::ShortcutContext context = Qt::WindowShortcut ) : int
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QKeySequence* rhs = QtValue<QKeySequence>::check( L, 2 );
	Qt::ShortcutContext f;
	int fInt = Util::toInt( L, 3 );
	if( (fInt >= Qt::WidgetShortcut && fInt <= Qt::ApplicationShortcut ) )
	{
		f = (Qt::ShortcutContext) fInt;
		Lua::Util::push( L, obj->grabShortcut( *rhs, f ) );
	}
	else
		Lua::Util::error( L, "expecting a valid Qt::ShortcutContext" );
	return 1;
}
int Widget::heightForWidth(lua_State * L) // ( int w ) const : int
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	Lua::Util::push( L, obj->heightForWidth(Util::toInt( L, 2 )) );
	return 1;
}
int Widget::inputContext(lua_State * L) // QInputContext
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QtObject<QInputContext>::create( L, obj->inputContext());
	return 1;
}
int Widget::inputMethodQuery(lua_State * L) // ( Qt::InputMethodQuery query ) const : QVariant
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	Qt::InputMethodQuery f;
	switch( Util::toInt( L, 2 ) )
	{
	case Qt::ImMicroFocus: 
		f = Qt::ImMicroFocus;
		Variant::push( L, obj->inputMethodQuery( f ) );
		break;
	case Qt::ImFont: 
		f = Qt::ImFont;
		Variant::push( L, obj->inputMethodQuery( f ) );
		break;
	case Qt::ImCursorPosition: 
		f = Qt::ImCursorPosition;
		Variant::push( L, obj->inputMethodQuery( f ) );
		break;
	case Qt::ImSurroundingText: 
		f = Qt::ImSurroundingText;
		Variant::push( L, obj->inputMethodQuery( f ) );
		break;
	case Qt::ImCurrentSelection: 
		f = Qt::ImCurrentSelection;
		Variant::push( L, obj->inputMethodQuery( f ) );
		break;
	default :
		Lua::Util::error( L, "expecting a valid Qt::InputMethodQuery" );
	}
	return 1;
}
int Widget::insertAction(lua_State * L) // ( QAction * before, QAction * action )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QAction* before = QtObject<QAction>::check( L, 2);
    QAction* action = QtObject<QAction>::check( L, 3);
	obj->insertAction( before, action );
	return 0;
}
int Widget::insertActions(lua_State * L) // ( QAction * before, table {QAction action, QAction action ...} )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QAction* before = QtObject<QAction>::check( L, 2);
	if (lua_istable(L,3))
	{QList<QAction *> res;
	 int i=0;
	 lua_pushnil(L);
	 while (lua_next(L, 3) != 0)
		 {
			
            QAction* r = QtObject<QAction>::check( L, -1);
			res.append(r);
			i++;
			lua_pop(L, 1);
		 }
	obj->insertActions(before, res);
	}
	return 0;
}
int Widget::isAncestorOf(lua_State * L) // ( const QWidget * child ) const : bool
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QWidget* rhs = QtObject<QWidget>::check( L, 2);
	Lua::Util::push( L, obj->isAncestorOf(rhs) );
	return 1;
}
int Widget::isEnabledTo(lua_State * L) // ( QWidget * ancestor ) const :: bool
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QWidget* rhs = QtObject<QWidget>::check( L, 2);
	Lua::Util::push( L, obj->isEnabledTo(rhs) );
	return 1;
}
int Widget::isHidden(lua_State * L) // const : bool
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	Lua::Util::push( L, obj->isHidden() );
	return 1;
}
int Widget::isVisibleTo(lua_State * L) // ( QWidget * ancestor ) const : bool
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QWidget* rhs = QtObject<QWidget>::check( L, 2);
	Lua::Util::push( L, obj->isVisibleTo(rhs) );
	return 1;
}
int Widget::isWindow(lua_State * L) // const : bool
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	Lua::Util::push( L, obj->isWindow() );
	return 1;
}
int Widget::layout(lua_State * L) // const : QLayout
{
    // TODO
//    QWidget* obj = QtObject<QWidget>::check( L, 1);
//    QtObject<QWidget>::create( L, obj->layout(), true);
    Util::push(L);
	return 1;
}
int Widget::mapFrom(lua_State * L) // ( QWidget * parent, const QPoint & pos ) const : QPoint
{	
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QWidget* rhs = QtObject<QWidget>::check( L, 2);
    QPointF* p = QtValue<QPointF>::check( L, 3 );
    QPointF* res = QtValue<QPointF>::create( L );
	*res = obj->mapFrom( rhs, p->toPoint() );
	return 1;
}
int Widget::mapFromGlobal(lua_State * L) // ( const QPoint & pos ) const : QPoint
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	if( Util::isNum( L, 2 ) )
	{
		const QPoint p = obj->mapFromGlobal( QPoint( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) ) );
		lua_pushnumber( L, p.x() );
		lua_pushnumber( L, p.y() );
		return 2;
	}else
	{
		QPointF* p = QtValue<QPointF>::check( L, 2 );
		QPointF* res = QtValue<QPointF>::create( L );
		*res = obj->mapFromGlobal( p->toPoint() );
		return 1;
	}
}
int Widget::mapFromParent(lua_State * L) // ( const QPoint & pos ) const : QPoint
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	if( Util::isNum( L, 2 ) )
	{
		const QPoint p = obj->mapFromParent( QPoint( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) ) );
		lua_pushnumber( L, p.x() );
		lua_pushnumber( L, p.y() );
		return 2;
	}else
	{
		QPointF* p = QtValue<QPointF>::check( L, 2 );
		QPointF* res = QtValue<QPointF>::create( L );
		*res = obj->mapFromParent( p->toPoint() );
		return 1;
	}
}
int Widget::mapTo(lua_State * L) // ( QWidget * parent, const QPoint & pos ) const : QPoint
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QWidget* rhs = QtObject<QWidget>::check( L, 2);
    QPointF* p = QtValue<QPointF>::check( L, 3 );
    QPointF* res = QtValue<QPointF>::create( L );
	*res = obj->mapTo( rhs, p->toPoint() );
	return 1;
}
int Widget::mapToGlobal(lua_State * L) // ( const QPoint & pos ) const : QPoint
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	if( Util::isNum( L, 2 ) )
	{
		const QPoint p = obj->mapToGlobal( QPoint( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) ) );
		lua_pushnumber( L, p.x() );
		lua_pushnumber( L, p.y() );
		return 2;
	}else
	{
		QPointF* p = QtValue<QPointF>::check( L, 2 );
		QPointF* res = QtValue<QPointF>::create( L );
		*res = obj->mapToGlobal( p->toPoint() );
		return 1;
	}
}
int Widget::mapToParent(lua_State * L) // ( const QPoint & pos ) const : QPoint
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	if( Util::isNum( L, 2 ) )
	{
		const QPoint p = obj->mapToParent( QPoint( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) ) );
		lua_pushnumber( L, p.x() );
		lua_pushnumber( L, p.y() );
		return 2;
	}else
	{
		QPointF* p = QtValue<QPointF>::check( L, 2 );
		QPointF* res = QtValue<QPointF>::create( L );
		*res = obj->mapToParent( p->toPoint() );
		return 1;
	}
}
int Widget::mask(lua_State * L) // const : QRegion
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QRegion* res = QtValue<QRegion>::create( L );
	*res = obj->mask();
	return 1;
}
int Widget::nextInFocusChain(lua_State * L) // const : QWidget
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QtObject<QWidget>::create( L, obj->nextInFocusChain());
	return 1;
}
int Widget::overrideWindowFlags(lua_State * L) // ( Qt::WindowFlags flags )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	Qt::WindowFlags f;
	f &= Lua::Util::toInt( L, 2 );
	obj->overrideWindowFlags( f );
	return 0;
}
int paintEngine(lua_State * L) // const : QPaintEngine
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    //QPaintEngine* res = QtValue<QPaintEngine>::create( L ); // TODO
	return 1;
}
int Widget::parentWidget(lua_State * L) // const : QWidget
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QtObject<QWidget>::create( L, obj->parentWidget());
	return 1;
}
int Widget::releaseKeyboard(lua_State * L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	obj->releaseKeyboard();
	return 0;
}
int Widget::releaseMouse(lua_State * L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	obj->releaseMouse();
	return 0;
}
int Widget::releaseShortcut(lua_State * L) // ( int id )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	obj->releaseShortcut(Util::toInt( L, 2 ) );
	return 0;
}
int Widget::removeAction(lua_State * L) // ( QAction * action )
{	
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QAction* action = QtObject<QAction>::check( L, 2);
	obj->removeAction( action );
	return 0;
}
int Widget::repaint(lua_State * L) // ( int x, int y, int w, int h )
// ( const QRect & r ) // ( const QRegion & rgn )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	if( Util::isNum(L, 2) )
		obj->repaint( Util::toInt( L, 2 ), Util::toInt( L, 3 ),
		Util::toInt( L, 4 ), Util::toInt( L, 5 ) );
    else if(QRectF* p = QtValue<QRectF>::cast( L, 2 ))
		obj->repaint( p->toRect() );
    else if(QRegion* p = QtValue<QRegion>::cast( L, 2 ))
		obj->repaint( *p );
	return 0;
}
int Widget::restoreGeometry(lua_State * L) // ( const QByteArray & geometry ) : bool
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	size_t len;
	Lua::Util::push( L, obj->restoreGeometry(lua_tolstring( L, 2, &len ) ) );
	return 1;
}
int Widget::saveGeometry(lua_State * L) // const : QByteArray
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	QByteArray b= obj->saveGeometry();
	lua_pushlstring( L, b , b.size() );
	return 1;
}
int Widget::scroll(lua_State * L) // ( int dx, int dy )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    if(QRectF* p = QtValue<QRectF>::cast( L, 4 ))
		obj->scroll( Util::toInt( L, 2 ), Util::toInt( L, 3 ), p->toRect() );
	else
		obj->scroll( Util::toInt( L, 2 ), Util::toInt( L, 3 ) );
	return 0;
}
int Widget::setAttribute(lua_State * L) //( Qt::WidgetAttribute attribute, bool on = true ) 
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	Qt::WidgetAttribute f;
	int fInt = Util::toInt( L, 2 );
	if( (fInt >= 0 && fInt <= Qt::WA_AttributeCount) )
	{	
		f = (Qt::WidgetAttribute) fInt;
		obj->setAttribute( f, Lua::Util::toBool( L, 3 ) ) ;
	}
	else
		Lua::Util::error( L, "expecting a valid Qt::WidgetAttribute" );
	return 0;
}
int Widget::setBackgroundRole(lua_State * L) // ( QPalette::ColorRole role )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	const int fInt = Util::toInt( L, 2 );
	if( (fInt >= 0 && fInt <= QPalette::Background ) )
	{
		obj->setBackgroundRole( (QPalette::ColorRole) fInt );
	}
	else
		Lua::Util::error( L, "expecting a valid QPalette::ColorRole" );
	return 0;	
}
int Widget::setContentsMargins(lua_State * L) // ( int left, int top, int right, int bottom )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	obj->setContentsMargins(Util::toInt( L, 2 ),
		Util::toInt( L, 3 ),
		Util::toInt( L, 4 ),
		Util::toInt( L, 5 ) );
	return 0;
}
int Widget::setFixedHeight(lua_State * L) // ( int h )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	obj->setFixedHeight( Util::toInt( L, 2 ) );
	return 0;
}
int Widget::setFixedSize(lua_State * L) // ( const QSize & s )
// ( int w, int h )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	if( Util::isNum(L, 2) )
		obj->setFixedSize( luaL_checknumber( L, 2 ), luaL_checknumber( L, 3 ) );
	else
	{
		QSizeF* p = QtValue<QSizeF>::check( L, 2 );
		obj->setFixedSize( p->toSize() );
	}
	return 0;
}
int Widget::setFixedWidth(lua_State * L) // ( int w )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	obj->setFixedWidth( Util::toInt( L, 2 ) );
	return 0;
}
int Widget::setFocus(lua_State * L) // ( Qt::FocusReason reason )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	if( Util::top( L ) == 1 )
		obj->setFocus();
	else
	{
		int fInt = Util::toInt( L, 2 );
		if(fInt >= Qt::MouseFocusReason && fInt<= Qt::NoFocusReason)
			obj->setFocus( (Qt::FocusReason)fInt );
		else
			Lua::Util::error( L, "expecting a valid Qt::FocusReason" );
	}
	return 0;
}
int Widget::setFocusProxy(lua_State * L) // ( QWidget * w )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QWidget* rhs = QtObject<QWidget>::check( L, 2);
	obj->setFocusProxy( rhs );
	return 0;
}
int Widget::setForegroundRole(lua_State * L) // ( QPalette::ColorRole role )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	QPalette::ColorRole f;
	int fInt = Util::toInt( L, 2 );
	if( (fInt >= 0 && fInt <= 17) )
	{
		f = (QPalette::ColorRole) fInt;
		obj->setForegroundRole( f );
	}
	else
		Lua::Util::error( L, "expecting a valid QPalette::ColorRole" );
	return 0;	
}
int Widget::setInputContext(lua_State * L) // ( QInputContext * context )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QInputContext* rhs = QtObject<QInputContext>::check( L, 2);
    obj->setInputContext( rhs );
	return 0;
}
int Widget::setLayout(lua_State * L) // ( QLayout * layout )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QLayout* rhs = QtObject<QLayout>::check( L, 1);
	obj->setLayout( rhs );
	return 0;
}
int Widget::setMask(lua_State * L) // ( const QBitmap & bitmap )
// ( const QRegion & region )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    if(QBitmap* p = QtValue<QBitmap>::cast( L, 2 ))
		obj->setMask( *p );
    else if(QRegion* p = QtValue<QRegion>::cast( L, 2 ))
		obj->setMask( *p );
	return 0;
}
int Widget::setParent(lua_State * L) // ( QWidget * parent )
// ( QWidget * parent, Qt::WindowFlags f )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QWidget* rhs = QtObject<QWidget>::check( L, 2);
	if( Util::isNum(L, 3) )
	{	
		Qt::WindowFlags f;
		f &= Lua::Util::toInt( L, 3 );
		obj->setParent(rhs , f);
	}
	else
		obj->setParent(rhs);
	return 0;
}
int Widget::setShortcutAutoRepeat(lua_State * L) // ( int id, bool enable = true )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	int id = Lua::Util::toInt( L, 2 );
	bool enable = Lua::Util::toBool( L, 3 ); 
	obj->setShortcutAutoRepeat(id, enable);
	return 0;
}
int Widget::setShortcutEnabled(lua_State * L) // ( int id, bool enable = true )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	int id = Lua::Util::toInt( L, 2 );
	bool enable = Lua::Util::toBool( L, 3 ); 
	obj->setShortcutEnabled(id, enable);
	return 0;
}
int Widget::setStyle(lua_State * L) // ( QStyle * style )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QStyle* rhs = QtObject<QStyle>::check( L, 2);
	obj->setStyle( rhs );
	return 0;
}
int Widget::setWindowFlags(lua_State * L) // ( Qt::WindowFlags type )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	Qt::WindowFlags f;
	f &= Lua::Util::toInt( L, 2 );
	obj->setWindowFlags( f );
	return 0;
}
int Widget::setWindowRole(lua_State * L) // ( const QString & role )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    obj->setWindowRole( QtValueBase::toString( L, 2 ) );
	return 0;
}
int Widget::setWindowState(lua_State * L) // ( Qt::WindowStates windowState )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	Qt::WindowStates f;
	switch( Util::toInt( L, 2 ) )
	{
	case Qt::WindowNoState: 
		f = Qt::WindowNoState;
		obj->setWindowState( f );
		break;
	case Qt::WindowMinimized: 
		f = Qt::WindowMinimized;
		obj->setWindowState( f );
		break;
	case Qt::WindowMaximized: 
		f = Qt::WindowMaximized;
		obj->setWindowState( f );
		break;
	case Qt::WindowFullScreen: 
		f = Qt::WindowFullScreen;
		obj->setWindowState( f );
		break;
	case Qt::WindowActive: 
		f = Qt::WindowActive;
		obj->setWindowState( f );
		break;
	default :
		Lua::Util::error( L, "expecting a valid Qt::WindowStates" );
	}
	return 0;
}
int Widget::stackUnder(lua_State * L) // ( QWidget * w )
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QWidget* rhs = QtObject<QWidget>::check( L, 2);
	obj->stackUnder(rhs);
	return 0;
}
int Widget::style(lua_State * L) // const : QStyle
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QtObject<QStyle>::create( L, obj->style());
	return 1;
	
}
int Widget::testAttribute(lua_State * L) // ( Qt::WidgetAttribute attribute ) const : bool
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	Qt::WidgetAttribute f;
	int fInt = Util::toInt( L, 2 );
	if( (fInt >= 0 && fInt <= 16) || 
		 (fInt >= 32 && fInt <= 86) )
	{	
		if( fInt != 40 && fInt != 65 )
		{
			f = (Qt::WidgetAttribute) fInt;
			Lua::Util::push( L, obj->testAttribute( f ) );
		}
	}
	else
		Lua::Util::error( L, "expecting a valid Qt::WidgetAttribute" );
	return 1;
}
int Widget::underMouse(lua_State * L) // const : bool
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	Lua::Util::push( L, obj->underMouse() );
	return 1;
}
int Widget::update(lua_State * L)
// ( int x, int y, int w, int h )
// ( const QRect & r )
// ( const QRegion & rgn )
// ()
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	if( Util::top( L ) == 1 )
		obj->update();
	else if( Util::isNum(L, 2) )
		obj->update( Util::toInt( L, 2 ), Util::toInt( L, 3 ),
					 Util::toInt( L, 4 ), Util::toInt( L, 5 ) );
    else if(QRectF* p = QtValue<QRectF>::cast( L, 2 ))
		obj->update( p->toRect() );
    else if(QRegion* p = QtValue<QRegion>::cast( L, 2 ))
		obj->update( *p );
	return 0;
}
int Widget::updateGeometry(lua_State * L)
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	obj->updateGeometry();
	return 0;
}
int Widget::visibleRegion(lua_State * L) // const : QRegion
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QRegion* res = QtValue<QRegion>::create( L );
	*res = obj->visibleRegion();
	return 1;
}
int Widget::window(lua_State * L) // const : QWidget
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    QtObject<QWidget>::create( L, obj->window());
	return 1;
}
int Widget::windowFlags(lua_State * L) // const : Qt::WindowFlags
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	Lua::Util::push( L, obj->windowFlags() );
	return 1;
}
int Widget::windowRole(lua_State * L) // const : QString
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
    *QtValue<QString>::create( L ) = obj->windowRole();
	return 1; 
}
int Widget::windowState(lua_State * L) // () const : Qt::WindowStates
{	
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	Lua::Util::push( L, obj->windowState() );
	return 1;
}
int Widget::windowType(lua_State * L) // const : Qt::WindowType
{
    QWidget* obj = QtObject<QWidget>::check( L, 1);
	Lua::Util::push( L, obj->windowType() );
	return 1;
}
static const luaL_reg _Widget[] = 
{
	{ "actions", Widget::actions }, // QT_DEVIATION
	{ "activateWindow", Widget::activateWindow },
	{ "addAction", Widget::addAction },
	{ "addActions", Widget::addActions }, // QT_DEVIATION
	{ "adjustSize", Widget::adjustSize },
	{ "backgroundRole" , Widget::backgroundRole },
	{ "childAt", Widget::childAt }, 
	{ "clearFocus", Widget::clearFocus },
	{ "clearMask", Widget::clearMask },
	{ "contentsRect", Widget::contentsRect },
	{ "ensurePolished", Widget::ensurePolished },
	{ "focusProxy", Widget::focusProxy },
	{ "focusWidget", Widget::focusWidget },
	{ "fontInfo", Widget::fontInfo },
	{ "fontMetrics", Widget::fontMetrics },
	{ "foregroundRole", Widget::foregroundRole },
	{ "getContentsMargins", Widget::getContentsMargins }, // QT_DEVIATION
	{ "grabKeyboard", Widget::grabKeyboard },
	{ "grabMouse", Widget::grabMouse }, //( const QCursor & cursor ) to be tested
	{ "grabShortcut", Widget::grabShortcut },  //to be tested
	{ "heightForWidth", Widget::heightForWidth },
	{ "inputContext", Widget::inputContext }, //to be tested
	{ "inputMethodQuery", Widget::inputMethodQuery },
	{ "insertAction", Widget::insertAction },
	{ "insertActions", Widget::insertActions }, // QT_DEVIATION
	{ "isAncestorOf", Widget::isAncestorOf },
	{ "isEnabledTo", Widget::isEnabledTo },
	{ "isHidden", Widget::isHidden },
	{ "isVisibleTo", Widget::isVisibleTo },
	{ "isWindow", Widget::isWindow },
	{ "layout", Widget::layout },
	{ "mapFrom", Widget::mapFrom },
	{ "mapFromGlobal", Widget::mapFromGlobal },
	{ "mapFromParent", Widget::mapFromParent },
	{ "mapTo", Widget::mapTo },
	{ "mapToGlobal", Widget::mapToGlobal },
	{ "mapToParent", Widget::mapToParent },
	{ "mask", Widget::mask },
	{ "nextInFocusChain", Widget::nextInFocusChain },
	{ "overrideWindowFlags", Widget::overrideWindowFlags }, 
	//paintEngine requires QPaintEngine
	{ "parentWidget", Widget::parentWidget },
	{ "releaseKeyboard", Widget::releaseKeyboard },
	{ "releaseMouse", Widget::releaseMouse },
	{ "releaseShortcut", Widget::releaseShortcut },
	{ "removeAction", Widget::removeAction },
	{ "repaint", Widget::repaint }, 
	{ "restoreGeometry", Widget::restoreGeometry },
	{ "saveGeometry", Widget::saveGeometry },
	{ "scroll", Widget::scroll },
	{ "setAttribute", Widget::setAttribute }, 
	{ "setBackgroundRole", Widget::setBackgroundRole },
	{ "setContentsMargins", Widget::setContentsMargins },
	{ "setFixedHeight", Widget::setFixedHeight },
	{ "setFixedSize", Widget::setFixedSize },
	{ "setFixedWidth", Widget::setFixedWidth },
	{ "setFocus", Widget::setFocus },
	{ "setFocusProxy", Widget::setFocusProxy },
	{ "setForegroundRole", Widget::setForegroundRole },
	{ "setInputContext", Widget::setInputContext }, //to be tested
	{ "setLayout", Widget::setLayout },
	{ "setMask", Widget::setMask },
	{ "setParent", Widget::setParent },
	{ "setShortcutAutoRepeat", Widget::setShortcutAutoRepeat },
	{ "setShortcutEnabled", Widget::setShortcutEnabled },
	{ "setStyle", Widget::setStyle }, // to be tested
	{ "setWindowFlags", Widget::setWindowFlags },
	{ "setWindowRole", Widget::setWindowRole },
	{ "setWindowState", Widget::setWindowState },
	{ "stackUnder", Widget::stackUnder },
	{ "style", Widget::style },
	{ "testAttribute", Widget::testAttribute },
	{ "underMouse", Widget::underMouse },
	{ "update", Widget::update },
	{ "updateGeometry", Widget::updateGeometry },
	{ "visibleRegion", Widget::visibleRegion },
	{ "window", Widget::window },
	{ "windowFlags", Widget::windowFlags },
	{ "windowRole", Widget::windowRole }, 
	{ "windowState", Widget::windowState },
	{ "windowType", Widget::windowType },
	{ 0, 0 }
};

/*
static const luaL_reg _reg[] =
{
	{ "depth", PaintDevice< QtObject<QWidget> >::depth },
	{ "heightMM", PaintDevice< QtObject<QWidget> >::heightMM },
	{ "logicalDpiX", PaintDevice< QtObject<QWidget> >::logicalDpiX },
	{ "logicalDpiY", PaintDevice< QtObject<QWidget> >::logicalDpiY },
	{ "numColors", PaintDevice< QtObject<QWidget> >::numColors },
	{ "paintingActive", PaintDevice< QtObject<QWidget> >::paintingActive },
	{ "physicalDpiX", PaintDevice< QtObject<QWidget> >::physicalDpiX },
	{ "physicalDpiY", PaintDevice< QtObject<QWidget> >::physicalDpiY },
	{ "widthMM", PaintDevice< QtObject<QWidget> >::widthMM },
	{ 0, 0 }
};
*/

void Widget::install(lua_State * L){
	QtObject<QWidget,QObject>::install( L, "QWidget", _Widget );
	QtObject<QWidget>::addMethods( L, PaintDevice< QtObject<QWidget> >::_reg ); // Mixin
}

