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

#if !defined(_QTL_PIXMAP)
#define _QTL_PIXMAP


typedef struct lua_State lua_State;


namespace Qtl
{
	class Pixmap
	{
	public:
		static int alphaChannel(lua_State * L); // () const : QPixmap
		static int copy(lua_State * L); // ( const QRect & rect = QRect() ) const : QPixmap
		static int setAlphaChannel(lua_State * L); // ( const QPixmap & alpha )

		static int createHeuristicMask(lua_State * L); // ( bool ) const : QBitmap 
		static int createMaskFromColor(lua_State * L); // ( const QColor & ) const : QBitmap 
		// static int defaultDepth(lua_State * L); // () : int 
		// static int defaultOptimization(lua_State * L); // () : Optimization 
		static int depth(lua_State * L); // () const : int 
		static int detach(lua_State * L); // () 
		static int fill(lua_State * L); // ( const QColor & ) or ( const QWidget *, int, int ) 
		static int hasAlpha(lua_State * L); // () const : bool 
		static int hasAlphaChannel(lua_State * L); // () const : bool 
		static int height(lua_State * L); // () const : int 
		static int isNull(lua_State * L); // () const : bool 
		static int isQBitmap(lua_State * L); // () const : bool 
		static int load(lua_State * L); // ( const QString &, const char *, Qt::ImageConversionFlags ) : bool 
		static int loadFromData(lua_State * L); // ( const uchar *, uint, const char *, Qt::ImageConversionFlags ) : bool 
		static int mask(lua_State * L); // () const : const QBitmap * 
		static int rect(lua_State * L); // () const : QRect 
		static int save(lua_State * L); // ( const QString &, const char *, int ) const : bool 
		static int scaled(lua_State * L); // ( int, int, Qt::AspectRatioMode, Qt::TransformationMode ) const : QPixmap 
		static int scaledToHeight(lua_State * L); // ( int, Qt::TransformationMode mode = Qt::FastTransformation  ) const : QPixmap 
		static int scaledToWidth(lua_State * L); // ( int, Qt::TransformationMode mode = Qt::FastTransformation  ) const : QPixmap 
		static int serialNumber(lua_State * L); // () const : int 
		static int setMask(lua_State * L); // ( const QBitmap & ) 
		static int size(lua_State * L); // () const : QSize 
		static int toImage(lua_State * L); // () const : QImage 
		static int transformed(lua_State * L); // ( const QMatrix &, Qt::TransformationMode ) const : QPixmap 
		static int width(lua_State * L); // () const : int 
		static int init(lua_State * L); 
 		static void install(lua_State * L);
 	};
}

#endif // !defined(_QTL_PIXMAP)
