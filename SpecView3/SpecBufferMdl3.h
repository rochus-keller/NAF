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

#if !defined(SpecBufferMdl3__INCLUDED_)
#define SpecBufferMdl3__INCLUDED_

#include <Root/Subject.h>
#include <Spec/Buffer.h>
#include <Spec/Spectrum.h>
#include <SpecView3/ViewAreaMdl3.h>

namespace Spec
{
	//* SpecBufferMdl3
    //. Diese Klasse schneidet den von ViewAreaMdl3 gewünschten Ausschnitt aus
	//. dem übergebenen Spektrum. Letzteres muss bereits so reduziert und 
	//. rotiert sein, dass es auf X/Y passt (bzw. die von ViewArea verlangten
	//. Dimensionen). Die Y-Achse wird beim Laden invertiert und es werden
	//. Auflösungsreduktion und Faltung berücksichtigt.

	class SpecBufferMdl3 : public Root::Subject
	{
	public:
        SpecBufferMdl3(ViewAreaMdl3*, Spectrum* = 0, bool load = true );

		const Buffer& getBuffer() const { return d_buffer; }
        ViewAreaMdl3* getArea() const { return d_area; }
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
        void copy( SpecBufferMdl3* ); // Alternative zu Reload; kopiere inhalt ab gegebenem Buffer

        Points getPointsPerSample( Dimension d ) const
		{ 
			if( d_buffer.getDimCount() == 1 )
                return d_area->getPointsPerPpm( d ) * d_buffer.getScale( DimX ).getDelta();
			else
                return d_area->getPointsPerPpm( d ) * d_buffer.getScale( d ).getDelta();
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
		virtual ~SpecBufferMdl3();
		void handle( Root::Message & );

        Root::Ref<ViewAreaMdl3> d_area;
		SpecRef<Spectrum> d_spec;
		Buffer d_buffer;
		Root::Byte d_resolution;
		bool d_scale;
		bool d_folding;
		bool d_down;
	};

}

#endif // !defined(SpecBufferMdl3__INCLUDED_)
