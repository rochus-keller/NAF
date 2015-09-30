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

#if !defined(_QTL_FONTINFO)
#define _QTL_FONTINFO

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

//#include <Root/Units.h>

typedef struct lua_State lua_State;




namespace Qtl
{
	class FontInfo
	{
	public:
		static int bold(lua_State * L); // const : bool
		static int exactMatch(lua_State * L); // const : bool 
		static int family(lua_State * L); // const : QString 
		static int fixedPitch(lua_State * L); // const : bool
		static int italic(lua_State * L); // const : bool
		static int pixelSize(lua_State * L); // const : int
		static int pointSize(lua_State * L); // const : int
		static int pointSizeF(lua_State * L); // const : qreal
		static int rawMode(lua_State * L); // const : bool 
		static int style(lua_State * L); // const : QFont::Style
		static int styleHint(lua_State * L); // const : QFont::StyleHint
		static int weight(lua_State * L); // const : int
		static int init(lua_State * L);
		static void install(lua_State * L);


	};

}

#endif // !defined(_QTL_FONTINFO)
