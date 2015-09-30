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

#if !defined(AFX_POSITIONLIST_H__22E542A1_A055_413F_870E_D68401AB6F38__INCLUDED_)
#define AFX_POSITIONLIST_H__22E542A1_A055_413F_870E_D68401AB6F38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Subject.h>
#include <Root/Command.h>
#include <QColor>
#include <Spec/Units.h>
#include <Root/SymbolString.h>
#include <Spec/AtomType.h>
#include <Spec/Spectrum.h>
#include <Spec/PeakModel.h>
#include <Root/Vector.h>

namespace Spec
{
	class Spectrum;
	class PointSet;
	class PeakList;
	class Project;

	//* Peak
	//. Dies ist ein einzelner Peak mit allen Ausprägungen pro Spektrum.
	//. Es existiert mind. immer die Position zu d_home. Mit mean wird
	//. im Falle der Nicht-Existenz zum gewünschten Spektrum ein
	//. Mittelwert über alle PpmPoint gebildet, ansonsten wird d_home verwendet.

	class Peak : public Root::Object
	{
	public:
		
		typedef Root::FixPoint<Root::Index, PpmPoint::MAX_DIM > Assig;	// Verweis auf Spin-IDs
		
		class Guess : public Root::Object
		{
		public:
			const Assig& getAssig() const { return d_assig; }
			Root::Float getProb() const { return d_prob; }
			Root::Index getId() const { return d_id; }
			Peak* getOwner() const { return d_owner; }
			Guess();
			//* Overrides von Object
			QByteArray  getInstanceName(bool = false) const;
			//-
		private:
			friend class PeakList;
			friend class _XmlParser;
			Assig d_assig;
			Root::Float d_prob; // Probability
			Root::Index d_id;
			Peak* d_owner;
		};
		typedef std::map<Root::Index,Root::Ref<Guess> > GuessMap;

		struct Position
		{
			PeakPos d_pos; // Darf empty sein, wenn nur andere Attribute,
							// nicht aber Position pro Spektrum anders ist.
			Amplitude d_amp;
			Amplitude d_vol;
			PeakModel* d_mdl; // Verweis auf PeakModel

			Position():
				d_amp(0),d_vol(0), d_mdl(0) 
			{ 
				d_pos.init( Nirwana ); // Damit kann Amp/Vol auch ohne Pos existieren.
			}
		};
		
		typedef std::map<Root::Index,Position> PositionMap;

		const GuessMap& getGuesses() const { return d_guesses; }
		Guess* getGuess( Root::Index ) const;

		void getPosition( Position&, Spectrum* = 0, bool* alias = 0 ) const;
		PPM getPos( Dimension, Spectrum* = 0 ) const;
		const PeakPos& getPos( Spectrum* = 0 ) const;
		const PeakPos& getPos( Spectrum*, bool& alias ) const;
		const PositionMap& getPositions() const { return d_pos; }
		Root::Index getId() const { return d_id; }
		Root::SymbolString getTag() const { return d_tag; }
		Amplitude getAmp( Spectrum* = 0 ) const;
		Amplitude getVol( Spectrum* = 0 ) const;
		PeakModel* getModel( Spectrum* = 0 ) const;
		bool hasPos( Spectrum* s ) const { return d_pos.find( (s)?s->getId():0 ) != d_pos.end(); }
		Dimension getDimCount() const;
        Root::Index getHome() const;
		const Assig& getAssig() const { return d_assig; }
		Root::UInt8 getColor() const { return d_color; }
		PeakList* getOwner() const { return d_owner; }

		//* Overrides von Object
		QByteArray  getInstanceName(bool = false) const;
		//-

		Peak( const Position&, Root::SymbolString,
			Root::UInt8 clr, const Assig& );
	protected:
		void rotate( const Rotation& );
		Peak(Root::Index=0);
		~Peak();
	private:
		friend class PeakList;
		friend class _XmlParser;

		PeakList* d_owner;
		Root::Index d_id;
		Root::SymbolString d_tag;
		PositionMap d_pos;
		//Root::Index d_homeSpecId; // kann 0 sein
		Assig d_assig;	
		GuessMap d_guesses;
		Root::UInt8 d_color;
	};

	class PeakColorList : public Root::Subject
	{
	public:
		typedef std::map<Root::UInt8,QColor> Colors;

		void setColor( Root::UInt8, QColor );
		QColor getColor( Root::UInt8 ) const;
		const Colors& getColors() const { return d_colors; }
		PeakColorList();

		class Update : public Root::UpdateMessage
		{
		public:
            Update( Root::Subject* m, Root::UInt8 c ):
                Root::UpdateMessage( m ),d_code(c) {}
            Root::UInt8 d_code;
		};
	protected:
		~PeakColorList();
	private:
		Colors d_colors;
	};


	//* PeakList
	//. Dies ist eine konkrete, formatunabhängige "Peakliste", welche als
	//. Teil eines Project geführt wird.

	class PeakList : public Root::Subject
	{
	public:
		typedef std::set<Peak*> Selection;
		typedef std::set<Selection> SelectionSet;
		typedef std::map<Root::Index,Root::Ref<PeakModel> > Models;
		typedef std::map<Root::Index,Root::Ref<Peak> > Peaks;
		typedef Root::Vector<Root::Index> SpecList;

		//* Interface
		void deleteGuess( Peak*, Root::Index );
		void setGuess( Peak*, Root::Index, const Peak::Assig&, Root::Float prob );
		Peak::Guess* addGuess( Peak* peak, const Peak::Assig&, Root::Float prob );
		Peak* addPosition( Peak::Position&, Spectrum* = 0, Root::Index id = 0 );
		Peak* addPosition( Peak* );
		void deletePosition( Peak* );
		void setTag( Peak*, Root::SymbolString );
		void setPeak( Peak*, const Peak::Position&, Spectrum* = 0 );
		Peak* getPeak( Root::Index ) const;

		void setPos( Peak*, const PeakPos&, Spectrum* = 0 );
		void setPos( Peak*, const PeakPos&, Amplitude, Spectrum* = 0 );
		// Liest Amplitude aus mitgegebenem Spektrum.
		void setVol( Peak*, Amplitude, Spectrum* = 0);
		void setAmp(Peak *, Amplitude, Spectrum* = 0);
		void setModel(Peak *, Root::Index, Spectrum* = 0);
		void setAssig( Peak*, const Peak::Assig& );
		void setColor( Peak*, Root::UInt8 );

		Root::Index getHome() const { return d_homeSpecId; }
		Dimension getDimCount() const { return d_colors.size(); }
		Selection find( const PpmCube&, Spectrum* = 0 ) const;
		//. Wenn ein Range (0,0) ist, wird diese Dimension nicht eingegrenzt.
		const PeakPos& getPos( Peak*, Spectrum* = 0 ) const;
		//-

		void rotateAll( const Rotation& );
		void move( const PeakPos& );
		void append( PointSet* );
		void updateAllAmps( Spectrum *, const Rotation& rot, bool folding = false );
		bool hasDoubles(Spectrum*) const;
		SelectionSet findDoubles( Spectrum* ) const;

		enum IntegrationMethod
		{
			LinEq,		// Integriere mit Rochus' Diss-Idee, Default
			SumRect,	// Rechteck durch BaseWidth
			MAX_IntegrationMethod
		};
		static const char* integrationMethodName[];
		void integrateAll( Spectrum*, IntegrationMethod = LinEq );
		void integrateLinEq( Spectrum* ); 
		void integrateSumRect( Spectrum* ); 

		void setId( Root::Index id ) { d_id = id; } // RISK
		Root::Index getId() const { return d_id; }
		bool isNew() const { return d_id == 0; } // Noch nicht mit Projekt registriert.
		const Peaks& getPeaks() const { return d_peaks; }
		const ColorMap& getColors() const { return d_colors; }
		Root::SymbolString getName() const { return d_name; }
		void setName( Root::SymbolString );

		PeakModel* addModel( const char* name = "" );
		PeakModel* getModel( Root::Index ) const;
		PeakModel* getModel() const { return d_model; }
		const Models& getModels() const { return d_models; }

		const SpecList& getSpecs() const { return d_specs; }
		void setSpecs( const SpecList& );
		bool setHome( Spectrum* );
		Project* getOwner() const { return d_owner; }

		//* Overrides von Object
		QByteArray  getInstanceName(bool = false) const;
		//-

		bool isDirty() const { return d_dirty; }
		void clearDirty() { d_dirty = false; }

		PeakList(const ColorMap&,Spectrum* home = 0 );
		PeakList(Spectrum* home = 0);
		PeakList( const PeakList& );

		class Update : public Root::UpdateMessage
		{
		public:
			enum Type { Add, Delete, Position, Data, Guess, Everything, Name, SpecList, Models };

			Update( Root::Subject* m, Type t = Everything, Peak* p = 0,
				const PeakPos& old = PeakPos() ):
				Root::UpdateMessage( m ), d_type( t ), d_peak( p ), d_old( old ) {}

			Type getType() const { return d_type; }
			Peak* getPeak() const { return d_peak; }
			const PeakPos& getOld() const { return d_old; }
		private:
			Type d_type;
			Peak* d_peak;
			PeakPos d_old;
		};

		UPDATE_MESSAGE_DEF( Added, PeakList );
		UPDATE_MESSAGE_DEF( Removed, PeakList );

	protected:
		virtual ~PeakList();
		void handle( Root::Message & );
	private:
		friend class _XmlParser;
		friend class Project;

		Peaks d_peaks;
		ColorMap d_colors;
		Root::SymbolString d_name;	// Eindeutig innerhalb Project
		Root::Index d_id;
		Root::Index d_homeSpecId;	// kann 0 sein
		Root::Ref<PeakModel> d_model;	// Default-Model, ID=0
		Models d_models;
		SpecList d_specs;	// Batch-Liste von Spektren, auf welche diese Peaks
							// mit Integrator angewendet werden müssen.
		Project* d_owner;
		bool d_dirty;
	};

	class PickPeakCmd : public Root::Command
	{
		Root::Ref<PeakList> d_peaks;
		Root::Ref<Peak> d_peak;
		PeakPos d_point; 
		Amplitude d_amp;
		Root::Ref<Spectrum> d_spec;
	public:
		PickPeakCmd( PeakList*, const PeakPos&, Amplitude, Spectrum* = 0 );
		const char* getPrettyName() const { return "Pick Peak"; }
		void unexecute();
		void execute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

	class PickPeakCmd2 : public Root::Command
	{
		Root::Ref<PeakList> d_peaks;
		Root::Ref<Peak> d_peak;
	public:
		PickPeakCmd2( PeakList*, const Peak::Position&, Root::SymbolString,
			Root::UInt8 clr, const Peak::Assig&, Spectrum* );
		Peak* getPeak() const { return d_peak; }
		const char* getPrettyName() const { return "Pick Peak"; }
		void unexecute();
		void execute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

	class DeletePeaksCmd : public Root::Command
	{
		typedef Root::Vector< Root::Ref<Peak> > Sel;
		Sel d_sel;
	public:
		DeletePeaksCmd( const PeakList::Selection& );
		DeletePeaksCmd( Peak* );
		const char* getPrettyName() const { return "Delete Peaks"; }
		void unexecute();
		void execute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

	class MovePeakCmd : public Root::Command
	{
		Root::Ref<Peak> d_peak;
		PeakPos d_to, d_old;
		Amplitude d_amp, d_olda;
		Root::Ref<Spectrum> d_spec;
	public:
		MovePeakCmd( Peak*, const PeakPos& to, Amplitude, Spectrum* = 0 );
		const char* getPrettyName() const { return "Move Peak"; }
		void unexecute();
		void execute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

	class AmpVolPeakCmd : public Root::Command
	{
		Root::Ref<Peak> d_peak;
		Amplitude d_vol, d_oldv;
		Amplitude d_amp, d_olda;
		Root::Ref<Spectrum> d_spec;
	public:
		AmpVolPeakCmd( Peak*, Amplitude amp, Amplitude vol, Spectrum* = 0 );
		const char* getPrettyName() const { return "Amplitude & Volume"; }
		void unexecute();
		void execute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

	class LabelPeakCmd : public Root::Command
	{
		Root::Ref<Peak> d_peak;
		Root::SymbolString d_old, d_new; 
	public:
		LabelPeakCmd( Peak*, Root::SymbolString );
		const char* getPrettyName() const { return "Label Peak"; }
		void unexecute();
		void execute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

	class PeakCalibrateCmd : public Root::Command
	{
		PeakPos d_off;
		Root::Ref<PeakList> d_peaks;
	public:
		PeakCalibrateCmd( PeakList*, const PeakPos& );
		const char* getPrettyName() const { return "Calibrate Peaklist"; }
		void unexecute();
		void execute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};
}

#endif // !defined(AFX_POSITIONLIST_H__22E542A1_A055_413F_870E_D68401AB6F38__INCLUDED_)
