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

#if !defined(_QTL_STRINGLIST)
#define _QTL_STRINGLIST

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

//#include <Root/Units.h>

typedef struct lua_State lua_State;




namespace Qtl
{
	class StringList
	{
	public:
		static int init(lua_State * L);
		static int contains (lua_State * L);//( const QString & str, Qt::CaseSensitivity cs = Qt::CaseSensitive ) const bool
		static int filter (lua_State * L);//( const QRegExp & rx ) const QStringList
		static int indexOf (lua_State * L);//( const QRegExp & rx, int from = 0 ) int
		static int join(lua_State * L);//( const QString & separator ) const QString
		static int lastIndexOf (lua_State * L);//( const QRegExp & rx, int from = -1 ) int
		static int sort(lua_State * L);//void
		static int operatorPlus (lua_State * L);//( const QStringList & other ) const QStringList
		static int replaceInStrings (lua_State * L);
	    static int operatorShiftLeft (lua_State * L);
		static void install(lua_State * L);
	};

}

#endif // !defined(_QTL_STRINGLIST)
