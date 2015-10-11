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

#include "QtlPrinter.h"
#include "QtlWidget.h"
#include <QPrinter>
#include <Script2/ValueBinding.h>
#include <Script/Util.h>
#include <Script2/QtValue.h>
using namespace Qtl;
using namespace Lua;

class MyQPrinter : public QPrinter
{
 public:
	MyQPrinter():QPrinter(QPrinter::HighResolution) {}
	MyQPrinter(const MyQPrinter& ):QPrinter(QPrinter::HighResolution) {}
	void operator =(const MyQPrinter &){}
};

int Printer::abort(lua_State * L) // : bool
{ 
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	Util::push( L, lhs->abort() );
	return 1; 
}
int Printer::collateCopies(lua_State * L) // const : bool 
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	Util::push( L, lhs->collateCopies() );
	return 1;
}  
int Printer::colorMode(lua_State * L) // const : ColorMode 
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	Util::push( L, lhs->colorMode() );
	return 1; 
} 
int Printer::creator(lua_State * L) // const : QString 
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
    *QtValue<QString>::create( L ) = lhs->creator();
	return 1; 
}
int Printer::docName(lua_State * L) // const : QString 
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
    *QtValue<QString>::create( L ) = lhs->docName();
	return 1; 
}
int Printer::doubleSidedPrinting(lua_State * L) // const : bool 
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	Util::push( L, lhs->doubleSidedPrinting() );
	return 1;
}
int Printer::fontEmbeddingEnabled(lua_State * L) // const : bool 
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	Util::push( L, lhs->fontEmbeddingEnabled() );
	return 1;
}
int Printer::fromPage(lua_State * L) // const : int 
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	Util::push( L, lhs->fromPage() );
	return 1;
}
int Printer::fullPage(lua_State * L) // const : bool 
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	Util::push( L, lhs->fullPage() );
	return 1;
}
int Printer::newPage(lua_State * L) // : bool 
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	Util::push( L, lhs->newPage() );
	return 1;
}
int Printer::numCopies(lua_State * L) // const : int 
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	Util::push( L, lhs->numCopies() );
	return 1;
} 
int Printer::orientation(lua_State * L) // const : Orientation 
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	Util::push( L, lhs->orientation() );
	return 1; 
} 
int Printer::outputFileName(lua_State * L) // const : QString 
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
    *QtValue<QString>::create( L ) = lhs->outputFileName();
	return 1; 
}
int Printer::outputFormat(lua_State * L) // const : OutputFormat 
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	Util::push( L, lhs->outputFormat() );
	return 1; 
} 
int Printer::pageOrder(lua_State * L) // const : PageOrder 
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	Util::push( L, lhs->pageOrder() );
	return 1; 
}
int Printer::pageRect(lua_State * L) // const : QRect 
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	*ValueBinding<QRectF>::create( L ) = lhs->pageRect();
	return 1; 
}
int Printer::pageSize(lua_State * L) // const : PageSize 
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	Util::push( L, lhs->pageSize() );
	return 1; 
}
int Printer::paperRect(lua_State * L) // const : QRect 
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	*ValueBinding<QRectF>::create( L ) = lhs->paperRect();
	return 1; 
}
int Printer::paperSource(lua_State * L) // const : PaperSource 
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	Util::push( L, lhs->paperSource() );
	return 1; 
}
int Printer::printProgram(lua_State * L) // const : QString 
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
    *QtValue<QString>::create( L ) = lhs->printProgram();
	return 1; 
}
int Printer::printRange(lua_State * L) // const : PrintRange 
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	Util::push( L, lhs->printRange() );
	return 1; 
}
int Printer::printerName(lua_State * L) // const : QString 
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
    *QtValue<QString>::create( L ) = lhs->printerName();
	return 1; 
}
int Printer::printerSelectionOption(lua_State * L) // const : QString 
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
#ifndef _WIN32
	*QtValue<QString>::create( L ) = lhs->printerSelectionOption();
#endif
	return 1; 
}
int Printer::printerState(lua_State * L) // const : PrinterState 
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	Util::push( L, lhs->printerState() );
	return 1; 
}
int Printer::resolution(lua_State * L) // const : int 
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	Util::push( L, lhs->resolution() );
	return 1;
}
int Printer::setCollateCopies(lua_State * L) // ( bool collate )
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	lhs->setCollateCopies( Util::toBool( L, 2 ) );
	return 0;
}
int Printer::setColorMode(lua_State * L) // ( ColorMode newColorMode )
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	QPrinter::ColorMode f=(QPrinter::ColorMode)Util::toInt( L, 2 );
	lhs->setColorMode( f );
	return 0;
}
int Printer::setCreator(lua_State * L) // ( const QString & creator )
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	lhs->setCreator( QtValueBase::toString( L, 2 ));
	return 0;
}
int Printer::setDocName(lua_State * L) // ( const QString & name )
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	lhs->setDocName( QtValueBase::toString( L, 2 ));
	return 0;
}
int Printer::setDoubleSidedPrinting(lua_State * L) // ( bool doubleSided )
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	lhs->setDoubleSidedPrinting( Util::toBool( L, 2 ) );
	return 0;
}
int Printer::setFontEmbeddingEnabled(lua_State * L) // ( bool enable )
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	lhs->setFontEmbeddingEnabled( Util::toBool( L, 2 ) );
	return 0;
}
int Printer::setFromTo(lua_State * L) // ( int from, int to )
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	lhs->setFromTo( Util::toInt( L, 2 ), Util::toInt( L, 3 ) );
	return 0;
}
int Printer::setFullPage(lua_State * L) // ( bool fp )
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	lhs->setFullPage( Util::toBool( L, 2 ) );
	return 0;
}
int Printer::setNumCopies(lua_State * L) // ( int numCopies )
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	lhs->setNumCopies( Util::toInt( L, 2 ) );
	return 0;
}
int Printer::setOrientation(lua_State * L) // (  Orientation orientation )
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	QPrinter::Orientation f=(QPrinter::Orientation)Util::toInt( L, 2 );
	lhs->setOrientation( f );
	return 0;
}
int Printer::setOutputFileName(lua_State * L) // ( const QString & fileName )
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	lhs->setOutputFileName( QtValueBase::toString( L, 2 ));
	return 0;
}
int Printer::setOutputFormat(lua_State * L) // (  OutputFormat format )
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	QPrinter::OutputFormat f=(QPrinter::OutputFormat)Util::toInt( L, 2 );
	lhs->setOutputFormat( f );
	return 0;
}
int Printer::setPageOrder(lua_State * L) // (  PageOrder pageOrder  )
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	QPrinter::PageOrder f=(QPrinter::PageOrder)Util::toInt( L, 2 );
	lhs->setPageOrder( f );
	return 0;
}
int Printer::setPageSize(lua_State * L) // (  PageSize newPageSize  )
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	QPrinter::PageSize f=(QPrinter::PageSize)Util::toInt( L, 2 );
	lhs->setPageSize( f );
	return 0;
}
int Printer::setPaperSource(lua_State * L) // (  PaperSource source  )
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	QPrinter::PaperSource f=(QPrinter::PaperSource)Util::toInt( L, 2 );
	lhs->setPaperSource( f );
	return 0;
}
int Printer::setPrintProgram(lua_State * L) // ( const QString & printProg )
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	lhs->setPrintProgram( QtValueBase::toString( L, 2 ));
	return 0;
}
int Printer::setPrintRange(lua_State * L) // ( PrintRange range )
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	QPrinter::PrintRange f=(QPrinter::PrintRange)Util::toInt( L, 2 );
	lhs->setPrintRange( f );
	return 0;
}
int Printer::setPrinterName(lua_State * L) // ( const QString & name )
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	lhs->setPrinterName( QtValueBase::toString( L, 2 ));
	return 0;
}
int Printer::setPrinterSelectionOption(lua_State * L) // ( const QString & option )
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
#ifndef _WIN32
	lhs->setPrinterSelectionOption( QtValueBase::toString( L, 2 ));
#endif
	return 0;
}
int Printer::setResolution(lua_State * L) // ( int dpi )
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	lhs->setResolution( Util::toInt( L, 2 ));
	return 0;
}
int Printer::toPage(lua_State * L) // const : int 
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	Util::push( L, lhs->toPage() );
	return 1;
}
int Printer::supportedResolutions(lua_State * L) // const : QList<int>
{
	QPrinter* lhs = ValueBinding<MyQPrinter>::check( L, 1 );
	QList<int> resolutions = lhs->supportedResolutions();
	lua_createtable( L, resolutions.size(), 0 );
	const int table = lua_gettop( L );
	for( int i = 0; i < resolutions.size(); i++ )
	{
		Util::push( L, resolutions[i] );
		lua_rawseti( L, table, i + 1 );
	}
	return 1;
}
static const luaL_reg _Printer[] = 
{
	{ "abort", Printer::abort },
	{ "collateCopies", Printer::collateCopies },
	{ "colorMode", Printer::colorMode },
	{ "creator", Printer::creator },
	{ "docName", Printer::docName },
	{ "doubleSidedPrinting", Printer::doubleSidedPrinting },
	{ "fontEmbeddingEnabled", Printer::fontEmbeddingEnabled },
	{ "fromPage", Printer::fromPage },
	{ "fullPage", Printer::fullPage },
	{ "newPage", Printer::newPage },
	{ "numCopies", Printer::numCopies },
	{ "orientation", Printer::orientation },
	{ "outputFileName", Printer::outputFileName },
	{ "outputFormat", Printer::outputFormat },
	{ "pageOrder", Printer::pageOrder },
	{ "pageRect", Printer::pageRect },
	{ "pageSize", Printer::pageSize },
	{ "paperRect", Printer::paperRect },
	{ "paperSource", Printer::paperSource },
	{ "printProgram", Printer::printProgram },
	{ "printRange", Printer::printRange },
	{ "printerName", Printer::printerName },
	{ "printerSelectionOption", Printer::printerSelectionOption },
	{ "printerState", Printer::printerState },
	{ "resolution", Printer::resolution },
	{ "setCollateCopies", Printer::setCollateCopies },
	{ "setColorMode", Printer::setColorMode },
	{ "setCreator", Printer::setCreator },
	{ "setDocName", Printer::setDocName },
	{ "setDoubleSidedPrinting", Printer::setDoubleSidedPrinting },
	{ "setFontEmbeddingEnabled", Printer::setFontEmbeddingEnabled },
	{ "setFromTo", Printer::setFromTo },
	{ "setFullPage", Printer::setFullPage },
	{ "setNumCopies", Printer::setNumCopies },
	{ "setOrientation", Printer::setOrientation },
	{ "setOutputFileName", Printer::setOutputFileName },
	{ "setOutputFormat", Printer::setOutputFormat },
	{ "setPageOrder", Printer::setPageOrder },
	{ "setPageSize", Printer::setPageSize },
	{ "setPaperSource", Printer::setPaperSource },
	{ "setPrintProgram", Printer::setPrintProgram },
	{ "setPrintRange", Printer::setPrintRange },
	{ "setPrinterName", Printer::setPrinterName },
	{ "setPrinterSelectionOption", Printer::setPrinterSelectionOption },
	{ "setResolution", Printer::setResolution },
	{ "toPage", Printer::toPage },
	{ "supportedResolutions", Printer::supportedResolutions },
	{ 0, 0 }
};
void Printer::install(lua_State * L){
	ValueBinding<MyQPrinter>::install( L, "QPrinter", _Printer );
	ValueBinding<MyQPrinter>::addMethods( L, PaintDevice< ValueBinding<MyQPrinter> >::_reg ); // Mixin
}
