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

#include "ValueBinding.h"
using namespace Lua;

void ValueBindingBase::ensureSuperClass(lua_State *L, const int metaTable )
{
    StackTester test( L, 0 );
    // Verwendet die metaTable und stellt anhand __super darin
    // sicher, dass lua_getmetatable auf die richtige Tabelle zeigt.
    if( lua_getmetatable( L, metaTable ) )
    {
		// Mission erfüllt, es ist eine Metatable vorhanden
        lua_pop( L, 1 );
        return;
    }else
    {
		lua_rawgeti(L, metaTable, SuperClassName );
        // Stack: string | nil
        if( lua_isstring( L, -1 ) )
        {
			luaL_getmetatable( L, lua_tostring( L, -1 ) );
			lua_setmetatable( L, metaTable ); // Superklasse ist nun via meta von meta erreichbar
            lua_pop( L, 1 );
        }else
        {
            // Es gibt keine Superclass
            lua_pop( L, 1 );
        }
    }
}

QByteArray ValueBindingBase::getTypeName(lua_State *L, int n)
{
	QByteArray name;
	if( lua_getmetatable( L, n ) )
	{
		lua_rawgeti(L, -1, ClassName );
		name = lua_tostring(L, -1 );
		lua_pop(L, 2 ); // meta und string
	}
	return name;
}

QByteArray ValueBindingBase::getBindingName(lua_State *L, int n)
{
	QByteArray name;
	if( lua_getmetatable( L, n ) )
	{
		lua_rawgeti(L, -1, BindingName );
		name = lua_tostring(L, -1 );
		lua_pop(L, 2 ); // meta und string
	}
	return name;
}

int ValueBindingBase::pushTypeName(lua_State *L, int n)
{
	if( !lua_getmetatable( L, n ) )
	{
		lua_pushnil( L );
	}else
	{
		lua_rawgeti(L, -1, ClassName );
		lua_remove(L, -2 ); // entferne Metatable
	}
	return 1;
}

int ValueBindingBase::newindex(lua_State *L)
{
    StackTester test( L, 0 );
    if( !lua_isuserdata( L, 1 ) )
        luaL_error( L, "no user object!" );
    const char* fieldName = luaL_checkstring( L, 2 );

    if( lua_getdatatable( L, 1 ) == 0 )
    {
        // Es gibt noch keine datatable.
        // Wenn wir eh nil zuweisen wollen, brechen wir hier ab
        if( lua_isnil( L, 3 ) )
            return 0;
        // Erzeuge also eine und weise sie Userdata zu.
        lua_newtable ( L ); // Stack: table
        lua_pushvalue( L, -1 );	// Stack: table, table
        lua_setdatatable( L, 1 ); // Stack: table
    }
    const int dataTable = lua_gettop( L );
    lua_pushstring( L, fieldName );
    lua_pushvalue( L, 3 );
    lua_rawset( L, dataTable );
    // Stack: table
    lua_pop( L, 1 );
    return 0;
}

bool ValueBindingBase::isSubOrSameClass(lua_State *L, int narg, const char* typeName, bool recursive )
{
    StackTester test( L, 0 );
    // Prüfe, ob Userdata bei narg eine Unterklasse von T ist.
    if( !lua_getmetatable(L, narg ) )
        return false;
    const int subMeta = lua_gettop( L );
    // Stack: subMeta
    luaL_getmetatable( L, typeName );
    const int superMeta = lua_gettop( L );
    if( lua_isnil( L, superMeta ) )
    {
        // Stack: subMeta, nil
        lua_pop( L, 2 );
        return false;
    }
    // Stack: subMeta, superMeta
    while( !lua_rawequal( L, superMeta, subMeta ) )
    {
        // Die Klassenhierarchie ist via Metatable-Kette abgebildet. Die Metatable der Oberklasse
        // namens __super ist via lua_getmetatable zugänglich
        ensureSuperClass( L, subMeta );
        if( !recursive || !lua_getmetatable( L, subMeta ) )
        {
            lua_pop( L, 2 );
            return false;
        }
        lua_replace( L, subMeta );
    }
    lua_pop( L, 2 ); // subMeta und superMeta
    return true;
}

bool ValueBindingBase::fetch(lua_State *L, bool doMethodLookup, bool doDataLookup )
{
    // Stack: -
    const int top = lua_gettop(L);
    StackTester test( L, 1 );
    if( !lua_isuserdata( L, 1 ) )
        luaL_error( L, "no user object!" );
    QByteArray fieldName = luaL_checkstring( L, 2 );

    if( fieldName == "class" || fieldName == "type" )
    {
        // Spezialfeld: enthält den öffentlichen Namen der Klasse
        if( !lua_getmetatable( L, 1 ) )
        {
            lua_pushstring(L, "<unknown class>" );
        }else
        {
			lua_rawgeti(L, -1, ClassName );
            lua_remove(L, -2 ); // entferne Metatable
        }
        if( test.isViolation() )
            test.dumpStack();
		return true;
    }else if( fieldName == "data" )
    {
        // Spezialfeld: enthält die datatable
        if( lua_getdatatable( L, 1 ) == 0 )
        {
            // Es gibt noch keine datatable.
            // Erzeuge also eine und weise sie Userdata zu.
            lua_newtable ( L ); // Stack: table
            lua_pushvalue( L, -1 );	// Stack: table, table
            lua_setdatatable( L, 1 );
        }
        // Stack: table
        if( test.isViolation() )
            test.dumpStack();
		return true;
	}

	Q_ASSERT( top == lua_gettop(L) );
	// Stack: -

	if( doMethodLookup )
	{
		// Prüfe nun, ob eine Methode mit diesem Namen vorhanden ist; muss vor Abfrage von data table geschehen!
		if( !lua_getmetatable( L, 1 ) )
			lua_pushnil(L);
		lookupMethod( L, fieldName );
		// Stack: method | nil
		if( !lua_isnil( L, -1 ) )
		{
			if( test.isViolation() )
				test.dumpStack();
			return true;
		}else
			lua_pop( L, 1 ); // drop nil
	}

	// Stack: -
	Q_ASSERT( top == lua_gettop(L) );

	if( doDataLookup )
	{
		// Prüfe nun noch, ob es sich allenfalls um einen Wert in der data table handelt;
		// das ist ein shortcut, so dass statt obj.data.value direkt obj.value geschrieben werden kann
		if( lua_getdatatable( L, 1 ) != 0 )
		{
			// Stack: dataTable
			// Es ist eine datatable vorhanden. Prüfe, ob fieldName dort vorhanden ist
			const int dataTable = lua_gettop( L );
			lua_pushstring( L, fieldName );
			lua_rawget( L, dataTable );
			// Stack: dataTable, value | nil
			if( !lua_isnil( L, -1 ) )
			{
				// Wir haben einen Wert gefunden und geben den zurück
				lua_remove( L, dataTable );
				// Stack: value | nil
				if( test.isViolation() )
					test.dumpStack();
				return true;
			}else
				lua_pop( L, 2 ); // drop dataTable, nil
		}
	}
	// Stack: -
	Q_ASSERT( top == lua_gettop(L) );

	lua_pushnil( L );
	if( test.isViolation() )
		test.dumpStack();
	return false;
}

int ValueBindingBase::index(lua_State *L)
{
	fetch( L, true, true );
	return 1;
}

void ValueBindingBase::lookupMethod(lua_State *L, const char* fieldName, bool recursive )
{
    // Erwartet Stack: metaTable | nil und gibt Ergebnis auch auf diese Position aus
    StackTester test( L, 0 ); // Der Rückgabewert kommt auf den bereits bestehenden Stackplatz
    const int metaTable = lua_gettop(L);
    while( lua_istable( L, metaTable ) )
    {
        // Durchsuche die in der metaTable referenzierte methodTable und gehe die
        // Klassenhierarchie hoch, bis gefunden oder Ende Feuer.
        // Stack: metaTable
		lua_rawgeti( L, metaTable, MethodTable );
        const int methodTable = lua_gettop(L);
        // Stack: metaTable, methodTable
        if( lua_istable( L, methodTable ) )
        {
            // Versuche die Methode in der Metatabelle zu finden
            lua_pushstring( L, fieldName );
            lua_rawget( L, methodTable );
            // Stack: metaTable, methodTable, method | nil
            if( !lua_isnil( L, -1 ) )
            {
                // Wir wurden fündig und geben method zurück
                // Stack: metaTable, methodTable, method
                lua_replace( L, metaTable );
                // Stack: method, methodTable
                lua_pop( L, 1 );
                // Stack: method
				// TODO: ev. cachen in erster Methodtable damit künftig schneller zugreifbar
                return;
			}else if( !recursive )
            {
                // Stack: metaTable, methodTable, nil
                lua_remove( L, -2 );
                // Stack: metaTable, nil
                lua_remove( L, -2 );
                // Stack: nil
                return;
            }else
                lua_pop( L, 2 );
            // Stack: metaTable
        }else
            lua_pop(L, 1);
        // Stack: metaTable
        // Die Metatable der metaTable ist diejenige der Oberklasse, welche ihrerseits wieder gleich
        // funktioniert wie die Metatable dieser Klasse.
        // Verwende den für die metaTable reservierten Stackslot gleich wieder für die neue Metatable
        ensureSuperClass( L, metaTable );
        if( !lua_getmetatable( L, metaTable ) )
            lua_pushnil( L );
        // Stack: metaTable, superMetaTable | nil
        lua_replace( L, metaTable );
        // Stack: superMetaTable | nil
    }
    // Stack: nil
    // Es gibt keine Superklasse, wir geben nil zurück.
}

int ValueBindingBase::createInstanceByCall( lua_State *L )
{
    // stack: table + params
    // __call wird mit methodTable als erstem Parameter aufgerufen. Auch "new" findet man dort.
    const int numOfArgs = lua_gettop( L );
    lua_pushstring(L, "new" );
    lua_gettable( L, 1 );
    // stack: table + params, method | nil
    if( lua_isnil( L, -1 ) )
        luaL_error( L, "Cannot find function 'new'!" );
    if( !lua_isfunction( L, -1 ) )
        luaL_error( L, "'new' is not a function!" );
    for( int j = 2; j <= numOfArgs; j++ )
        lua_pushvalue(L, j );	// Kopiere alle Argumente hoch
    // Call ist hier OK, da createInstanceByCall immer nur von Lua selber aufgerufen wird
    lua_call(L, numOfArgs - 1, 1);
    return 1;
}

void ValueBindingBase::addMetaMethodImp( lua_State *L,  const char* name, lua_CFunction f, const char* typeName )
{
	StackTester test( L, 0 );
	luaL_getmetatable( L, typeName );
    const int metaTable = lua_gettop( L );
    if( !lua_istable( L, metaTable ) )
        throw Exception( "not a metatable" );
    lua_pushstring(L, name );
    lua_pushcfunction(L, f );
    lua_rawset(L, metaTable);
    lua_pop(L, 1);  // drop meta
}

void ValueBindingBase::addMetaMethodsImp( lua_State *L,  const luaL_reg* ms, const char* typeName )
{
	StackTester test( L, 0 );
	luaL_getmetatable( L, typeName );
    const int metaTable = lua_gettop( L );
    if( !lua_istable( L, metaTable ) )
        throw Exception( "not a metatable" );

    for( const luaL_reg* l = ms; l && l->name; l++ )
    {
        lua_pushstring(L, l->name);
        lua_pushcfunction(L, l->func );
        lua_rawset(L, metaTable);
    }
    lua_pop(L, 1);  // drop meta
}

void ValueBindingBase::addMethodImp(lua_State *L, const char *name, lua_CFunction f, const char *typeName)
{
	StackTester test( L, 0 );
	luaL_getmetatable( L, typeName );
    const int metaTable = lua_gettop( L );
    if( !lua_istable( L, metaTable ) )
        throw Exception( "not a metatable" );
	lua_rawgeti( L, metaTable, MethodTable );
    const int methodTable = lua_gettop(L);
    // Stack: metaTable, methodTable
    if( !lua_istable( L, methodTable ) )
        throw Exception( "no method table" );
#ifdef _DEBUG
	lua_pushstring(L, name);
	lua_rawget(L, methodTable);
	if( !lua_isnil( L, -1 ) )
	{
		qWarning() << "ValueBindingBase::addMethodImp: overwriting method" << name << "of" << typeName;
	}
	lua_pop( L, 1 );
#endif
	lua_pushstring(L, name );
    lua_pushcfunction(L, f );
    lua_rawset(L, methodTable);
    lua_pop(L, 2);  // drop meta und methodTable
}

void ValueBindingBase::addMethodsImp( lua_State *L,  const luaL_reg* ms, const char* typeName )
{
	StackTester test( L, 0 );
	luaL_getmetatable( L, typeName );
    const int metaTable = lua_gettop( L );
    if( !lua_istable( L, metaTable ) )
        throw Exception( "not a metatable" );

	lua_rawgeti( L, metaTable, MethodTable );
    const int methodTable = lua_gettop(L);
    // Stack: metaTable, methodTable
    if( !lua_istable( L, methodTable ) )
        throw Exception( "no method table" );

    for( const luaL_reg* l = ms; l && l->name; l++ )
    {
#ifdef _DEBUG
		lua_pushstring(L, l->name);
		lua_rawget(L, methodTable);
		if( !lua_isnil( L, -1 ) )
		{
			qWarning() << "ValueBindingBase::addMethodsImp: overwriting method" << l->name << "of" << typeName;
		}
		lua_pop( L, 1 );
#endif
        lua_pushstring(L, l->name);
        lua_pushcfunction(L, l->func );
        lua_rawset(L, methodTable);
    }
    lua_pop(L, 2);  // drop meta und methodTable
}

int ValueBindingBase::tostring( lua_State *L )
{
    void* obj = lua_touserdata( L, 1 );
    if( !lua_getmetatable( L, 1 ) )
        lua_pushstring(L,"<unknown class>");
    else
    {
		lua_rawgeti(L, -1, ClassName );
        lua_remove(L, -2 ); // entferne Metatable
    }
    lua_pushfstring( L, " %p", obj );
    lua_concat( L, 2 );
    return 1;
}
