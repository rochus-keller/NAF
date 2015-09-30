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

#if !defined(AFX_PRINTER_H__EFF8E48F_5BA2_428E_9473_6FD47077ACF8__INCLUDED_)
#define AFX_PRINTER_H__EFF8E48F_5BA2_428E_9473_6FD47077ACF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qprinter.h> 
#include <Q3Picture>
#include <Lexi/Units.h>
#include <Lexi/Viewport.h>
struct _PrinterImp;

namespace Lexi
{
	class Printer : public QPrinter  
	{
	public:
		struct PageSize
		{
			float w, h;
		};
		static const PageSize s_sizes[];
		static const char* s_sizeNames[];

		void print();
		Glyph* getBody() const;
		void setBody( Glyph* body );
		Viewport* getViewport() const;
		Printer(Glyph* body = 0);
		virtual ~Printer();
		void setPageSize(int size);

		static float inchToMm( float inch ) { return inch * 25.42; }
		static float mmToInch( float mm ) { return mm * 3.94e-2; }
		static float mmToPoint( float mm ) { return mm * 3.94e-2 * 72.0; }
		static float mmToTwip( float mm ) { return mm * 3.94e-2 * 1440.0; }
		static float twipToMm( float twip ) { return twip / ( 3.94e-2 * 1440.0 ); }
	private:
		Root::Ptr<_PrinterImp> d_this;
	};

	class Picture : public Q3Picture  
	{
	public:
		void print();
		void setPageSize(int size);
		void setLandscape( bool l ) { d_landscape = l; }
		Glyph* getBody() const;
		void setBody( Glyph* body );
		Viewport* getViewport() const;
		Picture(Glyph* body = 0);
		virtual ~Picture();
	private:
		Root::Ptr<_PrinterImp> d_this;
		bool d_landscape;
	};
}

#endif // !defined(AFX_PRINTER_H__EFF8E48F_5BA2_428E_9473_6FD47077ACF8__INCLUDED_)
