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

#include <Spec/Repository.h>
#include <LuaSpec/LuaObject.h>

namespace Lua
{
	class LuaRepository : public LuaObject 
	{
	public:
		int createProject(lua_State *L);
		int touch(lua_State *L);
		int open(lua_State *L);
		int create(lua_State *L);
		int save(lua_State *L);
		int getExperiment(lua_State *L);
		int getRecords(lua_State *L);
		int removeRecord(lua_State *L);
		int getRecord(lua_State *L);
		int createRecord(lua_State *L);
		int getResidueType(lua_State *L);
		int getSpectrumType(lua_State *L);
		int getSystemTypes(lua_State *L);
		int getSpectrumTypes(lua_State *L);
		int getResidueTypes(lua_State *L);
		int getProject(lua_State *L);	// Project = f( string )
		int getProjects(lua_State *L);	// Table( string, Project ) = f()
		int getAuthor(lua_State *L);	// string = f() 
		LuaRepository( Spec::Repository* );
		DECLARE_LUA_OBJECT( LuaRepository, Spec::Repository )
	};
}

#endif // !defined(AFX_REPOSITORYPEER_H__863596D2_422D_40E5_93DB_F093A32D39AA__INCLUDED_)
