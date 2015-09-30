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

#if !defined(_QTL_LINEEDIT)
#define _QTL_LINEEDIT

typedef struct lua_State lua_State;




namespace Qtl
{
	class LineEdit
	{
	public:
		
		static int init(lua_State * L);
		static int backspace(lua_State * L);
		static int completer(lua_State * L); // const : QCompleter
		static int createStandardContextMenu(lua_State * L); //  : QMenu
		static int cursorBackward(lua_State * L); //  ( bool mark, int steps = 1 )
		static int cursorForward(lua_State * L); //  ( bool mark, int steps = 1 )
		static int cursorPositionAt(lua_State * L); //  ( const QPoint & pos )
		static int cursorWordBackward(lua_State * L); //  ( bool mark )
		static int cursorWordForward(lua_State * L); //  ( bool mark )
		static int del(lua_State * L);
		static int deselect(lua_State * L);
		static int displayText(lua_State * L); // const : QString
		static int end(lua_State * L); //  ( bool mark )
		static int home(lua_State * L); //  ( bool mark )
		static int insert(lua_State * L); //  ( const QString & newText )
		static int minimumSizeHint(lua_State * L); // const : QSize
		static int selectionStart(lua_State * L); // const : int
		static int setCompleter(lua_State * L); //  ( bool mark )
		static int setSelection(lua_State * L); //  ( int start, int length )
		static int setValidator(lua_State * L); //  ( const QValidator * v )
		static int sizeHint(lua_State * L); // const : QSize
		static int validator(lua_State * L); // const : const QValidator 
		static void install(lua_State * L);


	};

}

#endif // !defined(_QTL_STACKEDWIDGET)
