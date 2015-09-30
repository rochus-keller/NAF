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

#if !defined(_QTL_TEXTOPTION)
#define _QTL_TEXTOPTION


typedef struct lua_State lua_State;


namespace Qtl
{
class TextOption
	{
	public:
		static int alignment(lua_State * L); // const Qt::Alignment 
		static int flags (lua_State * L); //const Flags
		static int setAlignment (lua_State * L); //( Qt::Alignment alignment )void
		static int setFlags (lua_State * L); // ( Flags flags )void
		static int setTabStop(lua_State * L); // ( qreal tabStop )void
		static int setUseDesignMetrics(lua_State * L);//  ( bool enable )void
		static int tabStop(lua_State * L); //  const qreal
		static int setTextDirection(lua_State * L); // ( Qt::LayoutDirection direction ) void
		static int useDesignMetrics(lua_State * L); //const bool
		static int operatorEq(lua_State * L); //( const QTextOption & other )QTextOption
		static int textDirection(lua_State * L); //const Qt::LayoutDirection  
		static int setWrapMode(lua_State * L);//( WrapMode mode ) void

		static int wrapMode(lua_State * L); //const
		static int init(lua_State * L);
		static void install(lua_State * L);
	};
}

#endif // !defined(_QTL_TEXTOPTION)
