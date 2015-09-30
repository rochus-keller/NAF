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

#if !defined(AFX_LUAPAINTER_H__45EE2CB7_A9D8_4970_A456_2C3EA5CA50BA__INCLUDED_)
#define AFX_LUAPAINTER_H__45EE2CB7_A9D8_4970_A456_2C3EA5CA50BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Script/LuaEngine.h>
#include <Script/PeerHelper.h>
#include <LuaQt/LuaPeer.h>
#include <qpainter.h>

namespace Spec
{
	class LuaPainter : public LuaPeer
	{
	public:
		int setZoomFactor(lua_State *L);
		int drawSlice(lua_State *L);
		int drawIcon(lua_State *L);
		int drawContour(lua_State *L);
		int drawImage(lua_State *L);
		int setBrush(lua_State *L);
		int setPen(lua_State *L);
		int getBounding(lua_State *L);
		int setFont(lua_State *L);
		int drawText(lua_State *L);
		int fillRect(lua_State *L);
		int drawRect(lua_State *L);
		int drawLine(lua_State *L);
		int drawPoint(lua_State *L);
		int lineTo(lua_State *L);
		int moveTo(lua_State *L);
		int drawEllipse(lua_State *L);

		LuaPainter();
		virtual ~LuaPainter();

		static const char s_className[];
		static PeerHelper<LuaPainter>::MethodSlot s_methods[];

		QPainter d_p;
		QPoint d_pt;
	};
}

#endif // !defined(AFX_LUAPAINTER_H__45EE2CB7_A9D8_4970_A456_2C3EA5CA50BA__INCLUDED_)
