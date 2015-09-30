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

#if !defined(_QTL_TEXTDOCUMENT)
#define _QTL_TEXTDOCUMENT



typedef struct lua_State lua_State;
namespace Qtl
{
	class TextDocument
	{
	public:			
		static int init(lua_State * L);
		static int addResource(lua_State * L);//( int type, const QUrl & name, const QVariant & resource )void
		static int adjustSize(lua_State * L);//void
		static int begin(lua_State * L);//const QTextBlock
		static int blockCount(lua_State * L);//const int
		static int clear(lua_State * L);//virtual void
		static int clone(lua_State * L);// ( QObject * parent = 0 ) const QTextDocument
		static int defaultFont(lua_State * L);//const QFont
		static int defaultStyleSheet(lua_State * L);//const QString
		static int documentLayout(lua_State * L);//const QAbstractTextDocumentLayout
		static int drawContents(lua_State * L);//( QPainter * p, const QRectF & rect = QRectF() )void
		static int end(lua_State * L);//const QTextBlock
		static int find(lua_State * L);
		static int findBlock(lua_State * L);//( int pos ) const QTextBlock
		static int idealWidth(lua_State * L);//const qreal
		static int isEmpty(lua_State * L);//const bool
		static int isModified(lua_State * L);//const bool
		static int isRedoAvailable(lua_State * L);//const bool
		static int isUndoAvailable(lua_State * L);//const bool
		static int isUndoRedoEnabled(lua_State * L);//const bool
		static int markContentsDirty(lua_State * L);//( int position, int length )void
		static int maximumBlockCount(lua_State * L);//const int
		static int metaInformation(lua_State * L);//( MetaInformation info ) const QString
		static int object(lua_State * L);//( int objectIndex ) const QTextObject
		static int objectForFormat(lua_State * L);//( const QTextFormat & f ) const QTextObject
		static int pageCount(lua_State * L);//const int
		static int pageSize(lua_State * L);//const QSizeF
		static int print(lua_State * L);//( QPrinter * printer ) const void
		static int redo(lua_State * L);//( QTextCursor * cursor )void
		static int resource(lua_State * L);//( int type, const QUrl & name ) const QVariant
		static int rootFrame(lua_State * L);//const QTextFrame
		static int setDefaultFont(lua_State * L);//( const QFont & font )void
		static int setDefaultStyleSheet(lua_State * L);//( const QString & sheet )void
		static int setDocumentLayout(lua_State * L);//( QAbstractTextDocumentLayout * layout )void
		static int setHtml(lua_State * L);//( const QString & html )void
		static int setMaximumBlockCount(lua_State * L);//( int maximum )void
		static int setMetaInformation(lua_State * L);//( MetaInformation info, const QString & string )void
		static int setPageSize(lua_State * L);//( const QSizeF & size )void
		static int setPlainText(lua_State * L);//( const QString & text )void
		static int setTextWidth(lua_State * L);//( qreal width )void
		static int setUndoRedoEnabled(lua_State * L);//( bool enable )void
		static int setUseDesignMetrics(lua_State * L);//( bool b )void
		static int size(lua_State * L);//const QSizeF
		static int textWidth(lua_State * L);//const qreal
		static int toHtml(lua_State * L);//( const QByteArray & encoding = QByteArray() ) const QString
		static int toPlainText(lua_State * L);//const QString
		static int undo(lua_State * L);//( QTextCursor * cursor )void
		static int useDesignMetrics(lua_State * L);//const bool

		static void install(lua_State * L);
	};

}

#endif // !defined(_QTL_TEXTDOCUMENT)
