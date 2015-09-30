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

#if !defined(_QTL_TEXTEDIT)
#define _QTL_TEXTEDIT

typedef struct lua_State lua_State;




namespace Qtl
{
	class TextEdit
	{
	public:
				
		static int alignment(lua_State * L); // const : Qt::Alignment
		static int anchorAt(lua_State * L); // ( const QPoint & pos ) const : QString
		static int canPaste(lua_State * L); // const : bool
		static int createStandardContextMenu(lua_State * L); // const : QMenu*
		static int currentCharFormat(lua_State * L); // const : QTextCharFormat
		static int currentFont(lua_State * L); // const : QFont
		//static int cursorForPosition(lua_State * L); // ( const QPoint & pos ) const : QTextCursor
		static int cursorRect(lua_State * L); // ( const QTextCursor & cursor ) const : QRect
		static int cursorWidth(lua_State * L); // const : int
		static int document(lua_State * L); // const : QTextDocument*
		static int ensureCursorVisible(lua_State * L);
		static int find(lua_State * L); // ( const QString & exp, QTextDocument::FindFlags options = 0 ) : bool
		static int fontFamily(lua_State * L); // const : QString
		static int fontItalic(lua_State * L); // const : bool
		static int fontPointSize(lua_State * L); // const : qreal
		static int fontUnderline(lua_State * L); // const : bool
		static int fontWeight(lua_State * L); // const : int
		static int loadResource(lua_State * L); // const : QVariant virtual
		static int mergeCurrentCharFormat( lua_State * L ); //( const QTextCharFormat & modifier )
		static int moveCursor( lua_State * L ); // ( QTextCursor::MoveOperation operation, QTextCursor::MoveMode mode = QTextCursor::MoveAnchor )
		static int overwriteMode(lua_State * L); // const : bool
		static int setCurrentCharFormat( lua_State * L ); // ( const QTextCharFormat & format )
		static int setCursorWidth(lua_State * L); // ( int width )
		static int setDocument(lua_State * L); // ( QTextDocument * document )
		//static int setExtraSelections(lua_State * L); // ( const QList<ExtraSelection> & selections )
		static int setOverwriteMode(lua_State * L); // ( bool overwrite )
		static int textColor(lua_State * L); // const : QColor
		static int textCursor(lua_State * L); // const : QTextCursor
		static int toHtml(lua_State * L); // const : QString
		static int toPlainText(lua_State * L); // const : QString
		static int textInteractionFlags(lua_State * L); // const : Qt::TextInteractionFlags
		static int init(lua_State * L);
		static void install(lua_State * L);


	};

}

#endif // !defined(_QTL_TEXTEDIT)
