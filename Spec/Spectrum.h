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

#if !defined(AFX_SPECTRUM_H__3C1DDB71_9CF8_11D4_ABF1_000000000000__INCLUDED_)
#define AFX_SPECTRUM_H__3C1DDB71_9CF8_11D4_ABF1_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Root/Subject.h>
#include <Root/Tag.h>
#include <Root/Command.h>
#include <Spec/Units.h>
#include <Spec/SpectrumType.h>
#include <Spec/Scale.h>
#include <Root/Vector.h>

namespace Spec
{
	using Root::Subject;
	using Root::Cube;
	using Root::Tag;
	class Scale;
	class Buffer;
	class BioSample;

	namespace Private
	{
		struct _Slot;
	}

	template<class T>
	class SpecRef
	{
	public:
		SpecRef( T* t = 0 )
		{
			if( t )
			{
				t->addRef();
				t->open();
			}
			d_ref = t;
		}
		SpecRef( const SpecRef<T>& r )
		{
			d_ref = 0;
			*this = r;
		}
		~SpecRef()
		{
			if( d_ref )
			{
				d_ref->close();
				d_ref->release();
			}
		}
		T* operator->() const { return d_ref; }
		T& operator*() const { return *d_ref; }
		operator T*() const { return d_ref; }
		SpecRef<T>& operator=( T* t )
		{
			if( d_ref == t )
				return *this;
			if( d_ref )
			{
				d_ref->close();
				d_ref->release();
			}
			d_ref = t;
			if( d_ref )
			{
				d_ref->addRef();
				d_ref->open();
			}
			return *this;
		}
		SpecRef<T>& operator=( const SpecRef<T>& r )
		{
			*this = r.deref();
			return *this;
		}	
		bool isNull() const { return d_ref == 0; }
		T* deref() const { return d_ref; }
		bool operator==(const SpecRef<T>& rhs) const { return d_ref == rhs.d_ref; }
		bool operator!=(const SpecRef<T>& rhs) const { return d_ref != rhs.d_ref; }
		bool operator==(T* t) const { return d_ref == t; }
		bool operator!=(T* t) const { return d_ref != t; }
		bool operator<(const SpecRef<T>& rhs) const { return d_ref < rhs.d_ref; }
	private:
		T* d_ref;
	};

	//* Spectrum
	//. Dies ist die abstrakte Basisklasse für alle Spektren. Die Unterklassen
	//. müssen nicht unbedingt die Spektraldaten enthalten, jedoch zumindest
	//. den aktiven Link zur Datei (meist memory mapped) oder Datenbank.

	//. NOTE 8.10.01:
	//. Rotation ist nicht mehr Sache des Spektrums, sondern eines vorgeschalteten
	//. Adapters. Dies vorallem, da sich "Permutation" als Implementationsdetail 
	//. der Param-Datei herausgestellt hat.

	class Spectrum : public Subject
	{
	public:
		struct Levels 
		{
			Amplitude d_pMax;	// Positives Maximum
			Amplitude d_pNoise;	// Positiver Noise-Floor (default Mittelwert)
			Amplitude d_nMax;	// Negatives Maximum
			Amplitude d_nNoise; // Negativer Noise-Floor (default Mittelwert)

			Levels():d_pMax(0),d_pNoise(0),d_nMax(0),d_nNoise(0) {}
			Amplitude getNoise() const;
			Amplitude getMax() const;
			void amplify( Amplitude a )
			{
				if( a == 1.0 )
					return;
				d_pMax *= a;
				d_pNoise *= a;
				d_nMax *= a;
				d_nNoise *= a;
			}
		};
		struct Histo 
		{
			Root::Vector<Root::UInt32> d_pos;
			Root::Vector<Root::UInt32> d_neg;
			Amplitude d_pMax;
			Amplitude d_nMax;
			Histo():d_pMax(0),d_nMax(0){}
		};

		//* Lokale Helper
		Extension getExtension() const;
		//. Ausdehnung des Spektrums in Anzahl Sample-Punkten pro Dimension.
		void getCube( PpmCube& ) const;
		void getColors( ColorMap& ) const;
		AtomType getColor( Dimension ) const;
		const char* getDimName( Dimension ) const;
		Hz ppmToHz( Dimension d, PPM p) const { return p * getRfFreq( d ); }
		SpinLabel getKeyLabel( Dimension ) const;
		bool isNoesy( Dimension, Dimension ) const;
		bool hasNoesy() const;
		bool isNoesy( Dimension ) const;
		int getNoesyDir(Dimension a, Dimension b) const;
        Rotation getTypeMapping( bool inverted = false ) const;
		//-

		//* Interface mit lokaler Default-Implementation (Override, Upcall)
		virtual void fillSlice( Buffer & plane, Dimension x, const PpmCube& cube, 
			bool fill = false, bool fold = false, Sample ns = 0 );
			// ns ist die maximale Anzahl Samples, die der Buffer erhalten soll.
			// Damit kann wie in XEASY direkt beim Lesen eine Skalenreduktion stattfinden.
		virtual void fillPlane( Buffer & plane, Dimension x, Dimension y, const PpmCube& roi,
			bool fill = false, bool fold = false, Sample nsx = 0, Sample nsy = 0 );
		//. Vereinfachter Zugriff auf Plane und Slice, optional mit Zero-Filling und Folding.
		virtual Amplitude getAt( const PpmCube&, bool fold = false ) const;
		//. Gebe das Maximum innerhalb des Kreuzes durch Cube zurück.
		virtual Amplitude getAt( const PpmPoint&, bool fill = false, bool fold = false ) const;
		//. Gebe die Amplitude am Punkt zurück.
		virtual Dimension mapToType( Dimension ) const; // Adapter zu SpectrumType
		virtual const SpinLabelSet& getSpinLabels( Dimension d ) const; // Adapter zu SpectrumType
		virtual Levels getLevels(bool force = false) const;
		virtual bool fillHisto( Histo& ) const;
		//. Fülle das übergebene Histo. Es werden die Anzahl Elemente gem. 
		//. dem übergebenen Histo. Zuerst wird es genullt.
		virtual PPM getPeakWidth( Dimension ) const;
		//-

		//* Interface (Downcall)
		virtual void fillBuffer( Buffer &, const Rotation&, 
			const Root::Cube& cube, const Root::Point& ns = Root::Point() ) const;
		//. Fülle den Buffer gem. dem gewünschten Mapping. Dieses entscheidet zugleich
		//. über die Anzahl Dimensionen, die der Buffer erhalten wird. ns.size == rot.size.
		virtual Dimension getDimCount() const = 0;
		//. Anzahl der im Spektrum vorhandener Dimensionen.
		virtual const Scale& getScale( Dimension dim ) const = 0;
		//. Die mit jeder Dimension des Spektrums assoziierte PPM-Skala.
		virtual Amplitude getAt( const Point& ) const;
		//. Gebe die Amplitude am Punkt zurück.
		virtual const char* getFilePath() const { return ""; }
		//. Gebe den Dateipfad, aus dem das Spektrum gelesen wurde (Param-Datei bei Easy).
		virtual const char* getName() const;
		//. Gebe eine Bezeichnung des Spektrums zurück, welche als Titel im Fenster angezeigt wird.
		virtual void setName( const char* ) {}
		//. Gebe dem Spektrum eine Lesbarere Bezeichnung als den FilePath.
		virtual void adjustScale( Dimension d, PPM offset ); 
		//. Schiebe und skaliere die PPM-Skala der Dimension und passe so ein Spektrum
		//. einem oder zwei Referenzpunkten an.
		virtual void setFolding( Dimension, Scale::Folding f );
		//. Übersteuere Folding des Spektrums
		virtual const char* getLabel( Dimension ) const;
		//. Mit jeder Dimension kann ein Tag assoziiert sein. Dieser sagt etwas
		//. aus über die zu erwartenden Peaks (z.B. Hn/N). Bei Xeasy-Spektren
		//. werden die Labels des Paramfiles a) auf Color und b) auf Label abgebildet.
		virtual SpectrumType* getType() const { return 0; }
		virtual Root::Index getId() const { return 0; }
		virtual int open() const;
		//. Erst mit Aufruf von Open wird das File geöffnet bzw. als Memory-Map geladen.
		//. Darf mehrfach aufgerufen werden. Throws Exception (z.B. bei ungültigem Pfad).
		//. Gibt den neuen Use-Count zurück.
		virtual int close() const;
		//. Gegenteil von Open. Darf so häufig aufgerufen werden wie ersteres.
		//. Gibt den neuen Use-Count zurück.
		virtual bool canSave() const;
		//. true, wenn sich dieses Spektrum speichern kann.
		virtual void saveParamFile();
		//. Speichere das Param-File
		virtual MHz getRfFreq( Dimension ) const;
		//. Gebe die Freq des Spektrometers oder 0.0, wenn nicht implementiert.
		virtual Amplitude getRaw( Root::Int64 ) const;
		virtual void setThreshold( Amplitude );
		virtual Amplitude getThreshold() const;
		virtual bool setPeakWidth( Dimension, PPM );
		virtual BioSample* getSample() const { return 0; } // 0=Default

		virtual bool canDownsize() const;
		// true if spectrum can handle ns (maximum number of samples) params.

        virtual bool isDummy() const { return false; }
		//-

		//* Geerbt von Object
		QByteArray  getInstanceName(bool=false) const;

		Spectrum();
		static const char* s_fileFilter;

		class Update : public Root::UpdateMessage
		{
		public:
			enum Type { Scale, Rotation, Name, All, Closing, Data, 
				Levels, Threshold, PeakWidth, Sample };

			Update( Root::Subject* m, Type t, Dimension d = Root::DimUndefined ):
				Root::UpdateMessage( m ), d_type( t ), d_dim( d ) {}

			Type getType() const { return d_type; }
			Dimension getDim() const { return d_dim; }
		private:
			Type d_type;
			Dimension d_dim;
		};

	protected:
		virtual ~Spectrum();

	private:
		/// Fülle den Buffer mit den im Cube enthaltenen Punkten. Die Anzahl
		/// Samples pro Dimension ergibt sich aus den PPM-Ranges. first wird immer auf
		/// Sample-Index Null abgebildet. Damit bestimmt die Relation first>second
		/// die Achsenrichtung. x und y legen fest, welche Dimensionen des Spektrums auf die
		/// X- und Y-Dimension des Buffers abgebildet werden. Bei allen Dimensionen des Spektrums, 
		/// die von x und y nicht genannt werden, muss der Range first=second sein. 
		/// cube bezieht sich auf das Spektrum, d.h. in Bezug auf Rotation, Scale-Color und 
		/// DimensionCount gleich, auch wenn die Ranges disjunkt sein können.
		void fillPlane0( Buffer & plane, Dimension x, Dimension y, const Cube& roi, 
			Sample nsx, Sample nsy ) const;
		/// Wie fillPlane, ausser dass der Zielbuffer ein 1D-Slice aus dem Spektrum erhält.
		/// Entsprechend muss bei allen Dimensionen, die von x nicht genannt werden, der
		/// PPM-Range first=second sein.
		void fillSlice0( Buffer & plane, Dimension x, const Cube& roi, Sample ns ) const;

		/// Gleich wie virtuelle Variante, jedoch für PpmCube.
		void fillPlane1( Buffer & plane, Dimension x, Dimension y, const PpmCube& roi, 
			Sample nsx, Sample nsy ) const;
		/// Gleich wie virtuelle Variante, jedoch für PpmCube.
		void fillSlice1( Buffer & plane, Dimension x, const PpmCube& cube, Sample ns ) const;

		/// Wie fillPlane(), jedoch wird immer ganzer roi gefüllt, entweder mit Value oder 0.
		void fillPlane2( Buffer & plane, Dimension x, Dimension y, const PpmCube& roi, 
			Sample nsx, Sample nsy ) const;
		/// Wie fillPlane(), jedoch wird immer ganzer roi gefüllt, entweder mit Value oder 0.
		void fillPlane2( Buffer & plane, Dimension x, Dimension y, const Cube& roi, 
			Sample nsx, Sample nsy ) const;

		/// Wie fillSlice(), jedoch wird immer ganzer roi gefüllt, entweder mit 0 oder Value.
		void fillSlice2( Buffer & plane, Dimension x, const PpmCube& roi, Sample ns ) const;
		/// Wie fillSlice(), jedoch wird immer ganzer roi gefüllt, entweder mit 0 oder Value.
		void fillSlice2( Buffer & plane, Dimension x, const Cube& roi, Sample ns ) const;

		/// Wie fillPlane(), jedoch wird immer ganzer roi gefüllt, mit Value oder Faltung
		void fillPlane3( Buffer & plane, Dimension x, Dimension y, const PpmCube& roi, 
			Sample nsx, Sample nsy ) const;
		/// Wie fillSlice(), jedoch wird immer ganzer roi gefüllt, mit Value oder Faltung
		void fillSlice3( Buffer & plane, Dimension x, const PpmCube& roi, Sample ns ) const;
	};

	class SpecCalibrateCmd : public Root::Command
	{
		PpmPoint d_off;
		Root::Ref<Spectrum> d_spec;
	public:
		SpecCalibrateCmd( Spectrum*, const PpmPoint& );
		const char* getPrettyName() const { return "Calibrate Spectrum"; }
		void unexecute();
		void execute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

}

Q_DECLARE_METATYPE ( Spec::Spectrum* )

#endif // !defined(AFX_SPECTRUM_H__3C1DDB71_9CF8_11D4_ABF1_000000000000__INCLUDED_)
