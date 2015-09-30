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

#if !defined(AFX_LUASPECFACTORY_H__86E5BF3B_2A75_4F83_B08B_A3A996B1B019__INCLUDED_)
#define AFX_LUASPECFACTORY_H__86E5BF3B_2A75_4F83_B08B_A3A996B1B019__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Spec/Repository.h>
#include <Script/Engine.h>

namespace Lua
{
	class LuaSpecFactory  
	{
	public:
		static int killRepository(lua_State * L );
		static const char* s_cara;
		static int install( lua_State* );
		static Spec::Repository* getRepository( lua_State* );
		static void installRepository(  lua_State*, Spec::Repository* );
	};
}

#endif // !defined(AFX_LUASPECFACTORY_H__86E5BF3B_2A75_4F83_B08B_A3A996B1B019__INCLUDED_)
