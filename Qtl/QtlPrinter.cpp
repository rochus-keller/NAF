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
#include "QtlMyPrinter.h"
#include <QPrinter>
#include <QPaintEngine>
#include <QString>
#include "Enums.h"
#include <Script/ValueInstaller.h>
#include "Variant.h"
using namespace std;
using namespace Qtl;
using namespace Lua;

GCC_IGNORE(-Wunused-variable);

int Printer::abort(lua_State * L) // : bool
{ 
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	Util::push( L, lhs->abort() );
	return 1; 
}
int Printer::collateCopies(lua_State * L) // const : bool 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	Util::push( L, lhs->collateCopies() );
	return 1;
}  
int Printer::colorMode(lua_State * L) // const : ColorMode 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	Util::push( L, lhs->colorMode() );
	return 1; 
} 
int Printer::creator(lua_State * L) // const : QString 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	lua_pushstring(L, lhs->creator());
	return 1; 
}
int Printer::docName(lua_State * L) // const : QString 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	lua_pushstring(L, lhs->docName());
	return 1; 
}
int Printer::doubleSidedPrinting(lua_State * L) // const : bool 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	Util::push( L, lhs->doubleSidedPrinting() );
	return 1;
}
int Printer::fontEmbeddingEnabled(lua_State * L) // const : bool 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	Util::push( L, lhs->fontEmbeddingEnabled() );
	return 1;
}
int Printer::fromPage(lua_State * L) // const : int 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	Util::push( L, lhs->fromPage() );
	return 1;
}
int Printer::fullPage(lua_State * L) // const : bool 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	Util::push( L, lhs->fullPage() );
	return 1;
}
int Printer::newPage(lua_State * L) // : bool 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	Util::push( L, lhs->newPage() );
	return 1;
}
int Printer::numCopies(lua_State * L) // const : int 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	Util::push( L, lhs->numCopies() );
	return 1;
} 
int Printer::orientation(lua_State * L) // const : Orientation 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	Util::push( L, lhs->orientation() );
	return 1; 
} 
int Printer::outputFileName(lua_State * L) // const : QString 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	lua_pushstring(L, lhs->outputFileName());
	return 1; 
}
int Printer::outputFormat(lua_State * L) // const : OutputFormat 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	Util::push( L, lhs->outputFormat() );
	return 1; 
} 
int Printer::pageOrder(lua_State * L) // const : PageOrder 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	Util::push( L, lhs->pageOrder() );
	return 1; 
}
int Printer::pageRect(lua_State * L) // const : QRect 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	QRectF* res = ValueInstaller2<QRectF>::create( L );
	*res = lhs->pageRect();
	return 1; 
}
int Printer::pageSize(lua_State * L) // const : PageSize 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	Util::push( L, lhs->pageSize() );
	return 1; 
}/*
int Printer::paintEngine(lua_State * L) // const : QPaintEngine* 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	QPaintEngine* res = ValueInstaller2<QPaintEngine>::create( L );
	*res = lhs->paintEngine();
	return 1; 
}*/
int Printer::paperRect(lua_State * L) // const : QRect 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	QRectF* res = ValueInstaller2<QRectF>::create( L );
	*res = lhs->paperRect();
	return 1; 
}
int Printer::paperSource(lua_State * L) // const : PaperSource 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	Util::push( L, lhs->paperSource() );
	return 1; 
}
int Printer::printProgram(lua_State * L) // const : QString 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	lua_pushstring(L, lhs->printProgram());
	return 1; 
}
int Printer::printRange(lua_State * L) // const : PrintRange 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	Util::push( L, lhs->printRange() );
	return 1; 
}
int Printer::printerName(lua_State * L) // const : QString 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	lua_pushstring(L, lhs->printerName());
	return 1; 
}/*
int Printer::printerSelectionOption(lua_State * L) // const : QString 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	lua_pushstring( L, lhs->printerSelectionOption() );
	return 1; 
}*/
int Printer::printerState(lua_State * L) // const : PrinterState 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	Util::push( L, lhs->printerState() );
	return 1; 
}
int Printer::resolution(lua_State * L) // const : int 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	Util::push( L, lhs->resolution() );
	return 1;
}
int Printer::setCollateCopies(lua_State * L) // ( bool collate )
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	lhs->setCollateCopies( Util::toBool( L, 2 ) );
	return 0;
}
int Printer::setColorMode(lua_State * L) // ( ColorMode newColorMode )
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	Enums enums(L);
	QPrinter::ColorMode f=(QPrinter::ColorMode)enums.ColorMode( 2 );
	lhs->setColorMode( f );
	return 0;
}
int Printer::setCreator(lua_State * L) // ( const QString & creator )
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	lhs->setCreator( Lua::Util::toStr( L, 2 ));
	return 0;
}
int Printer::setDocName(lua_State * L) // ( const QString & name )
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	lhs->setDocName( Lua::Util::toStr( L, 2 ));
	return 0;
}
int Printer::setDoubleSidedPrinting(lua_State * L) // ( bool doubleSided )
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	lhs->setDoubleSidedPrinting( Util::toBool( L, 2 ) );
	return 0;
}
int Printer::setFontEmbeddingEnabled(lua_State * L) // ( bool enable )
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	lhs->setFontEmbeddingEnabled( Util::toBool( L, 2 ) );
	return 0;
}
int Printer::setFromTo(lua_State * L) // ( int from, int to )
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	lhs->setFromTo( Util::toInt( L, 2 ), Util::toInt( L, 3 ) );
	return 0;
}
int Printer::setFullPage(lua_State * L) // ( bool fp )
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	lhs->setFullPage( Util::toBool( L, 2 ) );
	return 0;
}
int Printer::setNumCopies(lua_State * L) // ( int numCopies )
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	lhs->setNumCopies( Util::toInt( L, 2 ) );
	return 0;
}
int Printer::setOrientation(lua_State * L) // (  Orientation orientation )
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	Enums enums(L);
	QPrinter::Orientation f=(QPrinter::Orientation)enums.Orientation( 2 );
	lhs->setOrientation( f );
	return 0;
}
int Printer::setOutputFileName(lua_State * L) // ( const QString & fileName )
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	lhs->setOutputFileName( Lua::Util::toStr( L, 2 ));
	return 0;
}
int Printer::setOutputFormat(lua_State * L) // (  OutputFormat format )
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	Enums enums(L);
	QPrinter::OutputFormat f=(QPrinter::OutputFormat)enums.OutputFormat( 2 );
	lhs->setOutputFormat( f );
	return 0;
}
int Printer::setPageOrder(lua_State * L) // (  PageOrder pageOrder  )
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	Enums enums(L);
	QPrinter::PageOrder f=(QPrinter::PageOrder)enums.PageOrder( 2 );
	lhs->setPageOrder( f );
	return 0;
}
int Printer::setPageSize(lua_State * L) // (  PageSize newPageSize  )
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	Enums enums(L);
	QPrinter::PageSize f=(QPrinter::PageSize)enums.PageSize( 2 );
	lhs->setPageSize( f );
	return 0;
}
int Printer::setPaperSource(lua_State * L) // (  PaperSource source  )
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	Enums enums(L);
	QPrinter::PaperSource f=(QPrinter::PaperSource)enums.PaperSource( 2 );
	lhs->setPaperSource( f );
	return 0;
}
int Printer::setPrintProgram(lua_State * L) // ( const QString & printProg )
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	lhs->setPrintProgram( Lua::Util::toStr( L, 2 ));
	return 0;
}
int Printer::setPrintRange(lua_State * L) // ( PrintRange range )
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	Enums enums(L);
	QPrinter::PrintRange f=(QPrinter::PrintRange)enums.PrintRange( 2 );
	lhs->setPrintRange( f );
	return 0;
}
int Printer::setPrinterName(lua_State * L) // ( const QString & name )
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	lhs->setPrinterName( Lua::Util::toStr( L, 2 ));
	return 0;
}
/*
int Printer::setPrinterSelectionOption(lua_State * L) // ( const QString & option )
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	lhs->setPrinterSelectionOption( Lua::Util::toStr( L, 2 ));
	return 0;
}*/
int Printer::setResolution(lua_State * L) // ( int dpi )
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	lhs->setResolution( Lua::Util::toInt( L, 2 ));
	return 0;
}

int Printer::setWinPageSize(lua_State * L) // ( int pageSize )
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	//lhs->setWinPageSize( Lua::Util::toInt( L, 2 ));
	return 0;
}

int Printer::toPage(lua_State * L) // const : int 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	Util::push( L, lhs->toPage() );
	return 1;
}
int Printer::winPageSize(lua_State * L) // const : int 
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	//Util::push( L, lhs->winPageSize() );
	return 1;
}/*
int Printer::supportedResolutions(lua_State * L) // const : QList<int>
{
	MyPrinter* lhs = ValueInstaller2<MyPrinter>::check( L, 1 );
	QList* res = ValueInstaller2<QList>::create( L );
	return 1;
}*/
int Printer::init(lua_State * L)
{
	return 0;
}
static const luaL_reg _Printer[] = 
{
	{ Util::s_init, Printer::init },
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
	//{ "paintEngine", Printer::paintEngine },
	{ "paperRect", Printer::paperRect },
	{ "paperSource", Printer::paperSource },
	{ "printProgram", Printer::printProgram },
	{ "printRange", Printer::printRange },
	{ "printerName", Printer::printerName },
	//{ "printerSelectionOption", Printer::printerSelectionOption },
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
	//{ "setPrinterSelectionOption", Printer::setPrinterSelectionOption },
	{ "setResolution", Printer::setResolution },
	//{ "setWinPageSize", Printer::setWinPageSize },
	{ "toPage", Printer::toPage },
	//{ "winPageSize", Printer::winPageSize },
	//{ "supportedResolutions", Printer::supportedResolutions },
	{ 0, 0 }
};
void Printer::install(lua_State * L){
	ValueInstaller2<MyPrinter>::install( L, _Printer, "QPrinter" );
}
