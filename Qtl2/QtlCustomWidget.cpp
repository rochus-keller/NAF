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
#include "QtlCustomWidget.h"
#include "QtlWidget.h"
#include <QCloseEvent>
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script/Engine2.h>
using namespace Qtl;
using namespace Lua;

CustomWidget::CustomWidget(int ref, QWidget *parent) :
	QWidget(parent), d_classRef(ref)
{
}

CustomWidget::~CustomWidget()
{
	if( d_classRef != LUA_NOREF )
	{
		Engine2* e = Engine2::getInst();
		if( e != 0 )
		{
			CustomWidget::pushClassTable( e->getCtx() );
			luaL_unref( e->getCtx(), -1, d_classRef );
			lua_pop( e->getCtx(), 1 ); // class table
		}
	}
}

static int init(lua_State *L)
{
	CustomWidget* w = 0;
	StackTester test( L, 0 );
	// Das erste Argument ist die Methodentabelle oder nil
	int ref = LUA_NOREF;
	if( lua_istable( L, 2 ) )
	{
		CustomWidget::pushClassTable( L );
		lua_pushvalue( L, 2 );
		ref = luaL_ref( L, -2 );
		lua_pop( L, 1 ); // Class Table
	}
	// Das zweite Argument ist QWidget oder nil bzw. optional
	w = new CustomWidget( ref, QtObject<QWidget>::cast( L, 3 ) );
	QtObject<CustomWidget>::setPointer( L, 1, w );
	return 0;
}

static const luaL_reg _reg[] =
{
	{ "init", init },
	{ 0, 0 }
};

void CustomWidget::install(lua_State *L)
{
	QtObject<CustomWidget,QWidget,HasCustomInit>::install( L, "CustomWidget", _reg );
	QtObject<CustomWidget>::addMetaMethod( L, "__index", index );
}

int CustomWidget::index(lua_State *L)
{
	StackTester test( L, 1 );
	CustomWidget* obj = QtObject<CustomWidget>::check( L, 1 );
	QByteArray name = luaL_checkstring( L, 2 );

	if( obj->d_classRef != LUA_NOREF )
	{
		// Schaue zuerst, ob im Datatable das Feld "methods" vorhanden ist und dieses auf eine Tabelle zeigt
		pushClassTable( L );
		lua_rawgeti( L, -1, obj->d_classRef );
		lua_remove( L, -2 );
	}else
		lua_pushnil(L);
	const int methods = lua_gettop(L);
	// Stack: obj, name, classTable | nil

	if( name == "owner" || name == "class" || name == "type" || name == "data" )
	{
		// name ist ein intrinsisches attribut des Bindings
		if( !lua_isnil( L, methods ) && name == "class" )
		{
			lua_getfield( L, methods, "__class" );
			// Stack: obj, name, classTable, value | nil
			if( !lua_isnil( L, -1 ) )
			{
				lua_remove( L, -2 );
				return 1;
			}else
				lua_pop( L, 1 ); // nil
		} // else
		lua_pop( L, 1 ); // methods
		return ValueBindingBase::fetch( L, false, true );
	}

	if( !lua_isnil( L, methods ) )
	{
		// prüfe, ob diese Tabelle eine Funktion unter gegebenem Namen hat.
		lua_getfield( L, methods, name );
		lua_remove( L, methods );

		if( !lua_isnil( L, -1 ) )
			return 1;
	}
	// else
	// ansonsten delegiere an die Vererbung von QtObject
	lua_pop( L, 1 ); // nil
	return QtObjectBase::index2( L );
}

void CustomWidget::pushClassTable(lua_State *L)
{
	StackTester test( L, 1 );
	// Der ClassTable ist eine globale Tabelle mit weak values, welche mittels luaL_ref Referenzen auf
	// Methodentabellen speichert.
	static const char* s_ptr = "CustomWidgetClassTable";
	lua_pushlightuserdata( L, (void*)s_ptr );
	lua_rawget( L, LUA_REGISTRYINDEX );
	if( lua_isnil( L, -1 ) )
	{
		lua_pop(L, 1); // Drop nil
		lua_newtable ( L );
		const int t = lua_gettop(L);
		lua_pushlightuserdata(L, (void*)s_ptr );
		lua_pushvalue(L, t );
		lua_rawset( L, LUA_REGISTRYINDEX );
		lua_newtable( L );
		const int m = lua_gettop(L);
		lua_pushliteral( L, "v" );
		lua_setfield( L, m, "__mode" );
		lua_setmetatable( L, t );
	}
}

bool CustomWidget::pushMethodAndThis( const char *name) const
{
	Engine2* e = Engine2::getInst();
	Q_ASSERT( e != 0 );

	const int preTop = lua_gettop(e->getCtx()); // muss nicht null sein, da z.B. :show direkt Callback auslöst

#ifdef __via_index
	QtObject<CustomWidget>::create( e->getCtx(), const_cast<CustomWidget*>(this), false );
	lua_pushstring( e->getCtx(), name );
	// Stack: this, name
	index( e->getCtx() );
	// Stack: this, name, value
	if( !lua_isfunction( e->getCtx(), -1 ) )
	{
		lua_pop( e->getCtx(), 3 );
		return false;
	}
	lua_pushvalue( e->getCtx(), 1 );
	lua_remove( e->getCtx(), 1 );
	lua_remove( e->getCtx(), 1 );
	// Stack: function, this
#else
	// Verwende nicht index(), sondern suche nur in MethodTable und dessen Metas
	if( d_classRef != LUA_NOREF )
	{
		pushClassTable( e->getCtx() );
		// Stack: classTable
		lua_rawgeti( e->getCtx(), -1, d_classRef );
		// Stack: classTable, methodTable | nil
		if( !lua_isnil( e->getCtx(), -1 ) )
		{
			// Stack: classTable, methodTable
			lua_getfield( e->getCtx(), -1, name );
			// Stack: classTable, methodTable, function | nil
			if( lua_isnil( e->getCtx(), -1 ) )
			{
				// Stack: classTable, methodTable, nil
				lua_pop( e->getCtx(), 3 );
				Q_ASSERT( lua_gettop( e->getCtx() ) == preTop );
				return false;
			}
			// Stack: classTable, methodTable, function
			lua_remove( e->getCtx(), -2 );
			lua_remove( e->getCtx(), -2 );
			// Stack: function
			QtObject<CustomWidget>::create( e->getCtx(), const_cast<CustomWidget*>(this), false );
			// Stack: function, this
		}else
		{
			// Stack: classTable, nil
			lua_pop( e->getCtx(), 2 );
			Q_ASSERT( lua_gettop( e->getCtx() ) == preTop );
			return false;
		}
	}else
	{
		Q_ASSERT( lua_gettop( e->getCtx() ) == preTop );
		return false;
	}
#endif
	Q_ASSERT( lua_gettop( e->getCtx() ) == preTop + 2 );
	return true;
}

bool CustomWidget::call(int nargs, int nresults)
{
	Engine2* e = Engine2::getInst();
	Q_ASSERT( e != 0 );
	const int preTop = lua_gettop( e->getCtx() );
	if( !e->runFunction( nargs, nresults ) && !e->isSilent() )
	{
		try
		{
			qDebug() << "CustomWidget::call:" << e->getLastError();
			e->error( e->getLastError() );
		}catch( const std::exception& e )
		{
			qDebug( "CustomWidget::call: Error calling host: %s", e.what() );
		}
		Q_ASSERT( lua_gettop( e->getCtx() ) == ( preTop - nargs - 1 ) );
		for( int i = 0; i < nresults; i++ )
			lua_pushnil( e->getCtx() );
		return false;
	}else
	{
		Q_ASSERT( lua_gettop( e->getCtx() ) == ( preTop - nargs - 1 + nresults ) );
		return true;
	}
}

int CustomWidget::heightForWidth(int w) const
{
	lua_State* L = Engine2::getInst()->getCtx();
	Q_ASSERT( L != 0 );
	StackTester test( L, 0 );
	if( pushMethodAndThis( "heightForWidth") )
	{
		// Push Args: this, w (int)
		lua_pushinteger( L, w );
		if( call( 2, 1 ) )
		{
			const int h = lua_tointeger( L, -1 );
			lua_pop( L, 1 ); // return value
			return h;
		}
	}//else
	return QWidget::heightForWidth( w );
}

QSize CustomWidget::minimumSizeHint() const
{
	lua_State* L = Engine2::getInst()->getCtx();
	Q_ASSERT( L != 0 );
	StackTester test( L, 0 );
	if( pushMethodAndThis( "minimumSizeHint") )
	{
		// Push Args: this
		if( call( 1, 1 ) )
		{
			QSize res;
			if( QSizeF* s = QtValue<QSizeF>::cast( L, -1 ) )
				res = s->toSize();
			lua_pop( L, 1 ); // return value
			return res;
		}
	}//else
	return QWidget::minimumSizeHint();
}

QSize CustomWidget::sizeHint() const
{
	lua_State* L = Engine2::getInst()->getCtx();
	Q_ASSERT( L != 0 );
	StackTester test( L, 0 );
	if( pushMethodAndThis( "sizeHint") )
	{
		// Push Args: this
		if( call( 1, 1 ) )
		{
			QSize res;
			if( QSizeF* s = QtValue<QSizeF>::cast( L, -1 ) )
				res = s->toSize();
			lua_pop( L, 1 ); // return value
			return res;
		}
	}//else
	return QWidget::sizeHint();
}

void CustomWidget::changeEvent(QEvent *event)
{
	lua_State* L = Engine2::getInst()->getCtx();
	Q_ASSERT( L != 0 );
	StackTester test( L, 0 );
	if( pushMethodAndThis( "changeEvent") )
	{
		// Push Args: this, eventType (int)
		lua_pushinteger( L, event->type() );
		if( call( 2, 1 ) && lua_isboolean( L, -1 ) )
			(lua_toboolean( L, -1 ))?event->accept():event->ignore();
		lua_pop( L, 1 ); // return value
	}
}

void CustomWidget::closeEvent(QCloseEvent *event)
{
	lua_State* L = Engine2::getInst()->getCtx();
	Q_ASSERT( L != 0 );
	StackTester test( L, 0 );
	if( pushMethodAndThis( "closeEvent") )
	{
		// Push Args: this
		if( call( 1, 1 ) && lua_isboolean( L, -1 ) )
			(lua_toboolean( L, -1 ))?event->accept():event->ignore();
		lua_pop( L, 1 ); // return value
	}
}

void CustomWidget::contextMenuEvent(QContextMenuEvent *event)
{
	lua_State* L = Engine2::getInst()->getCtx();
	Q_ASSERT( L != 0 );
	StackTester test( L, 0 );
	if( pushMethodAndThis( "contextMenuEvent") )
	{
		// Push Args: this, modifiers (int), globalPos (QPoint), pos (QPoint), reason (int)
		lua_pushinteger( L, event->modifiers() );
		*QtValue<QPointF>::create( L ) = event->globalPos();
		*QtValue<QPointF>::create( L ) = event->pos();
		lua_pushinteger( L, event->reason() );
		if( call( 5, 1 ) && lua_isboolean( L, -1 ) )
			(lua_toboolean( L, -1 ))?event->accept():event->ignore();
		lua_pop( L, 1 ); // return value
	}
}

void CustomWidget::enterEvent(QEvent *event)
{
	lua_State* L = Engine2::getInst()->getCtx();
	Q_ASSERT( L != 0 );
	StackTester test( L, 0 );
	if( pushMethodAndThis( "enterEvent") )
	{
		// Push Args: this
		if( call( 1, 1 ) && lua_isboolean( L, -1 ) )
			(lua_toboolean( L, -1 ))?event->accept():event->ignore();
		lua_pop( L, 1 ); // return value
	}
}

void CustomWidget::focusInEvent(QFocusEvent *event)
{
	lua_State* L = Engine2::getInst()->getCtx();
	Q_ASSERT( L != 0 );
	StackTester test( L, 0 );
	if( pushMethodAndThis( "focusInEvent") )
	{
		// Push Args: this, gotFocus (bool), lostFocus (bool), reason (int)
		lua_pushboolean( L, event->gotFocus() );
		lua_pushboolean( L, event->lostFocus() );
		lua_pushinteger( L, event->reason() );
		if( call( 4, 1 ) && lua_isboolean( L, -1 ) )
			(lua_toboolean( L, -1 ))?event->accept():event->ignore();
		lua_pop( L, 1 ); // return value
	}
}

void CustomWidget::focusOutEvent(QFocusEvent *event)
{
	lua_State* L = Engine2::getInst()->getCtx();
	Q_ASSERT( L != 0 );
	StackTester test( L, 0 );
	if( pushMethodAndThis( "focusOutEvent") )
	{
		// Push Args: this, gotFocus (bool), lostFocus (bool), reason (int)
		lua_pushboolean( L, event->gotFocus() );
		lua_pushboolean( L, event->lostFocus() );
		lua_pushinteger( L, event->reason() );
		if( call( 4, 1 ) && lua_isboolean( L, -1 ) )
			(lua_toboolean( L, -1 ))?event->accept():event->ignore();
		lua_pop( L, 1 ); // return value
	}
}

void CustomWidget::hideEvent(QHideEvent *event)
{
	lua_State* L = Engine2::getInst()->getCtx();
	Q_ASSERT( L != 0 );
	StackTester test( L, 0 );
	if( pushMethodAndThis( "hideEvent") )
	{
		// Push Args: this
		if( call( 1, 1 ) && lua_isboolean( L, -1 ) )
			(lua_toboolean( L, -1 ))?event->accept():event->ignore();
		lua_pop( L, 1 ); // return value
	}
}

void CustomWidget::keyPressEvent(QKeyEvent *event)
{
	lua_State* L = Engine2::getInst()->getCtx();
	Q_ASSERT( L != 0 );
	StackTester test( L, 0 );
	if( pushMethodAndThis( "keyPressEvent") )
	{
		// Push Args: this, count (int), autoRepeat (bool), key (int), modifiers (int), text (QString)
		lua_pushinteger( L, event->count() );
		lua_pushboolean( L, event->isAutoRepeat() );
		lua_pushinteger( L, event->key() );
		lua_pushinteger( L, event->modifiers() );
		*QtValue<QString>::create( L ) = event->text();
		if( call( 6, 1 ) && lua_isboolean( L, -1 ) )
			(lua_toboolean( L, -1 ))?event->accept():event->ignore();
		lua_pop( L, 1 ); // return value
	}
}

void CustomWidget::keyReleaseEvent(QKeyEvent *event)
{
	lua_State* L = Engine2::getInst()->getCtx();
	Q_ASSERT( L != 0 );
	StackTester test( L, 0 );
	if( pushMethodAndThis( "keyReleaseEvent") )
	{
		// Push Args: this, count (int), autoRepeat (bool), key (int), modifiers (int), text (QString)
		lua_pushinteger( L, event->count() );
		lua_pushboolean( L, event->isAutoRepeat() );
		lua_pushinteger( L, event->key() );
		lua_pushinteger( L, event->modifiers() );
		*QtValue<QString>::create( L ) = event->text();
		if( call( 6, 1 ) && lua_isboolean( L, -1 ) )
			(lua_toboolean( L, -1 ))?event->accept():event->ignore();
		lua_pop( L, 1 ); // return value
	}
}

void CustomWidget::leaveEvent(QEvent *event)
{
	lua_State* L = Engine2::getInst()->getCtx();
	Q_ASSERT( L != 0 );
	StackTester test( L, 0 );
	if( pushMethodAndThis( "leaveEvent") )
	{
		// Push Args: this
		if( call( 1, 1 ) && lua_isboolean( L, -1 ) )
			(lua_toboolean( L, -1 ))?event->accept():event->ignore();
		lua_pop( L, 1 ); // return value
	}
}

void CustomWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	lua_State* L = Engine2::getInst()->getCtx();
	Q_ASSERT( L != 0 );
	StackTester test( L, 0 );
	if( pushMethodAndThis( "mouseDoubleClickEvent") )
	{
		// Push Args: this, modifiers (int), button (int), buttons (int), globalPos (QPoint), pos (QPoint)
		lua_pushinteger( L, event->modifiers() );
		lua_pushinteger( L, event->button() );
		lua_pushinteger( L, event->buttons() );
		*QtValue<QPointF>::create( L ) = event->globalPos();
		*QtValue<QPointF>::create( L ) = event->pos();
		if( call( 6, 1 ) && lua_isboolean( L, -1 ) )
			(lua_toboolean( L, -1 ))?event->accept():event->ignore();
		lua_pop( L, 1 ); // return value
	}
}

void CustomWidget::mouseMoveEvent(QMouseEvent *event)
{
	lua_State* L = Engine2::getInst()->getCtx();
	Q_ASSERT( L != 0 );
	StackTester test( L, 0 );
	if( pushMethodAndThis( "mouseMoveEvent") )
	{
		// Push Args: this, modifiers (int), button (int), buttons (int), globalPos (QPoint), pos (QPoint)
		lua_pushinteger( L, event->modifiers() );
		lua_pushinteger( L, event->button() );
		lua_pushinteger( L, event->buttons() );
		*QtValue<QPointF>::create( L ) = event->globalPos();
		*QtValue<QPointF>::create( L ) = event->pos();
		if( call( 6, 1 ) && lua_isboolean( L, -1 ) )
			(lua_toboolean( L, -1 ))?event->accept():event->ignore();
		lua_pop( L, 1 ); // return value
	}
}

void CustomWidget::mousePressEvent(QMouseEvent *event)
{
	lua_State* L = Engine2::getInst()->getCtx();
	Q_ASSERT( L != 0 );
	StackTester test( L, 0 );
	if( pushMethodAndThis( "mousePressEvent") )
	{
		// Push Args: this, modifiers (int), button (int), buttons (int), globalPos (QPoint), pos (QPoint)
		lua_pushinteger( L, event->modifiers() );
		lua_pushinteger( L, event->button() );
		lua_pushinteger( L, event->buttons() );
		*QtValue<QPointF>::create( L ) = event->globalPos();
		*QtValue<QPointF>::create( L ) = event->pos();
		if( call( 6, 1 ) && lua_isboolean( L, -1 ) )
			(lua_toboolean( L, -1 ))?event->accept():event->ignore();
		lua_pop( L, 1 ); // return value
	}
}

void CustomWidget::mouseReleaseEvent(QMouseEvent *event)
{
	lua_State* L = Engine2::getInst()->getCtx();
	Q_ASSERT( L != 0 );
	StackTester test( L, 0 );
	if( pushMethodAndThis( "mouseReleaseEvent") )
	{
		// Push Args: this, modifiers (int), button (int), buttons (int), globalPos (QPoint), pos (QPoint)
		lua_pushinteger( L, event->modifiers() );
		lua_pushinteger( L, event->button() );
		lua_pushinteger( L, event->buttons() );
		*QtValue<QPointF>::create( L ) = event->globalPos();
		*QtValue<QPointF>::create( L ) = event->pos();
		if( call( 6, 1 ) && lua_isboolean( L, -1 ) )
			(lua_toboolean( L, -1 ))?event->accept():event->ignore();
		lua_pop( L, 1 ); // return value
	}
}

void CustomWidget::moveEvent(QMoveEvent *event)
{
	lua_State* L = Engine2::getInst()->getCtx();
	Q_ASSERT( L != 0 );
	StackTester test( L, 0 );
	if( pushMethodAndThis( "moveEvent") )
	{
		// Push Args: this, oldPos (QPoint), pos (QPoint)
		*QtValue<QPointF>::create( L ) = event->oldPos();
		*QtValue<QPointF>::create( L ) = event->pos();
		if( call( 3, 1 ) && lua_isboolean( L, -1 ) )
			(lua_toboolean( L, -1 ))?event->accept():event->ignore();
		lua_pop( L, 1 ); // return value
	}else
		QWidget::moveEvent( event );
}

void CustomWidget::paintEvent(QPaintEvent *event)
{
	lua_State* L = Engine2::getInst()->getCtx();
	Q_ASSERT( L != 0 );
	StackTester test( L, 0 );
	if( pushMethodAndThis( "paintEvent") )
	{
		// Push Args: this, rect (QRect), region (QRegion)
		*QtValue<QRectF>::create( L ) = event->rect();
		*QtValue<QRegion>::create( L ) = event->region();
		if( call( 3, 1 ) && lua_isboolean( L, -1 ) )
			(lua_toboolean( L, -1 ))?event->accept():event->ignore();
		lua_pop( L, 1 ); // return value
	}else
		QWidget::paintEvent( event );
}

void CustomWidget::resizeEvent(QResizeEvent *event)
{
	lua_State* L = Engine2::getInst()->getCtx();
	Q_ASSERT( L != 0 );
	StackTester test( L, 0 );
	if( pushMethodAndThis( "resizeEvent") )
	{
		// Push Args: this, oldSize (QSize), size (QSize)
		*QtValue<QSizeF>::create( L ) = event->oldSize();
		*QtValue<QSizeF>::create( L ) = event->size();
		if( call( 3, 1 ) && lua_isboolean( L, -1 ) )
			(lua_toboolean( L, -1 ))?event->accept():event->ignore();
		lua_pop( L, 1 ); // return value
	}else
		QWidget::resizeEvent( event );
}

void CustomWidget::showEvent(QShowEvent *event)
{
	lua_State* L = Engine2::getInst()->getCtx();
	Q_ASSERT( L != 0 );
	StackTester test( L, 0 );
	if( pushMethodAndThis( "showEvent") )
	{
		// Push Args: this
		if( call( 1, 1 ) && lua_isboolean( L, -1 ) )
			(lua_toboolean( L, -1 ))?event->accept():event->ignore();
		lua_pop( L, 1 ); // return value
	}
}

void CustomWidget::connectNotify(const char *signal)
{
	lua_State* L = Engine2::getInst()->getCtx();
	Q_ASSERT( L != 0 );
	StackTester test( L, 0 );
	if( pushMethodAndThis( "connectNotify") )
	{
		// Push Args: this, signal (string)
		lua_pushstring( L, signal );
		call( 2, 0 );
	}
}

void CustomWidget::disconnectNotify(const char *signal)
{
	lua_State* L = Engine2::getInst()->getCtx();
	Q_ASSERT( L != 0 );
	StackTester test( L, 0 );
	if( pushMethodAndThis( "disconnectNotify") )
	{
		// Push Args: this, signal (string)
		lua_pushstring( L, signal );
		call( 2, 0 );
	}
}

void CustomWidget::timerEvent(QTimerEvent *event)
{
	lua_State* L = Engine2::getInst()->getCtx();
	Q_ASSERT( L != 0 );
	StackTester test( L, 0 );
	if( pushMethodAndThis( "timerEvent") )
	{
		// Push Args: this, timerId (int)
		lua_pushinteger( L, event->timerId() );
		if( call( 2, 1 ) && lua_isboolean( L, -1 ) )
			(lua_toboolean( L, -1 ))?event->accept():event->ignore();
		lua_pop( L, 1 ); // return value
	}
}
