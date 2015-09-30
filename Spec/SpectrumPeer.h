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

#if !defined(AFX_SPECADAPTER_H__F76386A0_7F4A_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_SPECADAPTER_H__F76386A0_7F4A_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Spec/Spectrum.h>
#include <Root/SymbolString.h>
#include <Root/Any.h>
#include <Spec/Scale.h>
#include <Root/Vector.h>
#include <Spec/BioSample.h>

namespace Spec
{
	class Project;

	//* SpectrumPeer
	//. Diese Klasse kann einem beliebigen Spektrum transparent vorgeschaltet werden,
	//. um den Namen, die Rotation und den Offset zu ändern, ohne die Originalklasse 
	//. tangieren zu müssen. Normalerweise wird jedem geladenen Spektrum ein Adapter
	//. vorgeschaltet, um die Kalibration zu verwalten und dieses an den
	//. Spektrentyp anzupassen. Hier werden auch alle Attribute gespeichert.

	//. NOTE: um Rechenzeit zu sparen, wird die Ausführung des Offsets an das 
	//. Originalspektrum delegiert. Hier werden zwar die Werte gespeichert, jedoch
	//. der eigentliche Adjust findet in Scale des verdeckten Spektrums statt.

	class SpectrumPeer : public Spectrum
	{
	public:
		bool ambiguousDims() const;
		void pathAdjusted();
		void reloadSpec();
		SpectrumPeer* clone() const;
		void replaceSpectrum( Spectrum* ); // Throws
		void calcLevels();
		void setRawLevels( const Levels& );
		const Levels& getRawLevels() const { return d_levels; }
		void setType( SpectrumType*, bool checkColor = false );	// throws Exception
		void setId( Root::Index id ) { d_id = id; }
		Amplitude getFactor() const { return d_factor; }

		void setOffset( Dimension d, PPM off );
		PPM getOffset( Dimension d ) const { return d_offset[ d ]; }
		//. NOTE: Offset ist in der Rotation des nachgeschaltenten Spektrums.
		const PpmPoint& getWidths() const { return d_width; }

		PPM getScaleAdjust( Dimension d ) const { return d_offset[ d_rot[ d ] ]; }
		//. Wie getOffset, aber in der Rotation des Peers.

		void setSample(BioSample*);
		void setRotation( const Rotation& );
		const Rotation& getRotation() const { return d_rot; }
		Dimension getRotation( Dimension view ) const { return d_rot[ view ]; } // return spec
		void setRotation( Dimension view, Dimension spec );
		bool autoRotate( SpectrumType* );
		//. Stelle die Rotation so ein, dass die Colors des Spektrums in der Reihenfolge des
		//. vorgegebenen SpectrumType sind.
		bool checkRotation() const;
		//. Prüfe, ob Rotation disjunkt.

		Spectrum* getSpectrum() const { return d_spec; }
		Root::SymbolString getSymName() const { return d_name; }
		Project* getOwner() const { return d_owner; }


		//* Overrides von Spectrum
		void setFolding( Dimension d, Scale::Folding );
		void adjustScale( Dimension d, PPM offset );
		void fillBuffer( Buffer &, const Rotation&, const Root::Cube& cube, 
			const Root::Point& ns = Root::Point() ) const;
		Amplitude getAt( const Point& ) const;
		const char* getName() const { return (d_name.empty())?d_spec->getName():d_name.data(); }
		const char* getFilePath() const { return d_path.data(); }
		Dimension getDimCount() const { return d_spec->getDimCount(); }
		const Scale& getScale( Dimension d ) const { return d_spec->getScale( d_rot[ d ] ); }
		void setName( const char* );
		const char* getLabel( Dimension d ) const { return d_spec->getLabel( d_rot[ d ] ); }
		SpectrumType* getType() const { return d_type; }
		Root::Index getId() const { return d_id; }
		int open() const { return d_spec->open(); }
		int close() const { return d_spec->close(); }
		BioSample* getSample() const { return d_sample; }
		// Hier muss mapToType nicht übersteuert werden, da d_rot so eingestellt ist,
		// dass SpectrumPeer bzgl. Rotation mit SpectrumType übereinstimmt
		bool canSave() const { return d_spec->canSave(); }
		void saveParamFile();
		MHz getRfFreq( Dimension d ) const { return d_spec->getRfFreq( d ); }
		Amplitude getRaw( Root::Int64 i ) const { return d_spec->getRaw( i ); }
		Levels getLevels(bool force = false) const;
		void setThreshold( Amplitude );
		Amplitude getThreshold() const;
		PPM getPeakWidth( Dimension ) const;
		bool setPeakWidth( Dimension, PPM );
		bool canDownsize() const { return d_spec->canDownsize(); }
        bool isDummy() const { return d_spec->isDummy(); }
		//-

		//* Overrides von Object
		//-

		SpectrumPeer( Spectrum* s, const char* path = 0, Root::Index = 0 );
		SpectrumPeer( Spectrum* s, const char* path, const Rotation& );

		UPDATE_MESSAGE_DEF( Added, SpectrumPeer );
		UPDATE_MESSAGE_DEF( Removed, SpectrumPeer );
	protected:
		virtual ~SpectrumPeer();
		SpectrumPeer() {}
		void handle( Root::Message& );
	private:
		friend class _XmlParser;
		friend class Project;

		Root::Ref<Spectrum> d_spec;
		Root::Ref<BioSample> d_sample;
		Rotation d_rot;
		PpmPoint d_offset;
		PpmPoint d_width; // Peak Width, Originalrotation des Spektrums
		Root::SymbolString d_name;	// Eindeutig innerhalb Project
		QByteArray d_path; // absoluter Pfad
		Root::Index d_id;			// Primary Key innerhalb Project
		SpectrumType* d_type;	// RISK: ev. mit Ref
		Levels d_levels;
		Amplitude d_thres;
		Amplitude d_factor; // Default 1.
		Project* d_owner;
	};

    class DummySpectrum : public Spectrum
    {
    public:
        DummySpectrum() {}
        DummySpectrum( Dimension d ) { d_scales.assign( d ); }

        // Overrides
        Dimension getDimCount() const { return d_scales.size(); }
		const Scale& getScale(Dimension i ) const { return d_scales[i]; }
		void adjustScale(Dimension d, PPM off ) { d_scales[ d ].adjust( off ); }
        void setFolding(Dimension d, Scale::Folding f) { d_scales[ d ].setFolding( f );	}
        void fillBuffer( Buffer &, const Rotation&, const Root::Cube& cube,
			const Root::Point& ns = Root::Point() ) const;
        Amplitude getAt( const Point& ) const { return 0.0; }
        Amplitude getRaw( Root::Int64 i ) const { return 0.0; }
        bool isDummy() const { return true; }
		int open() const { return 1; }
		int close() const { return 1; }

        ScaleVector d_scales;
    };
}

#endif // !defined(AFX_SPECADAPTER_H__F76386A0_7F4A_11D5_8DB0_00D00918E99C__INCLUDED_)
