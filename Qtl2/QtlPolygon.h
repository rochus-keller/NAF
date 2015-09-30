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

#if !defined(_QTL_POLYGON)
#define _QTL_POLYGON


typedef struct lua_State lua_State;


namespace Qtl
{
	class Polygon
	{
	public:
		// TODO: erweitern
		static int append(lua_State * L); //  ( const T & )
		static int prepend(lua_State * L); // ( const T & ) 
		static int clear(lua_State * L); // () 
		static int count(lua_State * L); // () const : int 
		static int remove(lua_State * L); // ( int ) 
		static int point(lua_State * L); // ( int index ) const : QPoint
		static int setPoint(lua_State * L); // ( int i, const QPoint & p ) 
		static int translate(lua_State * L); // ( int dx, int dy )
		static int boundingRect(lua_State * L); // () const : QRectF
		static int isClosed(lua_State * L); // () const : bool
 		static int init(lua_State * L); 
		static void install(lua_State * L);
	};
}

#endif // !defined(_QTL_POLYGON)
