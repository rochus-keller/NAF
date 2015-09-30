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

#if !defined(_QTL_TEXTBLOCK)
#define _QTL_TEXTBLOCK

typedef struct lua_State lua_State;




namespace Qtl
{
	class TextBlock
	{
	public:			
		//static int cellWidget(lua_State * L); // ( int row, int column ) const : QWidget
		static int init(lua_State * L);
		static int begin (lua_State * L); //const iterator
		static int blockFormatIndex (lua_State * L); //const
		static int blockFormat (lua_State * L); //const
		static int charFormat (lua_State * L); //const QTextBlockFormat
		static int charFormatIndex (lua_State * L); //const
		static int contains (lua_State * L); //bool ( int position )
		static int isValid (lua_State * L); //bool 
		static int document (lua_State * L); //const QTextDocument
		static int end (lua_State * L); //const iterator
		static int layout (lua_State * L); //QTextLayout
		static int length (lua_State * L);//int
		static int next (lua_State * L); //QTextBlock
		static int position (lua_State * L); //int
		static int previous (lua_State * L); //QTextBlock
		static int setUserData (lua_State * L); //( QTextBlockUserData * data ) void
		static int setUserState (lua_State * L); // ( int state ) void
		static int text (lua_State * L); //const QString
		static int textList (lua_State * L);// const QTextList
		static int userData (lua_State * L); // const QTextBlockUserData 
		static int userState (lua_State * L); // const
		static int __ne(lua_State * L); // const
		static int __lt(lua_State * L); // const
		static int operatorEq(lua_State * L); // ( const QTextBlock & other ) bool
		static int __eq(lua_State * L); // const
		static void install(lua_State * L);
	};

}

#endif // !defined(_QTL_TEXTBLOCK)
