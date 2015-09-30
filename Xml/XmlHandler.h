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

#ifndef _XML_XMLHANDLER
#define _XML_XMLHANDLER

#include <Root/SymbolString.h>
#include <QString>
#include <QMap>

namespace Xml
{
	class XmlHandler
	{
	public:
		typedef QMap<Root::SymbolString,QByteArray> Atts;

		XmlHandler();
		~XmlHandler();

		//* To Override
		virtual bool characters( const QByteArray& );
		virtual bool endElement( Root::SymbolString name );
		virtual bool startElement( Root::SymbolString name, const Atts& atts );
		// Vorsicht: atts.value wird mit fromRawData gesetzt, also kopiere z.B. mit trimmed()
		//-

		bool parse( const char* fileName );
		bool parseString( const char* xml );
		const QString& getError() const { return d_error; }
		void* getParser() const { return d_parser; }
	private:
		void* d_parser;
	protected:
		QString d_error;
		void readError();
	};
}

#endif // _XML_XMLHANDLER
