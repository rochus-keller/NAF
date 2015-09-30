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

#include <QTextCursor>
#include <QTextDocument>
#include <QTextFrame>
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextBlockFormat>
#include <QTextListFormat>
#include <QTextTable>
#include <QTextDocumentFragment>
#include <QTextTableFormat>
#include <QTextList>
#include <QTextImageFormat>

#include "QtlTextCursor.h"
#include <Script/ValueInstaller.h>
#include "Qtl/Enums.h"
#include "Variant.h"

#include <Script/ObjectInstaller.h>

using namespace Qtl;
using namespace Lua;

GCC_IGNORE(-Wunused-variable);
GCC_IGNORE(-Wunused-but-set-variable);


int TextCursor::init( lua_State * L )
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	if ( Util::isNil( L, 2 ) )
	{
		*lhs = QTextCursor();
	}
	else if ( QTextDocument* document = ValueInstaller2<QTextDocument>::cast( L, 2 ) )
	{
		*lhs = QTextCursor( document );
	}
	else if ( QTextFrame* frame = ValueInstaller2<QTextFrame>::cast( L, 2 ) )
	{
		*lhs = QTextCursor( frame );
	}
	else if ( QTextBlock* block = ValueInstaller2<QTextBlock>::cast( L, 2 ) )
	{
		*lhs = QTextCursor( *block );
	}
	else if ( QTextCursor* cursor = ValueInstaller2<QTextCursor>::cast( L, 2 ) )
	{
		*lhs = QTextCursor( *cursor );
	}
	else
	{
		lhs->~QTextCursor();
	}
	return 0;
}
int TextCursor::anchor( lua_State * L )//const int
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	Util::push( L, lhs->anchor() );
	return 1;
}
int TextCursor::atBlockEnd( lua_State * L )//const bool
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	Util::push( L, lhs->atBlockEnd() );
	return 1;
}
int TextCursor::atBlockStart( lua_State * L )//const bool
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	Util::push( L, lhs->atBlockStart() );
	return 1;
}
int TextCursor::atEnd( lua_State * L )//const bool
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	Util::push( L, lhs->atEnd() );
	return 1;
}
int TextCursor::atStart( lua_State * L )//const bool
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	Util::push( L, lhs->atStart() );
	return 1;
}
int TextCursor::beginEditBlock ( lua_State * L )//void
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	lhs->beginEditBlock();
	return 0;
}
int TextCursor::block( lua_State * L )//const QTextBlock
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	QTextBlock* res = ValueInstaller2<QTextBlock>::create( L );
	*res = lhs->block();
	return 1;
}
int TextCursor::blockCharFormat( lua_State * L )//const QTextCharFormat
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	QTextCharFormat* res = ValueInstaller2<QTextCharFormat>::create( L );
	*res = lhs->blockCharFormat();
	return 1;
}
int TextCursor::blockFormat( lua_State * L )//const QTextBlockFormat
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	QTextBlockFormat* res = ValueInstaller2<QTextBlockFormat>::create( L );
	*res = lhs->blockFormat();
	return 1;
}
int TextCursor::blockNumber( lua_State * L )//const int
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	Util::push( L, lhs->blockNumber() );
	return 1;
}
int TextCursor::charFormat( lua_State * L )//const QTextCharFormat
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	QTextCharFormat* res = ValueInstaller2<QTextCharFormat>::create( L );
	*res = lhs->charFormat();
	return 1;
}
int TextCursor::clearSelection( lua_State * L )//void
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	lhs->clearSelection();
	return 0;
}
int TextCursor::columnNumber( lua_State * L )//const int
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	Util::push( L, lhs->columnNumber() );
	return 1;
}
int TextCursor::createList( lua_State * L )//const QTextList
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	//QTextList* res = ObjectInstaller<QTextList>::create( L );
	if ( QTextListFormat* format = ValueInstaller2<QTextListFormat>::cast( L, 2 ) )
	{
		//res = *lhs->createList( *format );
		ObjectPeer::pushPeer( L, lhs->createList( *format ), true );      //QTextList * createList ( const QTextListFormat & format )

	}
	else
	{
		Enums enums(L);
		QTextListFormat::Style f=(QTextListFormat::Style)enums.Style( 2 );
		ObjectPeer::pushPeer( L, lhs->createList( f ), true );      //QTextList * createList ( QTextListFormat::Style style )

	}
	return 1;
}
int TextCursor::currentFrame( lua_State * L )//const QTextFrame
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	ObjectPeer::pushPeer( L, lhs->currentFrame(), true );
	return 1;
}
int TextCursor::currentList( lua_State * L )//const QTextList
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	ObjectPeer::pushPeer( L, lhs->currentList(), true );
	return 1;
}
int TextCursor::currentTable( lua_State * L )//const QTextTable
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	ObjectPeer::pushPeer( L, lhs->currentTable(), true );
	return 1;
}
int TextCursor::deleteChar( lua_State * L )//void
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	lhs->deleteChar();
	return 0;
}
int TextCursor::deletePreviousChar( lua_State * L )//void
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	lhs->deletePreviousChar();
	return 0;
}
int TextCursor::endEditBlock( lua_State * L )//void
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	lhs->endEditBlock();
	return 0;
}
int TextCursor::hasComplexSelection( lua_State * L )//bool
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	Util::push( L, lhs->hasComplexSelection() );
	return 1;
}
int TextCursor::hasSelection( lua_State * L )//bool
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	Util::push( L, lhs->hasSelection() );
	return 1;
}
int TextCursor::insertBlock( lua_State * L )//void
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	if( QTextCharFormat* charFormat = ValueInstaller2<QTextCharFormat>::cast( L, 3 ) )
	{
		QTextBlockFormat* format = ValueInstaller2<QTextBlockFormat>::check( L, 2 );
		lhs->insertBlock( *format, *charFormat );		//void insertBlock ( const QTextBlockFormat & format, const QTextCharFormat & charFormat )

	}
	else if( QTextBlockFormat* format = ValueInstaller2<QTextBlockFormat>::cast( L, 2 ) )
	{
		lhs->insertBlock( *format );					//void insertBlock ( const QTextBlockFormat & format )

	}
	else
	{
		lhs->insertBlock();								//void insertBlock ()

	}
	return 0;
}
int TextCursor::insertFragment( lua_State * L )//( const QTextDocumentFragment & fragment )void
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	QTextDocumentFragment* fragment = ValueInstaller2<QTextDocumentFragment>::check( L, 2 );
	lhs->insertFragment( *fragment );
	return 0;
}
int TextCursor::insertFrame( lua_State * L )//( const QTextFrameFormat & fragment )QTextFrame
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	QTextFrameFormat* format = ValueInstaller2<QTextFrameFormat>::check( L, 2 );
	lhs->insertFrame( *format );
	return 1;
}
int TextCursor::insertHtml( lua_State * L )//( const QString & html )void
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	//QString* html = ValueInstaller2<QString>::check( L, 2 );
	lhs->insertHtml( Util::toString( L, 2 ) );
	return 0;
}
int TextCursor::insertImage( lua_State * L )//void
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	if( Util::isNum( L, 3 ) )
	{
		Enums enums(L);
		QTextFrameFormat::Position alignment = (QTextFrameFormat::Position)enums.Style( 3 );
		QTextImageFormat* format = ValueInstaller2<QTextImageFormat>::check( L, 2 );
		lhs->insertImage( *format, alignment );	// ( const QTextImageFormat & format, QTextFrameFormat::Position alignment )void

	}
	else if( QTextImageFormat* format = ValueInstaller2<QTextImageFormat>::cast( L, 2 ) )
	{
		lhs->insertImage( *format );			// ( const QTextImageFormat & format )void

	}
	else
	{
		//QString* name = ValueInstaller2<QString>::check( L, 2 );
		lhs->insertImage( Util::toString( L, 2 ) );				// ( const QString & name )void

	}
	return 0;
}
int TextCursor::insertList( lua_State * L )// ( const QTextListFormat & format ) QTextList
//( QTextListFormat::Style style )
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	if(QTextListFormat* format = ValueInstaller2<QTextListFormat>::check( L, 2 ))
	{
		QTextList* obj = ObjectHelper<QTextList>::check( L );
		obj = lhs->insertList( *format );
	}
	else
	{
		Enums enums(L);
		QTextListFormat::Style style = (QTextListFormat::Style)enums.Style( 2 );
		lhs->insertList( style );
	}
	return 1;
}
int TextCursor::insertTable( lua_State * L )//QTextTable
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	int rows = Util::toInt( L, 2 );
	int columns = Util::toInt( L, 3 );
	if ( QTextTableFormat* format = ValueInstaller2<QTextTableFormat>::cast( L, 4 ) )
	{
		ObjectPeer::pushPeer( L, lhs->insertTable( rows, columns, *format ), true );		//QTextTable * insertTable ( int rows, int columns, const QTextTableFormat & format )
	}
	else
	{
		ObjectPeer::pushPeer( L, lhs->insertTable( rows, columns ), true );					//QTextTable * insertTable ( int rows, int columns )

	}
	return 1;
}
int TextCursor::insertText( lua_State * L )
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	//QString* text = ValueInstaller2<QString>::check( L, 2 );
	if ( QTextCharFormat* format = ValueInstaller2<QTextCharFormat>::cast( L, 4 ) )
	{
		lhs->insertText( Util::toString( L, 2 ), *format );      //void insertText ( const QString & text, const QTextCharFormat & format )

	}
	else
	{
		lhs->insertText( Util::toString( L, 2 ) );				//void insertText ( const QString & text )

	}
	return 0;
}
int TextCursor::isCopyOf( lua_State * L )// ( const QTextCursor & other ) bool const
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	QTextCursor* other = ValueInstaller2<QTextCursor>::check( L, 2 );
	Util::push( L, lhs->isCopyOf( *other ) );
	return 1;
}
int TextCursor::isNull( lua_State * L )// const bool
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	Util::push( L, lhs->isNull() );
	return 1;
}
int TextCursor::joinPreviousEditBlock( lua_State * L )//void
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	lhs->joinPreviousEditBlock();
	return 0;
}
int TextCursor::mergeBlockCharFormat( lua_State * L )//( const QTextCharFormat & modifier )void
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	QTextCharFormat* modifier = ValueInstaller2<QTextCharFormat>::check( L, 2 );
	lhs->mergeBlockCharFormat( *modifier );
	return 0;
}
int TextCursor::mergeBlockFormat( lua_State * L )//( const QTextBlockFormat & modifier )void
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	const QTextBlockFormat* modifier = ValueInstaller2<QTextBlockFormat>::check( L, 2 );
	lhs->mergeBlockFormat( *modifier );
	return 0;
}
int TextCursor::mergeCharFormat( lua_State * L )//( const QTextCharFormat & modifier )void
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	QTextCharFormat* modifier = ValueInstaller2<QTextCharFormat>::check( L, 2 );
	lhs->mergeCharFormat( *modifier );
	return 0;
}
int TextCursor::movePosition( lua_State * L )//( MoveOperation operation, MoveMode mode = MoveAnchor, int n = 1 )bool
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	Enums enums(L);
	QTextCursor::MoveOperation operation=(QTextCursor::MoveOperation)enums.MoveMode( 2 );
	QTextCursor::MoveMode mode=(QTextCursor::MoveMode)enums.MoveMode( 3 );
	lhs->movePosition( operation, mode, Util::toInt( L, 4 ) );
	return 0;
}
int TextCursor::position( lua_State * L )//const int
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	Util::push( L, lhs->position() );
	return 1;
}
int TextCursor::removeSelectedText( lua_State * L )//void
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	lhs->removeSelectedText();
	return 0;
}
int TextCursor::select( lua_State * L )//( SelectionType selection )void
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	Enums enums(L);
	QTextCursor::SelectionType selection = (QTextCursor::SelectionType)enums.SelectionType( 2 );
	lhs->select( selection );
	return 0;
}
int TextCursor::selectedTableCells( lua_State * L )// ( int * firstRow, int * numRows, int * firstColumn, int * numColumns ) const void 
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	int firstRow = Util::toInt( L, 2 );
	int numRows = Util::toInt( L, 3 );
	int firstColumn = Util::toInt( L, 4 );
	int numColumns = Util::toInt( L, 5 );
	//lhs->selectedTableCells( firstRow, numRows, firstColumn, numColumns );
	return 0;
}
int TextCursor::selectedText( lua_State * L )// const QString
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	//QString* res = ValueInstaller2<QString>::create( L );
	//*res = lhs->selectedText();
	Util::push( L, lhs->selectedText() );
	return 1;
}
int TextCursor::selection( lua_State * L )//const QTextDocumentFragment
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	QTextDocumentFragment* res = ValueInstaller2<QTextDocumentFragment>::create( L );
	*res = lhs->selection();
	return 1;
}
int TextCursor::selectionEnd( lua_State * L )//const int
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	Util::push( L, lhs->selectionEnd() );
	return 1;
}
int TextCursor::selectionStart( lua_State * L )//const int
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	Util::push( L, lhs->selectionStart() );
	return 1;
}
int TextCursor::setBlockCharFormat( lua_State * L )//( const QTextCharFormat & format )void
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	QTextCharFormat* format = ValueInstaller2<QTextCharFormat>::check( L, 2 );
	lhs->setBlockCharFormat( *format );
	return 0;
}
int TextCursor::setBlockFormat( lua_State * L )//( const QTextCharFormat & format )void
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	QTextCharFormat* format = ValueInstaller2<QTextCharFormat>::check( L, 2 );
	lhs->setBlockCharFormat( *format );
	return 0;
}
int TextCursor::setCharFormat( lua_State * L )//( const QTextCharFormat & format )void
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	QTextCharFormat* format = ValueInstaller2<QTextCharFormat>::check( L, 2 );
	lhs->setBlockCharFormat( *format );
	return 0;
}
int TextCursor::setPosition( lua_State * L )//( int pos, MoveMode m = MoveAnchor ) void
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	Enums enums(L);
	QTextCursor::MoveMode m = (QTextCursor::MoveMode)enums.MoveMode( 3 );
	lhs->setPosition( Util::toInt( L, 2 ) , m );
	return 0;
}
int TextCursor::operatorEq( lua_State * L )//( const QTextCursor & cursor )QTextCursor
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	QTextCursor* cursor = ValueInstaller2<QTextCursor>::check( L, 2 );
	QTextCursor* res = ValueInstaller2<QTextCursor>::create( L );
	res =  cursor ;
	return 1;
}
int TextCursor::notEquals( lua_State * L )//( const QTextCursor & other ) const bool
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	QTextCursor* other = ValueInstaller2<QTextCursor>::check( L, 2 );
	Util::push( L, !(lhs==other) );
	return 1;
}
int TextCursor::lessThan( lua_State * L )//( const QTextCursor & other ) const bool
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	QTextCursor* other = ValueInstaller2<QTextCursor>::check( L, 2 );
	Util::push( L, (lhs<other) );
	return 1;
}
int TextCursor::lessThanEqualTo( lua_State * L )//( const QTextCursor & other ) const bool
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	QTextCursor* other = ValueInstaller2<QTextCursor>::check( L, 2 );
	Util::push( L, (lhs<=other) );
	return 1;
}

int TextCursor::__eq( lua_State * L )//( const QTextCursor & other ) const bool
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	QTextCursor* other = ValueInstaller2<QTextCursor>::check( L, 2 );
	Util::push( L, (lhs==other) );
	return 1;
}
int TextCursor::greaterThan( lua_State * L )//( const QTextCursor & other ) const bool
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	QTextCursor* other = ValueInstaller2<QTextCursor>::check( L, 2 );
	Util::push( L, (lhs>other) );
	return 1;
}
int TextCursor::greaterThanEqualTo( lua_State * L )//( const QTextCursor & other ) const bool
{
	QTextCursor* lhs = ValueInstaller2<QTextCursor>::check( L, 1 );
	QTextCursor* other = ValueInstaller2<QTextCursor>::check( L, 2 );
	Util::push( L, (lhs>=other) );
	return 1;
}
static const luaL_reg _TextCursor[] = 
{
	{ Util::s_init, TextCursor::init },
	{ "anchor", TextCursor::anchor },
	{ "atBlockEnd", TextCursor::atBlockEnd },
	{ "atBlockStart", TextCursor::atBlockStart },
	{ "atEnd", TextCursor::atEnd },
	{ "atStart", TextCursor::atStart },
	{ "beginEditBlock", TextCursor::beginEditBlock },
	{ "block", TextCursor::block },
	{ "blockCharFormat", TextCursor::blockCharFormat },
	{ "blockFormat", TextCursor::blockFormat },
	{ "blockNumber", TextCursor::blockNumber },
	{ "charFormat", TextCursor::charFormat },
	{ "clearSelection", TextCursor::clearSelection },
	{ "columnNumber", TextCursor::columnNumber },
	{ "createList", TextCursor::createList },
	{ "currentFrame", TextCursor::currentFrame },
	{ "currentList", TextCursor::currentList },
	{ "currentTable", TextCursor::currentTable },
	{ "deleteChar", TextCursor::deleteChar },
	{ "deletePreviousChar", TextCursor::deletePreviousChar },
	{ "endEditBlock", TextCursor::endEditBlock },
	{ "hasComplexSelection", TextCursor::hasComplexSelection },
	{ "hasSelection", TextCursor::hasSelection },
	{ "insertBlock", TextCursor::insertBlock },
	{ "insertFragment", TextCursor::insertFragment },
	{ "insertFrame", TextCursor::insertFrame },
	{ "insertHtml", TextCursor::insertHtml },
	{ "insertImage", TextCursor::insertImage },
  	{ "insertList", TextCursor::insertList },
  	{ "insertTable", TextCursor::insertTable },
	{ "insertText", TextCursor::insertText },
	{ "isCopyOf", TextCursor::isCopyOf },
	{ "isNull", TextCursor::isNull },
	{ "joinPreviousEditBlock", TextCursor::joinPreviousEditBlock },
	{ "mergeBlockCharFormat", TextCursor::mergeBlockCharFormat },
	{ "mergeBlockFormat", TextCursor::mergeBlockFormat },
	{ "mergeCharFormat", TextCursor::mergeCharFormat },
	{ "movePosition", TextCursor::movePosition },
	{ "position", TextCursor::position },
	{ "removeSelectedText", TextCursor::removeSelectedText },
	{ "select", TextCursor::select },
	{ "selectedTableCells", TextCursor::selectedTableCells },//not completed
	{ "selectedText", TextCursor::selectedText },
	{ "selection", TextCursor::selection },
	{ "selectionEnd", TextCursor::selectionEnd },
	{ "selectionStart", TextCursor::selectionStart },
	{ "setBlockCharFormat", TextCursor::setBlockCharFormat },
	{ "setBlockFormat", TextCursor::setBlockFormat },
	{ "setCharFormat", TextCursor::setCharFormat },
	{ "setPosition", TextCursor::setPosition },
	{ "operatorEq", TextCursor::operatorEq },
	{ Variant::notEquals,TextCursor::notEquals },
	{ Variant::lessThan,TextCursor::lessThan },
	{ Variant::lessThanEqualTo,TextCursor::lessThanEqualTo },
	{ "__eq",TextCursor::__eq },
	{ Variant::greaterThan,TextCursor::greaterThan },
	{ Variant::greaterThanEqualTo,TextCursor::greaterThanEqualTo },
	{ 0, 0 }
};
void TextCursor::install(lua_State * L){
	ValueInstaller2<QTextCursor>::install( L, _TextCursor, "QTextCursor" );
}
