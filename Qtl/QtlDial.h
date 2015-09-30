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

#if !defined(_QTL_DIAL)
#define _QTL_DIAL

typedef struct lua_State lua_State;




namespace Qtl
{
	class Dial
	{
	public:	
		//static int columnAt(lua_State * L); // ( int x ) const : int
		static int notchSize(lua_State * L); //  const : int
		static int notchTarget(lua_State * L); //  const : qreal
		static int notchesVisible(lua_State * L); //  const : bool
		static int setNotchTarget(lua_State * L); //  double target
		static int wrapping(lua_State * L); //  const : bool
		static void install(lua_State * L);
	};

}

#endif // !defined(_QTL_DIALOG)
