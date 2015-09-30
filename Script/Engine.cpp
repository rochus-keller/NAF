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

#include "Engine.h"
#include <Script/Lua.h>
#include <Root/Mem.h>
#include <QCoreApplication>
#include <math.h>

using namespace Lua;

static Root::ExRef<Lua::Engine> s_this;

static const char* s_path = "path";
static const char* s_cpath = "cpath";
static Engine::Breaks s_dummy;

static int _print (lua_State *L) 
{
    Engine* e = Engine::inst(false);
    Q_ASSERT( e != 0 );
	Engine::Update msg( e, Engine::Update::Print );
	int n = lua_gettop(L);  /* number of arguments */
	int i;
	lua_getglobal(L, "tostring");
	for (i=1; i<=n; i++) 
	{
		const char *s;
		lua_pushvalue(L, -1);  /* function to be called */
		lua_pushvalue(L, i);   /* value to print */
		lua_call(L, 1, 1);
		s = lua_tostring(L, -1);  /* get result */
		if (s == NULL)
			return luaL_error(L, "`tostring' must return a string to `print'");
		if (i>1) 
            msg.d_val1 += "\t";
        msg.d_val1 += s;
		lua_pop(L, 1);  /* pop result */
	}
	try
	{
		e->notifyObservers( msg );
        QCoreApplication::processEvents();
        if( msg.d_cancel )
            luaL_error( L, "Execution canceled by user" );
	}catch( Root::Exception& e )
	{
		luaL_error( L, "Error calling host: %s", e.what() );
	}
	return 0;
}

static void * Allocator(void *ud, void *ptr, size_t osize, size_t nsize) 
{
	(void)ud;
	(void)osize;
	if( nsize == 0 ) 
	{
		Root::Mem::free(ptr);
		return NULL;
	}
	else
		return Root::Mem::realloc( ptr, nsize );
}

static int Panic(lua_State *L)
{
	throw Root::Exception( "Severe Lua Exception" );
}

Engine::Engine():
    d_ctx( 0 ), d_debugging( false ), d_running(false), d_waitForCommand(false),
    d_dbgCmd(RunToBreakPoint), d_defaultDbgCmd(RunToBreakPoint), d_activeLevel(0)
{
#if LUA_VERSION_NUM >= 501
	lua_State* ctx = lua_newstate( Allocator, 0 );
	lua_atpanic( ctx, Panic );
#else
	lua_State* ctx = lua_open();
#endif
	if( ctx == 0 )
		throw Root::Exception( "Not enough memory to create scripting context" );

	d_ctx = ctx;

	addLibrary( BASE );	// Das muss hier stehen, sonst wird ev. print wieder überschrieben

	lua_pushcfunction( ctx, _print );
	lua_setglobal( ctx, "print" );
}

Engine::~Engine()
{
	if( d_ctx )
		lua_close( d_ctx );
}

void Engine::addStdLibs()
{
	addLibrary( TABLE );
	addLibrary( STRING );
	addLibrary( MATH );
}

void Engine::addLibrary(Lib what)
{
	if( d_debugging && d_running )
        return;
	switch( what )
	{
#if LUA_VERSION_NUM >= 501
	case PACKAGE:
		lua_pushcfunction( d_ctx, luaopen_package );
		lua_pushstring(d_ctx, LUA_LOADLIBNAME );
		lua_call(d_ctx, 1, 0);

        // am 28.12.11 Konzept geändert. Neu werden wieder Lua-Standard-Loader verwendet.
		break;
#endif
	case BASE:
		lua_pushcfunction( d_ctx, luaopen_base );
		lua_pushstring(d_ctx, "" );
		lua_call(d_ctx, 1, 0);
		break;
	case REMOVE_LOADS:
		lua_pushnil( d_ctx );
		lua_setglobal( d_ctx, "dofile" );
		lua_pushnil( d_ctx );
		lua_setglobal( d_ctx, "loadfile" );
		lua_pushnil( d_ctx );
		lua_setglobal( d_ctx, "load" );
		lua_pushnil( d_ctx );
		lua_setglobal( d_ctx, "loadstring" );
		break;
	case TABLE:
		lua_pushcfunction( d_ctx, luaopen_table );
		lua_pushstring(d_ctx, LUA_TABLIBNAME );
		lua_call(d_ctx, 1, 0);
		break;
	case STRING:
		lua_pushcfunction( d_ctx, luaopen_string );
		lua_pushstring(d_ctx, LUA_STRLIBNAME );
		lua_call(d_ctx, 1, 0);
		break;
	case MATH:
		lua_pushcfunction( d_ctx, luaopen_math );
		lua_pushstring(d_ctx, LUA_MATHLIBNAME );
		lua_call(d_ctx, 1, 0);
		break;
	case IO:
		lua_pushcfunction( d_ctx, luaopen_io );
		lua_pushstring(d_ctx, LUA_IOLIBNAME );
		lua_call(d_ctx, 1, 0);
		break;
#if LUA_VERSION_NUM >= 501
	case OS:
		lua_pushcfunction( d_ctx, luaopen_os );
		lua_pushstring(d_ctx, LUA_OSLIBNAME );
		lua_call(d_ctx, 1, 0);
		break;
#endif
	case DBG:
        lua_pushcfunction( d_ctx, luaopen_debug );
        lua_pushstring(d_ctx, LUA_DBLIBNAME );
        lua_call(d_ctx, 1, 0);
        break;
        break;
    default:
        break;
	}
}

bool Engine::pushFunction(const QByteArray &source, const QByteArray& name )
{
	d_lastError = "";
    const int status = luaL_loadbuffer( d_ctx, source, source.size(), name );
    switch( status )
    {
    case 0:
        // Stack: function
        break;
    case LUA_ERRSYNTAX:
    case LUA_ERRMEM:
        d_lastError = lua_tostring( d_ctx, -1 );
        lua_pop( d_ctx, 1 );  /* remove error message */
        // Stack: -
        return false;
    }
    return true;
}

const char* Engine::getVersion() const
{
#if LUA_VERSION_NUM >= 501
	return LUA_RELEASE;
#else
	return lua_version();
#endif
}

static int file_writer (lua_State *L, const void* b, size_t size, void* f)
{
	Q_UNUSED(L);
	int res = ::fwrite( b, size, 1, (FILE*)f );
	if( res != 1 )
		return -1; // = error
	return 0; // = no error
}

bool Engine::saveBinary(const QByteArray& source, const QByteArray& path)
{
    d_lastError.clear();
    FILE* f = ::fopen( path,"wb" );
    if( f == 0 )
    {
        d_lastError = "Unable to open file for writing";
        return false;
    }
    if( !pushFunction( source ) )
        return false;

    // Stack: Function
	const int res = lua_dump( d_ctx, file_writer, f);
    ::fclose( f );
    lua_pop( d_ctx, 1 ); // Function
#if LUA_VERSION_NUM >= 501
    if( res != 0 )
#else
    if( res != 1 )
#endif
    {
        d_lastError = "Unable to write compiled script";
        return false;
    }else
		return true;
}

static int array_writer (lua_State *L, const void* b, size_t size, void* f)
{
	Q_UNUSED(L);
	QByteArray* ba = (QByteArray*)f;
	ba->append( QByteArray( (const char *) b, size ) );
	return 0; // = no error
}

QByteArray Engine::getBinaryFromFunc(lua_State *L)
{
	if( lua_type(L, -1) != LUA_TFUNCTION )
		return QByteArray();

	QByteArray ba;
	const int res = lua_dump( L, array_writer, &ba );
#if LUA_VERSION_NUM >= 501
	if( res != 0 )
#else
	if( res != 1 )
#endif
		ba.clear();
	return ba;
}

bool Engine::executeCmd(const QByteArray &source, const QByteArray &name)
{
	if( d_debugging && d_running )
    {
        d_lastError = "Cannot run commands while another script is running!";
        error( d_lastError );
        return false;
    }
    d_lastError.clear();
    d_waitForCommand = false;
    if( !pushFunction( source, name ) )
    {
        error( d_lastError );
        return false;
    }
    const bool res = runFunction( 0, LUA_MULTRET );
    if( !res )
        error( d_lastError );
    return res;
}

bool Engine::executeFile(const QByteArray &path)
{
	if( d_debugging && d_running )
    {
        d_lastError = "Cannot run script while another script is running!";
        error( d_lastError );
        return false;
    }
    d_lastError.clear();
    d_waitForCommand = false;
    switch( luaL_loadfile( d_ctx, path ) )
    {
    case 0: // no Error
        break;
    case LUA_ERRFILE:
    case LUA_ERRSYNTAX:
    case LUA_ERRMEM:
        d_lastError = lua_tostring( d_ctx, -1);
        lua_pop( d_ctx, 1 );  /* remove error message */
        error( d_lastError );
        return false;
    }
    const bool res = runFunction( 0, LUA_MULTRET );
    if( !res )
        error( d_lastError );
    return res;
}

bool Engine::runFunction(int nargs, int nresults)
{
	if( d_debugging && d_running )
    {
        d_lastError = "Cannot call Lua function while script is running!";
        lua_pop( d_ctx, nargs + 1 ); // funktion + args
        return false;
    }
    d_lastError.clear();
    d_running = true;
    d_dbgCmd = d_defaultDbgCmd;
    notifyStart();
    // TODO: ev. Stacktrace mittels errfunc
    const int err = lua_pcall( d_ctx, nargs, nresults, 0 );
    d_running = false;
    switch( err )
    {
    case LUA_ERRRUN:
        d_lastError = lua_tostring( d_ctx, -1 );
        lua_pop( d_ctx, 1 );  /* remove error message */
        break;
    case LUA_ERRMEM:
        d_lastError = "Lua memory exception";
        break;
    case LUA_ERRERR:
        // should not happen
        d_lastError = "Lua unknown error";
        break;
    }
	notifyEnd();
	return (err == 0);
}

void Engine::collect()
{
#if LUA_VERSION_NUM >= 501
	if( d_ctx )
		lua_gc( d_ctx, LUA_GCCOLLECT, 0 );
#else
	if( d_ctx )
		lua_setgcthreshold( d_ctx,0 ); 
#endif
}

void Engine::setActiveLevel(int level, const QByteArray &script, int line)
{
    if( d_activeLevel == level )
        return;
    d_activeLevel = level;
    Engine::Update msg( this, Engine::Update::ActiveLevel, script, line );
    notifyObservers( msg );
}

void Engine::debugHook(lua_State *L, lua_Debug *ar)
{
    if( ar->event != LUA_HOOKLINE )
        return;

	Engine* e = Engine::inst();
	lua_getinfo( L, "Sf", ar );
    const QByteArray source = ar->source;
    const bool lineChanged = ( e->d_curLine != ar->currentline || e->d_curScript != source );
    if( lineChanged )
    {
        e->d_breakHit = false;
        e->d_curScript = source;
        e->d_curLine = ar->currentline;
        e->d_activeLevel = 0;
		if( e->d_curScript.startsWith('#') )
			e->d_curBinary = getBinaryFromFunc( L );
		lua_pop(L, 1); // Function


        if( e->isStepping() )
        {
            e->d_waitForCommand = true;
            Engine::Update msg( e, Engine::Update::LineHit, e->d_curScript, e->d_curLine );
            e->notifyObservers( msg );
        }else if( e->d_breaks.value( source ).contains( e->d_curLine ) )
        {
            e->d_waitForCommand = true;
            e->d_breakHit = true;
            Engine::Update msg( e, Engine::Update::BreakHit, e->d_curScript, e->d_curLine );
            e->notifyObservers( msg );
        }
		if( e->d_dbgCmd == Abort || e->d_dbgCmd == AbortSilently )
        {
            luaL_error( L, "Execution terminated by user" );
		}else
		{
			Engine::Update msg( e, Engine::Update::Continue );
			e->notifyObservers( msg );
		}
	}else
		lua_pop(L, 1); // function
}

void Engine::notifyStart()
{
    Engine::Update msg( this, Engine::Update::Start );
//	if( script ) TODO
//		msg.d_title = title;
    notifyObservers( msg );
}

void Engine::notifyEnd()
{
	Engine::Update msg( this, (d_dbgCmd == Abort || d_dbgCmd == AbortSilently)?
							Engine::Update::Abort:Engine::Update::Finish );
    notifyObservers( msg );
}

#ifdef __unused
static const char* toStr( int i )
{
	switch( i )
	{
	case LUA_TNIL:
		return "nil";
	case LUA_TNUMBER:
		return "number";
	case LUA_TBOOLEAN:
		return "bool";
	case LUA_TSTRING:
		return "string";
	case LUA_TTABLE:
		return "table";
	case LUA_TFUNCTION:
		return "fun";
	case LUA_TUSERDATA:
		return "ud";
	case LUA_TTHREAD:
		return "thread";
	case LUA_TLIGHTUSERDATA:
		return "lud";

	}
	return "?";
}
#endif

void Engine::setDebug(bool on)
{
    if( d_debugging == on )
        return;
    if( on )
        lua_sethook( d_ctx, debugHook, LUA_MASKLINE, 1);
    else
        lua_sethook( d_ctx, debugHook, 0, 0);
    d_debugging = on;

}

void Engine::runToNextLine()
{
    d_dbgCmd = RunToNextLine;
    d_waitForCommand = false;
}

void Engine::runToBreakPoint()
{
    d_dbgCmd = RunToBreakPoint;
    d_waitForCommand = false;
}

void Engine::terminate(bool silent)
{
	d_dbgCmd = (silent)?AbortSilently:Abort;
    d_waitForCommand = false;
}

Engine* Engine::inst(bool create)
{
	if( create && s_this.isNull() )
	{
		s_this = new Engine();
	}
	return s_this;
}

void Engine::addBreak(const QByteArray &s, quint32 l)
{
	d_breaks[s].insert( l );
    Engine::Update msg( this, Engine::Update::BreakPoints );
    msg.d_val1 = s.data();
	notifyObservers( msg );
}

void Engine::removeBreak(const QByteArray & s, quint32 l)
{
	d_breaks[s].remove( l );
    Engine::Update msg( this, Engine::Update::BreakPoints );
    msg.d_val1 = s.data();
	notifyObservers( msg );
}

const Engine::Breaks& Engine::getBreaks(const QByteArray & s) const
{
	BreaksPerScript::const_iterator i = d_breaks.find( s );
	if( i == d_breaks.end() )
		return s_dummy;
	else
		return (*i);
}

void Engine::removeAllBreaks(const QByteArray &s)
{
	if( s.isNull() )
	{
		if( d_breaks.empty() )
			return;
		d_breaks.clear();
        Engine::Update msg( this, Engine::Update::BreakPoints );
	}else
	{
		if( d_breaks[s].empty() )
			return;
		d_breaks[s].clear();
        Engine::Update msg( this, Engine::Update::BreakPoints );
        msg.d_val1 = s.data();
		notifyObservers( msg );
	}
}

void Engine::kill()
{
	s_this = 0;
}

void Engine::print(const char * str)
{
    Engine::Update msg( this, Engine::Update::Print, str );
	// log( str );
	notifyObservers( msg );
}

void Engine::error(const char * str)
{
    Engine::Update msg( this, Engine::Update::Error, str );
	// Doppelt log( str );
#ifdef _DEBUG
	qDebug( "Lua Error: %s", str );
#endif
	notifyObservers( msg );
}

void Engine::setPluginPath( const char* path, bool cpath )
{
    if( cpath )
        lua_pushstring( d_ctx, s_cpath ); // Hier muss der String als Name verwendet werden, nicht die Adresse
	else
        lua_pushstring( d_ctx, s_path ); // Hier muss der String als Name verwendet werden, nicht die Adresse
    lua_pushstring( d_ctx, path );
	lua_rawset( d_ctx, LUA_REGISTRYINDEX );
}

QByteArray Engine::getPluginPath(bool cpath) const
{
    if( cpath )
        lua_pushstring( d_ctx, s_cpath ); // Hier muss der String als Name verwendet werden, nicht die Adresse
    else
        lua_pushstring( d_ctx, s_path ); // Hier muss der String als Name verwendet werden, nicht die Adresse
	lua_rawget( d_ctx, LUA_REGISTRYINDEX );
	const char * path = lua_tostring(d_ctx, -1);
	if( path == 0 )
		return "";
	else
		return path;
}

QByteArray Engine::getTypeName(int arg) const
{
    const int t = lua_type( d_ctx, arg );
    switch( t )
    {
    case LUA_TNIL:
        return "-";
    case LUA_TFUNCTION:
        if( lua_iscfunction( d_ctx, arg ) )
            return "C function";
        else
            return "Lua function";
    case LUA_TUSERDATA:
        {
            if( lua_getmetatable( d_ctx, arg ) )
            {
                // Stack: meta
                lua_pushliteral( d_ctx, "__class" );
                lua_rawget( d_ctx, -2 );
                // Stack: meta, string | nil
                if( !lua_isnil( d_ctx, -1 ) )
                {
                    QByteArray value = lua_tostring( d_ctx, -1 );
                    lua_pop( d_ctx, 2 );
                    return value;
                }else
                    lua_pop( d_ctx, 2 );
            }
        }
        // fall through
    default:
        return lua_typename( d_ctx, t );
    }
    return "<unknown>";
}

static QByteArray _toHex(const void *p)
{
    return "0x" + QByteArray::number((quint32)p, 16 ); // table, thread, function, userdata
}

QByteArray Engine::getValueString(int arg) const
{
    switch( lua_type( d_ctx, arg ) )
    {
    case LUA_TNIL:
        return "nil";
    case LUA_TNUMBER:
        return QByteArray::number( lua_tonumber( d_ctx, arg ) );
    case LUA_TBOOLEAN:
        return ( lua_toboolean( d_ctx, arg ) ) ? "true" : "false";
    case LUA_TSTRING:
        return QByteArray("\"") + lua_tostring( d_ctx, arg ) + "\"";
    case LUA_TTABLE:
    case LUA_TTHREAD:
    case LUA_TFUNCTION:
        return _toHex( lua_topointer( d_ctx, arg) );
    case LUA_TLIGHTUSERDATA:
        return _toHex( lua_touserdata( d_ctx, arg ) );
    case LUA_TUSERDATA:
        {
            if( lua_getmetatable( d_ctx, arg ) )
            {
                // Stack: meta
                lua_pushliteral( d_ctx, "__meta" );
                lua_rawget( d_ctx, -2 );
                const QByteArray meta = lua_tostring( d_ctx, -1 );
                lua_pop( d_ctx, 2 ); // meta, string
                if( !meta.isEmpty() )
                    return __tostring( arg ); // Alle haben eine gültige tostring-Konvertierung
            } // else
            return "<unknown>";
        }
        break;
    }
    return QByteArray();
}

int Engine::pushLocalOrGlobal(const QByteArray &name)
{
    if( d_waitForCommand )
    {
        // Wir können auf den Stack zugreifen
        lua_Debug ar;
        if( lua_getstack( d_ctx, d_activeLevel, &ar ) )
        {
            int n = 1;
            while( const char* localName = lua_getlocal( d_ctx, &ar, n) )
            {
                Q_ASSERT( localName != 0 );
                if( localName == name )
                {
                    return 1;
                }else
                    lua_pop( d_ctx, 1 );
                n++;
            }
        }
    }
    lua_pushstring( d_ctx, name );
    lua_rawget( d_ctx, (d_waitForCommand)?LUA_ENVIRONINDEX:LUA_GLOBALSINDEX );
    return 1;
}

QByteArray Engine::__tostring(int arg) const
{
    if( luaL_callmeta( d_ctx, arg, "__tostring") )
    {
        QByteArray value = lua_tostring( d_ctx, -1 );
        lua_pop( d_ctx, 1 );
        return value;
    }else
        return "<bytes>";
}

void Engine::pop(int count)
{
    lua_pop( d_ctx, count );
}
