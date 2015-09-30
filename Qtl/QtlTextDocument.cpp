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

#include <QTextDocument>
#include <QString>
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QRectF>
#include <QTextCursor>
#include <QTextFormat>
#include <QPrinter>
#include <QUrl>
#include <QTextFrame>
#include <QFont>
#include <QByteArray>
#include "QtlTextDocument.h"
#include "Qtl/Enums.h"

#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;
int TextDocument::init( lua_State * L )
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	if( QTextDocument* parent = ObjectHelper<QTextDocument>::cast( L, 3 ) )
	{
		//QString* text = ValueInstaller2<QString>::check( L, 2 );
		//*obj = QTextDocument( Util::toString( L, 2 ), parent );
		obj->setParent( parent );
		obj->setPlainText( Util::toString( L, 2 ) );
	}
	else if( QTextDocument* parent = ObjectHelper<QTextDocument>::cast( L, 2 ) )
	{
		//*obj = QTextDocument( *parent );
		obj->setParent( parent );
	}
	else
	{
		//obj->~QTextDocument();
	}
	return 0;
}
int TextDocument::addResource( lua_State * L )//( int type, const QUrl & name, const QVariant & resource )void
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	QUrl* name = ValueInstaller2<QUrl>::check( L, 3 );
	QVariant* resource = ValueInstaller2<QVariant>::check( L, 4 );
	obj->addResource( Util::toInt( L, 2 ), *name , *resource );
	return 0;
}
int TextDocument::adjustSize( lua_State * L )//void
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	obj->adjustSize();
	return 0;
}
int TextDocument::begin( lua_State * L )//const QTextBlock
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	QTextBlock* res = ValueInstaller2<QTextBlock>::create( L );
	*res = obj->begin();
	return 1;
}
int TextDocument::blockCount( lua_State * L )//const int
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	Util::push( L, obj->blockCount() );
	return 1;
}
int TextDocument::clear( lua_State * L )//virtual void
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	obj->clear();
	return 0;
}
int TextDocument::clone( lua_State * L )// ( QObject * parent = 0 ) const QTextDocument
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	QObject* parent = ObjectHelper<QObject>::check( L, 2 );
	ObjectPeer::pushPeer( L, obj->clone( parent ), true );
	return 1;
}
int TextDocument::defaultFont( lua_State * L )//const QFont
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	QFont* res = ValueInstaller2<QFont>::create( L );
	*res = obj->defaultFont();
	return 0;
}
int TextDocument::defaultStyleSheet( lua_State * L )//const QString
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	//QString* res = ValueInstaller2<QString>::create( L );
	//*res = obj->defaultStyleSheet();
	Util::push( L, obj->defaultStyleSheet() );
	return 0;
}
int TextDocument::documentLayout( lua_State * L )//const QAbstractTextDocumentLayout
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	ObjectPeer::pushPeer( L, obj->documentLayout(), true );
	return 0;
}
int TextDocument::drawContents( lua_State * L )//( QPainter * p, const QRectF & rect = QRectF() )void
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	QPainter* p = ValueInstaller2<QPainter>::check( L, 2 );
	QRectF* rect = ValueInstaller2<QRectF>::check( L, 2 );
	obj->drawContents( p, *rect );
	return 0;
}
int TextDocument::end( lua_State * L )//const QTextBlock
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	QTextBlock* res = ValueInstaller2<QTextBlock>::create( L );
	*res = obj->begin();
	return 1;
}
int TextDocument::find( lua_State * L )
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	QTextCursor* res = ValueInstaller2<QTextCursor>::create( L );
	Enums enums( L );
	QTextDocument::FindFlags f = (QTextDocument::FindFlags)enums.FindFlags( 4 );
	//if ( QString* subString = ValueInstaller2<QString>::cast( L, 2 ) )
	if( Util::isStr( L, 2 ) )
	{
		if ( QTextCursor* cursor = ValueInstaller2<QTextCursor>::cast( L, 3 ) )
		{
			*res = obj->find( Util::toString( L, 2 ), *cursor, f);				//QTextCursor find ( const QString & subString, const QTextCursor & cursor, FindFlags options = 0 ) const
		}
		else
		{
			*res = obj->find( Util::toString( L, 2 ), Util::toInt( L, 3 ), f);	//QTextCursor find ( const QString & subString, int position = 0, FindFlags options = 0 ) const
		}
	}
	else if( QRegExp* expr = ValueInstaller2<QRegExp>::cast( L, 2 ) )
	{
		if ( QTextCursor* cursor = ValueInstaller2<QTextCursor>::cast( L, 3 ) )
		{
			*res = obj->find( *expr, *cursor, f);					//QTextCursor find ( const QRegExp & expr, const QTextCursor & cursor, FindFlags options = 0 ) const
		}
		else
		{
			*res = obj->find( *expr, Util::toInt( L, 3 ), f);		//QTextCursor find ( const QRegExp & expr, int position = 0, FindFlags options = 0 ) const
		}

	}
	return 1;
}
int TextDocument::findBlock( lua_State * L )//( int pos ) const QTextBlock
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	QTextBlock* res = ValueInstaller2<QTextBlock>::create( L );
	*res = obj->findBlock( Util::toInt( L, 2 ) );
	return 1;
}
int TextDocument::idealWidth( lua_State * L )//const qreal
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	Util::push( L, obj->idealWidth() );
	return 1;
}
int TextDocument::isEmpty( lua_State * L )//const bool
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	Util::push( L, obj->isEmpty() );
	return 1;
}
int TextDocument::isModified( lua_State * L )//const bool
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	Util::push( L, obj->isModified() );
	return 1;
}
int TextDocument::isRedoAvailable( lua_State * L )//const bool
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	Util::push( L, obj->isRedoAvailable() );
	return 1;
}
int TextDocument::isUndoAvailable( lua_State * L )//const bool
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	Util::push( L, obj->isUndoAvailable() );
	return 1;
}
int TextDocument::isUndoRedoEnabled( lua_State * L )//const bool
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	Util::push( L, obj->isUndoRedoEnabled() );
	return 1;
}
int TextDocument::markContentsDirty( lua_State * L )//( int position, int length )void
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	obj->markContentsDirty( Util::toInt( L, 2 ), Util::toInt( L, 3 ) );
	return 0;
}
int TextDocument::maximumBlockCount( lua_State * L )//const int
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	Util::push( L, obj->maximumBlockCount() );
	return 1;
}
int TextDocument::metaInformation( lua_State * L )//( MetaInformation info ) const QString
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	Enums enums( L );
	QTextDocument::MetaInformation info = (QTextDocument::MetaInformation)enums.MetaInformation( 2 );
	//QString* res = ValueInstaller2<QString>::create( L );
	//*res = obj->metaInformation( info );
	Util::push( L, obj->metaInformation( info ) );
	return 1;
}
int TextDocument::object( lua_State * L )//( int objectIndex ) const QTextObject
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	ObjectPeer::pushPeer( L, obj->object( Util::toInt( L, 2 ) ), true );
	return 1;
}
int TextDocument::objectForFormat( lua_State * L )//( const QTextFormat & f ) const QTextObject
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	QTextFormat* f = ValueInstaller2<QTextFormat>::check( L, 2 );
	ObjectPeer::pushPeer( L, obj->objectForFormat( *f ), true );
	return 1;
}
int TextDocument::pageCount( lua_State * L )//const int
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	Util::push( L, obj->pageCount() );
	return 1;
}
int TextDocument::pageSize( lua_State * L )//const QSizeF
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	QSizeF* res = ValueInstaller2<QSizeF>::create( L );
	*res = obj->pageSize();
	return 1;
}
int TextDocument::print( lua_State * L )//( QPrinter * printer ) const void
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	QPrinter* printer = ValueInstaller2<QPrinter>::check( L, 2 );
	obj->print( printer );
	return 0;
}
int TextDocument::redo( lua_State * L )//( QTextCursor * cursor )void
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	QTextCursor* cursor = ValueInstaller2<QTextCursor>::check( L, 2 );
	obj->redo( cursor );
	return 0;
}
int TextDocument::resource( lua_State * L )//( int type, const QUrl & name ) const QVariant
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	QVariant* res = ValueInstaller2<QVariant>::create( L );
	QUrl* name = ValueInstaller2<QUrl>::check( L, 3 );
	*res = obj->resource( Util::toInt( L, 2 ), *name );
	return 0;
}
int TextDocument::rootFrame( lua_State * L )//const QTextFrame
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	ObjectPeer::pushPeer( L, obj->rootFrame(), true );
	return 1;
}
int TextDocument::setDefaultFont( lua_State * L )//( const QFont & font )void
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	QFont* font = ValueInstaller2<QFont>::check( L, 2 );
	obj->setDefaultFont( *font );
	return 0;
}
int TextDocument::setDefaultStyleSheet( lua_State * L )//( const QString & sheet )void
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	//QString* sheet = ValueInstaller2<QString>::check( L, 2 );
	obj->setDefaultStyleSheet( Util::toString( L, 2 ) );
	return 0;
}
int TextDocument::setDocumentLayout( lua_State * L )//( QAbstractTextDocumentLayout * layout )void
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	QAbstractTextDocumentLayout* layout = ObjectHelper<QAbstractTextDocumentLayout>::check( L, 2 );
	obj->setDocumentLayout( layout );
	return 0;
}
int TextDocument::setHtml( lua_State * L )//( const QString & html )void
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	//QString* html = ValueInstaller2<QString>::check( L, 2 );
	obj->setHtml( Util::toString( L, 2 ) );
	return 0;
}
int TextDocument::setMaximumBlockCount( lua_State * L )//( int maximum )void
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	obj->setMaximumBlockCount( Util::toInt( L, 2 ) );
	return 0;
}
int TextDocument::setMetaInformation( lua_State * L )//( MetaInformation info, const QString & string )void
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	Enums enums( L );
	QTextDocument::MetaInformation info = (QTextDocument::MetaInformation)enums.MetaInformation( 2 );
	//QString* string = ValueInstaller2<QString>::check( L, 2 );
	obj->setMetaInformation( info, Util::toString( L, 2 ) );
	return 0;
}
int TextDocument::setPageSize( lua_State * L )//( const QSizeF & size )void
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	QSizeF* size = ValueInstaller2<QSizeF>::check( L, 2 );
	obj->setPageSize( *size );
	return 0;
}
int TextDocument::setPlainText( lua_State * L )//( const QString & text )void
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	//QString* text = ValueInstaller2<QString>::check( L, 2 );
	obj->setPlainText( Util::toString( L, 2 ) );
	return 0;
}
int TextDocument::setTextWidth( lua_State * L )//( qreal width )void
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	qreal width = Util::toInt( L, 2 );
	obj->setTextWidth( width );
	return 0;
}
int TextDocument::setUndoRedoEnabled( lua_State * L )//( bool enable )void
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	obj->setUndoRedoEnabled( Util::toBool( L, 2 ) );
	return 0;
}
int TextDocument::setUseDesignMetrics( lua_State * L )//( bool b )void
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	obj->setUseDesignMetrics( Util::toBool( L, 2 ) );
	return 0;
}
int TextDocument::size( lua_State * L )//const QSizeF
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	QSizeF* res = ValueInstaller2<QSizeF>::create( L );
	*res = obj->size();
	return 1;
}
int TextDocument::textWidth( lua_State * L )//const qreal
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	Util::push( L, obj->textWidth() );
	return 1;
}
int TextDocument::toHtml( lua_State * L )//( const QByteArray & encoding = QByteArray() ) const QString
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	//QByteArray* encoding = ValueInstaller2<QByteArray>::check( L, 2 );
	//QString* res = ValueInstaller2<QString>::create( L );
	//*res = obj->toHtml( *encoding );
	Util::push( L, obj->toHtml( Util::toStr( L, 2 ) ) );
	return 1;
}
int TextDocument::toPlainText( lua_State * L )//const QString
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	//QString* res = ValueInstaller2<QString>::create( L );
	//*res = obj->toPlainText();
	Util::push( L, obj->toPlainText() );
	return 1;
}
int TextDocument::undo( lua_State * L )//( QTextCursor * cursor )void
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	QTextCursor* cursor = ValueInstaller2<QTextCursor>::check( L, 2 );
	obj->undo( cursor );
	return 0;
}
int TextDocument::useDesignMetrics( lua_State * L )//const bool
{
	QTextDocument* obj = ObjectHelper<QTextDocument>::check( L, 1 );
	Util::push( L, obj->useDesignMetrics() );
	return 1;
}
static const luaL_reg _TextDocument[] = 
{
	{ Util::s_init, TextDocument::init },
	{ "addResource", TextDocument::addResource },
	{ "adjustSize", TextDocument::adjustSize },
//QVector<QTextFormat> allFormats () const
	{ "begin", TextDocument::begin },
	{ "blockCount", TextDocument::blockCount },
	{ "clear", TextDocument::clear },
	{ "clone", TextDocument::clone },
	{ "defaultFont", TextDocument::defaultFont },
	{ "defaultStyleSheet", TextDocument::defaultStyleSheet },
	{ "documentLayout", TextDocument::documentLayout },
	{ "drawContents", TextDocument::drawContents },
	{ "end", TextDocument::end },
	{ "find", TextDocument::find },
	{ "findBlock", TextDocument::findBlock },
	{ "idealWidth", TextDocument::idealWidth },
	{ "isEmpty", TextDocument::isEmpty },
	{ "isModified", TextDocument::isModified },
	{ "isRedoAvailable", TextDocument::isRedoAvailable },
	{ "isUndoAvailable", TextDocument::isUndoAvailable },
	{ "isUndoRedoEnabled", TextDocument::isUndoRedoEnabled },
	{ "markContentsDirty", TextDocument::markContentsDirty },
	{ "maximumBlockCount", TextDocument::maximumBlockCount },
	{ "metaInformation", TextDocument::metaInformation },
	{ "object", TextDocument::object },
	{ "objectForFormat", TextDocument::objectForFormat },
	{ "pageCount", TextDocument::pageCount },
	{ "pageSize", TextDocument::pageSize },
	{ "print", TextDocument::print },
	{ "redo", TextDocument::redo },
	{ "resource", TextDocument::resource },
	{ "rootFrame", TextDocument::rootFrame },
	{ "setDefaultFont", TextDocument::setDefaultFont },
	{ "setDefaultStyleSheet", TextDocument::setDefaultStyleSheet },
	{ "setDocumentLayout", TextDocument::setDocumentLayout },
	{ "setHtml", TextDocument::setHtml },
	{ "setMaximumBlockCount", TextDocument::setMaximumBlockCount },
	{ "setMetaInformation", TextDocument::setMetaInformation },
	{ "setPageSize", TextDocument::setPageSize },
	{ "setPlainText", TextDocument::setPlainText },
	{ "setTextWidth", TextDocument::setTextWidth },
	{ "setUndoRedoEnabled", TextDocument::setUndoRedoEnabled },
	{ "setUseDesignMetrics", TextDocument::setUseDesignMetrics },
	{ "size", TextDocument::size },
	{ "textWidth", TextDocument::textWidth },
	{ "toHtml", TextDocument::toHtml },
	{ "toPlainText", TextDocument::toPlainText },
	{ "undo", TextDocument::undo },
	{ "useDesignMetrics", TextDocument::useDesignMetrics },
	{ 0, 0 }
};
void TextDocument::install(lua_State * L){
	ObjectInstaller<QTextDocument>::install( L, _TextDocument, ObjectInstaller<QObject>::className());
}
