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

#if !defined(AFX_SPECBUFFERMDL_H__43712B35_0C01_4171_BD95_1FC3A371A82D__INCLUDED_)
#define AFX_SPECBUFFERMDL_H__43712B35_0C01_4171_BD95_1FC3A371A82D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Subject.h>
#include <Spec/Buffer.h>
#include <Spec/Spectrum.h>
#include <SpecView2/SvViewAreaMdl.h>

namespace Spec2
{
	//* SpecBufferMdl
	//. Diese Klasse schneidet den von ViewAreaMdl gewünschten Ausschnitt aus
	//. dem übergebenen Spektrum. Letzteres muss bereits so reduziert und 
	//. rotiert sein, dass es auf X/Y passt (bzw. die von ViewArea verlangten
	//. Dimensionen). Die Y-Achse wird beim Laden invertiert und es werden
	//. Auflösungsreduktion und Faltung berücksichtigt.

	class SpecBufferMdl : public Root::Subject
	{
	public:
		SpecBufferMdl(ViewAreaMdl*, Spectrum* = 0, bool load = true );

		void printPpmRect(const Allocation & r ) const;
		const Buffer& getBuffer() const { return d_buffer; }
		void setBuffer( const Buffer& );
		Dimension getDimCount() const 
			{ return d_area->getDimCount(); }	// area, da buffer ev. noch nicht geladen
		void setResolution( Root::Byte = 1, bool reload = true );
		void setScaling( bool on, bool load = true );
		void setDownsize( bool, bool load = true );
		bool getDownsize() const { return d_down; }
		bool getFolding() const { return d_folding; }
		void setFolding( bool on, bool load = true );
		void setSpectrum( Spectrum*, bool load = true );
		Spectrum* getSpectrum() const { return d_spec; }
		ViewAreaMdl* getViewArea() const { return d_area; }
		void copy( SpecBufferMdl* );

		TwipPerSample getTwipPerSample( Dimension d ) const
		{ 
			if( d_buffer.getDimCount() == 1 )
				return d_area->getTwipPerPpm( d ) * d_buffer.getScale( DimX ).getDelta(); 
			else
				return d_area->getTwipPerPpm( d ) * d_buffer.getScale( d ).getDelta(); 
		}
		Hz toHz( PPM p, Dimension d ) const
		{
			return p * d_spec->getRfFreq( d );
		}
		void fitToArea(bool notify = true);
		void fitToDim(Dimension, bool notify = true);
		void reload();

		struct UpdZoom : public Root::UpdateMessage
		{
			UpdZoom( Root::Subject* m ):UpdateMessage( m ){}
		};
		struct UpdBuffer : public Root::UpdateMessage
		{
			UpdBuffer( Root::Subject* m ):UpdateMessage( m ){}
		};
	protected:
		bool rangeChanged() const;
		virtual ~SpecBufferMdl();
		void handle( Root::Message & );

		Root::Ref<ViewAreaMdl> d_area;
		SpecRef<Spectrum> d_spec;
		Buffer d_buffer;
		Root::Byte d_resolution;
		bool d_scale;
		bool d_folding;
		bool d_down;
	};
}

#endif // !defined(AFX_SPECBUFFERMDL_H__43712B35_0C01_4171_BD95_1FC3A371A82D__INCLUDED_)
