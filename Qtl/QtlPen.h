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

#if !defined(_QTL_PEN)
#define _QTL_PEN


typedef struct lua_State lua_State;


namespace Qtl
{
	class Pen
	{
	public:
		static int brush(lua_State * L); // () const : QBrush 
		static int capStyle(lua_State * L); // () const : Qt::PenCapStyle 
		static int color(lua_State * L); // () const : QColor 
		static int isSolid(lua_State * L); // () const : bool 
		static int joinStyle(lua_State * L); // () const : Qt::PenJoinStyle 
		static int setBrush(lua_State * L); // ( const QBrush & ) 
		static int setCapStyle(lua_State * L); // ( Qt::PenCapStyle )  
		static int setColor(lua_State * L); // ( const QColor & ) 
		static int setJoinStyle(lua_State * L); // ( Qt::PenJoinStyle ) 
		static int setStyle(lua_State * L); // ( Qt::PenStyle ) 
		static int setWidthF(lua_State * L); // ( qreal ) 
		static int style(lua_State * L); // () const : Qt::PenStyle 
		static int widthF(lua_State * L); // () const : qreal 
   		static int init(lua_State * L); 
		static int __eq(lua_State * L);
		static void install(lua_State * L);
	};
}

#endif // !defined(_QTL_PEN)
