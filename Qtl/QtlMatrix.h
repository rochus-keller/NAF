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

#if !defined(_QTL_MATRIX)
#define _QTL_MATRIX


typedef struct lua_State lua_State;


namespace Qtl
{
class Matrix
	{
	public:			
		static int det(lua_State * L); // () const : qreal 
		static int dx(lua_State * L); // () const : qreal 
		static int dy(lua_State * L); // () const : qreal 
		static int inverted(lua_State * L); // ( bool * ) const : QMatrix 
		static int isIdentity(lua_State * L); // () const : bool 
		static int isInvertible(lua_State * L); // () const : bool 
		static int m11(lua_State * L); // () const : qreal 
		static int m12(lua_State * L); // () const : qreal 
		static int m21(lua_State * L); // () const : qreal 
		static int m22(lua_State * L); // () const : qreal 
		/*
		map ( qreal, qreal, qreal *, qreal * ) const 
		map ( const QPainterPath & ) const : QPainterPath 
		map ( const QPointF & ) const : QPointF 
		map ( const QLineF & ) const : QLineF  
		map ( const QPolygonF & ) const : QPolygonF 
		*/
		static int map(lua_State * L); // ( const QRegion & ) const : QRegion 
		static int mapRect(lua_State * L); // ( const QRectF & ) const : QRectF 
		static int mapToPolygon(lua_State * L); // ( const QRect & ) const : QPolygon 
		static int reset(lua_State * L); // () 
		static int rotate(lua_State * L); // ( qreal ) : QMatrix & 
		static int scale(lua_State * L); // ( qreal, qreal ) : QMatrix & 
		static int setMatrix(lua_State * L); // ( qreal, qreal, qreal, qreal, qreal, qreal ) 
		static int shear(lua_State * L); // ( qreal, qreal ) : QMatrix & 
		static int translate(lua_State * L); // ( qreal, qreal ) : QMatrix & 
  		static int notEquals(lua_State * L); // ( const QMatrix & matrix ) const bool
		static int multiply(lua_State * L); // ( const QMatrix & matrix ) const QMatrix
		static int __eq(lua_State * L);
		static int __mul(lua_State * L);	
		static int init(lua_State * L);
		static void install(lua_State * L);
	};
}

#endif // !defined(_QTL_MATRIX)
