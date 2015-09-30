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

#include "LuaDlg2.h"
#include <Script/Lua.h>
#include <Root/Application.h>
#include <Root/MessageLog.h>
#include <Root/Object.h>
#include <Lexi/Viewport.h>
#include <Spec/Repository.h>
#include "LuaCanvas2.h"
#include "LuaPlot2.h"
#include <LuaSpec2/LuaSpec2.h>
#include <Spec/Factory.h>
#include <SpecView/DynValueEditor.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <QFileDialog>
#include <qfileinfo.h> 
#include <q3progressdialog.h>
#include <qapplication.h>
#include <qcolordialog.h>
#include <qfontdialog.h>
using namespace Lua;
using namespace Spec;

static Q3ProgressDialog* s_progress = 0;	// TODO: Threading
static int s_use = 0;	// Anzahl aufgerufener Begin

static int getText(lua_State *L) 
{
	int n = lua_gettop(L);  /* number of arguments */
	QString title = "Lua Text Input";
	QString msg = "Please enter a string:";
	QString val = QString::null;
	if( n > 0 )
		title = lua_tostring( L, 1 );
	if( n > 1 )
		msg = lua_tostring( L, 2 );
	if( n > 2 )
		val = lua_tostring( L, 3 );
	bool ok = false;
	val = QInputDialog::getText( title, msg, QLineEdit::Normal, val, &ok );
	if( ok && !val.isEmpty() )
		lua_pushstring( L, val ); 
	else
		lua_pushnil( L );
	return 1;
}

static int getDecimal( lua_State *L )
{
	int n = lua_gettop(L);  /* number of arguments */
	QString title = "Lua Decimal Input";
	QString msg = "Please enter a decimal number:";
	double val = 0;
	if( n > 0 )
		title = lua_tostring( L, 1 );
	if( n > 1 )
		msg = lua_tostring( L, 2 );
	if( n > 2 )
		val = lua_tonumber( L, 3 );
	bool ok = false;
	QString tmp;
	tmp.sprintf( "%0.4f", val );
	while( true )
	{
		tmp = QInputDialog::getText( title, msg, QLineEdit::Normal, tmp, &ok );
		if( !ok || tmp.isEmpty() )
		{
			lua_pushnil( L );
			return 1;
		}else
		{
			val = tmp.toDouble( &ok );
			if( ok )
			{
				lua_pushnumber( L, val );
				return 1;
			}else
			{
				QMessageBox::critical( 0, title, "Invalid decimal number! Try again.", "&Cancel" );
				lua_pushnil( L );
			}
		}
	}
	return 1;
}

static int getInteger( lua_State *L )
{
	int n = lua_gettop(L);  /* number of arguments */
	QString title = "Lua Integer Input";
	QString msg = "Please enter a integer number:";
	int val = 0;
	if( n > 0 )
		title = lua_tostring( L, 1 );
	if( n > 1 )
		msg = lua_tostring( L, 2 );
	if( n > 2 )
		val = lua_tonumber( L, 3 );
	bool ok = false;
	QString tmp;
	tmp.sprintf( "%d", val );
	while( true )
	{
		tmp = QInputDialog::getText( title, msg, QLineEdit::Normal, tmp, &ok );
		if( !ok || tmp.isEmpty() )
		{
			lua_pushnil( L );
			return 1;
		}else
		{
			val = tmp.toInt( &ok );
			if( ok )
			{
				lua_pushnumber( L, val );
				return 1;
			}else
			{
				QMessageBox::critical( 0, title, "Invalid integer number! Try again.", "&Cancel" );
				lua_pushnil( L );
			}
		}
	}
	return 1;
}

static int getSymbol( lua_State *L )
{
	int n = lua_gettop(L);  /* number of arguments */
	if( n < 2 )
		luaL_error( L, "Expecting at least two string arguments" );
	QString title = "Lua Symbol Selector";
	QString msg = "Please select a symbol from the list:";
	if( n > 0 )
		title = lua_tostring( L, 1 );
	if( n > 1 )
		msg = lua_tostring( L, 2 );
	QStringList lst;
	for( int i = 3; i <= n; i++ )
		lst << lua_tostring( L, i );
	bool ok = false;
	QString res = QInputDialog::getItem( title, msg, lst, 0, false, &ok );
	if( ok )
		lua_pushstring( L, res ); 
	else
		lua_pushnil( L );
	return 1;
}

static int showInfo( lua_State *L )
{
	int n = lua_gettop(L);  /* number of arguments */
	if( n < 2 )
		luaL_error( L, "Expecting at least two string arguments" );
	QString title = lua_tostring( L, 1 );
	QString msg = lua_tostring( L, 2 );
	QString but0 = QString::null;
	QString but1 = QString::null;
	QString but2 = QString::null;
	int esc = 0;
	if( n > 2 )
	{
		but0 = lua_tostring( L, 3 );
	}
	if( n > 3 )
	{
		but1 = lua_tostring( L, 4 );
		esc++;
	}
	if( n > 4 )
	{
		but2 = lua_tostring( L, 5 );
		esc++;
	}
	int res = QMessageBox::information( 0, title, msg, but0, but1, but2, 0, esc );
	lua_pushnumber( L, res + 1 );
	return 1;
}

static int showWarning( lua_State *L )
{
	// 90% redundant mit showInfo
	int n = lua_gettop(L);  /* number of arguments */
	if( n < 2 )
		luaL_error( L, "Expecting at least two string arguments" );
	QString title = lua_tostring( L, 1 );
	QString msg = lua_tostring( L, 2 );
	QString but0 = QString::null;
	QString but1 = QString::null;
	QString but2 = QString::null;
	int esc = 0;
	if( n > 2 )
	{
		but0 = lua_tostring( L, 3 );
	}
	if( n > 3 )
	{
		but1 = lua_tostring( L, 4 );
		esc++;
	}
	if( n > 4 )
	{
		but2 = lua_tostring( L, 5 );
		esc++;
	}
	int res = QMessageBox::warning( 0, title, msg, but0, but1, but2, 0, esc );
	lua_pushnumber( L, res + 1 );
	return 1;
}

static int showError( lua_State *L )
{
	// 90% redundant mit showInfo
	int n = lua_gettop(L);  /* number of arguments */
	if( n < 2 )
		luaL_error( L, "Expecting at least two string arguments" );
	QString title = lua_tostring( L, 1 );
	QString msg = lua_tostring( L, 2 );
	QString but0 = QString::null;
	QString but1 = QString::null;
	QString but2 = QString::null;
	int esc = 0;
	if( n > 2 )
	{
		but0 = lua_tostring( L, 3 );
	}
	if( n > 3 )
	{
		but1 = lua_tostring( L, 4 );
		esc++;
	}
	if( n > 4 )
	{
		but2 = lua_tostring( L, 5 );
		esc++;
	}
	int res = QMessageBox::critical( 0, title, msg, but0, but1, but2, 0, esc );
	lua_pushnumber( L, res + 1 );
	return 1;
}

static int openCanvas(lua_State *L) 
{
	ObjectHelper<LuaCanvas2>::create( L, new LuaCanvas2(0) );
	return 1;
}

static int getSaveFileName(lua_State *L) 
{
	QString title;
	if( lua_gettop(L) > 0 )
		title = luaL_checkstring( L, 1 );
	QString pattern;
	if( lua_gettop(L) > 1 )
		pattern = luaL_checkstring( L, 2 );
	QString fileName = QFileDialog::getSaveFileName( 0, title,
		Root::AppAgent::getCurrentDir(), pattern );
	if( fileName.isNull() ) 
		lua_pushnil( L );
	else
	{
		QFileInfo info( fileName );
		Root::AppAgent::setCurrentDir( info.dirPath( true ) );
		lua_pushstring( L, fileName );
	}
	return 1;
}

static int getCurrentDir(lua_State *L) 
{
	lua_pushstring( L, Root::AppAgent::getCurrentDir() ); 
	return 1;
}

static int setCurrentDir(lua_State *L) 
{
	Root::AppAgent::setCurrentDir( luaL_checkstring( L, 1 ) );
	return 0;
}

static int getOpenFileName(lua_State *L) 
{
	QString title;
	if( lua_gettop(L) > 0 )
		title = luaL_checkstring( L, 1 );
	QString pattern;
	if( lua_gettop(L) > 1 )
		pattern = luaL_checkstring( L, 2 );
	QString fileName = QFileDialog::getOpenFileName( 0, title,
		Root::AppAgent::getCurrentDir(), pattern );
	if( fileName.isNull() ) 
		lua_pushnil( L );
	else
	{
		QFileInfo info( fileName );
		Root::AppAgent::setCurrentDir( info.dirPath( true ) );
		lua_pushstring( L, fileName );
	}
	return 1;
}

static int logWarning(lua_State *L) 
{
	Root::MessageLog::inst()->warning( "Lua", luaL_checkstring( L, 1 ) );
	return 0;
}

static int logError(lua_State *L) 
{
	Root::MessageLog::inst()->error( "Lua", luaL_checkstring( L, 1 ) );
	return 0;
}

static int logInfo(lua_State *L) 
{
	Root::MessageLog::inst()->info( "Lua", luaL_checkstring( L, 1 ) );
	return 0;
}

static int doscript(lua_State *L)
{
	Repository* rep = LuaSpec2::getRepository( L );
	assert( rep );
	const char* name = luaL_checkstring( L, 1 );
	Repository::ScriptMap::const_iterator p = rep->getScripts().find( name );
	if( p == rep->getScripts().end() )
		luaL_error( L, "Cannot find script %s", name );
	Script* s = (*p).second;

	int status = luaL_loadbuffer( L, s->getCode(), strlen( s->getCode() ), name );
	if( status == 0 )
	{
		status = lua_pcall( L, 0, LUA_MULTRET, 0 );
	}
	if( status != 0 )
	{
		// Auf Stack befindet sich die Fehlermeldung
		luaL_error( L, lua_tostring( L, -1) );
	}
	s->setCompiled();
	return 0;
}

static int require(lua_State *L)
{
	Repository* rep = LuaSpec2::getRepository( L );
	assert( rep );
	const char* name = luaL_checkstring( L, 1 );
	Repository::ScriptMap::const_iterator p = rep->getScripts().find( name );
	if( p == rep->getScripts().end() )
		luaL_error( L, "Cannot find script %s", name );
	Script* s = (*p).second;

	if( s->isCompiled() )
		return 0;	// Nur ausführen, wenn nicht bereits ausgeführt.
	int status = luaL_loadbuffer( L, s->getCode(), strlen( s->getCode() ), name );
	if( status == 0 )
	{
		status = lua_pcall( L, 0, LUA_MULTRET, 0 );
	}
	if( status != 0 )
	{
		// Auf Stack befindet sich die Fehlermeldung
		luaL_error( L, lua_tostring( L, -1) );
	}
	s->setCompiled();
	// TODO: es soll package.loaded[modname] zurückgegeben werden
	return 0;
}

static int beginProgress(lua_State *L)
{
	if( s_progress == 0 )
	{
		assert( s_use == 0 );
		if( lua_gettop(L) > 0 )
			luaL_checkstring( L, 1 );
		s_progress = new Q3ProgressDialog( 0, 0, true );
		s_progress->setAutoClose( false );
		s_progress->setAutoReset( false );
		s_progress->setMinimumDuration( 0 );
		s_progress->setProgress( 0 );
		s_use++;
		if( lua_gettop(L) > 0 )
			s_progress->setCaption( lua_tostring( L, 1 ) );
		if( lua_gettop(L) > 1 )
			s_progress->setLabelText( lua_tostring( L, 2 ) );
		if( lua_gettop(L) > 2 )
			s_progress->setCancelButtonText( lua_tostring( L, 3 ) );
		qApp->processEvents();
	}
	return 0;
}

static int endProgress(lua_State *L)
{
	if( s_progress == 0 )
		luaL_error( L, "Forgot to call dlg.beginProgress" );
	bool kill = false;
	if( lua_gettop(L) > 0 )
	{
		if( !lua_isboolean( L, 1 ) )
			luaL_error( L, "Expecting boolean argument 1" );
		kill = lua_toboolean( L, 1 );
	}
	if( kill )
		s_use = 0;
	else
		s_use--;
	if( s_use <= 0 )
	{
		delete s_progress;
		s_progress = 0;
		s_use = 0;
	}
	return 0;
}

static int updateProgress(lua_State *L)
{
	/* TEST
	if( lua_gettop(L) > 1 )
	{
		const char* str = luaL_checklstring( L, 2, 0 );
		
		qDebug( QString( "Progress: %1 %2" ).arg( luaL_checknumber( L, 1 ) ).
			arg( str ).toLatin1().data() );
	}else if( lua_gettop(L) > 0 )
		qDebug( "Progress: %d", luaL_checknumber( L, 1 ) );
	else
		qDebug( "Progress: ?" );
	return 0; 
	*/

	if( s_progress == 0 )
		luaL_error( L, "Forgot to call dlg.beginProgress" );
	qApp->processEvents();
	if( lua_gettop(L) > 0 )
		s_progress->setProgress( luaL_checknumber( L, 1 ) );
	if( lua_gettop(L) > 1 )
		s_progress->setLabelText( luaL_checklstring( L, 2, 0 ) );
	qApp->processEvents();
	if( s_progress->wasCanceled() )
	{
		delete s_progress;
		s_progress = 0;
		s_use = 0;
		luaL_error( L, "Execution canceled by user" );
	}
	return 0;
}

static int loadImage( lua_State* L )
{
	const char* path = luaL_checkstring( L, 1 );
	const char* format = 0;
	if( lua_gettop(L) > 1 )
		format = luaL_checkstring( L, 2 );
	QImage* img = ValueHelper<QImage>::create( L );
	if( !img->load( path, format ) )
		luaL_error( L, "Error loading image from file" );
	return 1;
}

static int createIcon( lua_State* L )
{
	const char* path = luaL_checkstring( L, 1 );
	QPixmap* img = ValueHelper<QPixmap>::create( L );
	const unsigned char * data = (const unsigned char *)path;
	if( !img->loadFromData( data, ::strlen( path ), "XPM" ) )
		luaL_error( L, "Error creating icon from string" );
	return 1;
}

static int loadIcon( lua_State* L )
{
	const char* path = luaL_checkstring( L, 1 );
	const char* format = 0;
	if( lua_gettop(L) > 1 )
		format = luaL_checkstring( L, 2 );
	QPixmap* img = ValueHelper<QPixmap>::create( L );
	if( !img->load( path, format ) )
		luaL_error( L, "Error loading icon from file" );
	return 1;
}

static int createContour( lua_State* L )
{
	Buffer* buf = 0;
	if( lua_gettop(L) > 0 )
		buf = ValueHelper<Buffer>::check( L, 1 );
	if( buf && buf->getDimCount() != 2 )
		luaL_error( L, "Buffer must have two dimensions" );
	LuaContourPlot2* plot = ValueInstaller<LuaContourPlot2>::create( L );
	if( buf )
		plot->model()->setBuffer( *buf );
	return 1;
}

static int createSlice( lua_State* L )
{
	int dim = luaL_checknumber( L, 1 ) - 1;
	if( dim != 0 && dim != 1 )
		luaL_error( L, "Invalid dimension" );
	Buffer* buf = 0;
	if( lua_gettop(L) > 1 )
		buf = ValueHelper<Buffer>::check( L, 2 );
	if( buf && buf->getDimCount() != 1 )
		luaL_error( L, "Buffer must have one dimension" );
	LuaSlicePlot2* plot = ValueInstaller<LuaSlicePlot2>::create( L );
	plot->init( dim );
	if( buf )
		plot->model()->setBuffer( *buf );
	return 1;
}

static int selectColor( lua_State* L )
{
	QColor clr;
	if( lua_gettop(L) > 0 )
		clr.setNamedColor( luaL_checkstring( L, 1 ) );
	clr = QColorDialog::getColor( clr );
	if( clr.isValid() )
		lua_pushstring( L, clr.name() ); 
	else
		lua_pushnil( L );
	return 1;
}

static int selectFont( lua_State* L )
{
	bool ok;
	QFont f;
	if( lua_gettop(L) > 0 )
	{
		f = QFont( luaL_checkstring( L, 1 ),	// Name
		luaL_checknumber( L, 2 ),			// pointsize
		( luaL_checknumber( L, 3 ) )?QFont::Bold:QFont::Normal,	// Bold
		luaL_checknumber( L, 4 ) );			// Italic
	}
	f = QFontDialog::getFont( &ok, f, 0 );
	if( !ok )
	{
		lua_pushnil( L );
		lua_pushnil( L );
		lua_pushnil( L );
		lua_pushnil( L );
	}else
	{
		lua_pushstring( L, f.family() );
		lua_pushnumber( L, f.pointSize() );
		lua_pushboolean( L, f.bold() );
		lua_pushboolean( L, f.italic() );
	}
	return 4;
}

static int pushCursor( lua_State* L )
{
	Lexi::Viewport::Cursor c = (Lexi::Viewport::Cursor)(int)luaL_checknumber( L, 1 );
	Lexi::Viewport::pushCursor( c );
	return 0;
}

static int pushHourglass( lua_State* L )
{
	Lexi::Viewport::pushHourglass();
	return 0;
}

static int popCursor( lua_State* L )
{
	Lexi::Viewport::popCursor();
	return 0;
}

static int openAttributeEditor( lua_State* L )
{
	// Params: Object, [string]
	Repository* rep = LuaSpec2::getRepository( L );
	assert( rep );
	Root::Object* obj = 0; 
	ObjectDef* od = 0;
	if( lua_gettop(L) > 1 )
	{
		const char* name = luaL_checkstring( L, 2 );
		od = rep->findObjectDef( name );
		if( od == 0 )
			luaL_error( L, "Cannot find attribute definitions for class %s", name );
	}
	DynValueEditor::edit( 0, od, obj );
	return 0;
}

static const luaL_reg dlglib[] = 
{
	{ "openAttributeEditor", openAttributeEditor },
	{ "pushCursor", pushCursor },
	{ "pushHourglass", pushHourglass },
	{ "popCursor", popCursor },
	{ "selectFont", selectFont },
	{ "selectColor", selectColor },
	{ "createSlice", createSlice },
	{ "createContour", createContour },
	{ "createIcon", createIcon },
	{ "loadIcon", loadIcon },
	{ "loadImage", loadImage },
	{ "beginProgress", beginProgress },
	{ "endProgress", endProgress },
	{ "updateProgress", updateProgress },

	{ "require", require },
	{ "doscript", doscript },
	{ "logWarning", logWarning },
	{ "logError", logError },
	{ "logInfo", logInfo },

	{ "getSaveFileName", getSaveFileName },
	{ "getCurrentDir", getCurrentDir },
	{ "setCurrentDir", setCurrentDir },
	{ "getOpenFileName", getOpenFileName },
	{ "createCanvas", openCanvas },	// RISK
	{ "showError", showError },
	{ "showWarning", showWarning },
	{ "showInfo", showInfo },
	{ "getSymbol", getSymbol },
	{ "getInteger", getInteger },
	{ "getDecimal", getDecimal },
	{ "getText", getText },
	{ 0, 0 }
};

int LuaDlg2::install(lua_State * L)
{
	luaL_openlib( L, "dlg", dlglib, 0 );
	LuaCanvas2::install( L );
	LuaContourPlot2::install( L );
	return 1;
}

static const luaL_reg loglib[] = 
{
	{ "warning", logWarning },
	{ "error", logError },
	{ "info", logInfo },
	{ 0, 0 }
};

static const luaL_reg scriptlib[] = 
{
	{ "require", require },
	{ "execute", doscript },
	{ 0, 0 }
};

int LuaDlg2::install2(lua_State * L)
{
	luaL_openlib( L, "log", loglib, 0 );
	luaL_openlib( L, "script", scriptlib, 0 );
	return 1;
}
