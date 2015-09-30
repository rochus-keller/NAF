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

#include "XmlHandler.h"
#include <Expat/expat.h>
#include <QFile>
#include <QFileInfo>
using namespace Xml;

static void _StartElementHandler( void *userData, 
								 const XML_Char *name, const XML_Char **atts)
{
	XmlHandler* xh = (XmlHandler*)userData;
	XmlHandler::Atts a;
	for( int i = 0; atts[i]; i += 2 ) 
	{
		const int s = ::strlen( atts[i + 1] );
		if( s != 0 )
			a[ atts[i] ] = QString::fromUtf8( atts[i + 1], s + 1  );
			// Der Text kommt in Utf-8!
		/* Das bringt keinen Vorteil, nur mehr Speicher
		else
			a[ atts[i] ] = ""; // QT-BUG: fromRawData mit s==0 produziert !isEmpty()
		*/
	}

	if( !xh->startElement( name, a ) )
		XML_StopParser( (XML_Parser)xh->getParser(), false );
}

static void _EndElementHandler( void *userData, const XML_Char *name )
{
	XmlHandler* xh = (XmlHandler*)userData;
	if( !xh->endElement( name ) )
		XML_StopParser( (XML_Parser)xh->getParser(), false );
}

static void _CharacterDataHandler(void *userData, const XML_Char *s, int len)
{
	XmlHandler* xh = (XmlHandler*)userData;
	// Der Text kommt hier immer in Utf8.
	QByteArray str = QString::fromUtf8( s, len ).toLatin1();
	if( !xh->characters( str ) )
		XML_StopParser( (XML_Parser)xh->getParser(), false );
}

XmlHandler::XmlHandler()
{
	d_parser = 0;
	XML_Parser p = XML_ParserCreate( 0 ); // Decoding from File
	d_parser = p;

	XML_SetUserData( p, this );

	XML_SetElementHandler( p, _StartElementHandler, _EndElementHandler);
	XML_SetCharacterDataHandler( p, _CharacterDataHandler );
	// TODO: braucht man CDATA Start und Stop Handler?
}

XmlHandler::~XmlHandler()
{
	if( d_parser )
	{
		XML_ParserFree( (XML_Parser)d_parser );
	}
}

bool XmlHandler::parse( const char* fileName )
{
	d_error = "";

	QFileInfo info( fileName );
	if( !info.exists() )
	{
		d_error.sprintf( "File '%s' doesn't exist", fileName );
		return false;
	}
    QFile file( fileName );
	if( !file.open( QIODevice::ReadOnly ) )
	{
		d_error.sprintf( "Cannot open file '%s'", fileName );
		return false;
	}
	const int bufSize = 128;
	char buf[ bufSize ];
	int len;
	XML_Parser p = (XML_Parser)d_parser;
	for(;;) 
	{
		len = file.read( buf, bufSize );
		if( len == -1 )
		{
			d_error.sprintf( "Cannot open file '%s'", fileName );
			return false;
		}

		if( XML_Parse( p, buf, len, len == 0 ) == XML_STATUS_ERROR ) 
		{
			readError();
			return false;
		}
		if( !d_error.isEmpty() )
			return false;

		if( len == 0 )
			break;
	}
	return true;
}

void XmlHandler::readError()
{
	XML_Parser p = (XML_Parser)d_parser;
	XML_Error c = XML_GetErrorCode( p );
	if( c != XML_ERROR_ABORTED )
		d_error.sprintf( "Syntax error at %d/%d:\n%s",
            int(XML_GetCurrentLineNumber( p )), int(XML_GetCurrentColumnNumber( p ) + 1),
			XML_ErrorString( c ) );
	else
	{
		QString tmp;
		tmp.sprintf( "Validation error at %d/%d:\n",
            int(XML_GetCurrentLineNumber( p )), int(XML_GetCurrentColumnNumber( p ) + 1) );
		d_error = tmp + d_error;
	}
}

bool XmlHandler::parseString( const char* xml )
{
	d_error = "";

	XML_Parser p = (XML_Parser)d_parser;

	if( XML_Parse( p, xml, ::strlen( xml ), true ) == XML_STATUS_ERROR ) 
	{
		readError();
		return false;
	}
	return d_error.isEmpty();
}

bool XmlHandler::characters( const QByteArray& )
{
	return true;
}

bool XmlHandler::endElement( Root::SymbolString name )
{
	return true;
}

bool XmlHandler::startElement( Root::SymbolString name, const Atts& atts )
{
	return true;
}

