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
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script/Util.h>

using namespace Qtl;
using namespace Lua;


int LineEdit::init(lua_State * L)
{
    return DefaultCreateObject<QLineEdit>::init( L );
}
int LineEdit::backspace(lua_State * L) 
{
	QLineEdit* obj = QtObject<QLineEdit>::check( L, 1);
	obj->backspace();
	return 0;
}
int LineEdit::completer(lua_State * L) // const : QCompleter
{
	QLineEdit* obj = QtObject<QLineEdit>::check( L, 1);
    QtObject<QCompleter>::create( L, obj->completer());
	return 1;
}
int LineEdit::createStandardContextMenu(lua_State * L) //  : QMenu
{
	QLineEdit* obj = QtObject<QLineEdit>::check( L, 1);
    QtObject<QMenu>::create( L, obj->createStandardContextMenu() );
	return 1;
}
int LineEdit::cursorBackward(lua_State * L) //  ( bool mark, int steps = 1 )
{
	QLineEdit* obj = QtObject<QLineEdit>::check( L, 1);
	obj->cursorBackward( Util::toBool( L, 2 ), Util::toInt( L, 3 ));
	return 0;
}
int LineEdit::cursorForward(lua_State * L) //  ( bool mark, int steps = 1 )
{
	QLineEdit* obj = QtObject<QLineEdit>::check( L, 1);
	obj->cursorForward( Util::toBool( L, 2 ), Util::toInt( L, 3 ));
	return 0;
}
int LineEdit::cursorPositionAt(lua_State * L) //  ( const QPoint & pos )
{
	QLineEdit* obj = QtObject<QLineEdit>::check( L, 1);
	QPointF* p = QtValue<QPointF>::check( L, 2);
	obj->cursorPositionAt( p->toPoint() );
	return 0;
}
int LineEdit::cursorWordBackward(lua_State * L) //  ( bool mark )
{
	QLineEdit* obj = QtObject<QLineEdit>::check( L, 1);
	obj->cursorBackward( Util::toBool( L, 2 ) );
	return 0;
}
int LineEdit::cursorWordForward(lua_State * L) //  ( bool mark )
{
	QLineEdit* obj = QtObject<QLineEdit>::check( L, 1);
	obj->cursorWordForward( Util::toBool( L, 2 ) );
	return 0;
}
int LineEdit::del(lua_State * L) 
{
	QLineEdit* obj = QtObject<QLineEdit>::check( L, 1);
	obj->del();
	return 0;
}
int LineEdit::deselect(lua_State * L) 
{
	QLineEdit* obj = QtObject<QLineEdit>::check( L, 1);
	obj->deselect();
	return 0;
}
int LineEdit::displayText(lua_State * L) // const : QString
{
	QLineEdit* obj = QtObject<QLineEdit>::check( L, 1);
    *QtValue<QString>::create( L ) = obj->displayText();
	return 1;
}
int LineEdit::end(lua_State * L) //  ( bool mark )
{
	QLineEdit* obj = QtObject<QLineEdit>::check( L, 1);
	obj->end(Util::toBool( L, 2 ) );
	return 0;
}
int LineEdit::home(lua_State * L) //  ( bool mark )
{
	QLineEdit* obj = QtObject<QLineEdit>::check( L, 1);
	obj->home(Util::toBool( L, 2 ) );
	return 0;
}
int LineEdit::insert(lua_State * L) //  ( const QString & newText )
{
	QLineEdit* obj = QtObject<QLineEdit>::check( L, 1);
	// QString* newText = QtValue<QString>::check( L, 2);
    obj->insert( QtValueBase::toString( L, 2 ) );
	return 0;
}
int LineEdit::minimumSizeHint(lua_State * L) // const : QSize
{
	QLineEdit* obj = QtObject<QLineEdit>::check( L, 1);
	QSizeF* res = QtValue<QSizeF>::create( L );
	*res = obj->minimumSizeHint();
	return 0;
}
int LineEdit::selectionStart(lua_State * L) // const : int
{
	QLineEdit* obj = QtObject<QLineEdit>::check( L, 1);
	Lua::Util::push( L, obj->selectionStart() );
	return 1;
}
int LineEdit::setCompleter(lua_State * L) //  ( bool mark )
{
	QLineEdit* obj = QtObject<QLineEdit>::check( L, 1);
	QCompleter* res = QtValue<QCompleter>::check( L, 2);
	obj->setCompleter( res );
	return 0;
}
int LineEdit::setSelection(lua_State * L) //  ( int start, int length )
{
	QLineEdit* obj = QtObject<QLineEdit>::check( L, 1);
	obj->setSelection( Util::toInt( L, 2 ), Util::toInt( L, 3 ) );
	return 0;
}
int LineEdit::setValidator(lua_State * L) //  ( const QValidator * v )
{
	QLineEdit* obj = QtObject<QLineEdit>::check( L, 1);
	QValidator* res = QtValue<QValidator>::check( L, 2);
	obj->setValidator( res );
	return 0;
}
int LineEdit::sizeHint(lua_State * L) // const : QSize
{
	QLineEdit* obj = QtObject<QLineEdit>::check( L, 1);
	QSizeF* res = QtValue<QSizeF>::create( L );
	*res = obj->sizeHint();
	return 1;
}
int LineEdit::validator(lua_State * L) // const : QValidator 
{
	QLineEdit* obj = QtObject<QLineEdit>::check( L, 1);
    QtObject<QValidator>::create( L, const_cast<QValidator*>( obj->validator() ) );
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
	{ "init", LineEdit::init },
	{ 0, 0 }
};
void LineEdit::install(lua_State * L){
    QtObject<QLineEdit,QWidget>::install( L, "QLineEdit", _LineEdit );
}
