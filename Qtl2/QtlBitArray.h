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

#if !defined(_QTL_BITARRAY)
#define _QTL_BITARRAY


typedef struct lua_State lua_State;


namespace Qtl
{

class BitArray
	{
	public:
		static int at(lua_State * L); // ( int ) const : bool 
		static int clear(lua_State * L); // () 
		static int clearBit(lua_State * L); // ( int ) 
		static int count(lua_State * L); // () const : int 
		static int fill(lua_State * L); // ( bool, int ) : bool 
		static int isEmpty(lua_State * L); // () const : bool 
		static int isNull(lua_State * L); // () const : bool 
		static int resize(lua_State * L); // ( int ) 
		static int setBit(lua_State * L); // ( int ) 
		static int size(lua_State * L); // () const : int 
		static int testBit(lua_State * L); // ( int ) const : bool 
		static int toggleBit(lua_State * L); // ( int ) : bool 
		static int truncate(lua_State * L); // ( int )
		static void install(lua_State * L);
	};
}

#endif // !defined(_QTL_BITARRAY)
