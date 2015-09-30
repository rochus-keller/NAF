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

#if !defined(AFX_LUAPROTONLIST_H__B0ABD26E_9D8C_4F54_9697_5227A17D5634__INCLUDED_)
#define AFX_LUAPROTONLIST_H__B0ABD26E_9D8C_4F54_9697_5227A17D5634__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Spec/EasyProtonList.h>
#include <LuaSpec/LuaObject.h>

namespace Lua
{
	class LuaProtonList : public LuaObject
	{
	public:
		int saveToFile(lua_State *L);
		int setAtom(lua_State *L);
		int getAtom(lua_State *L);
		int isValid(lua_State *L);
		int getCount(lua_State *L);
		LuaProtonList(Spec::EasyProtonList*);
		DECLARE_LUA_OBJECT( LuaProtonList, Spec::EasyProtonList )
	};
}

#endif // !defined(AFX_LUAPROTONLIST_H__B0ABD26E_9D8C_4F54_9697_5227A17D5634__INCLUDED_)
