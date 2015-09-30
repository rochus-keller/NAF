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

#if !defined(_QTL_IMAGE)
#define _QTL_IMAGE


typedef struct lua_State lua_State;


namespace Qtl
{
	class Image
	{
	public:
		static int allGray(lua_State * L); // () const : bool 
		static int alphaChannel(lua_State * L); // () const : QImage 
		// static int bits () : uchar * 
		// static int bits () const : const uchar * 
		static int bytesPerLine(lua_State * L); // () const : int 
		static int color(lua_State * L); // ( int ) const : QRgb 
		static int colorTable(lua_State * L); // () const : QVector<QRgb> 
		static int convertToFormat(lua_State * L); // ( Format, Qt::ImageConversionFlags ) const : QImage und ( Format, const QVector<QRgb> &, Qt::ImageConversionFlags ) const : QImage 
		static int copy(lua_State * L); // ( const QRect & ) const : QImage und ( int, int, int, int ) const : QImage 
		static int createAlphaMask(lua_State * L); // ( Qt::ImageConversionFlags ) const : QImage 
		static int createHeuristicMask(lua_State * L); // ( bool ) const : QImage 
		static int depth(lua_State * L); // () const : int 
		static int detach(lua_State * L); // () 
		static int dotsPerMeterX(lua_State * L); // () const : int 
		static int dotsPerMeterY(lua_State * L); // () const : int 
		static int fill(lua_State * L); // ( uint ) 
		static int format(lua_State * L); // () const : Format 
//		static int fromData(lua_State * L); // ( const uchar *, int, const char * ) : QImage und ( const QByteArray &, const char * ) : QImage 
		static int hasAlphaChannel(lua_State * L); // () const : bool 
		static int height(lua_State * L); // () const : int 
		static int heightMM(lua_State * L); // () const : int 
		static int invertPixels(lua_State * L); // ( InvertMode ) 
		static int isDetached(lua_State * L); // () const : bool 
		static int isGrayscale(lua_State * L); // () const : bool 
		static int isNull(lua_State * L); // () const : bool 
		static int load(lua_State * L); // ( const QString &, const char * ) : bool 
		static int loadFromData(lua_State * L); // ( const QByteArray &, const char * ) : bool und ( const uchar *, int, const char * ) : bool 
		static int logicalDpiX(lua_State * L); // () const : int 
		static int logicalDpiY(lua_State * L); // () const : int  
//		static int metric(lua_State * L); // ( PaintDeviceMetric ) const : int 
		static int mirrored(lua_State * L); // ( bool, bool ) const : QImage 
		static int numBytes(lua_State * L); // () const : int 
		static int numColors(lua_State * L); // () const : int 
		static int offset(lua_State * L); // () const : QPoint 
		static int paintEngine(lua_State * L); // () const : QPaintEngine * 
		static int painters(lua_State * L); // : ushort 
		static int paintingActive(lua_State * L); // () const : bool 
		static int pixel(lua_State * L); // ( int, int ) const : QRgb 
		static int pixelIndex(lua_State * L); // ( int, int ) const : int 
		static int rect(lua_State * L); // () const : QRect 
		static int rgbSwapped(lua_State * L); // () const : QImage 
		static int save(lua_State * L); // ( QIODevice *, const char *, int ) const : bool und ( const QString &, const char *, int ) const : bool 
		static int scaled(lua_State * L); // ( const QSize &, Qt::AspectRatioMode, Qt::TransformationMode ) const : QImage und ( int, int, Qt::AspectRatioMode, Qt::TransformationMode ) const : QImage 
		static int scaledToHeight(lua_State * L); // ( int, Qt::TransformationMode ) const : QImage 
		static int scaledToWidth(lua_State * L); // ( int, Qt::TransformationMode ) const : QImage 
		// static int scanLine ( int ) : uchar * 
		// static int scanLine ( int ) const : const uchar * 
		static int serialNumber(lua_State * L); // () const : int 
		static int setAlphaChannel(lua_State * L); // ( const QImage & ) 
		static int setColor(lua_State * L); // ( int, QRgb ) 
		static int setColorTable(lua_State * L); // ( const QVector<QRgb> ) 
		static int setDotsPerMeterX(lua_State * L); // ( int ) 
		static int setDotsPerMeterY(lua_State * L); // ( int ) 
		static int setNumColors(lua_State * L); // ( int ) 
		static int setOffset(lua_State * L); // ( const QPoint & ) 
		static int setPixel(lua_State * L); // ( int, int, uint ) 
		// static int setText(lua_State * L); // ( const char *, const char *, const QString & ) 
		static int size(lua_State * L); // () const : QSize 
		// static int text(lua_State * L); // ( const QImageTextKeyLang & ) const : QString und ( const char *, const char * ) const : QString 
		// static int textKeys(lua_State * L); // () const : QStringList 
		// static int textLanguages(lua_State * L); // () const : QStringList 
		// static int textList(lua_State * L); // () const : QList<QImageTextKeyLang> 
		static int transformed(lua_State * L); // ( const QMatrix &, Qt::TransformationMode ) const : QImage 
		static int trueMatrix(lua_State * L); // ( const QMatrix &, int, int ) : QMatrix 
		static int valid(lua_State * L); // ( int, int ) const : bool 
		static int width(lua_State * L); // () const : int 
		static int widthMM(lua_State * L); // () const : int 
  		static int init(lua_State * L); 
		static int __eq(lua_State * L);
		static void install(lua_State * L);
	};
}

#endif // !defined(_QTL_IMAGE)
