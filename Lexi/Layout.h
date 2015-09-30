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

#if !defined(AFX_LAYOUT_H__6A660540_DB29_11D4_B847_00D00918E99C__INCLUDED_)
#define AFX_LAYOUT_H__6A660540_DB29_11D4_B847_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi/Allocation.h>
#include <Lexi/Requisition.h>

namespace Lexi
{
	class Layout 
	{
	protected:
		Layout();
	public:
		virtual ~Layout();

		virtual void request( const RequisitionVector&, Requisition& result );
		//.	Layout::request is given an array of requisitions for the individual components
		//.	and computes a single requisition for the result.
		virtual void allocate( const Allocation& given, const RequisitionVector&,
			AllocationVector& result );
		//. Layout::allocate is given the requisitions of the components and the overall allocation,
		//. returning the allocations of the individual components.
	};
}

#endif // !defined(AFX_LAYOUT_H__6A660540_DB29_11D4_B847_00D00918E99C__INCLUDED_)
