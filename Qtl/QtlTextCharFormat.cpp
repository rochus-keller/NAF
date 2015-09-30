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

#include <QTextCharFormat>
#include <QString>
#include "QtlTextCharFormat.h"
#include "Enums.h"
#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;



int TextCharFormat::anchorHref(lua_State * L) // const : QString
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	lua_pushstring(L, lhs->anchorHref().toLatin1() );
	return 1;
}
int TextCharFormat::anchorName(lua_State * L) // const : QString
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	lua_pushstring(L, lhs->anchorName().toLatin1() );
	return 1;
}
int TextCharFormat::font(lua_State * L) // const : QFont
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	QFont* res = ValueInstaller2<QFont>::create( L );
	*res = lhs->font();
	return 1;
}
int TextCharFormat::fontFamily(lua_State * L) // const : QString
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	lua_pushstring(L, lhs->fontFamily().toLatin1() );
	return 1;
}
int TextCharFormat::fontFixedPitch(lua_State * L) // const : bool
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	Util::push( L, lhs->fontFixedPitch() );
	return 1;
}
int TextCharFormat::fontItalic(lua_State * L) // const : bool
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	Util::push( L, lhs->fontItalic() );
	return 1;
}
int TextCharFormat::fontOverline(lua_State * L) // const : bool
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	Util::push( L, lhs->fontOverline() );
	return 1;
}
int TextCharFormat::fontPointSize(lua_State * L) // const : qreal
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	Util::push( L, lhs->fontPointSize() );
	return 1;
}
int TextCharFormat::fontStrikeOut(lua_State * L) // const : bool
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	Util::push( L, lhs->fontStrikeOut() );
	return 1;
}
int TextCharFormat::fontUnderline(lua_State * L) // const : bool
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	Util::push( L, lhs->fontUnderline() );
	return 1;
}
int TextCharFormat::fontWeight(lua_State * L) // const : int
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	Util::push( L, lhs->fontWeight() );
	return 1;
}
int TextCharFormat::isAnchor(lua_State * L) // const : bool
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	Util::push( L, lhs->isAnchor() );
	return 1;
}
int TextCharFormat::isValid(lua_State * L) // const : bool
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	Util::push( L, lhs->isValid() );
	return 1;
}
int TextCharFormat::setAnchor(lua_State * L) // ( bool anchor ) 
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	lhs->setAnchor( Util::toBool( L, 2 ) );
	return 0;
}
int TextCharFormat::setAnchorHref(lua_State * L) // ( const QString & value ) 
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	//QString* value = ValueInstaller2<QString>::check( L, 2 );
	lhs->setAnchorHref( Util::toString( L, 2 ) );
	return 0;
}
int TextCharFormat::setAnchorName(lua_State * L) // ( const QString & name ) 
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	//QString* name = ValueInstaller2<QString>::check( L, 2 );
	lhs->setAnchorName( Util::toString( L, 2 ) );
	return 0;
}
int TextCharFormat::setFont(lua_State * L) // ( const QFont & font ) 
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	QFont* font = ValueInstaller2<QFont>::check( L, 2 );
	lhs->setFont( *font );
	return 0;
}
int TextCharFormat::setFontFamily(lua_State * L) // ( const QString & family ) 
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	//QString* family = ValueInstaller2<QString>::check( L, 2 );
	lhs->setFontFamily( Util::toString( L, 2 ) );
	return 0;
}
int TextCharFormat::setFontFixedPitch(lua_State * L) // ( bool fixedPitch ) 
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	lhs->setFontFixedPitch( Util::toBool( L, 2 ) );
	return 0;
}
int TextCharFormat::setFontItalic(lua_State * L) // ( bool italic ) 
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	lhs->setFontItalic( Util::toBool( L, 2 ) );
	return 0;
}
int TextCharFormat::setFontOverline(lua_State * L) // ( bool overline ) 
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	lhs->setFontOverline( Util::toBool( L, 2 ) );
	return 0;
}
int TextCharFormat::setFontPointSize(lua_State * L) // ( qreal size ) 
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	lhs->setFontPointSize( Util::toDbl( L, 2 ) );
	return 0;
}
int TextCharFormat::setFontStrikeOut(lua_State * L) // ( bool strikeOut ) 
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	lhs->setFontStrikeOut( Util::toBool( L, 2 ) );
	return 0;
}
int TextCharFormat::setFontUnderline(lua_State * L) // ( bool underline ) 
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	lhs->setFontUnderline( Util::toBool( L, 2 ) );
	return 0;
}
int TextCharFormat::setFontWeight(lua_State * L) // ( int weight ) 
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	lhs->setFontWeight( Util::toInt( L, 2 ) );
	return 0;
}
int TextCharFormat::setTableCellColumnSpan(lua_State * L) // ( int tableCellColumnSpan ) 
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	lhs->setTableCellColumnSpan( Util::toInt( L, 2 ) );
	return 0;
}
int TextCharFormat::setTableCellRowSpan(lua_State * L) // ( int tableCellRowSpan ) 
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	lhs->setTableCellRowSpan( Util::toInt( L, 2 ) );
	return 0;
}
int TextCharFormat::setTextOutline(lua_State * L) // ( const QPen & pen ) 
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	QPen* pen = ValueInstaller2<QPen>::check( L, 2 );
	lhs->setTextOutline( *pen );
	return 0;
}
int TextCharFormat::setUnderlineColor(lua_State * L) // ( const QColor & color ) 
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	QColor* color = ValueInstaller2<QColor>::check( L, 2 );
	lhs->setUnderlineColor( *color );
	return 0;
}
int TextCharFormat::setUnderlineStyle(lua_State * L) // ( UnderlineStyle style )
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	Enums enums(L);
	QTextCharFormat::UnderlineStyle f=(QTextCharFormat::UnderlineStyle)enums.UnderlineStyle( 2 );
	lhs->setUnderlineStyle( f );
	return 0;
}
int TextCharFormat::setVerticalAlignment(lua_State * L) // ( VerticalAlignment alignment )
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	Enums enums(L);
	QTextCharFormat::VerticalAlignment f=(QTextCharFormat::VerticalAlignment)enums.VerticalAlignment( 2 );
	lhs->setVerticalAlignment( f );
	return 0;
}
int TextCharFormat::tableCellColumnSpan(lua_State * L) // const : int
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	Util::push( L, lhs->tableCellColumnSpan() );
	return 1;
}
int TextCharFormat::tableCellRowSpan(lua_State * L) // const : int
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	Util::push( L, lhs->tableCellRowSpan() );
	return 1;
}
int TextCharFormat::textOutline(lua_State * L) // const : QPen 
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	QPen* res = ValueInstaller2<QPen>::create( L );
	*res = lhs->textOutline();
	return 0;
}
int TextCharFormat::underlineColor(lua_State * L) // const : QColor 
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	QColor* res = ValueInstaller2<QColor>::create( L );
	*res = lhs->underlineColor();
	return 0;
}
int TextCharFormat::underlineStyle(lua_State * L) // const : UnderlineStyle
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	Util::push( L, lhs->underlineStyle() );
	return 1;
}
int TextCharFormat::verticalAlignment(lua_State * L) // const : VerticalAlignment
{
	QTextCharFormat* lhs = ValueInstaller2<QTextCharFormat>::check( L, 1 );
	Util::push( L, lhs->verticalAlignment() );
	return 1;
}
int TextCharFormat::init(lua_State * L)
{
	return 0;
}
static const luaL_reg _TextCharFormat[] = 
{
	{ Util::s_init, TextCharFormat::init },
	{ "anchorHref", TextCharFormat::anchorHref },
	{ "anchorName", TextCharFormat::anchorName },
	{ "font", TextCharFormat::font },
	{ "fontFamily", TextCharFormat::fontFamily },
	{ "fontFixedPitch", TextCharFormat::fontFixedPitch },
	{ "fontItalic", TextCharFormat::fontItalic },
	{ "fontOverline", TextCharFormat::fontOverline },
	{ "fontPointSize", TextCharFormat::fontPointSize },
	{ "fontStrikeOut", TextCharFormat::fontStrikeOut },
	{ "fontUnderline", TextCharFormat::fontUnderline },
	{ "fontWeight", TextCharFormat::fontWeight },
	{ "isAnchor", TextCharFormat::isAnchor },
	{ "isValid", TextCharFormat::isValid },
	{ "setAnchor", TextCharFormat::setAnchor },
	{ "setAnchorHref", TextCharFormat::setAnchorHref },
	{ "setAnchorName", TextCharFormat::setAnchorName },
	{ "setFont", TextCharFormat::setFont },
	{ "setFontFamily", TextCharFormat::setFontFamily },
	{ "setFontFixedPitch", TextCharFormat::setFontFixedPitch },
	{ "setFontItalic", TextCharFormat::setFontItalic },
	{ "setFontOverline", TextCharFormat::setFontOverline },
	{ "setFontPointSize", TextCharFormat::setFontPointSize },
	{ "setFontStrikeOut", TextCharFormat::setFontStrikeOut },
	{ "setFontUnderline", TextCharFormat::setFontUnderline },
	{ "setFontWeight", TextCharFormat::setFontWeight },
	{ "setTableCellColumnSpan", TextCharFormat::setTableCellColumnSpan },
	{ "setTableCellRowSpan", TextCharFormat::setTableCellRowSpan },
	{ "setTextOutline", TextCharFormat::setTextOutline },
	{ "setUnderlineColor", TextCharFormat::setUnderlineColor },
	{ "setUnderlineStyle", TextCharFormat::setUnderlineStyle },
	{ "setVerticalAlignment", TextCharFormat::setVerticalAlignment },
	{ "tableCellColumnSpan", TextCharFormat::tableCellColumnSpan },
	{ "tableCellRowSpan", TextCharFormat::tableCellRowSpan },
	{ "textOutline", TextCharFormat::textOutline },
	{ "underlineColor", TextCharFormat::underlineColor },
	{ "underlineStyle", TextCharFormat::underlineStyle },
	{ "verticalAlignment", TextCharFormat::verticalAlignment },
	{ 0, 0 }
};
void TextCharFormat::install(lua_State * L){
	ValueInstaller2<QTextCharFormat>::install( L, _TextCharFormat, "QTextCharFormat" );
}
