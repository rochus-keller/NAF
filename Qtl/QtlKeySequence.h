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

#if !defined(_QTL_KEYSEQUENCE)
#define _QTL_KEYSEQUENCE


typedef struct lua_State lua_State;


namespace Qtl
{
	class KeySequence
	{
	public:
		static int count(lua_State * L); // const : uint 
		static int isEmpty(lua_State * L); // const : bool
		static int matches(lua_State * L); // ( const QKeySequence & seq ) const : SequenceMatch
		static int toString(lua_State * L); // ( SequenceFormat format = PortableText ) const : QString
		static int QVariant(lua_State * L);
		static int notEquals(lua_State * L); // ( const QKeySequence & other ) const : bool
		static int lessThan(lua_State * L); // ( const QKeySequence & other ) const : bool
		static int lessThanEqualTo(lua_State * L); // ( const QKeySequence & other ) const : bool
		static int __eq(lua_State * L);
		static int greaterThan(lua_State * L); // ( const QKeySequence & other ) const : bool
		static int greaterThanEqualTo(lua_State * L); // ( const QKeySequence & other ) const : bool
		static int getElement(lua_State * L); //operator[]  ( uint index ) const : int
		static int init(lua_State * L);
		static void install(lua_State * L);
	};
}

#endif // !defined(_QTL_KEYSEQUENCE)
