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

#if !defined(AFX_TYPEBASE_H__305B08D7_43BF_482B_8DA4_3A311C229901__INCLUDED_)
#define AFX_TYPEBASE_H__305B08D7_43BF_482B_8DA4_3A311C229901__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Subject.h>
#include <Spec/SpectrumType.h>
#include <Spec/ResidueType.h>
#include <Spec/NmrExperiment.h>
#include <Root/Vector.h>

namespace Spec
{
	class SpinSystem; 
	class Repository;
	class TypeBase;

	class LabelingScheme : public Root::Object
	{
	public:
		LabelingScheme( Root::Index id, const char* name = "" );

		Root::Index getId() const { return d_id; }
		const char* getName() const { return d_name.data(); }
		void setName( const char* );
		//* Overrides
		void setFieldValue( Field f, const Root::Any& v );
		bool getDynValue( Field f, Root::Any& v ) const;
		FieldValues getFieldValues() const;
		QByteArray  getInstanceName(bool=false) const;
		//-

		UPDATE_MESSAGE_DEF( Added, LabelingScheme );
		UPDATE_MESSAGE_DEF( Removed, LabelingScheme );
	private:
		friend class _XmlParser;
		friend class TypeBase;

		QByteArray  d_name;
		Root::Index d_id;
		TypeBase* d_owner;
	};

	class TypeBase : public Root::Resource
	{
	public:
		typedef std::map<Root::SymbolString,Root::Ref<SpectrumType> > SpectrumTypeMap;
		typedef std::map<Root::SymbolString,Root::Ref<ResidueType> > ResidueTypeMap;
		typedef std::map<Root::Index,Root::Ref<LabelingScheme> > LabelingSchemeMap;
		// NOTE: für ID von ResidueType wird d_short verwendet.
		// NOTE: SpectrumTypes und ResidueTypes werden initial ab Projektdatei
		// geladen oder aus anderem Projekt importiert. Kein Editor vorgesehen.
		typedef std::map<Root::Index,Root::Ref<SystemType> > SystemTypeMap;

		void setSysType(ResidueType*, SystemType * st);
		const SystemTypeMap& getSystemTypes() const { return d_sysTypes; }
		SystemType* addSystemType( const char* name );
		bool remove( SystemType* );
		bool rename( SystemType*, const char* );
		bool inUse(SystemType * st) const;
		void setResiType( SystemType*, ResidueType* );
		SystemType* findSystemType( const char*, bool caseInsens = true ) const;
		void setMatches( SystemType*, const SystemType::ResiTypeMatches& );
		void setClass( SystemType*, Root::SymbolString );

		LabelingScheme* addLabelingScheme( Root::Index, const char* name = "" );
		bool remove( LabelingScheme* );
		bool rename( LabelingScheme*, const char* );
		const LabelingSchemeMap& getLabelingSchemes() const { return d_labelingSchemes; }
		LabelingScheme* getLabelingScheme( Root::Index ) const;

		SpectrumType* findSpectrumType( const char*, bool caseInsens = true ) const;
		const SpectrumTypeMap& getSpecTypes() const { return d_specTypes; }
		bool addSpectrumType( SpectrumType* );
		bool remove( SpectrumType* );
		bool rename( SpectrumType*, const char* );

		const ResidueTypeMap& getResidueTypes() const { return d_residueTypes; }
		ResidueType* findResidueType( const char*, bool caseInsens = true ) const;
		ResidueType* addResidueType( Root::SymbolString n, 
			Root::SymbolString s, Root::SymbolString l );
		ResidueType* addResidueType( ResidueType* ); 
		bool remove( ResidueType* );
		bool rename( ResidueType*, const char* );
		bool inUse( ResidueType* ) const;

		NmrExperiment* inferExperiment1( SpectrumType*, SystemType* = 0, Root::Index scheme = 0 ) const;
		NmrExperiment* inferExperiment2( SpectrumType*, SpinSystem* = 0, Spectrum* = 0 ) const;
		NmrExperiment* inferExperiment3( SpectrumType* st, bool pathsim ) const;
		NmrExperiment* inferExperiment4( SpectrumType*, ResidueType* = 0, Root::Index scheme = 0 ) const;

		void setTerms( Root::SymbolString n, Root::SymbolString c );
		void setTerms( SystemType*, Root::SymbolString n, Root::SymbolString c ); // New 12.6.05
		Root::SymbolString getNTerm(ResidueType* ) const;
		Root::SymbolString getCTerm(ResidueType* ) const;

		ResidueType* getGenFromSys(SpinSystem* center = 0) const;
		ResidueType* getGenFromType(ResidueType* center = 0) const;
		void setGenSys( ResidueType* genSys );
		void setGenSys( SystemType*, ResidueType* genSys ); // New 12.6.05

		TypeBase(Repository*);
		Repository* getOwner() const { return d_owner; }

		enum Hint { Term, GenSys, ModelCache };
		UPDATE_MESSAGE_DEF2( Changed, TypeBase, Hint );
	protected:
		NmrExperiment* getOrCreateExperiment( SpectrumType*, ResidueType* mid = 0,
			ResidueType* lhs = 0, ResidueType* rhs = 0, Root::Index scheme = 0 ) const;

		void clearModels();
		void invalidate( SpectrumType* );
		void invalidate( ResidueType* );
		void updateSysTypes();
		virtual ~TypeBase();
		void handle( Root::Message & );
	private:
		friend class _XmlParser;
		friend class Repository;

		SpectrumTypeMap d_specTypes;
		ResidueTypeMap d_residueTypes;
		SystemTypeMap d_sysTypes;
		LabelingSchemeMap d_labelingSchemes;
		Root::SymbolString d_nterm;	// Abschluss ResidueType Richtung N
		Root::SymbolString d_cterm;	// Abschluss ResidueType Richtung CO
		Root::Ref<ResidueType> d_gensys; // Generisches SpinSystem
		Repository* d_owner;

		// Runtime-Information
		struct Key
		{
			Root::Ptr<SpectrumType> d_spec;
			Root::Ptr<ResidueType> d_mid;
			Root::Ptr<ResidueType> d_lhs;
			Root::Ptr<ResidueType> d_rhs;
			Root::Index d_scheme;

			bool operator<( const Key& rhs ) const
			{
				return ::memcmp( this, &rhs, sizeof(Key) ) < 0;
			};
			Key( SpectrumType* st, ResidueType * rt,
				ResidueType* lhs, ResidueType* rhs, Root::Index scheme )
			{
				d_spec = st;
				d_mid = rt;
				d_lhs = lhs;
				d_rhs = rhs;
				d_scheme = scheme;
			}
		};
		typedef std::map<Key, Root::Ref<NmrExperiment> > Models;
		mutable Models d_models;
		/*
		typedef std::map< std::pair<SpectrumType*,ResidueType*>, PathTable > Cache;
		mutable Cache d_cacheN;
		mutable Cache d_cacheI;
		mutable Cache d_cacheC;
		*/
	};

}

#endif // !defined(AFX_TYPEBASE_H__305B08D7_43BF_482B_8DA4_3A311C229901__INCLUDED_)
