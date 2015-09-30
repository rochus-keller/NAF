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

#if !defined(AFX_EASYPROTONLIST_H__62AF191E_17F2_41DB_8359_DE7823D25480__INCLUDED_)
#define AFX_EASYPROTONLIST_H__62AF191E_17F2_41DB_8359_DE7823D25480__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Object.h>
#include <Root/Vector.h>
#include <Root/SymbolString.h>
#include <Spec/Units.h>

namespace Spec
{
	class EasyProtonList : public Root::Object
	{
	public:
		struct Atom 
		{
			Root::Index d_spin;		// Atom-Nummer
			Root::Index d_sys;		// Fragment-Nummer
			PPM d_shift;
			Root::SymbolString d_label;
			bool isValid() const { return d_shift != 999.0 &&
				d_spin != -9999; }
			Atom():d_spin(-9999),d_shift(999.0),d_sys(0){}
		};
		Root::Index getCount() const { return d_atoms.size(); }
		const Atom& getAtom( Root::Index i ) const { return d_atoms[ i ]; }
		void setAtom( Root::Index i, const Atom& a ) { d_atoms[ i ] = a; }
		void addAtom( const Atom& a ) { d_atoms.push_back( a ); }
		void writeToFile( const char* path );

		EasyProtonList(const char* filePath);
		EasyProtonList( Root::Index count );
		EasyProtonList() {}
	protected:
		virtual ~EasyProtonList();
	private:
		void readFromFile();
		QByteArray  d_filePath;
		Root::Vector<Atom> d_atoms;
	};
}

#endif // !defined(AFX_EASYPROTONLIST_H__62AF191E_17F2_41DB_8359_DE7823D25480__INCLUDED_)
