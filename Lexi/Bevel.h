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

#if !defined(AFX_BEVEL_H__9AF1BB80_1BBD_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_BEVEL_H__9AF1BB80_1BBD_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi/MonoGlyph.h>

namespace Lexi
{
	//* Bevel
	//. Ein Bevel ist ein spezieller Border mit genau 40 Twips Rahmenbreite.
	//. Es wird um body herum ein raised oder sunken Panel gezeichnet.

	class Bevel : public MonoGlyph
	{
	public:
		Bevel( Glyph* body = nil, bool raised = true, bool filled = true );
		virtual ~Bevel();

		//* Overrides from MonoGlyph
		void traverse( const Allocation&, GlyphTraversal&);
		virtual void draw( Canvas&, const Allocation&);
		virtual void allocate( const Allocation&);
		void request( Requisition& );
		virtual void pick( Twips x, Twips y, const Allocation&, Trace&);
	private:
		bool d_raised;
		bool d_filled;
	};
}

#endif // !defined(AFX_BEVEL_H__9AF1BB80_1BBD_11D5_8DB0_00D00918E99C__INCLUDED_)
