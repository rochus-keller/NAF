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

#include <QLineEdit>
#include <QValidator>
#include "QtlLineEdit.h"

#include <QCompleter>
#include <QMenu>
#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>

using namespace Qtl;
using namespace Lua;


int LineEdit::init(lua_State * L)
{
	return 0;
}
int LineEdit::backspace(lua_State * L) 
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1);
	obj->backspace();
	return 0;
}
int LineEdit::completer(lua_State * L) // const : QCompleter
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->completer(), true);
	return 1;
}
int LineEdit::createStandardContextMenu(lua_State * L) //  : QMenu
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->createStandardContextMenu(), true);
	return 1;
}
int LineEdit::cursorBackward(lua_State * L) //  ( bool mark, int steps = 1 )
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1);
	obj->cursorBackward( Util::toBool( L, 2 ), Util::toInt( L, 3 ));
	return 0;
}
int LineEdit::cursorForward(lua_State * L) //  ( bool mark, int steps = 1 )
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1);
	obj->cursorForward( Util::toBool( L, 2 ), Util::toInt( L, 3 ));
	return 0;
}
int LineEdit::cursorPositionAt(lua_State * L) //  ( const QPoint & pos )
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1);
	QPointF* p = ValueInstaller2<QPointF>::check( L, 2);
	obj->cursorPositionAt( p->toPoint() );
	return 0;
}
int LineEdit::cursorWordBackward(lua_State * L) //  ( bool mark )
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1);
	obj->cursorBackward( Util::toBool( L, 2 ) );
	return 0;
}
int LineEdit::cursorWordForward(lua_State * L) //  ( bool mark )
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1);
	obj->cursorWordForward( Util::toBool( L, 2 ) );
	return 0;
}
int LineEdit::del(lua_State * L) 
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1);
	obj->del();
	return 0;
}
int LineEdit::deselect(lua_State * L) 
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1);
	obj->deselect();
	return 0;
}
int LineEdit::displayText(lua_State * L) // const : QString
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1);
	//QString* res = ValueInstaller2<QString>::create( L );
	//*res = obj->displayText();
	Util::push( L, obj->displayText() );
	return 1;
}
int LineEdit::end(lua_State * L) //  ( bool mark )
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1);
	obj->end(Util::toBool( L, 2 ) );
	return 0;
}
int LineEdit::home(lua_State * L) //  ( bool mark )
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1);
	obj->home(Util::toBool( L, 2 ) );
	return 0;
}
int LineEdit::insert(lua_State * L) //  ( const QString & newText )
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1);
	// QString* newText = ValueInstaller2<QString>::check( L, 2);
	obj->insert( Util::toString( L, 2 ) );
	return 0;
}
int LineEdit::minimumSizeHint(lua_State * L) // const : QSize
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1);
	QSizeF* res = ValueInstaller2<QSizeF>::create( L );
	*res = obj->minimumSizeHint();
	return 0;
}
int LineEdit::selectionStart(lua_State * L) // const : int
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1);
	Lua::Util::push( L, obj->selectionStart() );
	return 1;
}
int LineEdit::setCompleter(lua_State * L) //  ( bool mark )
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1);
	QCompleter* res = ValueInstaller2<QCompleter>::check( L, 2);
	obj->setCompleter( res );
	return 0;
}
int LineEdit::setSelection(lua_State * L) //  ( int start, int length )
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1);
	obj->setSelection( Util::toInt( L, 2 ), Util::toInt( L, 3 ) );
	return 0;
}
int LineEdit::setValidator(lua_State * L) //  ( const QValidator * v )
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1);
	QValidator* res = ValueInstaller2<QValidator>::check( L, 2);
	obj->setValidator( res );
	return 0;
}
int LineEdit::sizeHint(lua_State * L) // const : QSize
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1);
	QSizeF* res = ValueInstaller2<QSizeF>::create( L );
	*res = obj->sizeHint();
	return 1;
}
int LineEdit::validator(lua_State * L) // const : QValidator 
{
	QLineEdit* obj = ObjectHelper<QLineEdit>::check( L, 1);
	//QValidator* res = ValueInstaller2<QValidator>::create( L );
	//res = obj->validator();
	ObjectPeer::pushPeer( L, const_cast<QValidator*>( obj->validator() ) );
	return 1;
}
static const luaL_reg _LineEdit[] = 
{
	{ "backspace", LineEdit::backspace },
	{ "completer", LineEdit::completer },
	{ "createStandardContextMenu", LineEdit::createStandardContextMenu },
	{ "cursorBackward", LineEdit::cursorBackward },
	{ "cursorForward", LineEdit::cursorForward },
	{ "cursorPositionAt", LineEdit::cursorPositionAt },
	{ "cursorWordBackward", LineEdit::cursorWordBackward },
	{ "cursorWordForward", LineEdit::cursorWordForward },
	{ "del", LineEdit::del },
	{ "deselect", LineEdit::deselect },
	{ "displayText", LineEdit::displayText },
	{ "end", LineEdit::end },
	{ "home", LineEdit::home },
	{ "insert", LineEdit::insert },
	{ "minimumSizeHint", LineEdit::minimumSizeHint },
	{ "selectionStart", LineEdit::selectionStart },
	{ "setCompleter", LineEdit::setCompleter },
	{ "setSelection", LineEdit::setSelection },
	{ "setValidator", LineEdit::setValidator },
	{ "sizeHint", LineEdit::sizeHint },
	{ "validator", LineEdit::validator },//check
	{ Util::s_init, LineEdit::init },
	{ 0, 0 }
};
void LineEdit::install(lua_State * L){
	ObjectInstaller<QLineEdit>::install( L, _LineEdit, ObjectInstaller<QWidget>::className());
}
