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

#if !defined(_QTL_REGION)
#define _QTL_REGION


typedef struct lua_State lua_State;


namespace Qtl
{
	class MyRegion
	{
	public:
		static int boundingRect(lua_State * L); // () const : QRect 
		static int contains(lua_State * L); // ( const QPoint & ) const : bool 
		// ( const QRect & ) const : bool 
		static int eor(lua_State * L); // ( const QRegion & ) const : QRegion 
		static int intersected(lua_State * L); // ( const QRegion & ) const : QRegion 
		static int intersects(lua_State * L); // ( const QRegion & ) const : bool
		//( const QRect & ) const : bool 
		static int isEmpty(lua_State * L); // () const : bool 
		static int rects(lua_State * L); // () const : QVector<QRect> 
		static int setRects(lua_State * L); // ( const QRect rects, QRect rects ...)
		// table {QRect rects, QRect rects ...}
		static int subtracted(lua_State * L); // ( const QRegion & ) const : QRegion  
		static int translate(lua_State * L); // ( int, int ) 
		// ( const QPoint & ) 
		static int translated(lua_State * L); // ( const QPoint & p ) const : QRegion
		static int united(lua_State * L); // ( const QRegion & ) const : QRegion 
		static int xored(lua_State * L); // ( const QRegion & ) const : QRegion 
		static int intersectedAssign(lua_State * L); // ( const QRegion & r ) const : QRegion
		static int unitedAssign(lua_State * L); //( const QRegion & r ) const : QRegion
		static int subtractedAssign(lua_State * L); //( const QRegion & r ) const : QRegion
		static int xoredAssign(lua_State * L); //( const QRegion & r ) const : QRegion
		static int init(lua_State * L); // ( const QRectF &, int = Rectangle ) 
		static int __add(lua_State * L);
		static int __sub(lua_State * L);
 		static void install(lua_State * L);
	};
}

#endif // !defined(_QTL_REGION)
