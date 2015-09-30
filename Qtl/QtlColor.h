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

#if !defined(_QTL_COLOR)
#define _QTL_COLOR


typedef struct lua_State lua_State;


namespace Qtl
{

	class Color
	{
	public:
		static int g_saturation(lua_State * L);
		static int g_hue(lua_State * L);
		static int g_yellow(lua_State * L);
		static int g_magenta(lua_State * L);
		static int g_cyan(lua_State * L);
		static int s_alpha(lua_State * L);
		static int g_alpha(lua_State * L);
		static int s_blue(lua_State * L);
		static int g_blue(lua_State * L);
		static int s_green(lua_State * L);
		static int g_green(lua_State * L);
		static int s_string(lua_State * L); 
		static int g_string(lua_State * L); 
		static int s_red(lua_State * L);
		static int g_red(lua_State * L);
		static int g_valid(lua_State * L); 
		static int setCmyk(lua_State * L);
		static int setRgb(lua_State * L);
		static int setHsv(lua_State * L);
		static int __eq(lua_State * L);
		static int init(lua_State * L);
		static void install(lua_State * L);
	};
}

#endif // !defined(_QTL_COLOR)
