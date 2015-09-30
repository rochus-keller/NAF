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

#if !defined(_QTL_RECT)
#define _QTL_RECT


typedef struct lua_State lua_State;


namespace Qtl
{
	class Rect
	{
	public:
		static int adjust(lua_State * L); // ( qreal, qreal, qreal, qreal ) 
		static int adjusted(lua_State * L); // ( qreal, qreal, qreal, qreal ) const : QRectF 
		static int bottom(lua_State * L); // () const : qreal 
		static int bottomLeft(lua_State * L); // () const : QPointF 
		static int bottomRight(lua_State * L); // () const : QPointF 
		static int center(lua_State * L); // () const : QPointF 
		static int contains(lua_State * L); // ( qreal, qreal ) const : bool 
		// ( const QRectF & ) const : bool  // ( const QPointF & ) const : bool 
		static int getCoords(lua_State *L); // ( qreal * x1, qreal * y1, qreal * x2, qreal * y2 )  
		static int getRect(lua_State *L); // ( qreal * x, qreal * y, qreal * width, qreal * height )
		static int height(lua_State * L); // () const : qreal 
		static int intersected(lua_State * L); // ( const QRectF & ) const : QRectF 
		static int intersects(lua_State * L); // ( const QRectF & ) const : bool 
		static int isEmpty(lua_State * L); // () const : bool 
		static int isNull(lua_State * L); // () const : bool 
		static int isValid(lua_State * L); // () const : bool 
		static int left(lua_State * L); // () const : qreal 
		static int moveBottom(lua_State * L); // ( qreal ) 
		static int moveBottomLeft(lua_State * L); // ( const QPointF & ) 
		static int moveBottomRight(lua_State * L); // ( const QPointF & ) 
		static int moveCenter(lua_State * L); // ( const QPointF & ) 
		static int moveLeft(lua_State * L); // ( qreal ) 
		static int moveRight(lua_State * L); // ( qreal ) 
		static int moveTo(lua_State * L); // ( qreal, qreal ) ( const QPointF & ) 
		// ( const QPointF & ) 
		static int moveTop(lua_State * L); // ( qreal ) 
		static int moveTopLeft(lua_State * L); // ( const QPointF & ) 
		static int moveTopRight(lua_State * L); // ( const QPointF & ) 
		static int normalized(lua_State * L); // () const : QRectF  
		static int right(lua_State * L); // () const : qreal 
		static int setBottom(lua_State * L); // ( qreal ) 
		static int setBottomLeft(lua_State * L); // ( const QPointF & ) 
		static int setBottomRight(lua_State * L); // ( const QPointF & ) 
		static int setCoords(lua_State * L); // ( qreal, qreal, qreal, qreal ) 
		static int setHeight(lua_State * L); // ( qreal ) 
		static int setLeft(lua_State * L); // ( qreal ) 
		static int setRect(lua_State * L); // ( qreal, qreal, qreal, qreal ) 
		static int setRight(lua_State * L); // ( qreal ) 
		static int setSize(lua_State * L); // ( const QSizeF & ) 
		static int setTop(lua_State * L); // ( qreal ) 
		static int setTopLeft(lua_State * L); // ( const QPointF & ) 
		static int setTopRight(lua_State * L); // ( const QPointF & ) 
		static int setWidth(lua_State * L); // ( qreal ) 
		static int setX(lua_State * L); // ( qreal ) 
		static int setY(lua_State * L); // ( qreal ) 
		static int size(lua_State * L); // () const : QSizeF 
		static int top(lua_State * L); // () const : qreal 
		static int topLeft(lua_State * L); // () const : QPointF 
		static int topRight(lua_State * L); // () const : QPointF 
		static int translate(lua_State * L); // ( qreal, qreal ) 
		// ( const QPointF & ) 
		static int translated(lua_State * L); // ( qreal, qreal ) const : QRectF 
		// ( const QPointF & ) const : QRectF 
		static int unite(lua_State * L); // ( const QRectF & ) const : QRectF 
		static int width(lua_State * L); // () const : qreal 
		static int x(lua_State * L); // () const : qreal 
		static int y(lua_State * L); // () const : qreal 
		static int intersectedAssign(lua_State * L); // ( const QRectF & ) const : QRectF 
		static int unitedAssign(lua_State * L); // ( const QRectF & ) const : QRectF 
		static int init(lua_State * L);
		static void install(lua_State * L);
	};

}

#endif // !defined(_QTL_RECT)
