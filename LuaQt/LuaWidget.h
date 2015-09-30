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

#if !defined(AFX_LUAWIDGET_H__16129B8F_8A62_4908_986A_485373FC65AE__INCLUDED_)
#define AFX_LUAWIDGET_H__16129B8F_8A62_4908_986A_485373FC65AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qwidget.h> 
#include <Script/LuaEngine.h>
#include <Script/PeerHelper.h>
#include <map>
#include <set>

namespace Spec
{
	// LuaWidgetPeer wird von Lua alloziiert und dealloziiert. Es wird dem
	// übergebenen Object als Subobject namens "__data" eingefügt.

	class LuaWidgetPeer : public QObject
	{
		int d_data;			// Referenz auf DataObject, initial LUA_NOREF
		int d_this;			// Referenz auf dieses Userdata-Object
		typedef std::map<int,int> Cb;
		Cb d_cb;
		QObject* d_obj;
	public:
		typedef int (*Dispatcher)(lua_State *L);

		static int pushPeer( lua_State*, QWidget* );
		static LuaWidgetPeer* findPeer( QWidget* );

		int setCallback(lua_State*);
		int getData(lua_State*);
		int destroy(lua_State *L);
		int pushPeer( lua_State* );

		LuaWidgetPeer(QObject*, lua_State* = 0); // State ist nun überflüssig, da Singleton.
		virtual ~LuaWidgetPeer();
		void* operator new( size_t );
		void* operator new( size_t, void* );
		void operator delete( void*, size_t );

		QObject* _obj() const { return d_obj; }
		int checkAlive(lua_State *L);
		bool pushCallback(int);
		lua_State* getLua() const;

		typedef std::set<std::string> Children;
		static PeerHelper<LuaWidgetPeer>::MethodSlot s_methods[];
		static const char s_className[];
		static Children s_children;
		static int installParent( lua_State* L, int m, Dispatcher, const char* );
	protected:
		void call(  int nargs, int nresults );
		void unthis();
	};

// RISK: folgende Zeilen müssen als Unix-File gespeichert werden, sonst GCC-Fehler
#define DECLARE_WIDGET( _class, _host )  \
static const char s_className[]; \
static PeerHelper<_class>::MethodSlot s_methods[]; \
static Children s_children; \
_host* obj() const { _host* h = dynamic_cast<_host*>( _obj() ); assert( h ); return h; } \
static int install(lua_State * L); \
static int installParent( lua_State* L, int m, Dispatcher, const char* );

	class LuaWidget : public LuaWidgetPeer
	{
	public:
		int getSize(lua_State *L);
		int resize(lua_State *L);
		int move(lua_State *L);
		int lower(lua_State *L);
		int raise(lua_State *L);
		int showNormal(lua_State *L);
		int showFullScreen(lua_State *L);
		int showMinimized(lua_State *L);
		int showMaximized(lua_State *L);
		int update(lua_State *L);
		int updateAll(lua_State *L);
		int setUpdatesEnabled(lua_State *L);
		int clearFocus(lua_State *L);
		int setFocus(lua_State *L);
		int setEnabled(lua_State *L);
		int getParentWidget(lua_State *L);
		int updateGeometry(lua_State *L);
		int isMaximized(lua_State *L);
		int isMinimized(lua_State *L);
		int isHidden(lua_State *L);
		int isVisible(lua_State *L);
		int close(lua_State *L);
		int hasFocus(lua_State *L);
		int isFocusEnabled(lua_State *L);
		int setActiveWindow(lua_State *L);
		int isActiveWindow(lua_State *L);
		int getCaption(lua_State *L);
		int setFont(lua_State *L);
		int getFont(lua_State *L);
		int getFgColor(lua_State *L);
		int getBgColor(lua_State *L);
		int setBgColor(lua_State *L);
		int hide(lua_State *L);
		int show(lua_State *L);
		int getTopLevelWidget(lua_State *L);
		int mapFromParent(lua_State *L);
		int mapToParent(lua_State *L);
		int mapFromGlobal(lua_State *L);
		int mapToGlobal(lua_State *L);
		int setCaption(lua_State *L);
		int setFixedSize(lua_State *L);
		int setMaximumSize(lua_State *L);
		int setMinimumSize(lua_State *L);
		int getMaximumSize(lua_State *L);
		int getMinimumSize(lua_State *L);
		int getGeometry(lua_State *L);
		int getFrameGeometry(lua_State *L);
		int isEnabled(lua_State *L);
		int isDesktop(lua_State *L);
		int isPopup(lua_State *L);
		int isModal(lua_State *L);
		int isTopLevel(lua_State *L);

		LuaWidget( QWidget*, lua_State* );

		DECLARE_WIDGET( LuaWidget, QWidget )
	};

#define IMPLEMENT_WIDGET( _class, _parent, _name ) \
const char _class::s_className[] = _name; \
_class::Children _class::s_children; \
int _class::install(lua_State *L) \
{ \
	lua_newtable(L); \
	int s_methods = lua_gettop(L); \
	_parent::installParent( L, s_methods, &PeerHelper<_class>::dispatch, s_className ); \
	PeerHelper<_class>::installClass( L, s_methods, false ); \
	lua_pop(L, 1);   \
	return 1; \
} \
int _class::installParent(lua_State *L, int m, Dispatcher dp, const char* child ) \
{ \
	_parent::installParent( L, m, dp, child ); \
	s_children.insert( child ); \
	PeerHelper<_class>::installMethods( L, m, dp ); \
	return 1; \
}\
PeerHelper<_class>::MethodSlot _class::s_methods[] = 

#define IMPLEMENT_CREATE( _class, _host ) \
QWidget* parent = 0; \
if( lua_gettop(L) > 0 ) \
	parent = PeerHelper<LuaWidget>::checkSameOrSubClass( L, 1 )->obj(); \
	_host* h = new _host( parent ); \
	if( parent == 0 ) h->setAttribute( Qt::WA_DeleteOnClose ); \
PeerHelper<_class>::instance2( L, h ); \
return 1;

#define IMPLEMENT_CREATE2( _class, _host ) \
QWidget* parent = PeerHelper<LuaWidget>::checkSameOrSubClass( L, 1 )->obj(); \
PeerHelper<_class>::instance2( L, new _host( parent ) ); \
return 1;

}

#endif // !defined(AFX_LUAWIDGET_H__16129B8F_8A62_4908_986A_485373FC65AE__INCLUDED_)
