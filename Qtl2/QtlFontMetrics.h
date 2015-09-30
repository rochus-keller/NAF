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

#if !defined(_QTL_FONTMETRICS)
#define _QTL_FONTMETRICS


typedef struct lua_State lua_State;


namespace Qtl
{
	class FontMetrics
	{
	public:
		static int ascent(lua_State * L); // const : qreal
		static int averageCharWidth(lua_State * L); // const : qreal
		static int boundingRect(lua_State * L); // ( const QString & text ) const : QRectF
		// ( const QRectF & rect, int flags, const QString & text, int tabStops = 0, int * tabArray = 0 ) const : QRectF
		static int descent(lua_State * L); // const : qreal
		static int elidedText(lua_State * L); // ( const QString & text, Qt::TextElideMode mode, qreal width, int flags = 0 ) const : QString
		static int height(lua_State * L); // const : qreal
		static int inFont(lua_State * L); // ( QChar ch ) const : bool
		static int leading(lua_State * L); // const : qreal
		static int leftBearing(lua_State * L); // ( QChar ch ) const : qreal 
		static int lineSpacing(lua_State * L); // const : qreal 
		static int lineWidth(lua_State * L); // const : qreal
		static int maxWidth(lua_State * L); // const : qreal
		static int minLeftBearing(lua_State * L); // const : qreal 
		static int minRightBearing(lua_State * L); // const : qreal 
		static int overlinePos(lua_State * L); // const : qreal 
		static int rightBearing(lua_State * L); // ( QChar ch ) const : qreal 
		static int size(lua_State * L); // ( int flags, const QString & text, int tabStops = 0, int * tabArray = 0 ) const : QSizeF
		static int strikeOutPos(lua_State * L); // const : qreal 
		static int underlinePos(lua_State * L); // const : qreal 
		static int width(lua_State * L); // ( const QString & text ) const : qreal 
		static int xHeight(lua_State * L); // const : qreal 
		static int init(lua_State * L); // ( const QFont & font )
		// ( const QFontMetrics & fontMetrics )
		// ( const QFont & font, QPaintDevice * paintdevice )
		static void install(lua_State * L);
	};
    class MyFontMetrics : public QFontMetricsF {
     public:
         MyFontMetrics():QFontMetricsF(QFont()) {}
    };


}

#endif // !defined(_QTL_FONTMETRICS)
