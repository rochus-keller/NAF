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

#ifndef _LuaQt3_LuaPainter2
#define _LuaQt3_LuaPainter2

#include <Script2/QtValue.h>
#include <QPainter>

namespace Lua
{
	class LuaPainter2
	{
	public:
		LuaPainter2();
		LuaPainter2( const LuaPainter2& ) {}
		LuaPainter2& operator=( const LuaPainter2& ) { return *this; }

		static const luaL_reg methods[];
		static void install( lua_State *L );

		static int setZoomFactor(lua_State *L);
		static int drawSlice(lua_State *L);
		static int drawIcon(lua_State *L);
		static int drawContour(lua_State *L);
		static int drawImage(lua_State *L);
		static int setBrush(lua_State *L);
		static int setPen(lua_State *L);
		static int getBounding(lua_State *L);
		static int setFont(lua_State *L);
		static int drawText(lua_State *L);
		static int fillRect(lua_State *L);
		static int drawRect(lua_State *L);
		static int drawLine(lua_State *L);
		static int drawPoint(lua_State *L);
		static int lineTo(lua_State *L);
		static int moveTo(lua_State *L);
		static int drawEllipse(lua_State *L);

		QPainter d_p;
		QPoint d_pt;
	};
}

#endif //_LuaQt3_LuaPainter2
