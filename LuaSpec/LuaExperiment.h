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

#if !defined(AFX_LUAEXPERIMENT_H__DD7694DC_5795_4202_BACD_A2F4F6AE78C2__INCLUDED_)
#define AFX_LUAEXPERIMENT_H__DD7694DC_5795_4202_BACD_A2F4F6AE78C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Spec/NmrExperiment.h>
#include <LuaSpec/LuaObject.h>

namespace Lua
{
	class LuaExperiment : public LuaObject
	{
	public:
		int getSpectrumType(lua_State *L);
		int getResidueType(lua_State *L);
		int setSpectrumType(lua_State *L);
		int setResidueType(lua_State *L);
		int getPath(lua_State *L);
		int getCount(lua_State *L);
		int toString(lua_State *L);
		LuaExperiment(Spec::NmrExperiment*);
		DECLARE_LUA_OBJECT( LuaExperiment, Spec::NmrExperiment )
	};
}

#endif // !defined(AFX_LUAEXPERIMENT_H__DD7694DC_5795_4202_BACD_A2F4F6AE78C2__INCLUDED_)
