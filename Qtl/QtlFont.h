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

#if !defined(_QTL_FONT)
#define _QTL_FONT

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

//#include <Root/Units.h>

typedef struct lua_State lua_State;




namespace Qtl
{
	class Font
	{
	public:
		static int g_string(lua_State * L);
		static int s_bold(lua_State * L);
		static int g_bold(lua_State * L);
		static int s_family(lua_State * L);
		static int g_family(lua_State * L);
		static int s_fixedPitch(lua_State * L);
		static int g_fixedPitch(lua_State * L);
		static int s_italic(lua_State * L);
		static int g_italic(lua_State * L);
		static int s_kerning(lua_State * L);
		static int g_kerning(lua_State * L);
		static int s_overline(lua_State * L);
		static int g_overline(lua_State * L);
		static int s_pixelSize(lua_State * L);
		static int g_pixelSize(lua_State * L);
		static int s_pointSize(lua_State * L);
		static int g_pointSize(lua_State * L);
		static int s_rawMode(lua_State * L);
		static int g_rawMode(lua_State * L);
		static int s_rawName(lua_State * L);
		static int g_rawName(lua_State * L);
		static int s_stretch(lua_State * L);
		static int g_stretch(lua_State * L);
		static int s_strikeOut(lua_State * L);
		static int g_strikeOut(lua_State * L);
		static int s_style(lua_State * L);
		static int g_style(lua_State * L);
		static int s_styleHint(lua_State * L);
		static int g_styleHint(lua_State * L);
		static int s_styleStrategy(lua_State * L);
		static int g_styleStrategy(lua_State * L);
		static int s_underline(lua_State * L);
		static int g_underline(lua_State * L);
		static int s_weight(lua_State * L);
		static int g_weight(lua_State * L);
		static int __eq(lua_State * L);
		static int __lt(lua_State * L);
		static int init(lua_State * L);
	    static void install(lua_State * L);


	};

}

#endif // !defined(_QTL_FONT)
