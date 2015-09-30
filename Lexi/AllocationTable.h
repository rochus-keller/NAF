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

#if !defined(AFX_ALLOCATIONTABLE_H__000E5120_DA89_11D4_B823_00D00918E99C__INCLUDED_)
#define AFX_ALLOCATIONTABLE_H__000E5120_DA89_11D4_B823_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi/Allocation.h>

struct _AllocationTableImp;

namespace Lexi
{
	class AllocationTable 
	{
	public:
		class Entry 
		{
		public:
			Entry( const Allocation& a ):d_allocation( a ) {}
			const Allocation& getAllocation() const { return d_allocation; }
			const AllocationVector& getComponents() const { return d_components; }
			AllocationVector& getComponents() { return d_components; }
		private:
			friend class AllocationTable;
			Allocation d_allocation;
			AllocationVector d_components;
		};

		AllocationTable( GlyphIndex count = 1, long max = 5 );
		virtual ~AllocationTable();

		Entry* allocate(const Allocation&);
		//. Erzeuge eine neue Allocation am Ende der Liste.
		Entry* findSameSize(const Allocation& a, Twips& dx, Twips& dy );
		Entry* find( const Allocation& ) const;
		//. Suche nach der exakten Allocation. Falls sie vorhanden ist, verschiebe sie
		//. an den Schluss und gebe sie zurck. Andernfalls gebe nil zurck.
		Entry* mostRecent() const;
		void flush();
	private:
		_AllocationTableImp* d_this;
	};
}

#endif // !defined(AFX_ALLOCATIONTABLE_H__000E5120_DA89_11D4_B823_00D00918E99C__INCLUDED_)
