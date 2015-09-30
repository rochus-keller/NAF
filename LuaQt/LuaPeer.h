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

#if !defined(AFX_LUAPEER_H__7E06F913_053E_4011_9F44_370C4710E4B1__INCLUDED_)
#define AFX_LUAPEER_H__7E06F913_053E_4011_9F44_370C4710E4B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Object.h>
// #include <LuaSpec/LuaObject.h>

typedef struct lua_State lua_State;

namespace Spec
{
	class LuaPeer
	{
	protected:
		LuaPeer() {}
		virtual ~LuaPeer() {}
		//int getAttr(lua_State *L, Root::Object*);
		//int setAttr(lua_State *L, Root::Object*);
	public:
		//virtual Root::Str getInstanceName() const;
	};
}
#endif // !defined(AFX_LUAPEER_H__7E06F913_053E_4011_9F44_370C4710E4B1__INCLUDED_)
