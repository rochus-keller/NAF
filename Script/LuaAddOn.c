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
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License (LGPL) as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * You should have received a copy of the LGPL along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <Lua/lobject.h>
#include <Lua/llimits.h>
#include <Lua/lstate.h>
// aus lapi.c
#define api_incr_top(L)   {api_check(L, L->top < L->ci->top); L->top++;}

LUA_API int lua_setdatatable (lua_State *L, int objindex)
{
    // Vor Lua 5.1 gab es diese Funktion noch nicht, bzw. damals hatte ich eine
    // eigene Implementation; nun verwende ich die offizielle Funktion.
    return lua_setfenv(L,objindex);
}

LUA_API int lua_getdatatable (lua_State *L, int objindex)
{
    int equal;
    lua_getfenv(L,objindex);
    // NOTE: lua_getfenv gibt immer eine Tabelle zurück. Default ist getcurrenv(L)
    lua_pushvalue(L, LUA_GLOBALSINDEX); // TODO: geht das ev. effizienter?
    equal = lua_rawequal( L, -1, -2 );
    lua_pop( L, 1 );
    if( lua_isnil( L, -1 ) || equal )
    {
        lua_pop( L, 1 );
        return 0;
    }
    return 1;
}

LUA_API void lua_pushuserdata (lua_State *L, void *p)
{
    Udata *u = (Udata *)p;
    lua_lock(L);
    // Lua speichert Userdata mit einer fixen Header-Struktur Udata, gefolgt von einer variablen
    // Anzahl (Udata.len) Bytes, welche die eigentlichen Userdata repräsentieren.
    // p zeigt zuerst auf das erste dieser Bytes, also mitten in der Struktur Udata+Bytes.
    // Setze Pointer um die Grösse von Udata zurück. Damit liegt er genau auf dem
    // Anfang von Udata.
    u = u - 1;
    setuvalue(L, L->top, u);
    api_incr_top(L);
    lua_unlock(L);
}
