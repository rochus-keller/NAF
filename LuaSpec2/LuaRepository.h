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

#if !defined(AFX_REPOSITORYPEER_H__863596D2_422D_40E5_93DB_F093A32D39AA__INCLUDED_)
#define AFX_REPOSITORYPEER_H__863596D2_422D_40E5_93DB_F093A32D39AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Script2/RefBinding.h>

namespace Lua
{
	struct LuaRepository // : public LuaObject 
	{
		static int removeResidueType(lua_State *L);
		static int createResidueType(lua_State *L);
		static int createProject(lua_State *L);
		static int touch(lua_State *L);
		static int open(lua_State *L);
		static int create(lua_State *L);
		static int save(lua_State *L);
		static int getExperiment(lua_State *L);
		static int getRecords(lua_State *L);
		static int removeRecord(lua_State *L);
		static int getRecord(lua_State *L);
		static int createRecord(lua_State *L);
		static int getResidueType(lua_State *L);
		static int getSpectrumType(lua_State *L);
		static int getSystemTypes(lua_State *L);
		static int getSpectrumTypes(lua_State *L);
		static int getResidueTypes(lua_State *L);
		static int getProject(lua_State *L);	// Project = f( string )
		static int getProjects(lua_State *L);	// Table( string, Project ) = f()
		static int getAuthor(lua_State *L);	// string = f() 
        static int getScript(lua_State *L);	// string = f(string) gibt den Quelltext zurck
        static int getScriptNames(lua_State *L);	// Array(strin) = f()
		static void install(lua_State * L);
	};
}

#endif // !defined(AFX_REPOSITORYPEER_H__863596D2_422D_40E5_93DB_F093A32D39AA__INCLUDED_)
