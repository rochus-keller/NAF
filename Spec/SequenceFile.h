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

#if !defined(AFX_SEQUENCEFILE_H__6041238B_7F89_4CA2_8D39_99955CD81798__INCLUDED_)
#define AFX_SEQUENCEFILE_H__6041238B_7F89_4CA2_8D39_99955CD81798__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Object.h>
#include <Spec/Residue.h>
#include <Root/Vector.h>

namespace Spec
{
	class Repository;

	class SequenceFile : public Root::Object
	{
	public:
		struct Slot 
		{
			Root::Ref<Residue> d_resi;
			Root::Index d_sys;				// optionale System-ID oder 0
			Slot():d_sys(0) {}
		};
		typedef std::map<Root::Index,Slot> Slots;

		void loadFromFile( Repository*,	const char* path, 
			Root::Index start = 0 ); // throws Exception
			// start == 0 heisst, dass nur automatische ID generiert wird, wenn keine Nr. angegeben.
		bool checkSystems() const;
		//. Prüfe ob alle d_sys unique 
		bool addResidue( Residue*, Root::Index = 0 );
		void addResidue( Repository*, Root::Index, Root::SymbolString ); // throws Exception
		void writeToFile( const char*, bool useNr = false );
		void renumber();
		void removeResidue( Root::Index );

		const Slots& getSlots() const { return d_seq; }
		bool hasSys() const { return d_hasSys; }
		bool hasId() const { return d_hasId; }
		SequenceFile();
	protected:
		~SequenceFile() {}
	private:
		Slots d_seq;
		bool d_hasSys;
		bool d_hasId;
	};
}

#endif // !defined(AFX_SEQUENCEFILE_H__6041238B_7F89_4CA2_8D39_99955CD81798__INCLUDED_)
