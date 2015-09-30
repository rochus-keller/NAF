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

#if !defined(AFX_REPOSITORY_H__7DA7E098_1962_499F_9439_4F9B625454F7__INCLUDED_)
#define AFX_REPOSITORY_H__7DA7E098_1962_499F_9439_4F9B625454F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Subject.h>
#include <Spec/TypeBase.h>
#include <Spec/Project.h>
#include <Spec/ObjectDef.h>
#include <Spec/ObjectBase.h>
#include <Root/Vector.h>
#include <QMap>

namespace Spec
{
	class Repository;

	class Script : public Root::Object
	{
	public:
		enum Lang { Lua };
		static const char* s_lang[];

        Script( QByteArray, Lang = Lua );
		const char* getCode() const { return d_code.data(); }
		void setCode( const char* );
        const QByteArray& getName() const { return d_name; }
		Lang getLang() const { return d_lang; }
		bool isCompiled() const { return d_compiled; }
		void setCompiled();
		Repository* getOwner() const { return d_rep; }

		//* Overrides von Object
		QByteArray  getInstanceName(bool = false) const;
		//-
		enum Hint { Name, Code, Compiled };
		UPDATE_MESSAGE_DEF2( Changed, Script, Hint );
		UPDATE_MESSAGE_DEF( Added, Script );
		UPDATE_MESSAGE_DEF( Removed, Script );
	protected:
		virtual ~Script();
	private:
		friend class Repository;
		friend class _XmlParser;
		QByteArray  d_code;
        QByteArray d_name;
		Repository* d_rep;
		Lang d_lang;
		bool d_compiled;
	};

	//* Repository
	//. Dies ist das Hauptmodell, das in der Datei oder DB gespeichert
	//. wird. Es enthält Projekte und alle projektübergreifenden
	//. Daten und Konfigurationen.

	class Repository : public Root::Subject
	{
	public:
		void addRef() const;
		struct SpecColor 
		{
			QColor d_pos;
			QColor d_neg;
			SpecColor( QColor pos = Qt::red, 
				QColor neg = Qt::green):
				d_pos( pos ), d_neg( neg ) {}
		};
		typedef Root::Vector<SpecColor> SlotColors;
		typedef TypeBase::SpectrumTypeMap SpectrumTypeMap;
		typedef TypeBase::ResidueTypeMap ResidueTypeMap;
		typedef TypeBase::SystemTypeMap SystemTypeMap;

		typedef std::map<Root::SymbolString,Root::Ref<Project> > ProjectMap;
		typedef std::map<Root::SymbolString,Root::Ref<ObjectDef> > ObjectDefs;
        typedef std::map<QByteArray,Root::Ref<Script> > ScriptMap;

		const ObjectDefs& getClasses() const { return d_classes; }
		ObjectDef* findObjectDef( Root::SymbolString ) const;

		const SystemTypeMap& getSystemTypes() const { return d_types->getSystemTypes(); }
		SystemType* addSystemType( const char* name );
		bool inUse( SystemType* ) const;
		bool remove( SystemType* );
		bool rename( SystemType*, const char* );

		const SpectrumTypeMap& getSpecTypes() const { return d_types->getSpecTypes(); }
		bool addSpectrumType( SpectrumType* );
		bool inUse( SpectrumType* ) const;
		bool remove( SpectrumType* );
		bool rename( SpectrumType*, const char* );

		const ResidueTypeMap& getResidueTypes() const { return d_types->getResidueTypes(); }
		ResidueType* findResidueType( const char*, bool caseInsens = true ) const;
		ResidueType* addResidueType( Root::SymbolString n, 
			Root::SymbolString s, Root::SymbolString l );
		bool inUse( ResidueType* ) const;
		bool remove( ResidueType* );
		bool rename( ResidueType*, const char* );

		const ProjectMap& getProjects() const { return d_projects; }
		Project* findProject( const char* ) const;
		void addProject( Project* );	// throws "not unique"
		bool removeProject( Project* );
		bool rename( Project*, const char* name );

		void saveToFile( const char* path = "", const char* creator = "" );
		//. Implementiert SaveAs (path!="") oder Save (path=="").
		const char* getFilePath() const { return d_path.data(); }
		//. Gibt "" zurück, wenn noch nicht mit einer Datei verknüpft ist.
		bool isDirty() const { return d_dirty; }
		//. Gibt true zurück, falls ungesicherte Änderungen vorhanden.
		void touch();
		void notifyObservers( Root::UpdateMessage& ); 

		Repository();
		void copyFrom( const Repository& );
		void load( const char* path, bool asTemplate = false );
		void reload( const char* path, bool asTemplate = false );
		void reloadEmpty();// Mache daraus ein neues leeres Rep
		void reloadSpecs();

		const char* getTplName() const { return d_tplPath.data(); }
		const char* getOrigTplAuthor() const { return d_origTplAuthor.data(); }
		const char* getTplAuthor() const { return d_tplAuthor.data(); }
		const char* getAuthor() const { return d_author.data(); }

		PeakColorList* getColors() const { return d_colors; }
		ObjectBase* getBase() const { return d_base; }

		const ScriptMap& getScripts() const { return d_scripts; }
        Script* getScript( const char* name ) const;
		Script* addScript( Root::SymbolString, Script::Lang = Script::Lua );
		bool rename( Script*, const char* );
		void remove( Script* );

		TypeBase* getTypes() const { return d_types; }
		const SlotColors& getScreenClr() const { return d_screenClr; }
		void setScreenClr( const SlotColors& );
		const SlotColors& getPrintClr() const { return d_printClr; }

		//* Overrides von Object
		bool getDynValue( Field, Root::Any& ) const;	// ok wenn Feld vorhanden
		void setFieldValue( Field, const Root::Any& );
		FieldInfo getFieldInfo( Field ) const;
		FieldValues getFieldValues() const;
		QByteArray  getInstanceName(bool = false) const;
		//-

		static const char* keyRepository;
		static const char* keySpectrumType;
		static const char* keyResidueType;
		static const char* keySystemType;
		static const char* keyProject;
		static const char* keySpectrum;
		static const char* keyPeaklist;
		static const char* keyPeak;
		static const char* keyResidue;
		static const char* keySpinSystem;
		static const char* keySpin;
		static const char* keyScript;
		static const char* keyLink;
		static const char* keyGuess;
		static const char* keyLabelingScheme;
		static const char* keySample;
		static const char* keyAtom;

		static const char* keyAuthor;		// RW
		static const char* keyCreationDate;	// RW
		static const char* keyDesc;			// Repository Description, RW
		static const char* keyTplDesc;		// Template Description, RW
		static const char* keyTplDate;		// Template Creation Date, RW
		static const char* keyOrigTplDate;	// Template Creation Date, RO
		static const char* keyOrigTplDesc;	// Original Template Description, RO
		static const char* keyTplAuthor;	// RO
		static const char* keyTplPath;		// Template Path, RO
		static const char* keyOrigTplAuthor;// Original Template Author, RO

		enum Hint { Dirty, AboutToSave, Saved, Closing, Ready };
		UPDATE_MESSAGE_DEF2( Changed, Repository, Hint );
	protected:
		void clear();	
		virtual ~Repository();
		void handle( Root::Message & );
	private:
		void init();
		friend class _XmlParser;
		friend class Serializer;

		ProjectMap d_projects;
		ObjectDefs d_classes;
		Root::Ref<PeakColorList> d_colors;
		Root::Ref<ObjectBase> d_base;
		Root::Ref<TypeBase> d_types;
		ScriptMap d_scripts;
		bool d_dirty;
		QByteArray  d_path;
		QByteArray  d_author;
		QByteArray  d_tplAuthor;
		QByteArray  d_origTplAuthor;
		QByteArray  d_tplPath;
		SlotColors d_screenClr;
		SlotColors d_printClr;
	};

}

#endif // !defined(AFX_REPOSITORY_H__7DA7E098_1962_499F_9439_4F9B625454F7__INCLUDED_)
