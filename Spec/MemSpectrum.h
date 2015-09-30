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

#if !defined(AFX_MEMSPECTRUM_H__FBF21F35_EC6C_4D6A_9C49_DE079DA15A03__INCLUDED_)
#define AFX_MEMSPECTRUM_H__FBF21F35_EC6C_4D6A_9C49_DE079DA15A03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Spec/Spectrum.h>
#include <Spec/Buffer.h>

namespace Spec
{
	class MemSpectrum : public Spectrum
	{
	public:
		//* Overrides
		Dimension getDimCount() const { return d_buf.getDimCount(); }
		const char* getFilePath() const { return ""; }
		const Scale& getScale(Dimension i ) const { return d_buf.getScale( i ); }
		void adjustScale(Dimension d, PPM off ) { d_buf.adjustScale( d, off ); }
		void setFolding(Dimension d, Scale::Folding f ) { d_buf.setFolding( d, f ); }
		// const char* getLabel( Dimension d ) const { return ""; }
		int close() const { d_useCount--; return d_useCount; } 
		int open() const { d_useCount++; return d_useCount; } 
		Amplitude getRaw( Root::Int64 i ) const { return d_buf.getAt( i ); }

		void fillBuffer( Buffer &, const Rotation&, const Root::Cube& cube, 
			const Root::Point& ns = Root::Point() ) const;
		Amplitude getAt( const Point& ) const;
		virtual void setThreshold( Amplitude );
		virtual Amplitude getThreshold() const;
		// TODO: get/setPeakWidth
		//-

		void flatten(Spectrum*,const Rotation& );
		MemSpectrum(const Buffer&);
		MemSpectrum( const ScaleVector& );
		virtual ~MemSpectrum();
	private:
		Buffer d_buf;
		Amplitude d_thres;
		mutable int d_useCount;
	};
}

#endif // !defined(AFX_MEMSPECTRUM_H__FBF21F35_EC6C_4D6A_9C49_DE079DA15A03__INCLUDED_)
