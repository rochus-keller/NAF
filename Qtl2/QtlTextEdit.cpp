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

#include <QTextEdit>
#include "QtlTextEdit.h"
#include <QString>
#include <QMenu>
#include <QList>
#include <QPoint>
#include <QUrl>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QInputContext>
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script/Util.h>
#include "Variant.h"
using namespace Qtl;
using namespace Lua;


int TextEdit::alignment(lua_State * L) // const : Qt::Alignment
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
	Lua::Util::push( L, obj->alignment() );
	return 1;
}
int TextEdit::anchorAt(lua_State * L) // ( const QPoint & pos ) const : QString
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
	QPointF* p = QtValue<QPointF>::check( L, 2 );
    *QtValue<QString>::create( L ) = obj->anchorAt( p->toPoint() );
	return 1;
}
int TextEdit::canPaste(lua_State * L) // const : bool
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
	Lua::Util::push( L, obj->canPaste() );
	return 1;
}
int TextEdit::createStandardContextMenu(lua_State * L) // const : QMenu*
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
    QtObject<QMenu>::create( L, obj->createStandardContextMenu() );
	return 1;
}
int TextEdit::currentCharFormat(lua_State * L) // const : QTextCharFormat
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
	QTextCharFormat* res = QtValue<QTextCharFormat>::create( L );
	*res = 	obj->currentCharFormat();
	return 1;
}
int TextEdit::currentFont(lua_State * L) // const : QFont
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
	QFont* res = QtValue<QFont>::create( L );
	*res = 	obj->currentFont();
	return 1;
}/*
int TextEdit::cursorForPosition(lua_State * L) // ( const QPoint & pos ) const : QTextCursor
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
	QPoint* p = QtValue<QPoint>::check( L, 2 );
	QTextCursor* res = QtValue<QTextCursor>::create( L );
	*res = 	obj->cursorForPosition(  p->toPoint() );
	return 1;
}*/
int TextEdit::cursorRect(lua_State * L) // ( const QTextCursor & cursor ) const : QRect
// const : QRect
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1 );
	QRectF* res = QtValue<QRectF>::create( L );
	if(QTextCursor* cursor = QtValue<QTextCursor>::check( L, 2 ))
	{
		*res = obj->cursorRect( *cursor );
	}
	else
	{
		*res = 	obj->cursorRect();
	}
	return 1;
}
int TextEdit::cursorWidth(lua_State * L) // const : int
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
	Lua::Util::push( L, obj->cursorWidth() );
	return 1;
}
int TextEdit::document(lua_State * L) // const : QTextDocument*
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
    QtObject<QTextDocument>::create( L, obj->document() );
	return 1;
}
int TextEdit::ensureCursorVisible(lua_State * L) 
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
	obj->ensureCursorVisible();
	return 0;
}
int TextEdit::find(lua_State * L) // ( const QString & exp, QTextDocument::FindFlags options = 0 ) : bool
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
	QTextDocument::FindFlags f;
	int fInt = Util::toInt( L, 3 );
	if( (fInt >= 0 && fInt <= 17) )
	{
		f = (QTextDocument::FindFlags) fInt;
        Lua::Util::push( L, obj->find( QtValueBase::toString( L, 2 ), f ) );
	}
	else
		Lua::Util::error( L, "expecting a valid QTextDocument::FindFlags" );
	return 1;
}
int TextEdit::fontFamily(lua_State * L) // const : QString
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
    *QtValue<QString>::create( L ) = obj->fontFamily( );
	return 1;
}
int TextEdit::fontItalic(lua_State * L) // const : bool
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
	Lua::Util::push( L, obj->fontItalic() );
	return 1;
}
int TextEdit::fontPointSize(lua_State * L) // const : qreal
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
	Lua::Util::push( L, obj->fontPointSize() );
	return 1;
}
int TextEdit::fontUnderline(lua_State * L) // const : bool
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
	Lua::Util::push( L, obj->fontUnderline() );
	return 1;
}
int TextEdit::fontWeight(lua_State * L) // const : int
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
	Lua::Util::push( L, obj->fontWeight() );
	return 1;
}
int TextEdit::loadResource(lua_State * L) // ( int type, const QUrl & name ) : QVariant virtual
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
	QUrl* name = QtValue<QUrl>::create( L );
	Variant::push( L, obj->loadResource( Util::toInt( L, 2 ), *name ) );
	return 1;
}
int TextEdit::mergeCurrentCharFormat( lua_State * L )// ( const QTextCharFormat & modifier )
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
	QTextCharFormat* modifier = QtValue<QTextCharFormat>::check( L, 2 );
	obj->mergeCurrentCharFormat( *modifier );
	return 0;
}
int TextEdit::moveCursor( lua_State * L )// ( QTextCursor::MoveOperation operation, QTextCursor::MoveMode mode = QTextCursor::MoveAnchor )
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
    QTextCursor::MoveOperation operation = (QTextCursor::MoveOperation)Util::toInt( L, 2 );
    QTextCursor::MoveMode mode = QTextCursor::MoveAnchor;
    if( lua_gettop(L) == 3 )
        mode = (QTextCursor::MoveMode)Util::toInt( L, 3 );
    obj->moveCursor( operation, mode );
	return 0;
}
int TextEdit::overwriteMode(lua_State * L) // const : bool
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
	Lua::Util::push( L, obj->overwriteMode() );
	return 1;
}
int TextEdit::setCurrentCharFormat( lua_State * L )// ( const QTextCharFormat & format )
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
	QTextCharFormat* modifier = QtValue<QTextCharFormat>::check( L, 2 );
	obj->setCurrentCharFormat( *modifier );
	return 0;
} 
int TextEdit::setCursorWidth(lua_State * L) // ( int width )
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
	 obj->setCursorWidth( Util::toInt( L, 2) );
	return 0;
}
int TextEdit::setDocument(lua_State * L) // ( QTextDocument * document )
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
	QTextDocument* document = QtObject<QTextDocument>::check( L, 2 );
	if( obj->document() && obj->document()->parent() == obj )
		obj->document()->deleteLater(); // QT-BUG: QTextEdit::setDocument löscht das alte sonst nicht
	obj->setDocument( document );
	return 0;
}/*
int TextEdit::setExtraSelections(lua_State * L) // ( const QList<ExtraSelection> & selections )
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
	ExtraSelection* r = QtObject<ExtraSelection>::cast( L, 2 )
	int size;
		size=Util::top(L)-1;
		QList<ExtraSelection *> res;
		//rect_array=new QRect[size];
		for(int i = 2; i <= size+1; ++i)
		{
			ExtraSelection* r = QtObject<ExtraSelection>::check( L, i );
			res.append(r);
			//rect_array[i-2]=r->toRect();	
		}
	obj->setExtraSelections(res);
	
	return 0;
}*/
int TextEdit::setOverwriteMode(lua_State * L) // ( bool overwrite )
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
	obj->setOverwriteMode( Util::toBool( L, 2) );
	return 0;
}
int TextEdit::textColor(lua_State * L) // const : QColor
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
	QColor* res = QtValue<QColor>::create( L );
	*res = 	obj->textColor();
	return 1;
}
int TextEdit::textCursor(lua_State * L) // const : QTextCursor
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
	QTextCursor* res = QtValue<QTextCursor>::create( L );
	*res = 	obj->textCursor();
	return 1;
}
int TextEdit::toHtml(lua_State * L) // const : QString
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
    *QtValue<QString>::create( L ) = obj->toHtml();
	return 1;
}
int TextEdit::toPlainText(lua_State * L) // const : QString
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
    *QtValue<QString>::create( L ) = obj->toPlainText();
	return 1;
}
int TextEdit::textInteractionFlags(lua_State * L) // const : Qt::TextInteractionFlags
{
	QTextEdit* obj = QtObject<QTextEdit>::check( L, 1);
	Lua::Util::push( L, obj->textInteractionFlags() );
	return 1;
}
int TextEdit::init(lua_State * L)
{
    return DefaultCreateObject<QTextEdit>::init( L );
}
static const luaL_reg _TextEdit[] = 
{
	{ "init", TextEdit::init },
	{ "alignment", TextEdit::alignment },
	{ "anchorAt", TextEdit::anchorAt },
	{ "canPaste", TextEdit::canPaste },
	{ "createStandardContextMenu", TextEdit::createStandardContextMenu },
	{ "currentCharFormat", TextEdit::currentCharFormat },
	{ "currentFont", TextEdit::currentFont },
	//{ "cursorForPosition", TextEdit::cursorForPosition }, // not completed
	{ "cursorRect", TextEdit::cursorRect },
	{ "cursorWidth", TextEdit::cursorWidth },
	{ "document", TextEdit::document },
	{ "ensureCursorVisible", TextEdit::ensureCursorVisible },
	{ "find", TextEdit::find },
	{ "fontFamily", TextEdit::fontFamily },
	{ "fontItalic", TextEdit::fontItalic },
	{ "fontPointSize", TextEdit::fontPointSize },
	{ "fontUnderline", TextEdit::fontUnderline },
	{ "fontWeight", TextEdit::fontWeight },
	{ "loadResource", TextEdit::loadResource },
	{ "mergeCurrentCharFormat", TextEdit::mergeCurrentCharFormat },
	{ "moveCursor", TextEdit::moveCursor },
	{ "overwriteMode", TextEdit::overwriteMode },
	{ "setCurrentCharFormat", TextEdit::setCurrentCharFormat },
	{ "setCursorWidth", TextEdit::setCursorWidth },
	{ "setDocument", TextEdit::setDocument },
	//{ "setExtraSelections", TextEdit::setExtraSelections },//notcomplete
	{ "setOverwriteMode", TextEdit::setOverwriteMode },
	{ "textColor", TextEdit::textColor },
	{ "textCursor", TextEdit::textCursor },
	{ "toHtml", TextEdit::toHtml },
	{ "toPlainText", TextEdit::toPlainText },
	{ "textInteractionFlags", TextEdit::textInteractionFlags },
	{ 0, 0 }
};
void TextEdit::install(lua_State * L){
    QtObject<QTextEdit,QAbstractScrollArea>::install( L, "QTextEdit", _TextEdit );
}
