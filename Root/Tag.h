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

#if !defined(AFX_TAG_H__9A539B21_8A68_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_TAG_H__9A539B21_8A68_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Root/Units.h>

namespace Root
{
	//* Tag
	//. Ein Tag ist ein Symbol-String konstanter Länge, der sich in
	//. einen Integer konvertieren lässt. Ein Tag hat sowohl symbolische
	//. Bedeutung für die Verarbeitungssteuerung oder als Zustandsattribut, 
	//. und kann zugleich als Zeichenkette dargestellt werden.

	class Tag  
	{
	public:
		enum { MaxLen = 8 };

		int getLength() const;	// Anzahl Zeichen bis erstes 0 oder Ende.
		bool isNull() const;

		bool operator>( const Tag& ) const;
		bool operator!=( const Tag& ) const;
		bool operator<( const Tag& ) const;
		bool operator==( const Tag& ) const;
		bool operator ==(const char *) const;

		void getString( char*, int len ) const; // Länge inkl. Null
		QByteArray  getString() const;
		const char* data() const;

		Tag& operator =(int);
		Tag& operator =(const char*);
		Tag& operator=( const Tag& );
		Tag( int );
		Tag( const Tag& );
		Tag( const char*, int length = 0 );	// Wenn length=0 wird ::strlen verwendet.
		Tag();
		virtual ~Tag();
	private:
		Char d_tag[ MaxLen ];
	};
}

#endif // !defined(AFX_TAG_H__9A539B21_8A68_11D5_8DB0_00D00918E99C__INCLUDED_)
