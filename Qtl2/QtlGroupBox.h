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

#if !defined(_QTL_GROUPBOX)
#define _QTL_GROUPBOX

typedef struct lua_State lua_State;




namespace Qtl
{
	class GroupBox
	{
	public:	
		static int init(lua_State * L);
		static int alignment(lua_State * L); // const : Qt::Alignment
		static int isCheckable(lua_State * L); //  const : bool
		static int isChecked(lua_State * L); //  const : bool
		static int isFlat(lua_State * L); //  const : bool
		static int setAlignment(lua_State * L); //  int alignment
		static int setCheckable(lua_State * L); //  bool checkable
		static int setFlat(lua_State * L); //  bool flat
		static int setTitle(lua_State * L); //  const QString & title
		static int title(lua_State * L); //  const : QString
		static void install(lua_State * L);
		
	};

}

#endif // !defined(_QTL_GROUPBOX)
