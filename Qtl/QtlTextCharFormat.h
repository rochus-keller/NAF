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

#if !defined(_QTL_TEXTCHARFORMAT)
#define _QTL_TEXTCHARfORMAT

typedef struct lua_State lua_State;




namespace Qtl
{
	class TextCharFormat
	{
	public:
				
		static int init(lua_State * L);
		static int anchorHref(lua_State * L); // const : QString
		static int anchorName(lua_State * L); // const : QString
		static int font(lua_State * L); // const : QFont
		static int fontFamily(lua_State * L); // const : QString
		static int fontFixedPitch(lua_State * L); // const : bool
		static int fontItalic(lua_State * L); // const : bool
		static int fontOverline(lua_State * L); // const : bool
		static int fontPointSize(lua_State * L);// const : qreal
		static int fontStrikeOut(lua_State * L); // const : bool
		static int fontUnderline(lua_State * L); // const : bool
		static int fontWeight(lua_State * L); // const : int
		static int isAnchor(lua_State * L); // const : bool
		static int isValid(lua_State * L); // const : bool
		static int setAnchor(lua_State * L); // ( bool anchor )
		static int setAnchorHref(lua_State * L); // ( const QString & value ) 
		static int setAnchorName(lua_State * L); // ( const QString & name ) 
		static int setFont(lua_State * L); // ( const QFont & font ) 
		static int setFontFamily(lua_State * L); // ( const QString & family )
		static int setFontFixedPitch(lua_State * L); // ( bool fixedPitch ) 
		static int setFontItalic(lua_State * L); // ( bool italic ) 
		static int setFontOverline(lua_State * L); // ( bool overline ) 
		static int setFontPointSize(lua_State * L); // ( qreal size ) 
		static int setFontStrikeOut(lua_State * L); // ( bool strikeOut ) 
		static int setFontUnderline(lua_State * L); // ( bool underline ) 
		static int setFontWeight(lua_State * L); // ( int weight ) 
		static int setTableCellColumnSpan(lua_State * L); // ( int tableCellColumnSpan ) 
		static int setTableCellRowSpan(lua_State * L); // ( int tableCellRowSpan ) 
		static int setTextOutline(lua_State * L); // ( const QPen & pen ) 
		static int setUnderlineColor(lua_State * L); // ( const QColor & color ) 
		static int setUnderlineStyle(lua_State * L); // ( UnderlineStyle style )
		static int setVerticalAlignment(lua_State * L); // ( VerticalAlignment alignment )
		static int tableCellColumnSpan(lua_State * L); // const : int
		static int tableCellRowSpan(lua_State * L); // const : int
		static int textOutline(lua_State * L); // const : QPen 
		static int underlineColor(lua_State * L); // const : QColor 
		static int underlineStyle(lua_State * L); // const : UnderlineStyle
		static int verticalAlignment(lua_State * L); // const : VerticalAlignment
		static void install(lua_State * L);


	};

}

#endif // !defined(_QTL_TEXTCHARFORMAT)
