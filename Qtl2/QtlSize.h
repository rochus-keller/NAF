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

#if !defined(_QTL_SIZE)
#define _QTL_SIZE


typedef struct lua_State lua_State;


namespace Qtl
{

	class Size
	{
	public:
		static int isNull(lua_State * L); 
		static int isEmpty(lua_State * L); 
		static int isValid(lua_State * L); 
		static int setHeight(lua_State * L); 
		static int setWidth(lua_State * L); 
		static int height(lua_State * L); 
		static int width(lua_State * L);	
		static int scale(lua_State * L);	
		static int transpose(lua_State * L);	
		static int boundedTo(lua_State * L);	
		static int expandedTo(lua_State * L);	
		static int __add(lua_State * L);
		static int __sub(lua_State * L);
		static int __mul(lua_State * L);
		static int __div(lua_State * L);
		static int multiply(lua_State * L); // ( qreal factor )
		static int add(lua_State * L); // ( const QSizeF & size )
		static int subtract(lua_State * L); // ( const QSizeF & size )
		static int divide(lua_State * L); // ( qreal factor )
		static int init(lua_State * L);
		static void install(lua_State * L);
	};
}

#endif // !defined(_QTL_SIZE)
