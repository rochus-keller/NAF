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

#ifndef _LuaQt3_LuaWidget2
#define _LuaQt3_LuaWidget2

#include <Script2/QtObject.h>
#include <QWidget>

namespace Lua
{
	class LuaWidget2
	{
	public: 
		enum Event 
		{
			MIN_EVENT,
			Clicked,// PushButton, ButtonGroup
			Toggled,	// Button
			Changed,	// LineEdit, MLE
			Return,		// LineEdit, MLE
			Activated,	// MenuBar, PopupMenu
			Showing,	// PopupMenu
			Scrolled,	// ScrollView
			RightPressed,
			Selection,
			DblClicked,
			Expanded,
			Collapsed,
			MousePressed,
			MouseReleased,
			MouseMoved,
			KeyPressed,
			KeyReleased,
			FocusIn,
			FocusOut,
			Resized,
			Closing,
			Hiding,
			Paint,
			SizeHint,
			MAX_EVENT,
		};
		static void install( lua_State *L );

		static int getData(lua_State * L);
		static int destroy(lua_State *L);
		static int getSize(lua_State *L);
		static int resize(lua_State *L);
		static int move(lua_State *L);
		// static int lower(lua_State *L); -> slot
		// static int raise(lua_State *L); -> slot
		// static int showNormal(lua_State *L); -> slot
		// static int showFullScreen(lua_State *L); -> slot
		// static int showMinimized(lua_State *L); -> slot
		// static int showMaximized(lua_State *L); -> slot
		// static int update(lua_State *L); -> QWidget
		static int updateAll(lua_State *L);
		static int setUpdatesEnabled(lua_State *L);
		// static int clearFocus(lua_State *L); -> QWidget
		// static int setFocus(lua_State *L); -> QWidget
		static int setEnabled(lua_State *L);
		static int getParentWidget(lua_State *L);
		// static int updateGeometry(lua_State *L); -> QWidget
		static int isMaximized(lua_State *L);
		static int isMinimized(lua_State *L);
		// static int isHidden(lua_State *L); -> QWidget
		static int isVisible(lua_State *L);
		static int close(lua_State *L);
		static int hasFocus(lua_State *L);
		static int isFocusEnabled(lua_State *L);
		static int setActiveWindow(lua_State *L);
		static int isActiveWindow(lua_State *L);
		static int getCaption(lua_State *L);
		static int setFont(lua_State *L);
		static int getFont(lua_State *L);
		static int getFgColor(lua_State *L);
		static int getBgColor(lua_State *L);
		static int setBgColor(lua_State *L);
		// static int hide(lua_State *L); -> slot
		static int show(lua_State *L);
		static int getTopLevelWidget(lua_State *L);
		// static int mapFromParent(lua_State *L); -> QWidget
		// static int mapToParent(lua_State *L); -> QWidget
		// static int mapFromGlobal(lua_State *L); -> QWidget
		// static int mapToGlobal(lua_State *L); -> QWidget
		static int setCaption(lua_State *L);
		// static int setFixedSize(lua_State *L); -> QWidget
		static int setMaximumSize(lua_State *L);
		static int setMinimumSize(lua_State *L);
		static int getMaximumSize(lua_State *L);
		static int getMinimumSize(lua_State *L);
		static int getGeometry(lua_State *L);
		static int getFrameGeometry(lua_State *L);
		static int isEnabled(lua_State *L);
		static int isDesktop(lua_State *L);
		static int isPopup(lua_State *L);
		static int isModal(lua_State *L);
		static int isTopLevel(lua_State *L);
	};

    class LuaWidgetCallbacks : public QObject
    {
        Q_OBJECT
    public:
        static LuaWidgetCallbacks* inst();
        static int pushSlotTable(lua_State *L, LuaWidgetCallbacks*);
        static int setCallback(lua_State *L);
        static bool pushCallbackAndMe( QWidget *, LuaWidget2::Event callbackId);
        static bool call(int nargs, int nresults );
        static lua_State * getLua();
    protected slots:
        void onDeleted( QObject* );
    protected:
        LuaWidgetCallbacks(){}
    private:
        QHash<QObject*,QHash<int,int> > d_hash; // obj -> callbackId -> ref
    };

}

#endif //_LuaQt3_LuaWidget2
