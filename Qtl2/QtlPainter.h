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

#if !defined(_QTL_PAINTER)
#define _QTL_PAINTER

typedef struct lua_State lua_State;




namespace Qtl
{
	class Painter
	{
	public:		
		static int background(lua_State * L); // const : QBrush & 
		static int backgroundMode(lua_State * L); // const : Qt::BGMode 
		static int begin(lua_State * L); // ( QPaintDevice * device ) : bool 
		static int boundingRect(lua_State * L); // ( const QRect & rectangle, int flags, const QString & text ) : QRect 
		// ( int x, int y, int w, int h, int flags, const QString & text )
		// ( const QRectF & rectangle, const QString & text, const QTextOption & option = QTextOption() )
		static int brush(lua_State * L); // const : QBrush & 
		static int brushOrigin(lua_State * L); // const : QPoint 
		static int clipPath(lua_State * L); // const : QPainterPath 
		static int clipRegion(lua_State * L); // const : QRegion 
		static int combinedMatrix(lua_State * L); // const : QMatrix 
		static int compositionMode(lua_State * L); // const : CompositionMode 
		static int deviceMatrix(lua_State * L); // const : QMatrix & 
		static int drawArc(lua_State * L); // ( const QRect & rectangle, int startAngle, int spanAngle )
		// ( int x, int y, int width, int height, int startAngle, int spanAngle )
		static int drawChord(lua_State * L); // ( const QRect & rectangle, int startAngle, int spanAngle )
		// ( int x, int y, int width, int height, int startAngle, int spanAngle )
		static int drawConvexPolygon(lua_State * L); // ( const QPointF * points, int pointCount )
		// ( const QPolygonF & polygon )
		static int drawEllipse(lua_State * L); // ( const QRect & rectangle )
		// ( int x, int y, int width, int height )
		static int drawImage(lua_State * L); // ( const QRect & target, const QImage & image, const QRect & source, Qt::ImageConversionFlags flags = Qt::AutoColor )
		// ( const QPoint & point, const QImage & image )
// ( const QPoint & point, const QImage & image, const QRect & source, Qt::ImageConversionFlags flags = Qt::AutoColor )
// ( const QRect & rectangle, const QImage & image )
// ( int x, int y, const QImage & image, int sx = 0, int sy = 0, int sw = -1, int sh = -1, Qt::ImageConversionFlags flags = Qt::AutoColor )
		static int drawLine(lua_State * L); // ( const QLine & line )
		// ( const QPoint & p1, const QPoint & p2 )
		// ( int x1, int y1, int x2, int y2 )
		static int drawLines(lua_State * L); // ( const QLine * lines, int lineCount )
		// ( const QPoint * pointPairs, int lineCount )
		// ( const QVector<QPoint> & pointPairs )
		// ( const QVector<QLine> & lines )
		static int drawPath(lua_State * L); // ( const QPainterPath & path )
		// static int drawPicture(lua_State * L); // ( int x, int y, const QPicture & picture )
		// ( const QPoint & point, const QPicture & picture )
		static int drawPie(lua_State * L); // ( int x, int y, int width, int height, int startAngle, int spanAngle )
		// ( const QRect & rectangle, int startAngle, int spanAngle )
		static int drawPixmap(lua_State * L); 
		// ( const QRect & target, const QPixmap & pixmap, const QRect & source )
		// ( const QRect & rectangle, const QPixmap & pixmap )
		// ( const QPoint & point, const QPixmap & pixmap, const QRect & source )
		// ( const QPoint & point, const QPixmap & pixmap )
		// ( int x, int y, const QPixmap & pixmap )
		// ( int x, int y, const QPixmap & pixmap, int sx, int sy, int sw, int sh )
		// ( int x, int y, int w, int h, const QPixmap & pixmap, int sx, int sy, int sw, int sh )
		// ( int x, int y, int width, int height, const QPixmap & pixmap )
		static int drawPoint(lua_State * L); // ( const QPointF & position )
		// ( int x, int y )
		static int drawPoints(lua_State * L); // ( const QPointF * points, int pointCount )
		// ( const QPolygonF & points )
		static int drawPolygon(lua_State * L); // ( const QPointF * points, int pointCount, Qt::FillRule fillRule = Qt::OddEvenFill )
		// ( const QPolygonF & points, Qt::FillRule fillRule = Qt::OddEvenFill )
		static int drawPolyline(lua_State * L); // ( const QPointF * points, int pointCount )
		// ( const QPolygonF & points )
		static int drawRect(lua_State * L); // ( const QRectF & rectangle )
		// ( int x, int y, int width, int height )
		static int drawRects(lua_State * L); // ( const QRectF * rectangles, int rectCount )
		// ( const QVector<QRectF> & rectangles )
		static int drawRoundRect(lua_State * L); // ( const QRectF & r, int xRnd = 25, int yRnd = 25 )
		// ( int x, int y, int w, int h, int xRnd = 25, int yRnd = 25 )
		static int drawText(lua_State * L); // ( const QPointF & position, const QString & text )
		// ( const QRectF & rectangle, int flags, const QString & text, QRectF * boundingRect = 0 )
		// ( const QRectF & rectangle, const QString & text, const QTextOption & option = QTextOption() )
		// ( int x, int y, const QString & text )
		// ( int x, int y, int width, int height, int flags, const QString & text, QRect * boundingRect = 0 )
		static int drawTiledPixmap(lua_State * L); // ( const QRectF & rectangle, const QPixmap & pixmap, const QPointF & position = QPointF() )
		// ( int x, int y, int width, int height, const QPixmap & pixmap, int sx = 0, int sy = 0 )
		static int end(lua_State * L); // bool
		static int eraseRect(lua_State * L); // ( const QRectF & rectangle )
		// ( int x, int y, int width, int height )
		static int fillPath(lua_State * L); // ( const QPainterPath & path, const QBrush & brush )
		static int fillRect(lua_State * L); // ( const QRectF & rectangle, const QBrush & brush )
		// ( int x, int y, int width, int height, const QBrush & brush )
		static int font(lua_State * L); // const QFont & 
		// static int fontInfo(lua_State * L); // const QFontInfo
		// static int fontMetrics(lua_State * L); // const QFontMetrics
		static int hasClipping(lua_State * L); // const bool 
		static int initFrom(lua_State * L); // ( const QWidget * widget )
		static int isActive(lua_State * L); // const bool 
		static int layoutDirection(lua_State * L); // const Qt::LayoutDirection 
		static int opacity(lua_State * L); // const qreal 
		// static int paintEngine(lua_State * L); // const QPaintEngine *
		static int pen(lua_State * L); // const QPen &
		static int renderHints(lua_State * L); // const RenderHints
		static int resetMatrix(lua_State * L);
		static int restore(lua_State * L);
		static int rotate(lua_State * L); // ( qreal angle )
		static int save(lua_State * L);
		static int scale(lua_State * L); // ( qreal sx, qreal sy )
		static int setBackground(lua_State * L); // ( const QBrush & brush )
		static int setBackgroundMode(lua_State * L); // ( Qt::BGMode mode )
		static int setBrush(lua_State * L); // ( const QBrush & brush )
		// ( Qt::BrushStyle style )
		static int setBrushOrigin(lua_State * L); // ( const QPointF & position )
		// ( int x, int y )
		static int setClipPath(lua_State * L); // ( const QPainterPath & path, Qt::ClipOperation operation = Qt::ReplaceClip )
		static int setClipRect(lua_State * L); // ( const QRectF & rectangle, Qt::ClipOperation operation = Qt::ReplaceClip )
		// ( int x, int y, int width, int height, Qt::ClipOperation operation = Qt::ReplaceClip )
		// ( const QRect & rectangle, Qt::ClipOperation operation = Qt::ReplaceClip )
		static int setClipRegion(lua_State * L); // ( const QRegion & region, Qt::ClipOperation operation = Qt::ReplaceClip )
		static int setClipping(lua_State * L); // ( bool enable )
		static int setCompositionMode(lua_State * L); // ( CompositionMode mode )
		static int setFont(lua_State * L); // ( const QFont & font )
		static int setLayoutDirection(lua_State * L); // ( Qt::LayoutDirection direction )
		static int setOpacity(lua_State * L); // ( qreal opacity )
		static int setPen(lua_State * L); // ( const QPen & pen )
		// ( const QColor & color )
		// ( Qt::PenStyle style )
		static int setRenderHint(lua_State * L); // ( RenderHint hint, bool on = true )
		static int setRenderHints(lua_State * L); // ( RenderHints hints, bool on = true )
		static int setViewTransformEnabled(lua_State * L); // ( bool enable )
		static int setViewport(lua_State * L); // ( const QRect & rectangle )
		// ( int x, int y, int width, int height )
		static int setWindow(lua_State * L); // ( const QRect & rectangle )
		// ( int x, int y, int width, int height )
		static int setWorldMatrix(lua_State * L); // ( const QMatrix & matrix, bool combine = false )
		static int setWorldMatrixEnabled(lua_State * L); // ( bool enable )
		static int shear(lua_State * L); // ( qreal sh, qreal sv )
		static int strokePath(lua_State * L); // ( const QPainterPath & path, const QPen & pen )
		static int translate(lua_State * L); // ( const QPointF & offset )
		// ( qreal dx, qreal dy )
		static int viewTransformEnabled(lua_State * L); // const : bool 
		static int viewport(lua_State * L); // const : QRect 
		static int window(lua_State * L); // const : QRect 
		static int worldMatrix(lua_State * L); // const QMatrix & 
		static int worldMatrixEnabled(lua_State * L); // consts bool 
		static int init(lua_State * L);
		static void install(lua_State * L);
	};

}

#endif // !defined(_QTL_PAINTER)
