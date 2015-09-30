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

#ifndef LUA_H
#define LUA_H

extern "C"
{
#include <Lua/lua.h>
#include <Lua/lauxlib.h>
#include <Lua/lualib.h>

    /// Holt den lokalen Data-Table von User-Object an objindex
    /// Stack pre: - / post: table or nil
    /// VORSICHT: wenn keine Datatable vorhanden, ist return=0 und nichts geht auf den Stack!!!
    LUA_API int   (lua_getdatatable) (lua_State *L, int objindex);
    /// Setzt den lokalen Data-Table von User-Object an objindex auf pre und poppt table.
    /// Stack pre: table / post: -
    LUA_API int   (lua_setdatatable) (lua_State *L, int objindex);
    /// Setzt das userdata auf den Stack. RISK: Keine Prüfung, ob wirklich UserData.
    /// Stack pre: - / post: object
    LUA_API void  (lua_pushuserdata) (lua_State *L, void* userdata );
}

#endif // LUA_H
