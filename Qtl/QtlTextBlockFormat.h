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

#if !defined(_QTL_TEXTBLOCKFORMAT)
#define _QTL_TEXTBLOCKFORMAT

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

//#include <Root/Units.h>

typedef struct lua_State lua_State;




namespace Qtl
{
	class TextBlockFormat
	{
	public:
		static int init(lua_State * L);

		static int alignment (lua_State * L);
		static int bottomMargin (lua_State * L);
		static int indent (lua_State * L);
		static int isValid (lua_State * L);
		static int leftMargin (lua_State * L);
		static int nonBreakableLines (lua_State * L);
		static int pageBreakPolicy (lua_State * L);
		static int rightMargin (lua_State * L);
		static int setAlignment (lua_State * L);
		static int setBottomMargin (lua_State * L);
		static int setIndent (lua_State * L);
		static int setLeftMargin (lua_State * L);
		static int setNonBreakableLines (lua_State * L);
		static int setPageBreakPolicy (lua_State * L);
		static int setRightMargin (lua_State * L);
		static int setTextIndent (lua_State * L);
		static int setTopMargin (lua_State * L);
		static int textIndent (lua_State * L);
		static int topMargin (lua_State * L);

		static void install(lua_State * L);
	};

}

#endif // !defined(_QTL_TEXTBLOCKFORMAT)
