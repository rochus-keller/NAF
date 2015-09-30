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

#if !defined(AFX_SYMBOLSTRING_H__528EF813_C605_4EA6_AD65_7CCB4EBA4EA4__INCLUDED_)
#define AFX_SYMBOLSTRING_H__528EF813_C605_4EA6_AD65_7CCB4EBA4EA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Units.h>
#include <QString>
#include <set>

namespace Root
{
	//* SymbolString
	//. Value-Object. Repräsentiert einen "immutable" String, d.h. einer,
	//. der nach seiner Erzeugung nicht mehr geändert wird. Ein neuer
	//. String wird wenn vorhanden auf einen gleichlautenden bestehenden
	//. abgebildet. Zeichenkettenvergleiche erübrigen sich daher, es muss
	//. nur die Adresse auf Gleichheit geprüft werden.

	class SymbolString  
	{
	public:
		typedef Root::UInt32 Symbol;

		SymbolString();
		SymbolString( const QString& );
		SymbolString( const QByteArray& );
		SymbolString( const SymbolString& );
		SymbolString( const char* );
		SymbolString( const char*, int len );
		SymbolString( Symbol );
		virtual ~SymbolString();

		SymbolString& operator=(const SymbolString & rhs);

		static const char* getAtom( const char* );
		bool operator<(const SymbolString & rhs) const { return d_str < rhs.d_str; }
		bool operator>(const SymbolString & rhs) const { return d_str > rhs.d_str; }
		bool operator==( const SymbolString& rhs ) const { return d_str == rhs.d_str; }
		bool operator!=( const SymbolString& rhs ) const { return d_str != rhs.d_str; }
		bool operator==( const char* rhs ) const { return d_str == getAtom( rhs ); }
		bool operator!=( const char* rhs ) const { return d_str != getAtom( rhs ); }
		// RISK: da STL diesen Operator für map und set benötigt, muss hier Adresse 
		// verglichen werden, ansonsten Vorteil weg. Stringvergleich mit getString() und <.
		operator const char*() const { return d_str; }
		operator const QByteArray () const { return d_str; }
        operator const QString () const { return QString::fromLatin1( d_str ); }
		const char* data() const { return d_str; }
		bool empty() const { return d_str == 0 || *d_str == 0; }
		bool isNull() const { return d_str == 0 || *d_str == 0; }
		Symbol getSymbol() const { return (Symbol) d_str; } // RISK
	private:
		const char* d_str;
	};

	typedef std::set<SymbolString> SymbolSet;

}

extern uint qHash( const Root::SymbolString& s );

Q_DECLARE_METATYPE( Root::SymbolString )

#endif // !defined(AFX_SYMBOLSTRING_H__528EF813_C605_4EA6_AD65_7CCB4EBA4EA4__INCLUDED_)
