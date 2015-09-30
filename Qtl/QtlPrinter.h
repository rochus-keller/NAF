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

#if !defined(_QTL_PRINTER)
#define _QTL_PRINTER

typedef struct lua_State lua_State;




namespace Qtl
{
	class Printer
	{
	public:
		static int abort(lua_State * L); // : bool
		static int collateCopies(lua_State * L); // const : bool 
		static int colorMode(lua_State * L); // const : ColorMode
		static int creator(lua_State * L); // const : QString 
		static int docName(lua_State * L); // const : QString 
		static int doubleSidedPrinting(lua_State * L); // const : bool 
		static int fontEmbeddingEnabled(lua_State * L); // const : bool 
		static int fromPage(lua_State * L); // const : int
		static int fullPage(lua_State * L); // const : bool 
		static int newPage(lua_State * L); // : bool 
		static int numCopies(lua_State * L); // const : int 
		static int orientation(lua_State * L); // const : Orientation 
		static int outputFileName(lua_State * L); // const : QString 
		static int outputFormat(lua_State * L); // const : OutputFormat 
		static int pageOrder(lua_State * L); // const : PageOrder
		static int pageRect(lua_State * L); // const : QRect
		static int pageSize(lua_State * L); // const : PageSize 
		//static int paintEngine(lua_State * L); // const : QPaintEngine*
		static int paperRect(lua_State * L); // const : QRect 
		static int paperSource(lua_State * L); // const : PaperSource
		static int printProgram(lua_State * L); // const : QString 
		static int printRange(lua_State * L); // const : PrintRange 
		static int printerName(lua_State * L); // const : QString 
		//static int printerSelectionOption(lua_State * L); // const : QString 
		static int printerState(lua_State * L); // const : PrinterState
		static int resolution(lua_State * L); // const : int
		static int setCollateCopies(lua_State * L); // ( bool collate )
		static int setColorMode(lua_State * L); // ( ColorMode newColorMode )
		static int setCreator(lua_State * L); // ( const QString & creator )
		static int setDocName(lua_State * L); // ( const QString & name )
		static int setDoubleSidedPrinting(lua_State * L); // ( bool doubleSided )
		static int setFontEmbeddingEnabled(lua_State * L); // ( bool enable )
		static int setFromTo(lua_State * L); // ( int from, int to )
		static int setFullPage(lua_State * L); // ( bool fp )
		static int setNumCopies(lua_State * L); // ( int numCopies )
		static int setOrientation(lua_State * L); // (  Orientation orientation )
		static int setOutputFileName(lua_State * L); // ( const QString & fileName )
		static int setOutputFormat(lua_State * L); // (  OutputFormat format )
		static int setPageOrder(lua_State * L); // (  PageOrder pageOrder  )
		static int setPageSize(lua_State * L); // (  PageSize newPageSize  )
		static int setPaperSource(lua_State * L); // (  PaperSource source  )
		static int setPrintProgram(lua_State * L); // ( const QString & printProg )
		static int setPrintRange(lua_State * L); // ( PrintRange range )
		static int setPrinterName(lua_State * L); // ( const QString & name )
		//static int setPrinterSelectionOption(lua_State * L); // ( const QString & option )
		static int setResolution(lua_State * L); // ( int dpi )
		static int setWinPageSize(lua_State * L); // ( int pageSize )
		static int toPage(lua_State * L); // const : int 
		static int winPageSize(lua_State * L); // const : int
		//static int supportedResolutions(lua_State * L); // const : int 
		static int init(lua_State * L);
		static void install(lua_State * L);


	};

}

#endif // !defined(_QTL_PRINTER)
