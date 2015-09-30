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

#if !defined(_LuaQt2_LuaXml2)
#define _LuaQt2_LuaXml2

#include <Script/Engine.h>
#include <Script/ValueInstaller.h>

namespace Lua
{
	class LuaDomDocument2 // QDomDocument
	{
	public:
		static const luaL_reg methods[];
		static int getXml(lua_State *L);
		static int saveToFile( lua_State* L );
		static int getDocumentElement(lua_State *L);

		static void install(lua_State *L);
	};

	class LuaDomElement2 // QDomElement
	{
	public:
		static const luaL_reg methods[];
		static int normalize( lua_State* L );
		static int isText( lua_State* L );
		static int isElement( lua_State* L );
		static int createElement( lua_State* L );
		static int createText( lua_State* L );
		static int removeThis( lua_State* L );
		static int getNextSibling( lua_State* L );
		static int getPrevSibling( lua_State* L );
		static int getLastChild( lua_State* L );
		static int getFirstChild( lua_State* L );
		static int getChildren( lua_State* L );
		static int getAttributes( lua_State* L );
		static int hasAttribute( lua_State* L );
		static int removeAttribute( lua_State* L );
		static int setAttribute( lua_State* L );
		static int getAttribute( lua_State* L );
		static int getName( lua_State* L );
		static int setName( lua_State* L );
	};

	class LuaDomText2  // QDomText
	{
	public:
		static const luaL_reg methods[];
		static int setText(lua_State *L);
		static int getText( lua_State* L );
		static int isText( lua_State* L );
		static int isElement( lua_State* L );
		static int removeThis( lua_State* L );
		static int getNextSibling( lua_State* L );
		static int getPrevSibling( lua_State* L );
	};

	// TODO: ev. brauchen wir CDATA-Sections
}

#endif // !defined(AFX_LUAXML_H__FD96C16A_9E22_4156_9971_2E38A9B04B26__INCLUDED_)
