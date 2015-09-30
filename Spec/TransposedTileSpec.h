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

#if !defined(AFX_TRANSPOSEDTILESPEC_H__BE9A51D5_9561_424A_B66D_EBA887D79607__INCLUDED_)
#define AFX_TRANSPOSEDTILESPEC_H__BE9A51D5_9561_424A_B66D_EBA887D79607__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Spec/FileSpectrum.h>

namespace Spec
{
	// Identisch mit TileSpectrum, ausser dass die Speicherreihenfolge verdreht ist:
	/*
		Easy: Sparky:
		{0,0} {0,0}
		{1,0} {0,1}
		{2,0} {0,2}
		.
		.
		{7,0} {0,7}

		{0,1} {1,0}
		{1,1} {1,1}
		{2,1} {1,2}
		.
		.
		{7,1} {1,7}
	*/
	class TransposedTileSpec : public TileSpectrum
	{
	public:
		//* Overrides
		void fillBuffer( Buffer &, const Rotation&, const Root::Cube& cube, 
			const Root::Point& ns = Root::Point() ) const;
		Amplitude getAt( const Point& ) const;
		bool canDownsize() const;
		//-
		TransposedTileSpec();
	protected:
		virtual ~TransposedTileSpec();
	};
}

#endif // !defined(AFX_TRANSPOSEDTILESPEC_H__BE9A51D5_9561_424A_B66D_EBA887D79607__INCLUDED_)
