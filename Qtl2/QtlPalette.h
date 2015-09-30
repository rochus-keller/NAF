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

#if !defined(_QTL_PALETTE)
#define _QTL_PALETTE


typedef struct lua_State lua_State;


namespace Qtl
{
	class Palette
	{
	public:  		
		static int alternateBase(lua_State * L); // const : QBrush
		static int base(lua_State * L); // const : QBrush
		static int brightText(lua_State * L); // const : QBrush
		static int button(lua_State * L); // const : QBrush
		static int buttonText(lua_State * L); // const : QBrush
		static int dark(lua_State * L); // const : QBrush
		static int highlight(lua_State * L); // const : QBrush
		static int highlightedText(lua_State * L); // const : QBrush
		static int isCopyOf(lua_State * L); // ( const QPalette & p ) const : bool
		static int light(lua_State * L); // const : QBrush
		static int link(lua_State * L); // const : QBrush
		static int linkVisited(lua_State * L); // const : QBrush
		static int mid(lua_State * L); // const : QBrush
		static int midlight(lua_State * L); // const : QBrush
		static int resolve(lua_State * L); // ( const QPalette & other ) const : QPalette
		static int shadow(lua_State * L); // const : QBrush
		static int text(lua_State * L); // const : QBrush
		static int window(lua_State * L); // const : QBrush
		static int windowText(lua_State * L); // const : QBrush
		static int init(lua_State * L);
		static void install(lua_State * L);
	};
}

#endif // !defined(_QTL_PALETTE)
