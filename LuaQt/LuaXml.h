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

#if !defined(AFX_LUAXML_H__FD96C16A_9E22_4156_9971_2E38A9B04B26__INCLUDED_)
#define AFX_LUAXML_H__FD96C16A_9E22_4156_9971_2E38A9B04B26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qdom.h>
#include <Script/LuaEngine.h>
#include <Script/PeerHelper.h>
#include <LuaQt/LuaPeer.h>

namespace Spec
{
	class LuaDomDocument : public LuaPeer
	{
	public:
		int getXml(lua_State *L);
		int saveToFile( lua_State* L );
		int getDocumentElement(lua_State *L);
		LuaDomDocument();
		virtual ~LuaDomDocument();
		static const char s_className[];
		static PeerHelper<LuaDomDocument>::MethodSlot s_methods[];
		QDomDocument d_obj;

		static void install(lua_State *L);
	};

	class LuaDomElement : public LuaPeer
	{
	public:
		int normalize( lua_State* L );
		int isText( lua_State* L );
		int isElement( lua_State* L );
		int createElement( lua_State* L );
		int createText( lua_State* L );
		int removeThis( lua_State* L );
		int getNextSibling( lua_State* L );
		int getPrevSibling( lua_State* L );
		int getLastChild( lua_State* L );
		int getFirstChild( lua_State* L );
		int getChildren( lua_State* L );
		int getAttributes( lua_State* L );
		int hasAttribute( lua_State* L );
		int removeAttribute( lua_State* L );
		int setAttribute( lua_State* L );
		int getAttribute( lua_State* L );
		int getName( lua_State* L );
		int setName( lua_State* L );

		static const char s_className[];
		static PeerHelper<LuaDomElement>::MethodSlot s_methods[];
		QDomElement d_obj;
	};

	class LuaDomText : public LuaPeer
	{
	public:
		int setText(lua_State *L);
		int getText( lua_State* L );
		int isText( lua_State* L );
		int isElement( lua_State* L );
		int removeThis( lua_State* L );
		int getNextSibling( lua_State* L );
		int getPrevSibling( lua_State* L );

		static const char s_className[];
		static PeerHelper<LuaDomText>::MethodSlot s_methods[];
		QDomText d_obj;
	};

	// TODO: ev. brauchen wir CDATA-Sections
}

#endif // !defined(AFX_LUAXML_H__FD96C16A_9E22_4156_9971_2E38A9B04B26__INCLUDED_)
