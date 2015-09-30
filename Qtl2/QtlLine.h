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

#if !defined(_QTL_LINE)
#define _QTL_LINE


typedef struct lua_State lua_State;


namespace Qtl
{
	class Line
	{
	public:
		static int angle(lua_State * L); // ( const QLineF & ) const : qreal 
		static int dx(lua_State * L); // () const : qreal 
		static int dy(lua_State * L); // () const : qreal 
		static int intersect(lua_State * L); // ( const QLineF &, QPointF * ) const : IntersectType 
		static int isNull(lua_State * L); // () const : bool 
		static int length(lua_State * L); // () const : qreal 
		static int normalVector(lua_State * L); // () const : QLineF 
		static int p1(lua_State * L); // () const : QPointF  
		static int p2(lua_State * L); // () const : QPointF 
		static int pointAt(lua_State * L); // ( qreal ) const : QPointF 
		static int setLength(lua_State * L); // ( qreal ) 
		static int translate(lua_State * L); // ( qreal, qreal ) // ( const QPointF & )
		static int unitVector(lua_State * L); // () const : QLineF 
		static int x1(lua_State * L); // () const : qreal 
		static int x2(lua_State * L); // () const : qreal 
		static int y1(lua_State * L); // () const : qreal 
		static int y2(lua_State * L); // () const : qreal 
		static int setX1(lua_State * L); // () const : qreal 
		static int setX2(lua_State * L); // () const : qreal 
		static int setY1(lua_State * L); // () const : qreal 
		static int setY2(lua_State * L); // () const : qreal 
  		static int init(lua_State * L);
		static void install(lua_State * L);
	};
}

#endif // !defined(_QTL_LINE)
