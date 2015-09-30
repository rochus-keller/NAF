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

#if !defined(AFX_SPINSYSTEM_H__EC9B2EE4_5324_48ED_A31F_F5E9634B6920__INCLUDED_)
#define AFX_SPINSYSTEM_H__EC9B2EE4_5324_48ED_A31F_F5E9634B6920__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Subject.h>
#include <Root/MakroCommand.h>
#include <Root/Vector.h>
#include <Spec/SpinLabel.h>
#include <Spec/AtomType.h>
#include <Spec/PeakModel.h>
#include <Root/Any.h>
#include <Root/Vector.h>
#include <Root/Sequence.h>
#include <Root/Smart.h>

namespace Spec
{
	using Root::Index;
	class SpinSystem;
	class Spectrum;
	class Spin;
	class Residue;
	class Sequence;
	class Project;
	class SystemType;
	class ResidueType;
	class SpinBase;

	class SpinLink : public Root::Object
	{
	public:
		struct Alias 
		{
			Root::Float d_rating;
			Root::UInt8 d_code;
			bool d_visible;
			Alias():d_rating(0.000f), d_code(0), d_visible( true ) {}
		};
		typedef std::map<Root::Index,Alias> AliasMap;	// SpektrumId x Alias

		SpinLink( Index, Index );
		~SpinLink();
		const AliasMap& getAlias() const { return d_alias; }
		const Alias& getAlias( Spectrum* ) const;
		Root::UInt8 getCode( Spectrum* = 0 ) const;
		Amplitude getRating( Spectrum* = 0 ) const;
		bool isVisible( Spectrum* ) const;
		Root::Index getRhs() const { return d_rhs; }
		Root::Index getLhs() const { return d_lhs; }
		SpinBase* getOwner() const { return d_owner; }

		//* Overrides von Object
		QByteArray  getInstanceName(bool=false) const;
		//-

		class Update : public Root::UpdateMessage
		{
		public:
			enum Type { Link, Unlink, Param, All };

			Update( Root::Resource* m, Type t = All, Spin* lhs = 0, Spin* rhs = 0,
				SpinLink* link = 0 ):
				Root::UpdateMessage( m ), d_type( t ), 
					d_lhs( lhs ), d_rhs( rhs ), d_link( link ) {}
			
			Type getType() const { return d_type; }
			Spin* getRhs() const { return d_rhs; }
			Spin* getLhs() const { return d_lhs; }
			SpinLink* getLink() const { return d_link; } // bei Link und Param non null
		private:
			Type d_type;
			Spin* d_rhs;
			Spin* d_lhs;
			SpinLink* d_link;
		};

	private:
		friend class _XmlParser;
		friend class SpinBase;
		Root::Index d_rhs;
		Root::Index d_lhs;
		AliasMap d_alias;
		SpinBase* d_owner;
	protected:
		SpinLink( const SpinLink& );
	};

	class Location 
	{
	public:
        typedef Root::FixPoint<float,3> Position;

        Position d_pos;
		float d_dev;	// "Wolkenradius" um pos

		Location():d_dev(0.0) {}
	};

	class Spin : public Root::Object
	{
	public:
		typedef std::map<Root::Index,PPM> Shifts; 
		// typedef std::map<Root::Index,PPM> Shifts;
		typedef std::set<SpinLink*> Links;	// Keine Ownership, synchron mit SpinBase

		const Shifts& getShifts() const { return d_shifts; }
		const SpinLabel& getLabel() const { return d_label; }
		const Links& getLinks() const { return d_links; }
		SpinLink* findLink( Spin* ) const;
		AtomType getAtom() const { return d_atom; }
		Index getId() const { return d_id; }
		PPM getShift( Spectrum* = 0 ) const;
		PPM getShift( Spectrum*, bool& alias ) const;
		bool hasAlias( Spectrum* s ) const;
		SpinSystem* getSystem() const { return d_system; }
		Root::Index getHome() const { return d_home; }
		const Location* getLoc() const { return d_loc; }
		SpinBase* getOwner() const { return d_owner; }

		//* Overrides von Object
		QByteArray  getInstanceName(bool=false) const;
		//-

		class Update : public Root::UpdateMessage
		{
		public:
			enum Type { Create, Delete, Shift, Label, System, All, Visi, Loc };

			Update( Root::Resource* m, Type t = All, Spin* p = 0, PPM old = 0,
				Spectrum* s = 0 ):
				Root::UpdateMessage( m ), d_type( t ), d_spin( p ), 
				d_spec( s ), d_shift( old ) {}
			Update( Root::Resource* m, Type t, Spin* p, const SpinLabel& old ):
				Root::UpdateMessage( m ), d_type( t ), 
					d_spin( p ), d_label( old ), d_spec( 0 ) {}
			
			Type getType() const { return d_type; }
			Spin* getSpin() const { return d_spin; }
			PPM getShift() const { return d_shift; }
			PPM getOld() const { return d_shift; }
			Spectrum* getSpec() const { return d_spec; }
			const SpinLabel& getLabel() const { return d_label; }
		private:
			Type d_type;
			Spin* d_spin;
			PPM d_shift;
			Spectrum* d_spec;
			SpinLabel d_label;
		};

	protected:
		Spin( const Spin& );
		Spin();
		virtual ~Spin();
	private:
		friend class _XmlParser;
		friend class SpinSystem;
		friend class SpinBase;

		AtomType d_atom;
		SpinLabel d_label;
		Index d_id;
		SpinSystem* d_system;
		Root::Index d_home;	// NOTE: d_shifts mind. fr home immer bestckt.
		Shifts d_shifts;
		Links d_links;
		Location* d_loc;	// Optionale Position eines Spins:
		SpinBase* d_owner;
	};

	//* SpinPoint
	//. Ein SpinPoint ist ein Vektor aus Spins in derselben Anzahl Dimensionen
	//. wie sein SpinSpace. Ein SpinPoint ist innerhalb eines SpinSpace eindeutig.


	class SpinPoint : public Root::FixPoint<Spin*> 
	{
	public:
		SpinPoint() {}
		SpinPoint( const SpinPoint& rhs ):Root::FixPoint<Spin*>( rhs ) {}
		SpinPoint( const Root::FixVector<Spin*>& rhs )
		{
			zero();
			for( int i = 0; i < rhs.size(); i++ )
				d_point[ i ] = rhs[ i ];
		}
		SpinPoint( Spin* x )
		{
			zero();
			d_point[ DimX ] = x;
		}
		SpinPoint( Spin* x, Spin* y )
		{
			zero();
			d_point[ DimX ] = x;
			d_point[ DimY ] = y;
		}
		SpinPoint( Spin* x, Spin* y, Spin* z )
		{
			zero();
			d_point[ DimX ] = x;
			d_point[ DimY ] = y;
			d_point[ DimZ ] = z;
		}
	};
	typedef std::set<SpinPoint> SpinPointSet;

	class SpinIdPoint : public Root::FixPoint<Root::Index> 
	{
	public:
		SpinIdPoint() {}
		SpinIdPoint( const SpinIdPoint& rhs ):Root::FixPoint<Root::Index>( rhs ) {}
		SpinIdPoint( const SpinPoint& rhs )
		{
			for( int i = 0; i < MAX_DIM; i++ )
				if( rhs[ i ] )
					d_point[ i ] = rhs[ i ]->getId();
				else
					d_point[ i ] = 0;
		}
	};

	class SpinSystem : public Root::Object
	{
	public:
		typedef std::set<Root::Ref<Spin> > Spins;
		typedef Root::FixVector<Spin*> SpinTuple;
		typedef std::set<Root::Ref<ResidueType> > Candidates;

		static void formatLabel( const SpinSystem*, char*, int len );
        void formatLabel( char* buf, int len ) const { formatLabel( this, buf, len ); }
        QByteArray formatLabel();
		void formatCands( char*, int ) const;
		const Candidates& getCands() const { return d_cands; }
		bool isAcceptable( const SpinLabel& ) const;
		Spins findSpins( Root::SymbolString, Root::Index off, AtomType, 
			bool showNulls = false, bool showProjected = true, 
			bool showUnknown = false ) const;
		Spins findNoesySpins( AtomType, bool showNulls = false, 
			bool showProjected = true ) const;
		Spins getFinalSet() const;
		Spins findAllBut( const SpinLabelSet& ) const;
		Spins findAll( const SpinLabelSet& ) const;
		SpinLabelSet getAcceptables( const SpinLabelSet& ) const;
        SpinLabelPoints getAcceptables( const SpinLabelPoints&, Dimension count ) const;
        SpinTuple findTuple( const KeyLabels& ) const; // Finde Final-Tuple als Anchor.
		const Spins& getSpins() const { return d_spins; }
		Index getId() const { return d_id; }
		Residue* getAssig() const { return d_assig; }
		SpinSystem* getPred() const { return d_pred; }
		SpinSystem* getSucc() const { return d_succ; }
		SystemType* getSysType() const { return d_sysType; }
		SpinBase* getOwner() const { return d_owner; }

		Root::Index calcLabelingScheme( Spectrum* );

		//* Overrides von Object
		QByteArray  getInstanceName(bool=false) const;
		//-

		class Update : public Root::UpdateMessage
		{
		public:
			enum Type { Create, Delete, Succ, Pred, Assig, SysType, Candidates, Add, Remove, All };

			Update( Root::Resource* m, Type t = All, SpinSystem* p = 0, Spin* s = 0 ):
				Root::UpdateMessage( m ), d_type( t ), d_sys( p ), d_spin( s ) {}

			Type getType() const { return d_type; }
			SpinSystem* getSystem() const { return d_sys; }
			Spin* getSpin() const { return d_spin; }
		private:
			Type d_type;
			SpinSystem* d_sys;
			Spin* d_spin;
		};

	protected:
		SpinSystem( const SpinSystem& );
		virtual ~SpinSystem();
		SpinSystem();
	private:
		friend class _XmlParser;
		friend class SpinBase;

		Index d_id;
		SpinSystem* d_succ;
		SpinSystem* d_pred;
		Residue* d_assig;
		SystemType* d_sysType;	// Vorklassifikation fr Homonuclear-Assig.
		Candidates d_cands;			// Mgliche Assignments.
		Spins d_spins;
		SpinBase* d_owner;
	};

	typedef Root::Vector<SpinSystem*> SpinSystemString;	

	class SpinBase : public Root::Resource
	{
	public:
		typedef std::map< Root::Index, Root::Ref<Spin> > SpinMap;
		typedef std::map< Root::Index, Root::Ref<SpinSystem> > SpinSystemMap;
		typedef std::set< Root::Ref<SpinLink> > SpinLinkSet;

		void setCands( SpinSystem*, const SpinSystem::Candidates& );
		bool removeCand( SpinSystem*, ResidueType* );
		bool addCand( SpinSystem*, ResidueType* );
		const SpinSystemMap& getSystems() const { return d_systems; }
		SpinSystem* addSystem( Root::Index = 0, SystemType* = 0); // Optional ID vorschlagen
		SpinSystem* addSystem( SpinSystem* );
		void deleteSystem( SpinSystem* );
		void link( SpinSystem* pred, SpinSystem* succ, bool checkAssig = true );
		void unlink( SpinSystem* pred, SpinSystem* succ );
		void assignSystem( SpinSystem*, Residue* );
		void assignFragment( SpinSystem*, Residue* );
		void unassignSystem( SpinSystem* );
		bool contains( SpinSystem* ) const;
		SpinSystem* getSystem( Root::Index ) const;
		void classifySystem( SpinSystem*, SystemType* );
		SpinSystem* findSystem( const char* ) const;

		const SpinMap& getSpins() const { return d_spins; }
		Spin* getSpin( Root::Index ) const;
		Spin* addSpin( AtomType, PPM shift, Spectrum* spec = 0, Root::Index = 0 );
		Spin* addSpin( Spin* );
		bool setLabel( Spin*, const SpinLabel& );
		void setShift( Spin*, PPM, Spectrum* = 0 );
		void deleteSpin( Spin* );	// Spin darf nicht mehr zu System gehren
		void unassingSpin( Spin* );
		bool assignSpin( SpinSystem*, Spin* );
		void setLoc( Spin*, const Location& loc );
		void setLoc( Spin* );

		SpinLink* link( Spin* lhs, Spin* rhs );
		void unlink( Spin* lhs, Spin* rhs );
		void setRating( SpinLink*, Root::Float, Spectrum* = 0 );
		void setCode( SpinLink*, Root::UInt8, Spectrum* = 0 );
		void setVisible( SpinLink*, bool, Spectrum* = 0 );
		const SpinLinkSet& getLinks() const { return d_links; }
		void setAlias( SpinLink*, Spectrum*, Root::Float rate, Root::UInt8 code, bool visi );
		void showAllLinks( Spectrum* );

		void fillString( SpinSystem*, SpinSystemString& ) const;
		// Gibt das Fragment zurck, in dem SS enthalten ist.
		void fillString( SpinSystem* left, SpinSystem* right, SpinSystemString& ) const;
		// Gibt das Fragment zurck, das aus Join von left und right entsteht

		Sequence* getSeq() const;
		SpinBase( Project* );
		SpinBase( const SpinBase&, Project* );
		Project* getOwner() const { return d_owner; }

        const Location::Position& getOrig() const { return d_orig; }
        void setOrig(const Location::Position&);
	protected:
		~SpinBase();
		void notifyObservers( Root::UpdateMessage& ); 
	private:
		friend class _XmlParser;

		SpinMap d_spins;
		SpinSystemMap d_systems;
		SpinLinkSet d_links;
        Location::Position d_orig;
		Project* d_owner;
        Root::Index d_maxSysId;
        Root::Index d_maxSpinId;
	};

	class PickSpinCmd : public Root::Command
	{
		Root::Ref<SpinBase> d_base;
		AtomType d_type;
		PPM d_shift;
		Root::Ref<Spectrum> d_spec;
		Root::Ref<Spin> d_spin;
	public:
		Spin* getSpin() const { return d_spin; }
		PickSpinCmd( SpinBase*, AtomType, PPM, Spectrum* = 0 );
		const char* getPrettyName() const { return "Pick Spin"; }
		void unexecute();
		void execute();
		void reexecute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

	class HideSpinLinkCmd : public Root::Command
	{
		Root::Ref<SpinBase> d_base;
		Root::Ref<SpinLink> d_link;
		Root::Ref<Spectrum> d_spec;
	public:
		HideSpinLinkCmd( SpinBase*, SpinLink*, Spectrum* = 0 );
		const char* getPrettyName() const { return "Hide Pair"; }
		void unexecute();
		void execute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

	class LinkSpinCmd : public Root::Command
	{
		Root::Ref<SpinBase> d_base;
		Root::Ref<Spin> d_lhs;
		Root::Ref<Spin> d_rhs;
	public:
		LinkSpinCmd( SpinBase*, Spin* lhs, Spin* rhs );
		const char* getPrettyName() const { return "Link Spins"; }
		void unexecute();
		void execute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

	class UnlinkSpinCmd : public Root::Command
	{
		Root::Ref<SpinBase> d_base;
		Root::Ref<Spin> d_lhs;
		Root::Ref<Spin> d_rhs;
	public:
		UnlinkSpinCmd( SpinBase*, Spin* lhs, Spin* rhs );
		const char* getPrettyName() const { return "Unlink Spins"; }
		void unexecute();
		void execute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

	// Wie PickSpinCmd, aber zu System zugefgt im selben Arbeitsgang
	class PickSystemSpinCmd : public Root::Command
	{
		Root::Ref<SpinBase> d_base;
		Root::Ref<SpinSystem> d_sys;
		AtomType d_type;
		PPM d_shift;
		Root::Ref<Spectrum> d_spec;
		Root::Ref<Spin> d_spin;
	public:
		Spin* getSpin() const { return d_spin; }
		PickSystemSpinCmd( SpinBase*, SpinSystem*,AtomType type, PPM shift, Spectrum* = 0 );
		const char* getPrettyName() const { return "Pick Spin"; }
		void unexecute();
		void execute();
		void reexecute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

	// Wie PickSystemSpinLabelCmd, aber Label zugefgt im selben Arbeitsgang
	class PickSystemSpinLabelCmd : public Root::Command
	{
		Root::Ref<SpinBase> d_base;
		Root::Ref<SpinSystem> d_sys;
		AtomType d_type;
		PPM d_shift;
		Root::Ref<Spectrum> d_spec;
		Root::Ref<Spin> d_spin;
		SpinLabel d_lbl;
	public:
		Spin* getSpin() const { return d_spin; }
		PickSystemSpinLabelCmd( SpinBase*, SpinSystem*,AtomType, PPM, 
			const SpinLabel&, Spectrum* = 0 );
		const char* getPrettyName() const { return "Pick Spin"; }
		void unexecute();
		void execute();
		void reexecute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

	class CreateSystemCmd : public Root::Command
	{
		Root::Ref<SpinBase> d_base;
		Root::Ref<SpinSystem> d_sys;
		KeyLabels d_key;
		bool d_alias;
		PpmPoint d_point;
		Root::Ref<Spectrum> d_spec;  // Cmd verwendet Colormap des Spektrums.
	public:
		SpinSystem* getSystem() const { return d_sys; }
		CreateSystemCmd( SpinBase*, const KeyLabels&, const PpmPoint&, Spectrum*, bool alias );
		const char* getPrettyName() const { return "Create Spin System"; }
		void unexecute();
		void execute();
		void reexecute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

	class CreateSystemOnlyCmd : public Root::Command
	{
		Root::Ref<SpinBase> d_base;
		Root::Ref<SpinSystem> d_sys;
	public:
		SpinSystem* getSystem() const { return d_sys; }
		CreateSystemOnlyCmd( SpinBase* );
		const char* getPrettyName() const { return "Create Spin System"; }
		void unexecute();
		void execute();
		void reexecute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

	class CreateSystemPairCmd : public Root::Command
	{
		enum { MAX_DIM = 2 };
		Root::Ref<SpinBase> d_base;
		Root::Ref<Spin> d_spins[MAX_DIM];
		PeakPos d_point;
		Root::Ref<Spectrum> d_spec;  // Cmd verwendet Colormap des Spektrums.
		Root::Ref<SystemType> d_sysType;
        Root::Ref<SpinSystem> d_sys;
    public:
		SpinSystem* getSystem() const { return d_spins[ DimX ]->getSystem(); }
		Spin* getSpin( Dimension d ) const { return d_spins[ d ]; }
		CreateSystemPairCmd( SpinBase*,	const PpmPoint&, Spectrum*, SystemType* = 0 );
		const char* getPrettyName() const { return "Create Spin System"; }
		void unexecute();
		void execute();
        void reexecute();
        bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

	class DeleteSpinCmd : public Root::Command
	{
		Root::Ref<SpinBase> d_base;
		Root::Ref<Spin> d_spin;
		Root::Ref<Root::MakroCommand> d_undos;
	public:
		DeleteSpinCmd( SpinBase*, Spin* );
		const char* getPrettyName() const { return "Remove Spin"; }
		void unexecute();
        void execute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

	class MoveSpinCmd : public Root::Command
	{
		Root::Ref<SpinBase> d_base;
		PPM d_new, d_old;
		Root::Ref<Spectrum> d_spec;
		Root::Ref<Spin> d_spin;
	public:
		MoveSpinCmd( SpinBase*, Spin*, PPM shift, Spectrum* = 0 );
		const char* getPrettyName() const { return "Move Spin"; }
		void unexecute();
		void execute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

	class LabelSpinCmd : public Root::Command
	{
		Root::Ref<SpinBase> d_base;
		SpinLabel d_new, d_old;
		Root::Ref<Spin> d_spin;
	public:
		LabelSpinCmd( SpinBase*, Spin*, const SpinLabel& );
		const char* getPrettyName() const { return "Label Spin"; }
		void unexecute();
		void execute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

	class AssignSpinCmd : public Root::Command
	{
		Root::Ref<SpinBase> d_base;
		Root::Ref<Spin> d_spin;
		Root::Ref<SpinSystem> d_sys;
	public:
		AssignSpinCmd( SpinBase*, Spin*, SpinSystem* );
		const char* getPrettyName() const { return "Assign Spin"; }
		void unexecute();
		void execute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};
	
	class UnassignSpinCmd : public Root::Command
	{
		Root::Ref<SpinBase> d_base;
		Root::Ref<Spin> d_spin;
		Root::Ref<SpinSystem> d_sys;
	public:
		UnassignSpinCmd( SpinBase*, Spin* );
		const char* getPrettyName() const { return "Unassign Spin"; }
		void unexecute();
		void execute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

	class DeleteSystemCmd : public Root::Command
	{
		Root::Ref<SpinBase> d_base;
		Root::Ref<SpinSystem> d_sys;
		std::set< Root::Ref<Spin> > d_undo;
	public:
		DeleteSystemCmd( SpinBase*, SpinSystem* );
		const char* getPrettyName() const { return "Delete Spin System"; }
		void unexecute();
		void execute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

	class UnlinkSystemCmd : public Root::Command
	{
		Root::Ref<SpinBase> d_base;
		Root::Ref<SpinSystem> d_pred, d_succ;
	public:
		UnlinkSystemCmd( SpinBase*, SpinSystem* pred, SpinSystem* succ );
		const char* getPrettyName() const { return "Unlink Spin System"; }
		void unexecute();
		void execute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

	class UnassignSystemCmd : public Root::Command
	{
		// Unassign des gesamten durch SpinSystem definierten Fragments
		Root::Ref<SpinBase> d_base;
		Root::Ref<SpinSystem> d_sys;
		Root::Vector<Residue*> d_old;
	public:
		UnassignSystemCmd( SpinBase*, SpinSystem* );
		const char* getPrettyName() const { return "Unassign Spin System"; }
		void unexecute();
		void execute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

	class AssignSystemCmd : public Root::Command
	{
		// Assign des gesamten durch SpinSystem definierten Fragments
		Root::Ref<SpinBase> d_base;
		Root::Ref<SpinSystem> d_sys;
		Residue* d_resi; // RISK
		// RISK: sysType wird nicht wieder hergestellt.
		Root::Vector< Root::Ref<UnassignSystemCmd> > d_undo; // Reihenfolge wichtig
		void rollback();
	public:
		AssignSystemCmd( SpinBase*, SpinSystem*, Residue* );
		const char* getPrettyName() const { return "Assign Spin System"; }
		void unexecute();
		void execute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

	class ClassifySystemCmd : public Root::Command
	{
		// Assign des gesamten durch SpinSystem definierten Fragments
		Root::Ref<SpinBase> d_base;
		Root::Ref<SpinSystem> d_sys;
		SystemType* d_sysType; // RISK
		SystemType* d_old;
	public:
		ClassifySystemCmd( SpinBase*, SpinSystem*, SystemType* );
		const char* getPrettyName() const { return "Classify Spin System"; }
		void unexecute();
		void execute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

	class LinkSystemCmd : public Root::Command
	{
		Root::Ref<SpinBase> d_base;
		Root::Ref<SpinSystem> d_pred, d_succ;
		Root::Sequence< Root::Ref<UnlinkSystemCmd> > d_unlinks;
		Root::Ref<AssignSystemCmd> d_ass;
	public:
		LinkSystemCmd( SpinBase*, SpinSystem* pred, SpinSystem* succ );
		const char* getPrettyName() const { return "Link Spin System"; }
		void unexecute();
		void execute();
		bool hasUndo() const { return true; }
		bool hasRedo() const { return true; }
	};

}

#endif // !defined(AFX_SPINSYSTEM_H__EC9B2EE4_5324_48ED_A31F_F5E9634B6920__INCLUDED_)
