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

#if !defined(_QTL_TABLEWIDGETITEM)
#define _QTL_TABLEWIDGETITEM

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

//#include <Root/Units.h>

typedef struct lua_State lua_State;




namespace Qtl
{
	class TextCursor
	{
	public:
		static int init(lua_State * L);
		static int anchor(lua_State * L);//const int
		static int atBlockEnd(lua_State * L);//const bool
		static int atBlockStart(lua_State * L);//const bool
		static int atEnd(lua_State * L);//const bool
		static int atStart(lua_State * L);//const bool
		static int beginEditBlock(lua_State * L);//void
		static int block(lua_State * L);//const QTextBlock
		static int blockCharFormat(lua_State * L);//const QTextCharFormat
		static int blockFormat(lua_State * L);//const QTextBlockFormat
		static int blockNumber(lua_State * L);//const int
		static int charFormat(lua_State * L);//const QTextCharFormat
		static int clearSelection(lua_State * L);//void
		static int columnNumber(lua_State * L);//const int
		static int createList(lua_State * L);//const QTextList
		static int currentFrame(lua_State * L);//const QTextFrame
		static int currentList(lua_State * L);//const QTextList
		static int currentTable(lua_State * L);//const QTextTable
		static int deleteChar(lua_State * L);//void
		static int deletePreviousChar(lua_State * L);//void
		static int endEditBlock(lua_State * L);//void
		static int hasComplexSelection(lua_State * L);//bool
		static int hasSelection(lua_State * L);//bool
		static int insertBlock(lua_State * L);//void
		static int insertFragment(lua_State * L);//( const QTextDocumentFragment & fragment )void
		static int insertFrame(lua_State * L);//( const QTextFrameFormat  & format )QTextFrameFormat 
		static int insertHtml(lua_State * L);//( const QString & html )void
		static int insertImage(lua_State * L);//void
		static int insertList(lua_State * L);//QTextList
		static int insertTable(lua_State * L);//QTextTable
		static int insertText(lua_State * L);
		static int isCopyOf(lua_State * L);// ( const QTextCursor & other ) bool const
		static int isNull(lua_State * L);// const bool
		static int joinPreviousEditBlock(lua_State * L);//void
		static int mergeBlockCharFormat(lua_State * L);//( const QTextCharFormat & modifier )void
		static int mergeBlockFormat(lua_State * L);//( const QTextCharFormat & modifier )void
		static int mergeCharFormat(lua_State * L);//( const QTextCharFormat & modifier )void
		static int movePosition(lua_State * L);//( MoveOperation operation, MoveMode mode = MoveAnchor, int n = 1 )bool
		static int position(lua_State * L);//const int
		static int removeSelectedText(lua_State * L);//void
		static int select(lua_State * L);//( SelectionType selection )void
		static int selectedTableCells(lua_State * L);// ( int * firstRow, int * numRows, int * firstColumn, int * numColumns ) const void 
		static int selectedText(lua_State * L);// const QString
		static int selection(lua_State * L);//const QTextDocumentFragment
		static int selectionEnd(lua_State * L);//const int
		static int selectionStart(lua_State * L);//const int
		static int setBlockCharFormat(lua_State * L);//( const QTextCharFormat & format )void
		static int setBlockFormat(lua_State * L);//( const QTextCharFormat & format )void
		static int setCharFormat(lua_State * L);//( const QTextCharFormat & format )void
		static int setPosition(lua_State * L);//( int pos, MoveMode m = MoveAnchor ) void
		static int operatorEq(lua_State * L);//( const QTextCursor & cursor )QTextCursor
		static int notEquals(lua_State * L);//( const QTextCursor & other ) const bool
		static int lessThanEqualTo(lua_State * L);//( const QTextCursor & other ) const bool
		static int lessThan(lua_State * L);//( const QTextCursor & other ) const bool
		static int __eq(lua_State * L);//( const QTextCursor & other ) const bool
		static int greaterThanEqualTo(lua_State * L);//( const QTextCursor & other ) const bool
		static int greaterThan(lua_State * L);//( const QTextCursor & other ) const bool
		
	    static void install(lua_State * L);
	};

}

#endif // !defined(_QTL_TEXTCURSOR)
