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

#include "Serializer.h"
#include <qdatetime.h> 
#include <fstream>
#include <math.h>
#include <qdir.h>
#include <qfileinfo.h> 
#include <qmessagebox.h>
#include <QFileDialog>
#include <Spec/Repository.h>
#include <Spec/Factory.h>
#include <Spec/Structure.h>
#include <Root/MessageLog.h>
#include <Root/Application.h>
#include <Xml/XmlHandler.h>
using namespace Spec;
using namespace std;
using namespace Root;
using namespace Xml;

// CARA 1.2 -> 32
// CARA 1.2.3 -> 33.2
// CARA 1.3 -> 34.0
// CARA 1.3.3 -> 35.0
// CARA 1.3.4 -> 36.0
// CARA 1.8.4 -> 37.0

const int Serializer::s_version = 37;	// TODO: bei jeder Element-Änderung anpassen
const int Serializer::s_subversion = 0;	// TODO: bei jeder Attribut-Änderung anpassen

static QString s_tmp; // RISK: Threads

static const char* INIT						= "init";
static const char* REPOSITORY				= "repository";
static const char* RESIDUE_TYPE				= "residue-type";
//static const char* PATH						= "path";
static const char* _ATOM					= "atom";
static const char* BOND						= "bond";
static const char* SPECTRUM_TYPE			= "spectrum-type";
static const char* DIMENSION				= "dim";
static const char* LABEL					= "label";
static const char* PROJECT					= "project";
static const char* RESIDUE					= "residue";
static const char* PARAM					= "param";
static const char* SPECTRUM					= "spectrum";
static const char* CALIBRATE				= "cal";
static const char* ROTATE					= "map";
static const char* SPINSYS					= "spinsys";
static const char* SYSTYPE					= "systype";
static const char* SPIN						= "spin";
static const char* POS						= "pos";	
static const char* LINK						= "link";	// SpinSys zu Fragmenten
static const char* PEAKLIST					= "peaklist";
static const char* PEAK						= "peak";
static const char* POSITION					= "pos";
static const char* SEQUENCE					= "sequence";
static const char* SPINBASE					= "spinbase";
static const char* WIDTH					= "width";
static const char* LEVEL					= "level";
static const char* TOL						= "tol";
static const char* SPEC						= "spec";
static const char* MODEL					= "model";
static const char* GROUP					= "group";
static const char* CAND						= "cand";
static const char* STEP						= "step";
static const char* OBJECT					= "obj";
static const char* FIELD					= "fld";
static const char* PAIR						= "pair";
static const char* INST						= "inst";
static const char* ODEF						= "odef";
static const char* SCRIPT					= "script";
static const char* CODE						= "code";
static const char* COLOR					= "color";
static const char* DATABASE					= "database";
static const char* LIBRARY					= "library";
static const char* OVCLR					= "ovclr";
static const char* GUESS					= "guess";
static const char* STRUCT					= "struct";
static const char* CONF						= "conf";
static const char* COORD					= "coord";
static const char* SCHEME					= "scheme";
static const char* SAMPLE					= "sample";
static const char* RANGE					= "range";

///////////////////////////////////////////////////////////////////////////////

namespace Spec
{
	class _XmlParser : public XmlHandler
	{
	public:
		//* Overrides
		bool characters(  const QByteArray& );
		bool endElement( Root::SymbolString name );
		bool startElement( Root::SymbolString name, const Atts& atts );
		//-

		_XmlParser( Repository* p, bool asTemplate ):
			d_rep( p ), d_asTemplate( asTemplate ), d_interactive( true ), 
			d_pathChanged( false )
			{ d_state.push_back( INIT ); }

		bool pathChanged() const { return d_pathChanged; }
	private:
		Root::Ptr<Repository> d_rep;
		QByteArray d_text;
		Root::ExRef<Residue> d_res;
		Root::ExRef<ResidueType> d_restp;
		Root::ExRef<SpectrumType> d_st;
		SpecRef<SpectrumPeer> d_specPeer;	// Gerade öffnen
		Root::ExRef<Project> d_pro;
		Root::Index d_specId;
		Root::ExRef<Spin> d_spin;
		Root::ExRef<PeakList> d_currentPeakList;
		Root::ExRef<Peak::Guess> d_guess;
		Root::ExRef<Peak> d_currentPeak;
		Root::ExRef<PeakModel> d_model;
		Root::ExRef<SpinSystem> d_sys;
		Root::ExRef<MetaAtom> d_atom;
		Root::ExRef<SystemType> d_sst;
		Root::Object::Field d_field;
		Root::Any::Type d_type;
		Root::ExRef<SpinLink> d_link;
		Root::ExRef<ObjectDef> d_odef;
		Root::ExRef<Script> d_script;
		Root::ExRef<Record> d_obj;
		Root::ExRef<Structure> d_struct;
		Root::ExRef<Conformer> d_conf;
		Root::Ref<LabelingScheme> d_scheme;
		Root::Ref<BioSample> d_sample;
        Root::Ref<DummySpectrum> d_dummySpec;
		Root::SymbolString d_gensys;
		int d_version;
		int d_subversion;
		Dimension d_dim;
		Dimension d_dim2;
		Dimension d_dim3;
        Root::Index d_currentPeakHome;
		bool d_foundLevels;
		bool d_asTemplate;
		bool d_interactive;
		bool d_pathChanged;
	protected:
		std::deque<const char*> d_state;
		typedef bool ( _XmlParser::*Handler ) ( const XmlHandler::Atts & atts );
		typedef bool ( _XmlParser::*Poster ) ();
		struct Slot
		{
			const char* d_symbol;
			const char* d_state; // Verlangter Zustand oder 0
			Handler d_handler;
			const char* d_next; // Nächster Zustand oder 0
			Poster d_post;		// Wird von endElement aufgerufen, falls vorhanden.
			bool d_asTemplate;
		};
		static Slot d_slots[];
		std::deque<Slot*> d_current;
        QSet<QString> d_filePaths;

		bool handleFieldEnd(Object*);
        bool handleFieldEnd(SmallObject*);

        template<class T>
        bool _handleFieldEndImp( T* obj )
        {
            Root::Any v;
            if( d_type == Root::Any::Date )
            {
                Root::Date d;
                if( !d.parse( d_text, true ) )
                {
                    d_error.sprintf( "Invalid date: %s", d_text.data() );
                    return false;
                }
                v.setDate( d );
            }else if( d_type == Root::Any::DateTime )
            {
                Root::DateTime d;
                if( !d.parse( d_text, true ) )
                {
                    d_error.sprintf( "Invalid date/time: %s", d_text.data() );
                    return false;
                }
                v.setDateTime( d );
            }else if( !v.parse( d_text, d_type ) )
            {
                d_error.sprintf( "Invalid value: %s", d_text.data() );
                return false;
            }
            obj->setFieldValue( d_field, v );
            return true;

        }

		bool readRange( const XmlHandler::Atts & atts );
		bool readSample( const XmlHandler::Atts & atts );
		bool readLabelScheme( const XmlHandler::Atts & atts );
		bool readAtomScheme( const XmlHandler::Atts & atts );
		bool readCoord( const XmlHandler::Atts & atts );
		bool readLoc( const XmlHandler::Atts & atts );
		bool readConf( const XmlHandler::Atts & atts );
		bool readStruct( const XmlHandler::Atts & atts );
		bool readBond( const XmlHandler::Atts & atts );
		bool readAtom( const XmlHandler::Atts & atts );
		bool readGroup( const XmlHandler::Atts & atts );
		bool readResidueType( const XmlHandler::Atts & atts );
		bool readRepository( const XmlHandler::Atts & atts );
		bool readSysType( const XmlHandler::Atts & atts );
		bool readSpecType( const XmlHandler::Atts & atts );
		bool readSpecTypeEnd();
		bool readDim( const XmlHandler::Atts & atts );
		bool readStep( const XmlHandler::Atts & atts );
		bool readLabel( const XmlHandler::Atts & atts );
		bool readTol( const XmlHandler::Atts & atts );
		bool readProject( const XmlHandler::Atts & atts );
		bool readResidue( const XmlHandler::Atts & atts );
		bool readSequence( const XmlHandler::Atts & atts );
		bool readSequenceEnd();
		bool readParam( const XmlHandler::Atts & atts );
		bool readSpectrum( const XmlHandler::Atts & atts );
		bool readSpectrumEnd();
		bool readLibrary( const XmlHandler::Atts & atts );
		bool readLibraryEnd();
		bool readRot( const XmlHandler::Atts & atts );
		bool readCal( const XmlHandler::Atts & atts );
		bool readSpinSys( const XmlHandler::Atts & atts );
		bool readLink( const XmlHandler::Atts & atts );
		bool readCand( const XmlHandler::Atts & atts );
		bool readStCand( const XmlHandler::Atts & atts );
		bool readSpin( const XmlHandler::Atts & atts );
		bool readPair( const XmlHandler::Atts & atts );
		bool readInst( const XmlHandler::Atts & atts );
		bool readSpinEnd();
		bool readAlias( const XmlHandler::Atts & atts );
		bool readScript( const XmlHandler::Atts & atts );
		bool readCode( const XmlHandler::Atts & atts );
		bool readCodeEnd();
		bool readPeaklist( const XmlHandler::Atts & atts );
		bool readPeakListEnd();
		bool readPeakColor( const XmlHandler::Atts & atts );
		bool readGuess( const XmlHandler::Atts & atts );
		bool readPeak( const XmlHandler::Atts & atts );
		bool readGuessAssig( const XmlHandler::Atts & atts );
		bool readPeakAssig( const XmlHandler::Atts & atts );
		bool readPos( const XmlHandler::Atts & atts );
		bool readPosDim( const XmlHandler::Atts & atts );
		bool readWidth( const XmlHandler::Atts & atts );
		bool readLevel( const XmlHandler::Atts & atts );
		bool readSpecList( const XmlHandler::Atts & atts );
		bool readModel( const XmlHandler::Atts & atts );
		bool readModelParam( const XmlHandler::Atts & atts );
		bool readField( const XmlHandler::Atts & atts );
		bool readOdef( const XmlHandler::Atts & atts );
		bool readObject( const XmlHandler::Atts & atts );
		bool readColor( const XmlHandler::Atts & atts );
		bool readOvclr( const XmlHandler::Atts & atts );
		bool readFieldDef( const XmlHandler::Atts & atts );
		bool nop( const XmlHandler::Atts & atts ) { return true; }
		bool readModelField();
		bool readAtomField();
		bool readSampleField();
		bool readSpecTypeField();
		bool readSchemeField();
		bool readResiTypeField();
		bool readSysTypeField();
		bool readSpecField();
		bool readReposiField();
		bool readProField();
		bool readSpinField();
		bool readSysField();
		bool readResiField();
		bool readGuessField();
		bool readPeakField();
		bool readPlField();
		bool readLinkField();
		bool readScriptField();
		bool readStructField();
		bool readConfField();
		bool readObjectField();
	};

	_XmlParser::Slot _XmlParser::d_slots[] =
	{
		// Symbol	State		Handler			NextState PostHandler asTemplate
		{ DIMENSION, POSITION, &_XmlParser::readPosDim, 0, 0, false },
		{ FIELD, GUESS, &_XmlParser::readField, 0, &_XmlParser::readGuessField, false },
		{ SPIN, GUESS, &_XmlParser::readGuessAssig, 0, 0, false },
		{ GUESS, PEAK, &_XmlParser::readGuess, GUESS, 0, false },
		{ POSITION, PEAK, &_XmlParser::readPos, POSITION, 0, false },
		{ SPIN, PEAK, &_XmlParser::readPeakAssig, 0, 0, false },
		{ FIELD, PEAK, &_XmlParser::readField, 0, &_XmlParser::readPeakField, false },
		{ PEAK, PEAKLIST, &_XmlParser::readPeak, PEAK, 0, false },
		{ PARAM, MODEL, &_XmlParser::readModelParam, 0, 0, false },
		{ FIELD, MODEL, &_XmlParser::readField, 0, &_XmlParser::readModelField, false },
		{ MODEL, PEAKLIST, &_XmlParser::readModel, MODEL, 0, false },
		{ SPEC, PEAKLIST, &_XmlParser::readSpecList, 0, 0, false },
		{ DIMENSION, PEAKLIST, &_XmlParser::readPeakColor, 0, 0, false },
		{ FIELD, PEAKLIST, &_XmlParser::readField, 0, &_XmlParser::readPlField, false },
		{ PEAKLIST, PROJECT, &_XmlParser::readPeaklist, PEAKLIST, &_XmlParser::readPeakListEnd, false },
		{ INST, PAIR, &_XmlParser::readInst, 0, 0, false },
		{ POS, SPIN, &_XmlParser::readAlias, 0, 0, false },
		{ FIELD, PAIR, &_XmlParser::readField, 0, &_XmlParser::readLinkField, false },
		{ PAIR, SPINBASE, &_XmlParser::readPair, PAIR, 0, false },
		{ FIELD, SPIN, &_XmlParser::readField, 0, &_XmlParser::readSpinField, false },
		{ SPIN, SPINBASE, &_XmlParser::readSpin, SPIN, &_XmlParser::readSpinEnd, false },
		{ FIELD, SPINSYS, &_XmlParser::readField, 0, &_XmlParser::readSysField, false },
		{ SPINSYS, SPINBASE, &_XmlParser::readSpinSys, SPINSYS, 0, false },
		{ CAND, SPINSYS, &_XmlParser::readCand, 0, 0, false },
		{ LINK, SPINBASE, &_XmlParser::readLink, 0, 0, false },
		{ SPINBASE, PROJECT, &_XmlParser::nop, SPINBASE, 0, false },
		{ ROTATE, SPECTRUM, &_XmlParser::readRot, 0, 0, false },
		{ CALIBRATE, SPECTRUM, &_XmlParser::readCal, 0, 0, false },
		{ LEVEL, SPECTRUM, &_XmlParser::readLevel, 0, 0, false },
		{ FIELD, SPECTRUM, &_XmlParser::readField, 0, &_XmlParser::readSpecField, false },
		{ SPECTRUM, PROJECT, &_XmlParser::readSpectrum, SPECTRUM, &_XmlParser::readSpectrumEnd, false },
		{ FIELD, SAMPLE, &_XmlParser::readField, 0, &_XmlParser::readSampleField, false },
		{ RANGE, SAMPLE, &_XmlParser::readRange, 0, 0, false },
		{ SAMPLE, PROJECT, &_XmlParser::readSample, SAMPLE, 0, false },
		{ PARAM, RESIDUE, &_XmlParser::readParam, 0, 0, false },
		{ FIELD, RESIDUE, &_XmlParser::readField, 0, &_XmlParser::readResiField, false },
		{ RESIDUE, SEQUENCE, &_XmlParser::readResidue, RESIDUE, 0, false },
		{ SEQUENCE, PROJECT, &_XmlParser::readSequence, SEQUENCE, &_XmlParser::readSequenceEnd, false },
		{ COORD, CONF, &_XmlParser::readCoord, 0, 0, false },
		{ FIELD, CONF, &_XmlParser::readField, 0, &_XmlParser::readConfField, false },
		{ CONF, STRUCT, &_XmlParser::readConf, CONF, 0, false },
		{ COORD, STRUCT, &_XmlParser::readLoc, 0, 0, false },
		{ FIELD, STRUCT, &_XmlParser::readField, 0, &_XmlParser::readStructField, false },
		{ STRUCT, PROJECT, &_XmlParser::readStruct, STRUCT, 0, false },
		{ WIDTH, PROJECT, &_XmlParser::readWidth, 0, 0, false },
		{ TOL, PROJECT, &_XmlParser::readTol, 0, 0, false },
		{ FIELD, PROJECT, &_XmlParser::readField, 0, &_XmlParser::readProField, false },
		{ PROJECT, REPOSITORY, &_XmlParser::readProject, PROJECT, 0, false },

		{ LABEL, DIMENSION, &_XmlParser::readLabel, 0, 0, true },
		{ DIMENSION, SPECTRUM_TYPE, &_XmlParser::readDim, DIMENSION, 0, true },
		{ STEP, SPECTRUM_TYPE, &_XmlParser::readStep, 0, 0, true },
		{ FIELD, SPECTRUM_TYPE, &_XmlParser::readField, 0, &_XmlParser::readSpecTypeField, true },
		{ SPECTRUM_TYPE, REPOSITORY, &_XmlParser::readSpecType, SPECTRUM_TYPE, &_XmlParser::readSpecTypeEnd, true },
		{ FIELD, SYSTYPE, &_XmlParser::readField, 0, &_XmlParser::readSysTypeField, true },
		{ CAND, SYSTYPE, &_XmlParser::readStCand, 0, 0, false },
		{ SYSTYPE, REPOSITORY, &_XmlParser::readSysType, SYSTYPE, 0, true },
		{ FIELD, OBJECT, &_XmlParser::readField, 0, &_XmlParser::readObjectField, false },
		{ OBJECT, OBJECT, &_XmlParser::nop, OBJECT, 0, false },	// Nicht template
		{ OBJECT, DATABASE, &_XmlParser::readObject, OBJECT, 0, false },	// Nicht template
		{ DATABASE, REPOSITORY, &_XmlParser::nop, DATABASE, 0, false },
		{ FIELD, ODEF, &_XmlParser::readFieldDef, 0, 0, true },
		{ ODEF, REPOSITORY, &_XmlParser::readOdef, ODEF, 0, true },
		{ CODE, SCRIPT, &_XmlParser::readCode, CODE, &_XmlParser::readCodeEnd, true },
		{ FIELD, SCRIPT, &_XmlParser::readField, 0, &_XmlParser::readScriptField, true },
		{ SCRIPT, REPOSITORY, &_XmlParser::readScript, SCRIPT, 0, true },
		{ FIELD, REPOSITORY, &_XmlParser::readField, 0, &_XmlParser::readReposiField, true },
		{ REPOSITORY, INIT, &_XmlParser::readRepository, REPOSITORY, 0, true },
		{ FIELD, RESIDUE_TYPE, &_XmlParser::readField, 0, &_XmlParser::readResiTypeField, true },
		{ OVCLR, REPOSITORY, &_XmlParser::readOvclr, 0, 0, true },
		{ COLOR, REPOSITORY, &_XmlParser::readColor, 0, 0, true },
		{ RESIDUE_TYPE, REPOSITORY, &_XmlParser::readResidueType, RESIDUE_TYPE, 0, true },

		{ SPECTRUM_TYPE, LIBRARY, &_XmlParser::readSpecType, SPECTRUM_TYPE, &_XmlParser::readSpecTypeEnd, true },
		{ SYSTYPE, LIBRARY, &_XmlParser::readSysType, SYSTYPE, 0, true },
		{ RESIDUE_TYPE, LIBRARY, &_XmlParser::readResidueType, RESIDUE_TYPE, 0, true },
		{ SCHEME, LIBRARY, &_XmlParser::readLabelScheme, SCHEME, 0, true },
		{ FIELD, SCHEME, &_XmlParser::readField, 0, &_XmlParser::readSchemeField, true },
		{ LIBRARY, REPOSITORY, &_XmlParser::readLibrary, LIBRARY, &_XmlParser::readLibraryEnd, true },
		{ FIELD, GROUP, &_XmlParser::readField, 0, &_XmlParser::readAtomField, true },
		{ GROUP, RESIDUE_TYPE, &_XmlParser::readGroup, 0, 0, true },
		{ FIELD, _ATOM, &_XmlParser::readField, 0, &_XmlParser::readAtomField, true },
		{ _ATOM, RESIDUE_TYPE, &_XmlParser::readAtom, _ATOM, 0, true },
		{ SCHEME, _ATOM, &_XmlParser::readAtomScheme, 0, 0, true },
		{ BOND, RESIDUE_TYPE, &_XmlParser::readBond, 0, 0, true },
		{ 0, 0, 0, 0, 0, 0 }
	};

	bool _XmlParser::handleFieldEnd( Object* obj )
	{
        return _handleFieldEndImp( obj );
	}

    bool _XmlParser::handleFieldEnd( SmallObject* obj )
	{
        return _handleFieldEndImp( obj );
	}

	bool _XmlParser::readConfField()
	{
		return handleFieldEnd( d_conf );
	}

	bool _XmlParser::readStructField()
	{
		return handleFieldEnd( d_struct );
	}

	bool _XmlParser::readProField()
	{
		return handleFieldEnd( d_pro );
	}

	bool _XmlParser::readLinkField()
	{
		return handleFieldEnd( d_link );
	}

	bool _XmlParser::readSpecField()
	{
		if( d_specPeer.isNull() ) // Discard
			return true;
		return handleFieldEnd( d_specPeer );
	}

	bool _XmlParser::readAtomField()
	{
		return handleFieldEnd( d_atom );
	}

	bool _XmlParser::readModelField()
	{
		return handleFieldEnd( d_model );
	}

	bool _XmlParser::readSampleField()
	{
		return handleFieldEnd( d_sample );
	}

	bool _XmlParser::readReposiField()
	{
		return handleFieldEnd( d_rep );
	}

	bool _XmlParser::readSpecTypeField()
	{
		return handleFieldEnd( d_st );
	}

	bool _XmlParser::readSchemeField()
	{
		return handleFieldEnd( d_scheme );
	}

	bool _XmlParser::readResiTypeField()
	{
		return handleFieldEnd( d_restp );
	}

	bool _XmlParser::readSysTypeField()
	{
		return handleFieldEnd( d_sst );
	}

	bool _XmlParser::readSysField()
	{
		return handleFieldEnd( d_sys );
	}

	bool _XmlParser::readScriptField()
	{
		return handleFieldEnd( d_script );
	}

	bool _XmlParser::readSpinField()
	{
		return handleFieldEnd( d_spin );
	}

	bool _XmlParser::readPlField()
	{
		return handleFieldEnd( d_currentPeakList );
	}

	bool _XmlParser::readGuessField()
	{
		return handleFieldEnd( d_guess );
	}

	bool _XmlParser::readPeakField()
	{
		return handleFieldEnd( d_currentPeak );
	}

	bool _XmlParser::readResiField()
	{
		return handleFieldEnd( d_res );
	}

	bool _XmlParser::readObjectField()
	{
		return handleFieldEnd( d_obj );
	}
	
	bool _XmlParser::readSpecList( const XmlHandler::Atts & atts )
	{
		bool ok;
		int id = atts.value( "id" ).toInt( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid spectrum: %s", atts.value( "id" ).data() );
			return false;
		}
		if( d_pro->d_specs.count( id ) == 0 )
		{
			s_tmp.sprintf( "Referencing unknown spectrum %d in peak list %s",
				id, d_currentPeakList->getName().data() );
			Root::MessageLog::inst()->warning( "Repository", s_tmp );
		}

		d_currentPeakList->d_specs.push_back( id );
		return true;
	}

	bool _XmlParser::readModel( const XmlHandler::Atts & atts )
	{
		bool ok;
		/*
		int kind = atts.value( "kind" ).toUInt( &ok );
		if( !ok )
		{
			// TODO: Gültigkeit prüfen.
			d_error.sprintf( "Invalid model kind: %s", atts.value( "kind" ).data() );
			return false;
		}
		*/
		int id = atts.value( "id" ).toInt( &ok );
		if( !ok || ( id != 0 && d_currentPeakList->d_models.count( id ) != 0 ) )
		{
			d_error.sprintf( "Model already defined: %s", atts.value( "id" ).data() );
			return false;
		}
		d_model = new PeakModel();
		d_model->d_id = id;
		d_currentPeakList->d_models[ id ] = d_model;
		d_model->addObserver( d_currentPeakList );
		if( id == 0 )
			d_currentPeakList->d_model = d_model;
		d_dim = 0;
		return true;
	}

	bool _XmlParser::readModelParam( const XmlHandler::Atts & atts )
	{
		bool ok;
		PPM gain = atts.value( "gain" ).toFloat( &ok );
		if( !ok || gain < 0.0 || gain > 1.0 )
		{
			d_error.sprintf( "Invalid gain: %s", atts.value( "gain" ).data() );
			return false;
		}
		PPM bal = atts.value( "bal" ).toFloat( &ok );
		if( !ok || bal < 0.0 || bal > 1.0 )
		{
			d_error.sprintf( "Invalid balance: %s", atts.value( "bal" ).data() );
			return false;
		}
		PPM width = atts.value( "width" ).toFloat( &ok );
		if( !ok || width < 0.0 )
		{
			d_error.sprintf( "Invalid width: %s", atts.value( "width" ).data() );
			return false;
		}
		PPM tol = atts.value( "tol" ).toFloat( &ok );
		if( !ok || tol < 0.0 || tol > 2.0 )
		{
			d_error.sprintf( "Invalid peak tolerance: %s", atts.value( "tol" ).data() );
			return false;
		}
		if( d_dim >= PeakPos::MAX_DIM )
		{
			d_error.sprintf( "More dimensions specified than supported by CARA" );
			return false;
		}
		d_model->d_par.d_tol[ d_dim ] = tol;
		d_model->d_par.d_width[ d_dim ] = width;
		d_model->d_par.d_gain[ d_dim ] = gain;
		d_model->d_par.d_bal[ d_dim ] = bal;
		d_dim++;
		d_model->d_dim = d_dim;	// Richtige Reihenfolge. Anzahl, nicht Index.
		return true;
	}

	bool _XmlParser::readPosDim( const XmlHandler::Atts & atts )
	{
		bool ok = true;
		PPM pos = atts.value( "pos" ).toFloat( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid position: %s", atts.value( "pos" ).data() );
			return false;
		}
		if( d_dim2 >= PeakPos::MAX_DIM )
		{
			d_error.sprintf( "More dimensions specified than supported by CARA" );
			return false;
		}
		d_currentPeak->d_pos[ d_specId ].d_pos[ d_dim2++ ] = pos;
		return true;
	}

	bool _XmlParser::readPeakAssig( const XmlHandler::Atts & atts )
	{
		bool ok = true;
		Root::Index ass = atts.value( "id" ).toInt( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid assignment: %s", atts.value( "id" ).data() );
			return false;
		}
		if( d_dim >= Peak::Assig::MAX_DIM )
		{
			d_error.sprintf( "More dimensions specified than supported by CARA" );
			return false;
		}
		d_currentPeak->d_assig[ d_dim++ ] = ass; // TEST .push_back( ass );
		return true;
	}

	bool _XmlParser::readPos( const XmlHandler::Atts & atts )
	{
		// Position des Peaks pro Spektrum
		bool ok = true;
		Amplitude amp = atts.value( "amp" ).toFloat( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid amplitude: %s", atts.value( "amp" ).data() );
			return false;
		}
		Amplitude vol = atts.value( "vol" ).toFloat( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid volume: %s", atts.value( "vol" ).data() );
			return false;
		}
		d_specId = atts.value( "spec" ).toInt( &ok );
		if( !ok /* RISK || ( d_specId != 0 && d_pro->d_specs.count( d_specId ) == 0 ) */ )
		{
			d_error.sprintf( "Invalid spectrum: %s", atts.value( "spec" ).data() );
			return false;
		}
        if( d_currentPeakHome != d_currentPeakList->getHome() && d_specId == d_currentPeakHome )
            d_specId = d_currentPeakList->getHome();
            // Sorge dafür, dass Peaks mit einem von PeakList abweichenden Home auf das Home
            // der PeakList abgebildet werden
		if( d_specId != 0 && d_pro->d_specs.count( d_specId ) == 0 )
		{
			s_tmp.sprintf( "Referencing unknown spectrum %d in peak %d",
				d_specId, d_currentPeak->getId() );
			Root::MessageLog::inst()->warning( "Repository", s_tmp );
		}
		if( d_currentPeak->d_pos.count( d_specId ) != 0 )
		{
            // Seit 1.9.0b9: wir lassen zu, dass dies vorkommt. Das kann passieren, wenn
            // peak home=0 und peaklist home!=0. In diesem Fall wurde peak bereits auf home
            // der peaklist umgebogen, und ein weiteres pos ohne dim zum selben home kann existieren.
			//d_error.sprintf( "Position for spectrum id %d already defined!", d_specId );
			//return false;
            s_tmp.sprintf( "Position for spectrum id %d already defined in peak %d",
                           d_specId, d_currentPeak->getId() );
			Root::MessageLog::inst()->warning( "Repository", s_tmp );
		}
		PeakModel* mdl = 0;
		if( !atts.value( "mdl" ).isEmpty() )
		{
			int id = atts.value( "mdl" ).toUShort( &ok );
			if( !ok || id < 0 || d_currentPeakList->d_models[ id ] == 0 )
			{
				d_error.sprintf( "Invalid model reference: %s", atts.value( "mdl" ).data() );
				return false;
			}
			mdl = d_currentPeakList->d_models[ id ];
		}
		Peak::Position& p = d_currentPeak->d_pos[ d_specId ];
		p.d_amp = amp;
		p.d_vol = vol;
		p.d_mdl = mdl;
		d_dim2 = 0;
		return true;
	}

	bool _XmlParser::readGuessAssig( const XmlHandler::Atts & atts )
	{
		bool ok = true;
		Root::Index ass = atts.value( "id" ).toInt( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid assignment: %s", atts.value( "id" ).data() );
			return false;
		}
		if( d_dim3 >= Peak::Assig::MAX_DIM )
		{
			d_error.sprintf( "More dimensions specified than supported by CARA" );
			return false;
		}
		d_guess->d_assig[ d_dim3++ ] = ass; 
		return true;
	}

	bool _XmlParser::readGuess( const XmlHandler::Atts & atts )
	{
		bool ok = true;
		Root::Index id = atts.value( "id" ).toInt( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid guess id: %s", atts.value( "id" ).data() );
			return false;
		}
		if( d_currentPeak->d_guesses.count( id ) != 0 )
		{
			d_error.sprintf( "Guess id not unique: %s", atts.value( "id" ).data() );
			return false;
		}
		Root::Float p = atts.value( "prob" ).toFloat( &ok );
		if( !ok || p < 0.0 || p > 1.0 )
		{
			d_error.sprintf( "Invalid probability: %s", atts.value( "prob" ).data() );
			return false;
		}
		d_guess = new Peak::Guess();
		d_guess->d_id = id;
		d_guess->d_prob = p;
		d_guess->d_owner = d_currentPeak;
		d_currentPeak->d_guesses[ id ] = d_guess;
		d_dim3 = 0;
		return true;
	}

	bool _XmlParser::readPeak( const XmlHandler::Atts & atts )
	{
		bool ok = true;
        d_currentPeakHome = d_currentPeakList->getHome();
		Root::Index id = atts.value( "id" ).toInt( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid peak id: %s", atts.value( "id" ).data() );
			return false;
		}
		if( d_currentPeakList->d_peaks.count( id ) != 0 )
		{
			d_error.sprintf( "Peak id not unique: %s", atts.value( "id" ).data() );
			return false;
		}
		const Root::SymbolString tag = atts.value( "tag" ).data();
		const Root::Index home = atts.value( "home" ).toInt( &ok );
        if( ok )
            d_currentPeakHome = home;

        // Auskommentiert, weil neu die Positionen an PeakList:getHome zugeordnet werden.
//		if( !ok )
//		{
//			d_error.sprintf( "Invalid spectrum: %s", atts.value( "home" ).data() );
//			return false;
//		}
//		if( home != 0 && d_pro->d_specs.count( home ) == 0 )
//		{
//			s_tmp.sprintf( "Referencing unknown spectrum %d in peak %d",
//				home, id );
//			Root::MessageLog::inst()->warning( "Repository", s_tmp );
//		}
		int clr = 0;
		if( !atts.value( "color" ).isEmpty() )
		{
			clr = atts.value( "color" ).toUShort( &ok );
			if( !ok || clr > 255 )
			{
				d_error.sprintf( "Invalid peak color: %s", atts.value( "color" ).data() );
				return false;
			}
		}
		d_currentPeak = new Peak( home );
		d_currentPeak->d_tag = tag;
		d_currentPeak->d_id = id;
		d_currentPeak->d_color = clr;
		d_currentPeakList->d_peaks[ id ] = d_currentPeak;
		d_currentPeak->d_owner = d_currentPeakList;
		d_specPeer = 0;
		d_dim = 0;
		return true;
	}

	bool _XmlParser::readPeakColor( const XmlHandler::Atts & atts )
	{
		// ColorMap der PeakList
		AtomType a = AtomType::parseLabel( atts.value( "atom" ).data() );
		if( a.isNone() )
		{
			d_error.sprintf( "Invalid atom type: %s", atts.value( "atom" ).data() );
			return false;
		}
		d_currentPeakList->d_colors.push_back( a );
		return true;
	}

	bool _XmlParser::readPeakListEnd()
	{
		Dimension dim = d_currentPeakList->d_colors.size();
		if( d_currentPeakList->d_model->getDimCount() == 0 )
			d_currentPeakList->d_model->init( dim ); // RISK
		PeakList::Models::const_iterator p3;
		for( p3 = d_currentPeakList->getModels().begin(); p3 != d_currentPeakList->getModels().end(); ++p3 )
		{
			if( (*p3).second->getDimCount() != dim )
			{
				d_error.sprintf( "Invalid peak model %d for peaklist %d", 
					(*p3).first, d_currentPeakList->getId() );
				return false;
			}		
		}

		PeakList::Peaks& peaks = d_currentPeakList->d_peaks;
		PeakList::Peaks::iterator pi;
		for( pi = peaks.begin(); pi != peaks.end(); ++pi )
		{
			Peak::PositionMap& pos = (*pi).second->d_pos;
			if( pos.empty() )
			{
				d_error.sprintf( "Peak %d of peaklist %d has no positions", 
					(*pi).first, d_currentPeakList->getId() );
				return false;
			}
            if( pos.count( (*pi).second->getHome() ) == 0 )
			{
				d_error.sprintf( "Peak %d has no position for spectrum id %d", 
					d_currentPeakList->getId(), d_currentPeakList->getHome() );
				return false;
			}
			Peak::PositionMap::iterator p2;
			for( p2 = pos.begin(); p2 != pos.end(); ++p2 )
			{
				if( (*p2).second.d_mdl == 0 )
					(*p2).second.d_mdl = d_currentPeakList->getModel(); // RISK

				/*
 				if( ( !(*p2).second.d_pos.empty() ||
					(*pi).second->d_home == (*p2).first ) && 
					(*p2).second.d_pos.size() != dim )
					// NOTE: mind. Home-Alias muss Position enthalten. Ansonsten
					// sind leere Positionen zulässig.
				{
					d_error.sprintf( "Peak alias %d of peaklist %d has wrong number of dimensions", 
						(*pi).first, d_peaks->getId() );
					return false;
				}*/
			}
 			/* TEST
			if( !(*pi).second->d_assig.empty() &&
				(*pi).second->d_assig.size() != dim )
			{
				d_error.sprintf( 
					"Assignment of peak %d of peaklist %d has wrong number of dimensions", 
					(*pi).first, d_peaks->getId() );
				return false;
			}*/ 
		}
		d_currentPeakList->addObserver( d_pro );
		return true;
	}

	bool _XmlParser::readPeaklist( const XmlHandler::Atts & atts )
	{
		bool ok = true;
		if( atts.value( "name" ).isEmpty() )
		{
			d_error = "Peaklist without name";
			return false;
		}
        /* Auf Wunsch von Fred in 1.9.0 Beta 10
		if( d_pro->findPeakList( atts.value( "name" ).data() ) != 0 )
		{
			d_error.sprintf( "Peaklist name not unique: %s", atts.value( "name" ).data() );
			return false;
		}*/
		Root::Index id = atts.value( "id" ).toInt( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid peaklist id: %s", atts.value( "id" ).data() );
			return false;
		}
		if( d_pro->d_peaks.count( id ) != 0 )
		{
			d_error.sprintf( "Peaklist id not unique: %s", atts.value( "id" ).data() );
			return false;
		}
		Root::Index home = 0;
		if( !atts.value( "home" ).isEmpty() )
		{
			home = atts.value( "home" ).toInt( &ok );
			
			if( !ok )
			{
				d_error.sprintf( "Invalid spectrum: %s", atts.value( "home" ).data() );
				return false;
			}
			if( home != 0 && d_pro->d_specs.count( home ) == 0 )
			{
				s_tmp.sprintf( "Referencing unknown spectrum %d in peak list %s",
					id, atts.value( "name" ).data() );
				Root::MessageLog::inst()->warning( "Repository", s_tmp );
			}
		}
		d_currentPeakList = new PeakList();
		d_currentPeakList->d_owner = d_pro;
		d_currentPeakList->d_id = id;
		d_currentPeakList->d_homeSpecId = home;
		d_currentPeakList->d_name = atts.value( "name" ).data();
		d_pro->d_peaks[ id ] = d_currentPeakList;
		return true;
	}

	bool _XmlParser::readPair( const XmlHandler::Atts & atts )
	{
		bool ok;
		Spin* lhs = d_pro->d_spins->d_spins[ atts.value( "lhs" ).toInt( &ok ) ];
		if( !ok || lhs == 0 )
		{
			d_error.sprintf( "Invalid spin id: %s", atts.value( "lhs" ).data() );
			return false;
		}
		Spin* rhs = d_pro->d_spins->d_spins[ atts.value( "rhs" ).toInt( &ok ) ];
		if( !ok || rhs == 0 )
		{
			d_error.sprintf( "Invalid spin id: %s", atts.value( "rhs" ).data() );
			return false;
		}
		if( lhs->findLink( rhs ) != 0 )
		{
			s_tmp.sprintf( "Duplicate pair lhs=%d rhs=%d",
				lhs->getId(), rhs->getId() );
			Root::MessageLog::inst()->warning( "Repository", s_tmp );
			return true; // Ignorieren.
			/*d_error.sprintf( "Spin pair %d - %d already defined!", 
				lhs->getId(), rhs->getId() );
			return false;
			*/
		}
		d_link = new SpinLink( lhs->getId(), rhs->getId() );
		d_link->d_owner = d_pro->d_spins;
		d_pro->d_spins->d_links.insert( d_link.deref() );
		lhs->d_links.insert( d_link );
		rhs->d_links.insert( d_link );
		return true; 
	}

	bool _XmlParser::readInst( const XmlHandler::Atts & atts )
	{
		bool ok;
		int home = atts.value( "spec" ).toInt( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid spectrum: %s", atts.value( "spec" ).data() );
			return false;
		}
		if( home != 0 && d_pro->d_specs.count( home ) == 0 )
		{
			s_tmp.sprintf( "Referencing unknown spectrum %d in spin pair %d/%d",
				home, d_link->getLhs(), d_link->getRhs() );
			Root::MessageLog::inst()->warning( "Repository", s_tmp );
		}
		int visi = 0;
		if( !atts.value( "amp" ).isEmpty() )
		{
			visi = ( atts.value( "amp" ).toFloat( &ok ) != 0.0 );
			if( !ok )
			{
				d_error.sprintf( "Invalid amplitude: %s", atts.value( "amp" ).data() );
				return false;
			}
		}
		Root::Float rate = 0;
		if( !atts.value( "rate" ).isEmpty() )
		{
			rate = atts.value( "rate" ).toFloat( &ok );
			if( !ok )
			{
				d_error.sprintf( "Invalid rating: %s", atts.value( "rate" ).data() );
				return false;
			}
		}
		int code = 0;
		if( !atts.value( "code" ).isEmpty() )
		{
			code = atts.value( "code" ).toShort( &ok );
			if( !ok || code < 0 || code > 255 )
			{
				d_error.sprintf( "Invalid code: %s", atts.value( "code" ).data() );
				return false;
			}
		}
		if( !atts.value( "visi" ).isEmpty() )
		{
			visi = atts.value( "visi" ).toShort( &ok );
			if( !ok || visi < 0 || visi > 1 )
			{
				d_error.sprintf( "Invalid flag: %s", atts.value( "visi" ).data() );
				return false;
			}
		}
		SpinLink::Alias& a = d_link->d_alias[ home ];
		a.d_code = code;
		a.d_rating = rate;
		a.d_visible = visi;
		return true; 
	}

	bool _XmlParser::readAlias( const XmlHandler::Atts & atts )
	{
		bool ok;
		int home = atts.value( "spec" ).toInt( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid spectrum: %s", atts.value( "spec" ).data() );
			return false;
		}
		if( home != 0 && d_pro->d_specs.count( home ) == 0 )
		{
			s_tmp.sprintf( "Referencing unknown spectrum %d in spin %d",
				home, d_spin->getId() );
			Root::MessageLog::inst()->warning( "Repository", s_tmp );
		}
		PPM shift = atts.value( "shift" ).toFloat( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid spin shift: %s", atts.value( "shift" ).data() );
			return false;
		}
		if( d_spin->d_shifts.count( home ) != 0 )
		{
			d_error.sprintf( "Alias for spectrum id %d already defined.", home );
			return false;
		}

		d_spin->d_shifts[ home ] = shift;
		return true; 
	}

	bool _XmlParser::readSpinEnd()
	{
		const Spin::Shifts& sh = d_spin->getShifts();
		if( sh.empty() )
		{
			d_error.sprintf( "Spin %d has no shifts", d_spin->getId() );
			return false;
		}
		if( sh.count( d_spin->getHome() ) == 0 )
		{
			d_error.sprintf( "Spin %d has no shift for spectrum id %d", 
				d_spin->getId(), d_spin->getHome() );
			return false;
		}
		return true;
	}

	bool _XmlParser::readSpin( const XmlHandler::Atts & atts )
	{
		bool ok;
		Root::Index id = atts.value( "id" ).toInt( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid spin id: %s", atts.value( "id" ).data() );
			return false;
		}
		if( d_pro->d_spins->d_spins.count( id ) != 0 )
		{
			d_error.sprintf( "Spin id not unique within project: %s", atts.value( "id" ).data() );
			return false;
		}
		AtomType a = AtomType::parseLabel( atts.value( "atom" ).data() );
		if( a.isNone() )
		{
			d_error.sprintf( "Invalid atom type: %s", atts.value( "atom" ).data() );
			return false;
		}
		int home = atts.value( "home" ).toInt( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid spectrum: %s", atts.value( "home" ).data() );
			return false;
		}
		if( home != 0 && d_pro->d_specs.count( home ) == 0 )
		{
			s_tmp.sprintf( "Referencing unknown spectrum %d in spin %d",
				home, d_spin->getId() );
			Root::MessageLog::inst()->warning( "Repository", s_tmp );
		}
		SpinLabel l;
		if( !atts.value( "tag" ).isEmpty() )
		{
			l.setTag( atts.value( "tag" ).data() );
			if( l.getTag().isNull() )
			{
				d_error.sprintf( "Invalid label tag: %s", atts.value( "tag" ).data() );
				return false;
			}
			bool ok;
			l.setOffset( atts.value( "off" ).toShort( &ok ) );
			if( !ok )
			{
				d_error.sprintf( "Invalid offset value: %s", atts.value( "off" ).data() );
				return false;
			}
			int s = atts.value( "final" ).toUShort( &ok );
			if( !ok || s > 2 )
			{
				d_error.sprintf( "Invalid final flag: %s", atts.value( "final" ).data() );
				return false;
			}
			l.setState( (SpinLabel::State)s );
		}
		SpinSystem* sys = 0;
		if( !atts.value( "sys" ).isEmpty() )
		{
			sys = d_pro->d_spins->d_systems[ atts.value( "sys" ).toInt( &ok ) ];
			if( !ok || sys == 0 )
			{
				d_error.sprintf( "Invalid spin system id: %s", atts.value( "sys" ).data() );
				return false;
			}
		}

		Location loc;
		bool hasLoc = false;
		if( !atts.value( "x" ).isEmpty() && !atts.value( "y" ).isEmpty() &&
			!atts.value( "z" ).isEmpty() && !atts.value( "r" ).isEmpty() )
		{
			hasLoc = true;
			loc.d_pos[DimX] = atts.value( "x" ).toFloat( &ok );
			if( !ok )
			{
				d_error.sprintf( "Invalid x position: %s", atts.value( "x" ).data() );
				return false;
			}
			loc.d_pos[DimY] = atts.value( "y" ).toFloat( &ok );
			if( !ok )
			{
				d_error.sprintf( "Invalid y position: %s", atts.value( "y" ).data() );
				return false;
			}
			loc.d_pos[DimZ] = atts.value( "z" ).toFloat( &ok );
			if( !ok )
			{
				d_error.sprintf( "Invalid z position: %s", atts.value( "z" ).data() );
				return false;
			}
			loc.d_dev = atts.value( "r" ).toFloat( &ok );
			if( !ok )
			{
				d_error.sprintf( "Invalid r value: %s", atts.value( "r" ).data() );
				return false;
			}

		}

		d_spin = new Spin();
		d_spin->d_owner = d_pro->d_spins;
		d_spin->d_home = home;
		d_spin->d_atom = a;
		d_spin->d_label = l;
		d_spin->d_id = id;
		if( hasLoc )
		{
			d_spin->d_loc = new Location( loc );
		}
		if( sys )
		{
			if( !sys->isAcceptable( l ) )
			{
				d_error.sprintf( "Spin %d doesn't meet conditions to be added to system %d", 
					id, sys->getId() );
				return false;
			}
			sys->d_spins.insert( d_spin.deref() );
			d_spin->d_system = sys;
		}
		d_pro->d_spins->d_spins[ id ] = d_spin;
        if( id > d_pro->d_spins->d_maxSpinId )
            d_pro->d_spins->d_maxSpinId = id;
		return true; 
	}

	bool _XmlParser::readLink( const XmlHandler::Atts & atts )
	{
		bool ok;
		SpinBase::SpinSystemMap::const_iterator i;
		i = d_pro->d_spins->d_systems.find( atts.value( "pred" ).toInt( &ok ) );
		if( !ok || i == d_pro->d_spins->d_systems.end() )
		{
			d_error.sprintf( "Invalid spin system id: %s", atts.value( "pred" ).data() );
			return false;
		}
		SpinSystem* pred = (*i).second;
		i = d_pro->d_spins->d_systems.find( atts.value( "succ" ).toInt( &ok ) );
		if( !ok || i == d_pro->d_spins->d_systems.end() )
		{
			d_error.sprintf( "Invalid spin system id: %s", atts.value( "succ" ).data() );
			return false;
		}
		SpinSystem* succ = (*i).second;
		if( pred->d_succ != 0 || succ->d_pred != 0 )
		{
			d_error.sprintf( "System pair %d - %d already engaged in other fragment!", 
				pred->getId(), succ->getId() );
			return false;
		}
		pred->d_succ = succ;
		succ->d_pred = pred;
		return true; 
	}

	bool _XmlParser::readCand( const XmlHandler::Atts & atts )
	{
		ResidueType* t = d_rep->findResidueType( atts.value( "type" ).data() );
		if( t == 0 )
		{
			d_error.sprintf( "Unknown residue type: %s", atts.value( "type" ).data() );
			return false;
		}
		d_sys->d_cands.insert( t );
		return true;
	}

	bool _XmlParser::readSpinSys( const XmlHandler::Atts & atts )
	{
		bool ok;
		Root::Index id = atts.value( "id" ).toInt( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid spin system id: %s", atts.value( "id" ).data() );
			return false;
		}
		if( d_pro->d_spins->d_systems.count( id ) != 0 )
		{
			d_error.sprintf( "Spin system id not unique within project: %s", atts.value( "id" ).data() );
			return false;
		}
		Residue* ass = 0;
		if( !atts.value( "ass" ).isEmpty() )
		{
			Root::Index assId = atts.value( "ass" ).toInt( &ok );
			if( !ok )
			{
				d_error.sprintf( "Invalid residue number format: %s", 
					atts.value( "ass" ).data() );
				return false;
			}
			Sequence::ResidueMap::const_iterator a = d_pro->d_seq->d_resi.find( assId );
			if( a == d_pro->d_seq->d_resi.end() )
			{
				d_error.sprintf( "Cannot find residue with number %s!", atts.value( "ass" ).data() );
				return false;
			}
			ass = (*a).second;
		}
		SystemType* sst = 0;
		if( !atts.value( "systype" ).isEmpty() )
		{
			Repository::SystemTypeMap::const_iterator p = 
				d_rep->getSystemTypes().find( atts.value( "systype" ).toUShort( &ok ) );
			if( !ok || p == d_rep->getSystemTypes().end() )
			{
				d_error.sprintf( "Invalid spin system type reference: %s", 
					atts.value( "systype" ).data() );
				return false;
			}
			sst = (*p).second;
		}
		d_sys = new SpinSystem();
		d_sys->d_owner = d_pro->d_spins;
		d_sys->d_id = id;
		d_sys->d_sysType = sst;
		d_sys->d_assig = ass;
		if( ass )
			ass->d_ss = d_sys;
		// TODO: Sicherstellen, dass ganzes Fragment zugewiesen ist und zu Resi passt.
		d_pro->d_spins->d_systems[ id ] = d_sys;
        if( id > d_pro->d_spins->d_maxSysId )
            d_pro->d_spins->d_maxSysId = id;
		return true; 
	}

	bool _XmlParser::readRot( const XmlHandler::Atts & atts )
	{
		if( d_specPeer.isNull() ) // Discard
			return true;
		bool ok;
		Dimension dim = d_specPeer->getDimCount();
		Dimension view = atts.value( "view" ).toUShort( &ok );
		if( !ok || view >= dim )
		{
			d_error.sprintf( "Invalid mapping: view=%s", atts.value( "view" ).data() );
			return false;
		}
		Dimension spec = atts.value( "spec" ).toUShort( &ok );
		if( !ok || spec >= dim )
		{
			d_error.sprintf( "Invalid mapping: spec=%s", atts.value( "spec" ).data() );
			return false;
		}
		d_specPeer->setRotation( view, spec );
		return true; 
	}

	bool _XmlParser::readCal( const XmlHandler::Atts & atts )
	{
		if( d_specPeer.isNull() ) // Discard
			return true;
		bool ok;
		PPM off = atts.value( "off" ).toFloat( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid calibration offset: %s", atts.value( "off" ).data() );
			return false;
		}
		Dimension dim = atts.value( "dim" ).toUShort( &ok );
		if( !ok || dim >= d_specPeer->getDimCount() )
		{
			d_error.sprintf( "Invalid calibration dimension: %s", atts.value( "dim" ).data() );
			return false;
		}
		PPM w = 0.0;
		if( !atts.value( "width" ).isNull() )
		{
			w = atts.value( "width" ).toFloat( &ok );
			if( !ok || w < 0.0 )
			{
				d_error.sprintf( "Invalid peak width: %s", atts.value( "width" ).data() );
				return false;
			}
		}
		if( !atts.value( "fold" ).isNull() )
		{
			QString str = atts.value( "fold" );
			Scale::Folding f;
			if( str == Scale::getFoldingCode( Scale::Unfolded ) )
				f = Scale::Unfolded;
			else if( str == Scale::getFoldingCode( Scale::RSH ) )
				f = Scale::RSH;
			else if( str == Scale::getFoldingCode( Scale::TPPI ) )
				f = Scale::TPPI;
			else
			{
				d_error.sprintf( "Invalid folding code: %s", atts.value( "fold" ).data() );
				return false;
			}
			d_specPeer->getSpectrum()->setFolding( dim, f );
		}
		d_specPeer->setOffset( dim, off );
		d_specPeer->d_width[ dim ] = w;
        if( d_dummySpec )
        {
            // Mache das nach setOffset, da der rausgeschriebene Wert bereits den Offset hat
            const PPM ppm1 = atts.value( "ppm1" ).toFloat( &ok );
            if( ok )
            {
                const PPM ppm2 = atts.value( "ppm2" ).toFloat( &ok );
                if( ok )
                    d_dummySpec->d_scales[ dim ].setRange( ppm1, ppm2 );
            }
            const AtomType a = AtomType::parseLabel( atts.value( "atom" ) );
            if( !a.isNone() )
                d_dummySpec->d_scales[ dim ].setColor( a );
        }
		return true;
	}

	bool _XmlParser::readLevel( const XmlHandler::Atts & atts )
	{
		if( d_specPeer.isNull() ) // Discard
			return true;
		bool ok;
		Spectrum::Levels l;
		l.d_pMax = atts.value( "pmax" ).toFloat( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid positive maximum: %s", atts.value( "pmax" ).data() );
			return false;
		}
		l.d_nMax = atts.value( "nmax" ).toFloat( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid negative maximum: %s", atts.value( "nmax" ).data() );
			return false;
		}
		l.d_pNoise = atts.value( "pnoise" ).toFloat( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid positive noise floor: %s", atts.value( "pnoise" ).data() );
			return false;
		}
		l.d_nNoise = atts.value( "nnoise" ).toFloat( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid negative noise floor: %s", atts.value( "nnoise" ).data() );
			return false;
		}
		d_specPeer->setRawLevels( l );
		if( !atts.value( "thres" ).isNull() )
		{
			Amplitude t = atts.value( "thres" ).toFloat( &ok );
			if( !ok || t < 0.0 )
			{
				d_error.sprintf( "Invalid contour threshold: %s", atts.value( "thres" ).data() );
				return false;
			}
			d_specPeer->setThreshold( t );
		}
		if( !atts.value( "factor" ).isNull() )
		{
			Amplitude t = atts.value( "factor" ).toFloat( &ok );
			if( !ok )
			{
				d_error.sprintf( "Invalid amplitude factor: %s", atts.value( "factor" ).data() );
				return false;
			}
			if( t == 0.0 )
				t = 1.0;
			d_specPeer->d_factor = t;
		}
		d_foundLevels = true;
		return true;
	}

	bool _XmlParser::readRange( const XmlHandler::Atts & atts )
	{
		bool ok = true;
		Root::Index start = atts.value( "start" ).toInt( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid start: %s", atts.value( "start" ).data() );
			return false;
		}
		Root::Index end = atts.value( "end" ).toInt( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid end: %s", atts.value( "end" ).data() );
			return false;
		}
		if( end < start )
		{
			d_error.sprintf( "Invalid range: end < start" );
			return false;
		}
		Root::Index scheme = atts.value( "scheme" ).toInt( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid scheme: %s", atts.value( "scheme" ).data() );
			return false;
		}
		if( !d_sample->addRange( start, end, scheme, true ) )
		{
			d_error.sprintf( "Cannot add range to sample, probably due to range overlap" );
			return false;
		}
		return true;
	}

	bool _XmlParser::readSample( const XmlHandler::Atts & atts )
	{
		bool ok = true;
		Root::Index id = atts.value( "id" ).toInt( &ok );
		if( !ok || id == 0 )
		{
			d_error.sprintf( "Invalid sample id: %s", atts.value( "id" ).data() );
			return false;
		}
		d_sample = new BioSample();
		d_sample->d_id = id;
		d_sample->d_name = atts.value( "name" ).data();
		d_sample->d_owner = d_pro;
		d_pro->d_samples[ id ] = d_sample;
		return true;
	}

	bool _XmlParser::readSpectrumEnd()
	{
		if( d_specPeer.isNull() ) // Discard
			return true;
		if( !d_specPeer->checkRotation() )
		{
			d_error.sprintf( "Invalid spectrum rotation: %s", d_specPeer->getName() );
			return false;
		}
		/* Weggenommen aufgrund von Wunsch von Fred, 29.5.05
		if( !d_spec->getType()->isCompatible( d_spec ) )
			MessageLog::inst()->warning( "Load Spectrum",
				s_tmp.sprintf( "Incompatible dimension atom types in spectrum %s", 
				d_spec->getName() ) );
		*/
        /* Weggenommen 10.8.14; User soll calcLevels explizit wählen
		if( !d_foundLevels || d_specPeer->getLevels().getNoise() == 0.0 )
			d_specPeer->calcLevels();
            */

		d_specPeer->addObserver( d_pro );
		return true; 
	}

	bool _XmlParser::readSpectrum( const XmlHandler::Atts & atts )
	{
        d_dummySpec = 0;
        d_specPeer = 0;
		bool ok = true;
		d_foundLevels = false;
		if( atts.value( "name" ).isEmpty() )
		{
			d_error = "Spectrum without name";
			return false;
		}
		Root::Index id = atts.value( "id" ).toInt( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid spectrum id: %s", atts.value( "id" ).data() );
			return false;
		}
		if( d_pro->d_specs.find( id ) != d_pro->d_specs.end() )
		{
			d_error.sprintf( "Spectrum id not unique: %s", atts.value( "id" ).data() );
			return false;
		}
		Root::Index sample = 0;
		if( !atts.value( "sample" ).isEmpty() )
		{
			sample = atts.value( "sample" ).toInt( &ok );
			if( !ok )
			{
				d_error.sprintf( "Invalid sample id: %s", atts.value( "sample" ).data() );
				return false;
			}
		}
        SpectrumType* specType = 0;
        {
            const Repository::SpectrumTypeMap& specTypeMap = d_rep->getSpecTypes();
            Root::SymbolString typeName = atts.value( "type" ).data();
            Repository::SpectrumTypeMap::const_iterator typeIter = specTypeMap.find( typeName );
            if( typeName.empty() || typeIter == specTypeMap.end() )
            {
                d_error.sprintf( "Invalid spectrum type: %s", atts.value( "type" ).data() );
                return false;
            }
            specType = (*typeIter).second;
        }
        assert( specType != 0 );
        if( atts.value( "path" ).isEmpty() )
        {
            d_error = "Spectrum with empty file path";
            return false;
        }
		QString filePath = atts.value( "path" );
        QString relPath = atts.value( "relPath" );
		SpecRef<Spectrum> spec;	// Schon hier open, damit vernünftige Fehlermeldung

        QString errorString;
		while( spec.isNull() )
		{
            QFileInfo fileCheck( filePath );
            errorString = "File not found!";
            if( fileCheck.exists() )
            {
                // Datei existiert, versuche sie zu öffnen
                try
                {
                    spec = Spec::Factory::createSpectrum( fileCheck.absFilePath() );
                    if( spec.isNull() )
                        throw Root::Exception( "Unknown spectrum format" );
                    if( spec->getDimCount() != specType->getDimCount() )
                        throw Root::Exception(
                            "Spectrum doesn't meet expected number of dimensions!" );
                }catch( Root::Exception& e )
                {
                    // Man kann die Datei zwar öffnen, aber sie passt nicht
                    spec = 0;
                    errorString = e.what();
                    fileCheck = QFileInfo();
                    if( !d_interactive )
                    {
                        s_tmp.sprintf( "Opening '%s' at '%s' gives the following error: %s",
                                       atts.value( "name" ).data(), filePath.toLatin1().data(), e.what() );
                        Root::MessageLog::inst()->warning( "Repository", s_tmp );
                    }
                }
            }else if( !relPath.isEmpty() )
            {
                // Falls ein relativer Pfad gespeichert wurde, prüfe, ob dieser etwas bringt.
                // Es wurde bereits Dir::setCurrentDir(repositoryPath) aufgerufen.
                fileCheck.setFile( relPath );
                if( fileCheck.exists() )
                    filePath = fileCheck.absoluteFilePath();
                else
                {
                    relPath.clear();
                    continue; // springe hier raus, damit nochmals mit d_filePaths versucht werden kann
                }
            }else if( !d_filePaths.isEmpty() )
            {
                // Datei existiert nicht; versuche die Datei in einem bekannten Pfad zu finden
                foreach( QString path, d_filePaths )
                {
                    QDir dir( path );
                    fileCheck.setFile( dir.absFilePath( fileCheck.fileName() ) );
                    if( fileCheck.exists() )
                    {
                        filePath = fileCheck.filePath();
                        break;
                    }
                }
            }
            if( !fileCheck.exists() )
            {
				if( d_interactive )
				{
                    errorString.sprintf( "Opening spectrum '%s' at '%s' \n"
                        "gives the following error:\n"
						"'%s'\nDo you want to select another spectrum file and try again?",
                        atts.value( "name" ).data(), filePath.toLatin1().data(), errorString.toLatin1().data() );
                    QApplication::setOverrideCursor( Qt::ArrowCursor );
					switch( QMessageBox::information( 0, "Open Repository", errorString,
							  "&Yes", "&Ignore", "&Ignore &all", 0, 2 ) )
					{
					case 0: // Yes
						{
                            QDir dir( AppAgent::getCurrentDir() );
							QString path = QFileDialog::getOpenFileName( 0, "Select Spectrum",
                                                               dir.absFilePath( fileCheck.fileName() ),
                                                               "Spectrum (* *.*)" );
							if( !path.isNull() )
                            {
                                fileCheck.setFile( path );
                                filePath = path;
                                AppAgent::setCurrentDir( fileCheck.dirPath( true ) );
                                d_filePaths.insert( AppAgent::getCurrentDir() );
                            }
						}
                        QApplication::restoreOverrideCursor();
						break;
					case 1: // Discard
                        QApplication::restoreOverrideCursor();
                        d_dummySpec = new DummySpectrum( specType->getDimCount() );
						spec = d_dummySpec;
                        break;
                    case 2: // Discard all
                        QApplication::restoreOverrideCursor();
                        d_dummySpec = new DummySpectrum( specType->getDimCount() );
						spec = d_dummySpec;
                        d_interactive = false;
                        break;
					default: // Abort; gibt es nicht mehr
                        QApplication::restoreOverrideCursor();
                        d_error.sprintf( "Error opening spectrum: %s",
                            filePath.toLatin1().data() );
						return false;
					}
				}else // not interactive
				{              
                    d_dummySpec = new DummySpectrum( specType->getDimCount() );
                    spec = d_dummySpec;
 				} // if( d_interactive )
			}
		} // while( spec.isNull() )

        if( d_dummySpec )
        {
            // Offensichtlich konnte das Spektrum nicht geöffnet werden.
            // Stelle sicher, dass hier dummy wieder den gespeicherten Pfad hat.
            filePath = atts.value( "path" );
            s_tmp.sprintf( "Could not open spectrum '%s' at '%s'; using dummy instead",
                           atts.value( "name" ).data(), filePath.toLatin1().data() );
            Root::MessageLog::inst()->warning( "Repository", s_tmp );
        }else if( filePath != atts.value( "path" ) )
        {
            // Ansonsten konnte Spektrum zwar geöffnet werden, aber unter anderem Pfad.
            s_tmp.sprintf( "Changed path of spectrum '%s' to '%s'", atts.value( "name" ).data(),
                           filePath.toLatin1().data() );
            Root::MessageLog::inst()->warning( "Repository", s_tmp );
            d_pathChanged = true;
        }

		d_specPeer = new SpectrumPeer( spec, filePath, id );
		d_specPeer->d_owner = d_pro;
		d_specPeer->setType( specType ); // Hier Farben nicht prüfen. Dim bereits ok.
		if( sample != 0 )
		{
			Project::SampleMap::const_iterator si = d_pro->d_samples.find( sample );
			if( si == d_pro->d_samples.end() )
			{
				d_error.sprintf( "Unknown sample: %d", sample );
				return false;
			}
			d_specPeer->d_sample = (*si).second;
		}

		d_specPeer->setName( atts.value( "name" ).data() );
		d_pro->d_specs[ id ] = d_specPeer;
		if( d_pro->d_nextSpecId <= id )
			d_pro->d_nextSpecId = id + 1;
		return true;
	}

	bool _XmlParser::readParam( const XmlHandler::Atts & atts )
	{
		bool ok = true;
		Root::SymbolString atom = atts.value( "atom" ).data();
		/* NOTE folgende Prüfung unnötig, da es nicht stört, wenn nicht Atom
			TODO: ev. Warnung
		if( d_res->getType()->d_atoms[ atom ] == 0 )
		{
			d_error.sprintf( "Atom not known in residue: %s", atom.data() );
			return false;
		}
		*/
		DisPar dp;
		dp.d_mean = atts.value( "mean" ).toFloat( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid mean value: %s", atts.value( "mean" ).data() );
			return false;
		}
		dp.d_dev = atts.value( "dev" ).toFloat( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid deviation value: %s", atts.value( "dev" ).data() );
			return false;
		}
		if( d_res->d_params.find( atom ) != d_res->d_params.end() )
		{
			d_error.sprintf( "Parameter for atom already defined: %s", atom.data() );
			return false;
		}
		d_res->d_params[ atom ] = dp;
		return true;
	}

	bool _XmlParser::readResidue( const XmlHandler::Atts & atts )
	{
		bool ok;
		Root::Index id = atts.value( "id" ).toInt( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid residue id: %s", atts.value( "id" ).data() );
			return false;
		}
		if( d_pro->d_seq->d_resi.find( id ) != d_pro->d_seq->d_resi.end() )
		{
			d_error.sprintf( "Residue id not unique within project: %s", atts.value( "id" ).data() );
			return false;
		}
		ResidueType* t = d_rep->findResidueType( atts.value( "type" ).data() );
		if( t == 0 )
		{
			d_error.sprintf( "Unknown residue type: %s", atts.value( "type" ).data() );
			return false;
		}
		d_res = new Residue( id, t );
		d_res->d_owner = d_pro->d_seq;
		d_pro->d_seq->d_resi[ id ] = d_res;
		if( !atts.value( "nr" ).isEmpty() )
		{
			id = atts.value( "nr" ).toInt( &ok );
			if( !ok )
			{
				d_error.sprintf( "Invalid residue number: %s", atts.value( "nr" ).data() );
				return false;
			}
			d_res->d_nr = id;
		}
		if( !atts.value( "chain" ).isEmpty() )
		{
			d_res->d_chain = atts.value( "chain" ).data();
		}
		return true;
	}

	bool _XmlParser::readCoord( const XmlHandler::Atts & atts )
	{
		bool ok;
		Root::Index id = atts.value( "id" ).toInt( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid location id: %s", atts.value( "id" ).data() );
			return false;
		}
		if( d_conf->d_coords.count( id ) != 0 )
		{
			d_error.sprintf( "Location id not unique within conformer: %s", atts.value( "id" ).data() );
			return false;
		}
		Location loc;
		loc.d_pos[DimX] = atts.value( "x" ).toFloat( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid x position: %s", atts.value( "x" ).data() );
			return false;
		}
		loc.d_pos[DimY] = atts.value( "y" ).toFloat( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid y position: %s", atts.value( "y" ).data() );
			return false;
		}
		loc.d_pos[DimZ] = atts.value( "z" ).toFloat( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid z position: %s", atts.value( "z" ).data() );
			return false;
		}
		d_conf->d_coords[ id ] = loc.d_pos;
		return true;
	}

	bool _XmlParser::readConf( const XmlHandler::Atts & atts )
	{
		bool ok;
		Root::Index id = atts.value( "id" ).toInt( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid conformer id: %s", atts.value( "id" ).data() );
			return false;
		}
		if( d_struct->d_confs.count( id ) != 0 )
		{
			d_error.sprintf( "Conformer id not unique within structure: %s", atts.value( "id" ).data() );
			return false;
		}
		Root::Index nr = atts.value( "nr" ).toInt( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid conformer nr: %s", atts.value( "nr" ).data() );
			return false;
		}
		d_conf = new Conformer();
		d_conf->d_owner = d_struct;
		d_conf->d_id = id;
		d_conf->d_nr = nr;
		d_struct->d_confs[id] = d_conf;
		return true;
	}

	bool _XmlParser::readLoc( const XmlHandler::Atts & atts )
	{
		bool ok;
		Root::Index id = atts.value( "id" ).toInt( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid location id: %s", atts.value( "id" ).data() );
			return false;
		}
		if( d_struct->d_locs.count( id ) != 0 )
		{
			d_error.sprintf( "Location id not unique within structure: %s", atts.value( "id" ).data() );
			return false;
		}
		Location loc;
		loc.d_pos[DimX] = atts.value( "x" ).toFloat( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid x position: %s", atts.value( "x" ).data() );
			return false;
		}
		loc.d_pos[DimY] = atts.value( "y" ).toFloat( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid y position: %s", atts.value( "y" ).data() );
			return false;
		}
		loc.d_pos[DimZ] = atts.value( "z" ).toFloat( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid z position: %s", atts.value( "z" ).data() );
			return false;
		}
		loc.d_dev = atts.value( "r" ).toFloat( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid r value: %s", atts.value( "r" ).data() );
			return false;
		}
		d_struct->d_locs[ id ] = loc;
		return true;
	}

	bool _XmlParser::readStruct( const XmlHandler::Atts & atts )
	{
		bool ok;
		Root::Index id = atts.value( "id" ).toInt( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid structure id: %s", atts.value( "id" ).data() );
			return false;
		}
		if( d_pro->d_structs.count( id ) != 0 )
		{
			d_error.sprintf( "Structure id not unique within project: %s", atts.value( "id" ).data() );
			return false;
		}
		QByteArray  name = atts.value( "name" ).data();
        Location::Position o;
		o[DimX] = atts.value( "x" ).toFloat( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid x origin: %s", atts.value( "x" ).data() );
			return false;
		}
		o[DimY] = atts.value( "y" ).toFloat( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid y origin: %s", atts.value( "y" ).data() );
			return false;
		}
		o[DimZ] = atts.value( "z" ).toFloat( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid z origin: %s", atts.value( "z" ).data() );
			return false;
		}
		d_struct = new Structure();
		d_struct->d_owner = d_pro;
		d_struct->d_id = id;
		d_struct->d_name = name;
		d_struct->d_orig = o;
		d_pro->d_structs[id] = d_struct;
		return true;
	}

	bool _XmlParser::readSequenceEnd()
	{
		d_pro->d_seq->reindex();
		return true;
	}

	bool _XmlParser::readSequence( const XmlHandler::Atts & atts )
	{
		return true;
	}

	bool _XmlParser::readTol( const XmlHandler::Atts & atts )
	{
		bool ok = true;
		AtomType a = AtomType::parseLabel( atts.value( "atom" ).data() );
		if( a.isNone() )
		{
			d_error.sprintf( "Invalid atom type: %s", atts.value( "atom" ).data() );
			return false;
		}
		PPM ppm = atts.value( "ppm" ).toFloat( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid width: %s", atts.value( "ppm" ).data() );
			return false;
		}
		d_pro->d_matcher->d_tol[ a ] = ppm;
		return true;
	}

	bool _XmlParser::readWidth( const XmlHandler::Atts & atts )
	{
		bool ok = true;
		AtomType a = AtomType::parseLabel( atts.value( "atom" ).data() );
		if( a.isNone() )
		{
			d_error.sprintf( "Invalid atom type: %s", atts.value( "atom" ).data() );
			return false;
		}
		PPM ppm = atts.value( "ppm" ).toFloat( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid width: %s", atts.value( "ppm" ).data() );
			return false;
		}
		d_pro->d_stripWidth[ a ] = ppm;
		return true;
	}

	bool _XmlParser::readProject( const XmlHandler::Atts & atts )
	{
		Root::SymbolString id = atts.value( "name" ).data();
		if( id.empty() )
		{
			d_error = "Project without name";
			return false;
		}
		if( d_rep->d_projects.count( id ) != 0 )
		{
			d_error.sprintf( "Project name not unique: %s", id.data() );
			return false;
		}
		d_pro = new Project( d_rep, id );
		if( !atts.value( "next" ).isEmpty() )
			d_pro->d_nextSpecId = atts.value( "next" ).toLong();
		if( !atts.value( "swf" ).isEmpty() )
		{
			bool ok;
			float f = atts.value( "swf" ).toFloat( &ok );
			if( !ok || f <= 0.0 )
			{
				d_error.sprintf( "Invalid width factor: %s", atts.value( "swf" ).data() );
				return false;
			}
			d_pro->d_swf = f;
		}
		if( !atts.value( "x" ).isEmpty() && !atts.value( "y" ).isEmpty() &&
			!atts.value( "z" ).isEmpty() )
		{
            Location::Position orig;
			bool ok;
			orig[DimX] = atts.value( "x" ).toFloat( &ok );
			if( !ok )
			{
				d_error.sprintf( "Invalid x origin: %s", atts.value( "x" ).data() );
				return false;
			}
			orig[DimY] = atts.value( "y" ).toFloat( &ok );
			if( !ok )
			{
				d_error.sprintf( "Invalid y origin: %s", atts.value( "y" ).data() );
				return false;
			}
			orig[DimZ] = atts.value( "z" ).toFloat( &ok );
			if( !ok )
			{
				d_error.sprintf( "Invalid z origin: %s", atts.value( "z" ).data() );
				return false;
			}
			d_pro->getSpins()->setOrig( orig );
		}
		d_rep->d_projects[ id ] = d_pro;
		d_pro->addObserver( d_rep );
		return true;
	}

	bool _XmlParser::readLabel( const XmlHandler::Atts & atts )
	{
		SpinLabel l;
		l.setTag( atts.value( "tag" ).data() );
		if( l.getTag().isNull() )
		{
			d_error.sprintf( "Invalid label tag: %s", atts.value( "tag" ).data() );
			return false;
		}
		bool ok;
		l.setOffset( atts.value( "off" ).toShort( &ok ) );
		if( !ok )
		{
			d_error.sprintf( "Invalid offset value: %s", atts.value( "off" ).data() );
			return false;
		}
		int s = atts.value( "final" ).toUShort( &ok );
		if( !ok || s > 2 )
		{
			d_error.sprintf( "Invalid final flag: %s", atts.value( "final" ).data() );
			return false;
		}
		l.setState( (SpinLabel::State)s );
		if( d_st->d_dims.back().d_labels.count( l ) != 0 )
		{
			d_error.sprintf( "Label already defined: %s %s", 
				atts.value( "tag" ).data(), atts.value( "off" ).data() );
			return false;
		}
		d_st->d_dims.back().d_labels.insert( l );
		return true;
	}

	bool _XmlParser::readDim( const XmlHandler::Atts & atts )
	{
		AtomType a = AtomType::parseLabel( atts.value( "atom" ).data() );
		if( a.isNone() )
		{
			d_error.sprintf( "Invalid atom type: %s", atts.value( "atom" ).data() );
			return false;
		}
		XmlHandler::Atts::const_iterator index = atts.find( "width" );
		PPM w = 0;
		if( index != atts.end() )
		{
			bool ok;
			w = index.value().toFloat( &ok );
			if( !ok || w < 0.0 )
			{
				d_error.sprintf( "Invalid width value: %s", index.value().data() );
				return false;
			}
		}
		Root::SymbolString name = atts.value( "name" ).data();
		d_st->d_dims.push_back( SpectrumType::SpecDim() );
		SpectrumType::SpecDim* tmp = &d_st->d_dims.back();
		tmp->d_name = name;
		tmp->d_atom = a;
		tmp->d_width = w;
		return true;
	}

	bool _XmlParser::readStep( const XmlHandler::Atts & atts )
	{
		bool ok;
		AtomType a = AtomType::parseLabel( atts.value( "atom" ).data() );
		if( a.isNone() )
		{
			d_error.sprintf( "Invalid atom type: %s", atts.value( "atom" ).data() );
			return false;
		}
		d_st->d_proc.push_back( SpectrumType::Step() );
		SpectrumType::Step* tmp = &d_st->d_proc.back();
		if( !atts.value( "text" ).isEmpty() )
			tmp->d_text = atts.value( "text" ).data();
		tmp->d_atom = a;
		tmp->d_dim = atts.value( "dim" ).toShort( &ok );
		if( !ok || tmp->d_dim < DimUndefined )
		{
			d_error.sprintf( "Invalid dimension: %s", atts.value( "dim" ).data() );
			return false;
		}
		tmp->d_hops = atts.value( "hops" ).toShort( &ok );
		if( !ok || tmp->d_hops < -1 )
		{
			d_error.sprintf( "Invalid hop count: %s", atts.value( "hops" ).data() );
			return false;
		}
		tmp->d_repeat = atts.value( "rep" ).toUShort( &ok ) > 0;
		if( !ok )
		{
			d_error.sprintf( "Invalid repeat flag: %s", atts.value( "rep" ).data() );
			return false;
		}
		XmlHandler::Atts::const_iterator index = atts.find( "mean" );	// mean, num und dev sind optional
		if( index != atts.end() )
		{
			tmp->d_range.d_mean = index.value().toFloat( &ok );
			if( !ok )
			{
				d_error.sprintf( "Invalid mean value: %s", index.value().data() );
				return false;
			}
			tmp->d_range.d_dev = atts.value( "dev" ).toFloat( &ok );
			if( !ok )
			{
				d_error.sprintf( "Invalid deviation value: %s", atts.value( "dev" ).data() );
				return false;
			}
		}
		return true;
	}

	bool _XmlParser::readScript( const XmlHandler::Atts & atts )
	{
        QByteArray name = atts.value( "name" ).data();
		if( name.isNull() || d_rep->d_scripts.find( name ) != d_rep->d_scripts.end() )
		{
			d_error.sprintf( "Invalid or non-unique name: %s", atts.value( "name" ).data() );
			return false;
		}
		d_script = new Script( name ); // Immer Lua
		d_rep->d_scripts[ name ] = d_script;
		d_script->d_rep = d_rep;
		return true;
	}

	bool _XmlParser::readCode( const XmlHandler::Atts & atts )
	{
		if( atts.value( "lang" ) != "Lua" )
		{
			d_error.sprintf( "Language not supported: %s", atts.value( "lang" ).data() );
			return false;
		}
		return true;
	}

	bool _XmlParser::readCodeEnd()
	{
		if( !d_text.isEmpty() )
			d_script->setCode( d_text );
		return true;
	}

	bool _XmlParser::readSpecTypeEnd()
	{
		if( d_version < 18 )
		{
			MessageLog::inst()->warning( "Load Repository",
				s_tmp.sprintf( "Old file version. Please set step sequence of spectrum type %s", 
				d_st->getName().data() ) );
		}
		if( !d_st->d_proc.empty() )
		{
			try
			{
				d_st->checkValid( d_st->d_proc );
			}catch( Root::Exception& e )
			{
				d_error.sprintf( "Step sequence of %s is not valid: %s", 
					d_st->getName().data(), e.what() );
				return false;
			}
		}
		d_st->update();
		return true;
	}

	bool _XmlParser::readSpecType( const XmlHandler::Atts & atts )
	{
		Root::SymbolString id = atts.value( "name" ).data();
		if( id.empty() )
		{
			d_error = "Spectrum type without name";
			return false;
		}
		if( d_rep->d_types->d_specTypes.count( id ) != 0 )
		{
			d_error.sprintf( "Spectrum type not unique: %s", id.data() );
			return false;
		}
		d_st = new SpectrumType();
		d_st->d_owner = d_rep->d_types;
		d_st->d_name = id;
		d_rep->d_types->d_specTypes[ id ] = d_st;
		return true;
	}

	bool _XmlParser::readLibraryEnd()
	{
		if( !d_gensys.isNull() )
		{
			d_rep->d_types->d_gensys = d_rep->d_types->findResidueType( d_gensys.data() );
			if( d_rep->d_types->d_gensys.isNull() )
				MessageLog::inst()->warning( "Load Repository",
					s_tmp.sprintf( "Default spin system type %s not found", d_gensys.data() ) );
		}
		d_rep->d_types->updateSysTypes();
		return true;
	}

	bool _XmlParser::readLibrary( const XmlHandler::Atts & atts )
	{
		QByteArray str;
		str = atts.value( "nterm" );
		if( !str.isEmpty() && !Atom::validAtomName( str ) )
		{
			d_error.sprintf( "Invalid atom name: %s", atts.value( "nterm" ).data() );
			return false;
		}
		d_rep->d_types->d_nterm = str.data();
		str = atts.value( "cterm" );
		if( !str.isEmpty() && !Atom::validAtomName( str ) )
		{
			d_error.sprintf( "Invalid atom name: %s", atts.value( "cterm" ).data() );
			return false;
		}
		d_rep->d_types->d_cterm = str.data();
		if( d_rep->d_types->d_nterm == d_rep->d_types->d_cterm && 
			!d_rep->d_types->d_nterm.isNull() )
		{
			d_error.sprintf( "Left and right terminal must be different: %s %s", 
				atts.value( "nterm" ).data(), atts.value( "cterm" ).data() );
			return false;
		}
		d_gensys = atts.value( "gensys" ).data();
		return true;
	}

	bool _XmlParser::readBond( const XmlHandler::Atts & atts )
	{
		Root::SymbolString from = atts.value( "from" ).data();
		Root::SymbolString to = atts.value( "to" ).data();
		if( from.empty() || to.empty() || from == to )
		{
			d_error.sprintf( "Invalid bond specification: %s - %s", 
				atts.value( "from" ).data(), atts.value( "to" ).data() );
			return false;
		}
		Atom* a = d_restp->d_atoms[ from ];
		Atom* b = d_restp->d_atoms[ to ];
		if( a == 0 )
		{
			d_error.sprintf( "Cannot find atom %s in residue", from.data() );
			return false;
		}
		if( b == 0 )
		{
			d_error.sprintf( "Cannot find atom %s in residue", to.data() );
			return false;
		}
		// Immer beide Seiten. Redundante Bonds stören nicht.
		a->d_atoms.insert( b );
		b->d_atoms.insert( a );
		return true;
	}

	bool _XmlParser::readAtomScheme( const XmlHandler::Atts & atts )
	{
		bool ok = true;
		AtomType a = AtomType::parseLabel( atts.value( "type" ).data() );
		if( a.isNone() )
		{
			d_error.sprintf( "Invalid atom type: %s", atts.value( "type" ).data() );
			return false;
		}
		Root::Index i = atts.value( "id" ).toInt( &ok );
		if( !ok || i == 0 )
		{
			d_error.sprintf( "Invalid schema id: %s", atts.value( "id" ).data() );
			return false;
		}
		static_cast<Atom*>( d_atom.deref() )->d_types[ i ] = a; // RISK
		return true;
	}

	bool _XmlParser::readAtom( const XmlHandler::Atts & atts )
	{
		bool ok = true;
		AtomType a = AtomType::parseLabel( atts.value( "type" ).data() );
		if( a.isNone() )
		{
			d_error.sprintf( "Invalid atom type: %s", atts.value( "type" ).data() );
			return false;
		}
		QByteArray n = atts.value( "name" ).data();
		if( n.isEmpty() || !Atom::validAtomName( n ) )
		{
			d_error.sprintf( "Atom must have a valid name: %s", atts.value( "name" ).data() );
			return false;

		}
		Root::SymbolString name = n.data();
		if( d_restp->d_atoms.count( name ) != 0 )
		{
			d_error.sprintf( "Atom not unique in residue: %s", name.data() );
			return false;
		}
		Root::Ref<Atom> tmp = new Atom();
		tmp->d_owner = d_restp;
		tmp->d_num = 1; // Default
		XmlHandler::Atts::const_iterator index = atts.find( "mean" );	// mean, num und dev sind optional
		if( index != atts.end() )
		{
			tmp->d_dp.d_mean = index.value().toFloat( &ok );
			if( !ok )
			{
				d_error.sprintf( "Invalid mean value: %s", index.value().data() );
				return false;
			}
			tmp->d_dp.d_dev = atts.value( "dev" ).toFloat( &ok );
			if( !ok )
			{
				d_error.sprintf( "Invalid deviation value: %s", atts.value( "dev" ).data() );
				return false;
			}
		}
		index = atts.find( "num" );
		if( index != atts.end() )
		{
			tmp->d_num = index.value().toUShort( &ok );
			if( !ok )
			{
				d_error.sprintf( "Invalid number of atoms: %s", index.value().data() );
				return false;
			}
		}
		index = atts.find( "x" );
		if( index != atts.end() )
		{
			short i = index.value().toUShort( &ok );
			if( !ok || i < 0 || i > 255 )
			{
				d_error.sprintf( "Invalid x position: %s", index.value().data() );
				return false;
			}
			tmp->d_x = (Root::UInt8)i;
		}
		index = atts.find( "y" );
		if( index != atts.end() )
		{
			short i = index.value().toUShort( &ok );
			if( !ok || i < 0 || i > 255 )
			{
				d_error.sprintf( "Invalid y position: %s", index.value().data() );
				return false;
			}
			tmp->d_y = (Root::UInt8)i;
		}
		index = atts.find( "group" );
		if( index != atts.end() )
		{
			tmp->d_group = d_restp->getGroup( index.value().data() );
			if( tmp->d_group == 0 )
			{
				d_error.sprintf( "Invalid group reference: %s", index.value().data() );
				return false;
			}
			tmp->d_group->d_atoms.insert( tmp );
		}
		tmp->d_name = name;
		tmp->d_type = a;
		d_atom = tmp;
		d_restp->d_atoms[ name ] = tmp;
		return true;
	}

	bool _XmlParser::readGroup( const XmlHandler::Atts & atts )
	{
		bool ok = true;
		QByteArray n = atts.value( "name" ).data();
		if( n.isEmpty() || !Atom::validAtomName( n ) )
		{
			d_error.sprintf( "AtomGroup must have a valid name: %s", atts.value( "name" ).data() );
			return false;
		}
		Root::SymbolString name = n.data();
		if( d_restp->d_groups.count( name ) != 0 )
		{
			d_error.sprintf( "AtomGroup not unique in residue: %s", name.data() );
			return false;
		}
		Root::Ref<AtomGroup> tmp = new AtomGroup();
		tmp->d_owner = d_restp;
		XmlHandler::Atts::const_iterator index = atts.find( "x" );
		if( index != atts.end() )
		{
			short i = index.value().toUShort( &ok );
			if( !ok || i < 0 || i > 255 )
			{
				d_error.sprintf( "Invalid x position: %s", index.value().data() );
				return false;
			}
			tmp->d_x = (Root::UInt8)i;
		}
		index = atts.find( "y" );
		if( index != atts.end() )
		{
			short i = index.value().toUShort( &ok );
			if( !ok || i < 0 || i > 255 )
			{
				d_error.sprintf( "Invalid y position: %s", index.value().data() );
				return false;
			}
			tmp->d_y = (Root::UInt8)i;
		}
		tmp->d_name = name;
		d_atom = tmp;
		d_restp->d_groups[ name ] = tmp;
		return true;
	}

	bool _XmlParser::readLabelScheme( const XmlHandler::Atts & atts )
	{
		bool ok = true;
		Root::Index i = atts.value( "id" ).toInt( &ok );
		if( !ok || i == 0 )
		{
			d_error.sprintf( "Invalid schema id: %s", atts.value( "id" ).data() );
			return false;
		}
		d_scheme = new LabelingScheme( i, atts.value( "name" ).data() );
		d_scheme->d_owner = d_rep->d_types;
		d_rep->d_types->d_labelingSchemes[ i ] = d_scheme;
		return true;
	}

	bool _XmlParser::readStCand( const XmlHandler::Atts & atts )
	{
		d_sst->d_matches.insert( atts.value( "type" ).data() );
		return true;
	}

	bool _XmlParser::readSysType( const XmlHandler::Atts & atts )
	{
		bool ok = true;
		Root::SymbolString name = atts.value( "name" ).data();
		if( name.empty() )
		{
			// TODO: Check for uniqueness
			d_error.sprintf( "Spin system type must have a name: %s", atts.value( "name" ).data() );
			return false;
		}
		Root::Index id = atts.value( "id" ).toUShort( &ok );
		if( !ok || id == 0 || d_rep->d_types->d_sysTypes.count( id ) != 0 )
		{
			d_error.sprintf( "Invalid spin system type id: %s", atts.value( "id" ).data() );
			return false;
		}
		QByteArray tmp;
		Root::SymbolString nterm;
		if( !atts.value( "nterm" ).isEmpty() )
		{
			const char* tmp = atts.value( "nterm" );
			if( !Atom::validAtomName( tmp ) )
			{
				d_error.sprintf( "Invalid atom name: %s", tmp );
				return false;
			}
			nterm = tmp;
		}
		Root::SymbolString cterm;
		if( !atts.value( "cterm" ).isEmpty() )
		{
			tmp = atts.value( "cterm" );
			if( !Atom::validAtomName( tmp ) )
			{
				d_error.sprintf( "Invalid atom name: %s", atts.value( "cterm" ).data() );
				return false;
			}
			cterm = tmp.data();
		}
		Root::SymbolString gensys;
		if( !atts.value( "gensys" ).isEmpty() )
		{
			gensys = atts.value( "gensys" ).data();
		}
		Root::SymbolString cls;
		if( !atts.value( "class" ).isEmpty() )
		{
			cls = atts.value( "class" ).data();
		}
		
		d_sst = new SystemType( id, name );
		d_sst->d_owner = d_rep->d_types;
		d_rep->d_types->d_sysTypes[ id ] = d_sst;
		tmp = atts.value( "mdl" );
		if( !tmp.isEmpty() )
			d_sst->d_resiType = tmp.data();
		d_sst->d_nterm = nterm;
		d_sst->d_cterm = cterm;
		d_sst->d_gensys = gensys;
		d_sst->d_class = cls;
		return true;
	}

	bool _XmlParser::readResidueType( const XmlHandler::Atts & atts )
	{
		Root::SymbolString id = atts.value( "short" ).data();
		if( id.empty() || atts.value( "short" ).find( ' ' ) != -1 )
		{
			// NOTE: keine Spaces im Namen erlaubt.
			d_error = "Invalid residue short name!";
			return false;
		}
		if( d_rep->d_types->d_residueTypes.count( id ) != 0 )
		{
			d_error.sprintf( "Residue type not unique: %s", id.data() );
			return false;
		}
		SystemType* sst = 0;
		if( !atts.value( "systype" ).isEmpty() )
		{
			bool ok;
			Repository::SystemTypeMap::const_iterator p = 
				d_rep->getSystemTypes().find( atts.value( "systype" ).toUShort( &ok ) );
			if( !ok || p == d_rep->getSystemTypes().end() )
			{
				d_error.sprintf( "Invalid spin system type reference: %s", 
					atts.value( "systype" ).data() );
				return false;
			}
			sst = (*p).second;
		}
		d_restp = new ResidueType();
		d_restp->d_owner = d_rep->d_types;
		d_restp->d_name = atts.value( "name" ).data();
		d_restp->d_short = id;
		d_restp->d_letter = atts.value( "letter" ).data();
		d_restp->d_sysType = sst;
		if( d_restp->d_name.empty() || d_restp->d_letter.empty() )
			MessageLog::inst()->warning( "Load Repository",
				s_tmp.sprintf( "name or letter of residue type %s empty", id.data() ) );
		d_rep->d_types->d_residueTypes[ id ] = d_restp;
		return true;
	}

	bool _XmlParser::readColor( const XmlHandler::Atts & atts )
	{
		bool ok;
		int id = atts.value( "code" ).toUShort( &ok );
		if( !ok || id > 255 )
		{
			d_error.sprintf( "Invalid color code: %s", atts.value( "code" ).data() );
			return false;
		}
		QColor clr( atts.value( "name" ).data() );
		if( !clr.isValid() )
		{
			d_error.sprintf( "Invalid color name: %s", atts.value( "name" ).data() );
			return false;
		}
		d_rep->getColors()->setColor( id, ( clr ) );
		return true;
	}

	bool _XmlParser::readOvclr( const XmlHandler::Atts & atts )
	{
		bool screen = true;
		if( atts.value( "t" ) == "p" )
			screen = false;
		QColor pos( atts.value( "pos" ).data() );
		if( !pos.isValid() )
		{
			d_error.sprintf( "Invalid color name: %s", atts.value( "pos" ).data() );
			return false;
		}
		QColor neg( atts.value( "neg" ).data() );
		if( !neg.isValid() )
		{
			d_error.sprintf( "Invalid color name: %s", atts.value( "neg" ).data() );
			return false;
		}
		Repository::SpecColor sc;
		sc.d_pos = ( pos );
		sc.d_neg = ( neg );
		if( screen )
			d_rep->d_screenClr.push_back( sc );
		else
			d_rep->d_printClr.push_back( sc );
		return true;
	}

	bool _XmlParser::readObject( const XmlHandler::Atts & atts )
	{
		bool ok;
		Root::UInt32 id = atts.value( "oid" ).toULong( &ok );
		if( !ok || id < 1 )
		{
			d_error.sprintf( "Invalid object id: %s", atts.value( "oid" ).data() );
			return false;
		}
		if( d_rep->d_base->d_base.find( id ) != d_rep->d_base->d_base.end() )
		{
			d_error.sprintf( "Object with id=%d already seen", id );
			return false;
		}
		d_obj = new Record( id, d_rep->d_base );
		d_rep->d_base->d_base[ id ] = d_obj;
		return true;
	}

	bool _XmlParser::readFieldDef( const XmlHandler::Atts & atts )
	{
		if( d_odef.isNull() )
			return true;

		Root::SymbolString name = atts.value( "name" ).data();
		if( name.isNull() )
		{
			d_error = "Field must have a name";
			return false;
		}
		Root::Any::Type t;
		if( !Root::Any::parseTypeName( atts.value( "type" ), t ) )
		{
			d_error.sprintf( "Invalid data type '%s' of field '%s'", 
				atts.value( "type" ).data(), atts.value( "name" ).data() );
			return false;
		}
		QByteArray  desc;
		if( !atts.value( "desc" ).isEmpty() )
			desc = atts.value( "desc" ).data();
		d_odef->setField( name, t, desc );
		return true;
	}

	bool _XmlParser::readOdef( const XmlHandler::Atts & atts )
	{
		Root::SymbolString cls = atts.value( "class" ).data();
		d_odef = d_rep->findObjectDef( cls );
		if( d_odef.isNull() )
			MessageLog::inst()->warning( "Load Repository",
				s_tmp.sprintf( "Unknown class '%s'. Attributes ignored.", cls.data() ) );
		return true;
	}

	bool _XmlParser::readRepository( const XmlHandler::Atts & atts )
	{
		bool ok;
		d_version = atts.value( "version" ).toUInt( &ok );
		if( !ok )
		{
			d_error.sprintf( "Invalid version: %s", atts.value( "version" ).data() );
			return false;
		}
		d_subversion = 0;
		if( !atts.value( "sub" ).isNull() )
		{
			d_subversion = atts.value( "sub" ).toUInt( &ok );
			if( !ok )
			{
				d_error.sprintf( "Invalid sub-version: %s", atts.value( "sub" ).data() );
				return false;
			}
		}
		if( !atts.value( "author" ).isNull() )
			d_rep->d_author = atts.value( "author" ).data();
		if( !atts.value( "template-by" ).isNull() )
			d_rep->d_tplAuthor = atts.value( "template-by" ).data();
		if( !atts.value( "orig-template-by" ).isNull() )
			d_rep->d_origTplAuthor = atts.value( "orig-template-by" ).data();
		if( !atts.value( "template-name" ).isNull() )
			d_rep->d_tplPath = atts.value( "template-name" ).data();
		return true;
	}

	bool _XmlParser::readField( const XmlHandler::Atts & atts )
	{
		d_field = atts.value( "name" ).data();
		if( d_field.isNull() )
		{
			d_error = "Field must have a name";
			return false;
		}
		if( !Root::Any::parseTypeName( atts.value( "type" ), d_type ) )
		{
			d_error.sprintf( "Invalid data type '%s' of field '%s'", 
				atts.value( "type" ).data(), atts.value( "name" ).data() );
			return false;
		}
		return true;
	}

	bool _XmlParser::startElement( Root::SymbolString qName, const Atts& atts )
	{
		int i = 0;
		d_text = "";
		while( d_slots[ i ].d_symbol != 0 )	// NOTE: gleich teuer wie früher if-Sequenz
		{
			if( qName == d_slots[ i ].d_symbol )
			{
				if( d_slots[ i ].d_state && d_slots[ i ].d_state != d_state.back() )
					i++; 
                else if( ( d_asTemplate && !d_slots[ i ].d_asTemplate ) ||
					// Wenn Rep als Template dient, sollen nur die Template-Handler
					// ausgeführt werden.
					(this->*d_slots[ i ].d_handler)( atts ) )
				{
					if( d_slots[ i ].d_next )
						d_state.push_back( d_slots[ i ].d_next );
					d_current.push_back( &d_slots[ i ] );
					return true;
				}else
					return false;
			}else
				i++;
		}
		return false;
	}

	bool _XmlParser::endElement(Root::SymbolString qName)
	{
		Slot* slot = d_current.back();

		if( qName != slot->d_symbol ) // Es muss in jedem Falle Symmetrie herrschen.
			return false;

		if( slot->d_post )
		{
			if( d_asTemplate && !slot->d_asTemplate )
				// Wenn Rep als Template dient, sollen nur die Template-Handler
				// ausgeführt werden.
				; // NOP
			else if( !(this->*slot->d_post)() )
				return false;
		}

		if( slot->d_next )	
			// Es wurde in startElement in den Zustand d_next gewechselt. Hier wieder zurück.
			d_state.pop_back();

		// else
		d_current.pop_back();
		return true; // Success
	}

	bool _XmlParser::characters( const QByteArray& str )
	{
		d_text += str;
		return true;
	}

}
///////////////////////////////////////////////////////////////////////////////

#ifdef __obsolete
static void writeScript_1_obsolete( QTextStream& out, const QString& script )
{
	const QChar* str = script.constData();
	int i = 0, j = 0;
	out << "<![CDATA" << endl;
	while( !str[ j ].isNull() )
	{
		if( j >= 2 && str[j] == '>' && str[j-1]==']' && str[j-2]==']' ) 
		{
			out << QString( str + i, j - i ); // Schreibe alles bis ]]
			out << " "; // das ergibt dann "]] >" statt "]]>"
			i = j;
		}
		++j;
	}
	if( j > i )
		out << QString( str + i, j - i );
	out << endl;
	out << "]]>" << endl;
}

static void writeScript_2_obsolete( QTextStream& out, const QString& script )
{
	const QChar* str = script.constData();
	int i = 0, j = 0;
	while( !str[ j ].isNull() )
	{
		if( str[j] == '>' )
		{
			out << QString( str + i, j - i ); // Schreibe alles bis vor Zeichen
			out << "&gt;";
			i = j + 1;
		}else if( str[j] == '<' )
		{
			out << QString( str + i, j - i ); // Schreibe alles bis vor Zeichen
			out << "&lt;";
			i = j + 1;
		}else if( str[j] == '&' )
		{
			out << QString( str + i, j - i ); // Schreibe alles bis vor Zeichen
			out << "&amp;";
			i = j + 1;
		}
		++j;
	}
	if( j > i )
		out << QString( str + i, j - i );
	out << endl;
}
#endif

static void writeScript( QTextStream& out, const QString& script )
{
	out.flush();
	QIODevice* dev = out.device();
	QByteArray tmp = script.toLatin1();
	const char* str = tmp.constData();
	int i = 0, j = 0;
	while( str[ j ] != 0 )
	{
		if( str[j] == '>' )
		{
			dev->write( str + i, j - i ); // Schreibe alles bis vor Zeichen
			dev->write( "&gt;" );
			i = j + 1;
		}else if( str[j] == '<' )
		{
			dev->write( str + i, j - i ); // Schreibe alles bis vor Zeichen
			dev->write( "&lt;" );
			i = j + 1;
		}else if( str[j] == '&' )
		{
			dev->write( str + i, j - i ); // Schreibe alles bis vor Zeichen
			dev->write( "&amp;" );
			i = j + 1;
		}
		++j;
	}
	if( j > i )
		dev->write( str + i, j - i );
}

static QString encode( const char* str )
{
	if( str == 0 )
		str = "";
	QString tmp( str );
    uint len = tmp.length();
    uint i = 0;
    while ( i < len ) 
	{
		if ( tmp[(int)i] == '<' ) 
		{
			tmp.replace( i, 1, "&lt;" );
			len += 3;
			i += 4;
		} else if ( tmp[(int)i] == '"' ) 
		{
			tmp.replace( i, 1, "&quot;" );
			len += 5;
			i += 6;
		} else if ( tmp[(int)i] == '\'' ) 
		{
			tmp.replace( i, 1, "&apos;" );
			len += 5;
			i += 6;
		} else if ( tmp[(int)i] == '&' ) 
		{
			tmp.replace( i, 1, "&amp;" );
			len += 4;
			i += 5;
		} else if ( tmp[(int)i] == '>' && i>=2 && tmp[(int)i-1]==']' && tmp[(int)i-2]==']' ) 
		{
			tmp.replace( i, 1, "&gt;" );
			len += 3;
			i += 4;
		} else 
		{
			++i;
		}
    }
	
    return tmp;
}

Serializer::Serializer(Repository* rep):
	d_rep( rep )
{
	assert( rep );
}

Serializer::~Serializer()
{

}

/* TEST
#include <Xml/XmlHandler.h>
#include <fstream>

class _Handler : public Xml::XmlHandler
{
public:
	int level;
	QByteArray d_text;
	std::ofstream d_out;
	_Handler():level(0),d_out("out.txt"){}
	bool characters( const char* str, int len )
	{
		d_text += QByteArray( str, len );
		d_out.write( str, len ); 
		return true;
	}
	bool endElement( Root::SymbolString name )
	{
		if( !d_text.isEmpty() )
			; // qDebug( d_text.data() );
		d_text = "";
		QByteArray white( level * 3, ' ' );
		// qDebug( "%s</%s>", white.data(), name.data() );
		d_out << white.data() << "</" << name.data() << ">" << std::endl;
		level--;
		return true;
	}
	bool startElement( Root::SymbolString name, const Atts& atts )
	{
		d_text = "";
		level++;
		QByteArray white( level * 3, ' ' );
		// qDebug( "%s<%s>", white.data(), name.data() );
		Atts::const_iterator i;
		for( i = atts.begin(); i != atts.end(); ++i )
			; // qDebug( "%s %s='%s'", white.data(), i.key().data(), i.value() );
		d_out << white.data() << "<" << name.data() << " ";
		for( i = atts.begin(); i != atts.end(); ++i )
			d_out << i.key().data() << "='" << i.value().data() << "' ";
		d_out << ">" << std::endl;
		return true;
	}

};
*/

void Serializer::deserialize(const char *path, bool asTemplate )
{
	// NOTE: wir gehen davon aus, dass d_rep ein neues, leeres Repository ist.

	// Setze das aktuelle Verzeichnis auf dasjenige der Repository-Datei, damit
	// alle Pfade relativ zu dieser interpretiert werden.
	QFileInfo info( path );
	if( !info.exists() )
		throw Root::Exception( "Cannot find repository file ", path );

	QDir::setCurrent( info.dirPath( true ) );
	_XmlParser handler( d_rep, asTemplate );
    bool ok = handler.parse( path );
    if( !ok ) 
	{
		qDebug( "Serializer::deserialize: %s", handler.getError().toLatin1().data() );
        throw Root::Exception( handler.getError().toLatin1().data() );
	}else
        d_rep->d_dirty = handler.pathChanged();
}

void Serializer::serialize(const char *path, const char* creator)
{

	QString str;

    QFile data( path );
    if( !data.open(QFile::WriteOnly | QFile::Truncate) )
		throw Root::Exception( "Cannot open file for writing" );
    
	QTextStream out(&data);
	out << "<?xml version='1.0' encoding='ISO-8859-1'?>" << endl;	// RISK

	s_tmp = QString( "<%1 version='%2' sub='%3' creator='%4' modified='%5' author='%6' "
		"\ntemplate-by='%7' orig-template-by='%8' template-name='%9'>" ).
		arg( REPOSITORY ).arg( s_version ).arg( s_subversion ).arg( creator ). 
		arg( QDateTime::currentDateTime().toString() ). 
		arg( encode( d_rep->getAuthor() ) ). 
		arg( encode( d_rep->getTplAuthor() ) ).
		arg( encode( d_rep->getOrigTplAuthor() ) ).
		arg( encode( d_rep->getTplName() ) );
	out << s_tmp << endl;
	Repository::ObjectDefs::const_iterator p5;
	for( p5 = d_rep->getClasses().begin(); p5 != d_rep->getClasses().end(); ++p5 )
		writeObjectDef( out, (*p5).second );


	s_tmp = QString( "<%1 nterm='%2' cterm='%3' gensys='%4'>" ). 
		arg( LIBRARY ).arg( encode( d_rep->getTypes()->getNTerm( 0 ) ) ). 
		arg( encode( d_rep->getTypes()->getCTerm( 0 ) ) ).
		arg( ( d_rep->getTypes()->getGenFromType( 0 ) )?
			encode( d_rep->getTypes()->getGenFromType( 0 )->getId() ):"" );
	out << s_tmp << endl;
	Repository::SpectrumTypeMap::const_iterator p1;
	for( p1 = d_rep->getSpecTypes().begin(); p1 != d_rep->getSpecTypes().end(); ++p1 )
		writeSpecType( out, (*p1).second );
	Repository::SystemTypeMap::const_iterator p4;
	for( p4 = d_rep->getSystemTypes().begin(); p4 != d_rep->getSystemTypes().end(); ++p4 )
		writeSpinSystemType( out, (*p4).second );
	TypeBase::LabelingSchemeMap::const_iterator p2b;
	for( p2b = d_rep->getTypes()->getLabelingSchemes().begin(); 
		p2b != d_rep->getTypes()->getLabelingSchemes().end(); ++p2b )
		writeLabelingScheme( out, (*p2b).second );
	Repository::ResidueTypeMap::const_iterator p2;
	for( p2 = d_rep->getResidueTypes().begin(); p2 != d_rep->getResidueTypes().end(); ++p2 )
		writeResiType( out, (*p2).second );
	out << "</" << LIBRARY << ">" << endl;

	Repository::ProjectMap::const_iterator p3;
	for( p3 = d_rep->getProjects().begin(); p3 != d_rep->getProjects().end(); ++p3 )
		writeProject( out, (*p3).second );
	Repository::ScriptMap::const_iterator p6;
	for( p6 = d_rep->getScripts().begin(); p6 != d_rep->getScripts().end(); ++p6 )
	{
		s_tmp = QString( "<%1 name='%2'>" ).arg( SCRIPT ).arg( encode( (*p6).first ) );
		out << s_tmp << endl;
		s_tmp.sprintf( "<%s lang='%s'>", CODE, Script::s_lang[ (*p6).second->getLang() ] );
		out << s_tmp;
		writeScript( out, (*p6).second->getCode() );
		out << "</" << CODE << ">" << endl;
		writeObject( out, (*p6).second );
		out << "</" << SCRIPT << ">" << endl;
	}
	PeakColorList::Colors::const_iterator p7;
	PeakColorList* pcl = d_rep->getColors();
	for( p7 = pcl->getColors().begin(); p7 != pcl->getColors().end(); ++p7 )
	{
		s_tmp.sprintf( "<%s code='%d' name='%s'/>", COLOR, int( (*p7).first ), 
			(*p7).second.name().toLatin1().data() );
		out << s_tmp << endl;
	}
	const Repository::SlotColors& sclr = d_rep->getScreenClr();
	int i;
	for( i = 0; i < sclr.size(); i++ )
	{
		s_tmp.sprintf( "<%s t='s' pos='%s' neg='%s'/>", OVCLR, 
			sclr[ i ].d_pos.name().toLatin1().data(),
			sclr[ i ].d_neg.name().toLatin1().data() );
		out << s_tmp << endl;
	}
	const Repository::SlotColors& pclr = d_rep->getPrintClr();
	for( i = 0; i < pclr.size(); i++ )
	{
		s_tmp.sprintf( "<%s t='p' pos='%s' neg='%s'/>", OVCLR, 
			pclr[ i ].d_pos.name().toLatin1().data(),
			pclr[ i ].d_neg.name().toLatin1().data() );
		out << s_tmp << endl;
	}
	ObjectBase::Base::const_iterator p8;
	out << "<" << DATABASE << ">" << endl;
	for( p8 = d_rep->getBase()->getBase().begin(); 
		p8 != d_rep->getBase()->getBase().end(); ++p8 )
	{
		s_tmp.sprintf( "<%s oid='%d'", OBJECT, (*p8).first );
		out << s_tmp;
		if( (*p8).second->getFieldValues().empty() )
			out << "/>" << endl;
		else
		{
			out << ">" << endl;
			writeObject( out, (*p8).second );
			out << "</" << OBJECT << ">" << endl;
		}
	}
	out << "</" << DATABASE << ">" << endl;
	writeObject( out, d_rep );
	out << "</" << REPOSITORY << ">" << endl;
}

void Serializer::writeObjectDef(QTextStream & out, ObjectDef * f)
{
	if( f->getAttributes().empty() )
		return;
	s_tmp = QString( "<%1 class='%2'>" ).arg( ODEF ).arg( encode( f->getClass() ) );
	out << s_tmp << endl;
	ObjectDef::Attributes::const_iterator p;
	for( p = f->getAttributes().begin(); p != f->getAttributes().end(); ++p )
	{
		s_tmp = QString( "<%1 name='%2' type='%3' desc='%4'/>" ).arg( FIELD ). 
			arg( encode( (*p).first ) ).arg( Root::Any::getTypeName( (*p).second.d_type ) ).
			arg( encode( (*p).second.d_description.data() ) );
		out << s_tmp << endl;
		// RISK: ev. <!CDATA[
	}
	out << "</" << ODEF << ">" << endl;
}

void Serializer::writeSpinSystemType(QTextStream & out, SystemType * t)
{
	s_tmp.sprintf( "<%s id='%d' name='%s' mdl='%s' nterm='%s' cterm='%s' gensys='%s' class='%s'", 
		SYSTYPE, t->getId(), encode( t->getName() ).toLatin1().constData(), 
		encode( t->getResiType() ).toLatin1().constData(),
		encode( t->getNTerm() ).toLatin1().constData(),
		encode( t->getCTerm() ).toLatin1().constData(),
		encode( t->getGenSysSym() ).toLatin1().constData(),
		encode( t->getClass() ).toLatin1().constData() );
	out << s_tmp;
	if( t->getMatches().empty() && t->getFieldValues().empty() )
	{
		out << "/>" << endl;
	}else
	{
		out << ">" << endl;
		SystemType::ResiTypeMatches::const_iterator p;
		for( p = t->getMatches().begin(); p != t->getMatches().end(); ++p )
			out << "<" << CAND << " type='" << encode( (*p) ) << "'/>" << endl;
		writeObject( out, t );
		out << "</" << SYSTYPE << ">" << endl;
	}
}

void Serializer::writeSpecType(QTextStream & out, SpectrumType * t)
{
	s_tmp.sprintf( "<%s name='%s'>", SPECTRUM_TYPE, encode( t->getName() ).toLatin1().constData() );
	out << s_tmp << endl;
	Dimension d;
	for( d = 0; d < t->getDimCount(); d++ )
	{
		//  order='%d' hops='%d' rep='%d'
		// t->getOrder( d ), t->getHops( d ), t->isRepeat( d )
		s_tmp.sprintf( "<%s name='%s' atom='%s' width='%f'", 
			DIMENSION, encode( t->getName( d ) ).toLatin1().constData(), 
			t->getColor( d ).getIsoLabel(), t->getPeakWidth( d ) );
		out << s_tmp;
		const SpinLabelSet& s = t->getLabels( d );
		if( !s.empty() )
		{
			out << ">" << endl;
			SpinLabelSet::const_iterator p;
			for( p = s.begin(); p != s.end(); ++p )
			{
				s_tmp.sprintf( "<%s tag='%s' off='%d' final='%d'/>",
					LABEL, encode( (*p).getTag() ).toLatin1().constData(), 
					(*p).getOffset(), (*p).getState() );
				out << s_tmp << endl;
			}
			out << "</" << DIMENSION << ">" << endl;
		}else
			out << "/>" << endl;
	}
	const SpectrumType::Procedure& proc = t->getProc();
	for( d = 0; d < proc.size(); d++ )
	{
		s_tmp.sprintf( "<%s text='%s' atom='%s' dim='%d' hops='%d' rep='%d'", 
			STEP, encode( proc[d].d_text.data() ).toLatin1().constData(), proc[d].d_atom.getIsoLabel(),
			proc[d].d_dim, proc[d].d_hops, proc[d].d_repeat );
		out << s_tmp;
		if( proc[d].d_range.isValid() )
		{
			s_tmp.sprintf( " mean='%f' dev='%f'/>", proc[d].d_range.d_mean,
				proc[d].d_range.d_dev );
			out << s_tmp << endl;
		}else
			out << "/>" << endl;
	}
	writeObject( out, t );
	out << "</" << SPECTRUM_TYPE << ">" << endl;
}

void Serializer::writeLabelingScheme( QTextStream & out, LabelingScheme * t)
{
	s_tmp.sprintf( "<%s id='%d' name='%s' ", SCHEME, t->getId(),
		encode( t->getName() ).toLatin1().constData() );
	out << s_tmp;
	if( !t->getFieldValues().empty() )
	{
		out << ">" << endl;
		writeObject( out, t );
		out << "</" << SCHEME << ">" << endl;
	}else
		out << "/>" << endl;
}

void Serializer::writeResiType(QTextStream & out, ResidueType * t)
{
	s_tmp.sprintf( "<%s name='%s' short='%s' letter='%s' ", RESIDUE_TYPE,
		encode( t->getName() ).toLatin1().constData(), encode( t->getShort() ).toLatin1().constData(), 
		encode( t->getLetter() ).toLatin1().constData() );
	out << s_tmp;
	if( t->getSysType() )
		out << "systype='" << t->getSysType()->getId() << "' ";
	out << ">" << endl;
	ResidueType::GroupMap::const_iterator p3;
	// Schreibe zuerst alle Gruppen
	for( p3 = t->getGroups().begin(); p3 != t->getGroups().end(); ++p3 )
	{
		AtomGroup* g = (*p3).second;
			s_tmp.sprintf( 
			"<%s name='%s' x='%d' y='%d'", 
				GROUP, encode( g->getName() ).toLatin1().constData(), g->getX(), g->getY() );
		out << s_tmp;
		if( !g->getFieldValues().empty() )
		{
			out << ">" << endl;
			writeObject( out, g );
			out << "</" << GROUP << ">" << endl;
		}else
			out << "/>" << endl;
	}
	ResidueType::AtomMap::const_iterator p;
	DisPar dp;
	// Schreibe dann alle Atome...
	for( p = t->getAtoms().begin(); p != t->getAtoms().end(); ++p )
	{
		Atom* a = (*p).second;
		dp = a->getDisPar();
		if( dp.isValid() )
			s_tmp.sprintf( 
			"<%s name='%s' type='%s' num='%d' mean='%.3f' dev='%.3f' x='%d' y='%d' ", 
				_ATOM, encode( a->getName() ).toLatin1().constData(), a->getType().getIsoLabel(),
				a->getNum(), dp.d_mean, dp.d_dev, a->getX(), a->getY() );
		else
			s_tmp.sprintf( "<%s name='%s' type='%s' num='%d' x='%d' y='%d' ", 
				_ATOM, encode( a->getName() ).toLatin1().constData(), a->getType().getIsoLabel(), 
				a->getNum(), a->getX(), a->getY() );
		out << s_tmp;
		if( a->getGroup() )
			out << "group='" << a->getGroup()->getName().data() << "'";
		const Atom::Types& at = a->getTypes();
		if( at.empty() && a->getFieldValues().empty() )
			out << "/>" << endl;
		else
		{
			out << ">" << endl;
			Atom::Types::const_iterator i;
			for( i = at.begin(); i != at.end(); ++i )
			{
				s_tmp.sprintf( "<%s id='%d' type='%s'/>", 
					SCHEME, (*i).first, (*i).second.getIsoLabel() );
				out << s_tmp << endl;
			}
			writeObject( out, a );
			out << "</" << _ATOM << ">" << endl;
		}
	}
	// Schreibe dann alle Bonds zwischen den Atomen...
	Atom::Neighbours::const_iterator p2;
	for( p = t->getAtoms().begin(); p != t->getAtoms().end(); ++p )
	{
		Atom* a = (*p).second;
		for( p2 = a->getNeighbours().begin(); p2 != a->getNeighbours().end(); ++p2 )
		{
			s_tmp.sprintf( "<%s from='%s' to='%s'/>", BOND, 
				encode( a->getName() ).toLatin1().constData(), 
				encode( (*p2)->getName() ).toLatin1().constData() );
			out << s_tmp << endl;
		}
	}
	writeObject( out, t );
	out << "</" << RESIDUE_TYPE << ">" << endl;
}

void Serializer::writeProject(QTextStream & out, Project * pro)
{
    Location::Position orig = pro->getSpins()->getOrig();
	s_tmp.sprintf( "<%s name='%s' next='%d' swf='%f' x='%f' y='%f' z='%f'>", PROJECT, 
		encode( pro->getName() ).toLatin1().constData(), pro->getNextSpecId(),
		pro->getWidthFactor(), orig[DimX], orig[DimY], orig[DimZ] );
	out << s_tmp << endl;

	const Project::StripWidth& sw = pro->getStripWidth();
	Project::StripWidth::const_iterator p6;
	for( p6 = sw.begin(); p6 != sw.end(); ++p6 )
	{
		s_tmp.sprintf( "<%s atom='%s' ppm='%f'/>", WIDTH, 
			(*p6).first.getIsoLabel(), (*p6).second );
		out << s_tmp << endl;
	}

	const SpinMatcher::Tolerance& tol = pro->getMatcher()->getTol();
	SpinMatcher::Tolerance::const_iterator p7;
	for( p7 = tol.begin(); p7 != tol.end(); ++p7 )
	{
		s_tmp.sprintf( "<%s atom='%s' ppm='%f'/>", TOL, 
			(*p7).first.getIsoLabel(), (*p7).second );
		out << s_tmp << endl;
	}

	s_tmp.sprintf( "<%s >", SEQUENCE );
	out << s_tmp << endl;
	const Sequence::ResidueMap& seq = pro->getSequence()->getResi();
	Sequence::ResidueMap::const_iterator p;
	for( p = seq.begin(); p != seq.end(); ++p )
		writeResidue( out, (*p).second );
	out << "</" << SEQUENCE << ">" << endl;

	const Project::SampleMap& samp = pro->getSamples();
	Project::SampleMap::const_iterator p2b;
	for( p2b = samp.begin(); p2b != samp.end(); ++p2b )
		writeSample( out, (*p2b).second );

	const Project::SpectrumMap& spec = pro->getSpectra();
	Project::SpectrumMap::const_iterator p2;
	for( p2 = spec.begin(); p2 != spec.end(); ++p2 )
		writeSpectrum( out, (*p2).second );

	const Project::StructureMap& structs = pro->getStructs();
	Project::StructureMap::const_iterator p9;
	for( p9 = structs.begin(); p9 != structs.end(); ++p9 )
		writeStruct( out, (*p9).second );


	out << "<" << SPINBASE << ">" << endl;
	const SpinBase::SpinSystemMap& ss = pro->getSpins()->getSystems();
	SpinBase::SpinSystemMap::const_iterator p3;
	for( p3 = ss.begin(); p3 != ss.end(); ++p3 )
		writeSpinSystem( out, (*p3).second );
	// Zuerst alle SS serialisieren und erst dann die Fragmente. Ansonsten
	// sind beim deserialisieren SS ev. unbekannt.
	for( p3 = ss.begin(); p3 != ss.end(); ++p3 )
		writeLink( out, (*p3).second );

	const SpinBase::SpinMap& sp = pro->getSpins()->getSpins();
	SpinBase::SpinMap::const_iterator p4;
	for( p4 = sp.begin(); p4 != sp.end(); ++p4 )
		writeSpin( out, (*p4).second );

	const SpinBase::SpinLinkSet& ls = pro->getSpins()->getLinks();
	SpinBase::SpinLinkSet::const_iterator p8;
	for( p8 = ls.begin(); p8 != ls.end(); ++p8 )
		writePair( out, pro->getSpins(), (*p8) );

	out << "</" << SPINBASE << ">" << endl;

	const Project::PeakListMap& pl = pro->getPeakLists();
	Project::PeakListMap::const_iterator p5;
	for( p5 = pl.begin(); p5 != pl.end(); ++p5 )
		writePeakList( out, (*p5).second );

	writeObject( out, pro );
	out << "</" << PROJECT << ">" << endl;
}

void Serializer::writeStruct(QTextStream & out, Structure * res)
{
    Location::Position o = res->getOrig();
	s_tmp.sprintf( "<%s id='%d' name='%s' x='%f' y='%f' z='%f'>", STRUCT, res->getId(), 
		encode( res->getName() ).toLatin1().constData(), o[DimX], o[DimY], o[DimZ] );
	out << s_tmp << endl;
	Structure::Locations::const_iterator p;
	for( p = res->getLocs().begin(); p != res->getLocs().end(); ++p )
	{
		s_tmp.sprintf( "<%s id='%d' x='%f' y='%f' z='%f' r='%f'/>", COORD, (*p).first,
			(*p).second.d_pos[DimX], (*p).second.d_pos[DimY], 
			(*p).second.d_pos[DimZ], (*p).second.d_dev );
		out << s_tmp << endl;
	}
	Structure::Conformers::const_iterator q;
	Conformer* conf;
	for( q = res->getConfs().begin(); q != res->getConfs().end(); ++q )
	{
		conf = (*q).second;
		s_tmp.sprintf( "<%s id='%d' nr='%d'>", CONF, conf->getId(), conf->getNr() );
		out << s_tmp << endl;
		Conformer::Coords::const_iterator i;
		for( i = conf->getCoords().begin(); i != conf->getCoords().end(); ++i )
		{
			s_tmp.sprintf( "<%s id='%d' x='%f' y='%f' z='%f'/>", COORD, (*i).first,
				(*i).second[DimX], (*i).second[DimY], (*i).second[DimZ] );
			out << s_tmp << endl;
		}
		writeObject( out, conf );
		out << "</" << CONF << ">" << endl;
	}
	writeObject( out, res );
	out << "</" << STRUCT << ">" << endl;
}

void Serializer::writeResidue(QTextStream & out, Residue * res)
{
	const Residue::Parameters& par = res->getParams();
	if( par.empty() )
	{
		s_tmp.sprintf( "<%s id='%d' type='%s' nr='%d' chain='%s'/>", RESIDUE, res->getId(),
			encode( res->getType()->getId() ).toLatin1().constData(), res->getNr(),
			encode( res->getChain() ).toLatin1().constData() );
		out << s_tmp << endl;
		return;
	}// else
	s_tmp.sprintf( "<%s id='%d' type='%s' nr='%d' chain='%s'>", RESIDUE, res->getId(),
		encode( res->getType()->getId() ).toLatin1().constData(), res->getNr(),
		encode( res->getChain() ).toLatin1().constData() );
	out << s_tmp << endl;
	Residue::Parameters::const_iterator p;
	for( p = par.begin(); p != par.end(); ++p )
	{
		if( (*p).second.isValid() )
			s_tmp.sprintf( "<%s atom='%s' mean='%.3f' dev='%.3f'/>", 
				PARAM, encode( (*p).first ).toLatin1().constData(), 
				(*p).second.d_mean, (*p).second.d_dev );
		out << s_tmp << endl;
	}
	writeObject( out, res );
	out << "</" << RESIDUE << ">" << endl;
}

void Serializer::writeSample(QTextStream & out, BioSample * s)
{
	s_tmp.sprintf( "<%s id='%d' name='%s'>", SAMPLE, 
		s->getId(), encode( s->getName() ).toLatin1().constData() );
	out << s_tmp << endl;
	BioSample::Ranges::const_iterator i;
	for( i = s->getRanges().begin(); i != s->getRanges().end(); ++i )
	{
		s_tmp.sprintf( "<%s start='%d' end='%d' scheme='%d'/>", RANGE, 
			(*i).d_start, (*i).d_end, (*i).d_schema );
		out << s_tmp << endl;
	}
	writeObject( out, s );
	out << "</" << SAMPLE << ">" << endl;
}

void Serializer::writeSpectrum(QTextStream & out, SpectrumPeer * spec)
{
	// SpecType, Unique name und FilePath (relativ)
	assert( spec->getType() );

    const QByteArray absPath = QFileInfo( spec->getFilePath() ).absoluteFilePath().toAscii();
    const QByteArray relPath = QDir( QFileInfo(d_rep->getFilePath()).path() ).relativeFilePath( absPath ).toAscii();

    Root::Index sampId = ( spec->getSample() )?spec->getSample()->getId():0;
	s_tmp.sprintf( "<%s type='%s' id='%d' name='%s' path='%s' sample='%d' relPath='%s'>", SPECTRUM,
                   encode( spec->getType()->getName() ).toLatin1().constData(), spec->getId(),
                   encode( spec->getName() ).toLatin1().constData(),
                   encode( absPath ).toLatin1().constData(), sampId,
                   encode( relPath ).toLatin1().constData() );
    out << s_tmp << endl;
	const Spectrum::Levels& lev = spec->getRawLevels();
	s_tmp.sprintf( "<%s pmax='%f' pnoise='%f' nmax='%f' nnoise='%f' "
		"thres='%f' factor='%f'/>", LEVEL,
		lev.d_pMax, lev.d_pNoise, lev.d_nMax, lev.d_nNoise, 
		spec->getThreshold(), spec->getFactor() );
	out << s_tmp << endl;

	for( Dimension d = 0; d < spec->getDimCount(); d++ )
	{
		// 1.10.05 Da sonst Folding nicht geschrieben wird.
		// if( spec->getOffset( d ) != 0.0 || spec->getWidths()[ d ] != 0.0 )
		{
            // NOTE: Range und Color werden redundant gespeichert für DummySpec im Falle des Fehlens der Datei
            s_tmp.sprintf( "<%s dim='%d' off='%f' width='%f' fold='%s' atom='%s' ppm1='%f' ppm2='%f'  />",
                           CALIBRATE, d, spec->getOffset( d ), spec->getWidths()[ d ],
				Scale::getFoldingCode( spec->getSpectrum()->getScale( d ).getFolding() ),
                spec->getSpectrum()->getScale( d ).getColor().getIsoLabel(),
                spec->getSpectrum()->getScale( d ).getRange().first,
                spec->getSpectrum()->getScale( d ).getRange().second );
			out << s_tmp.toAscii().data() << endl;
		}
		if( spec->getRotation( d ) != d )
		{
			// TODO: nur speichern, wenn spec tatsächlich vorhanden
			s_tmp.sprintf( "<%s view='%d' spec='%d'/>", ROTATE, d, 
				spec->getRotation( d ) );
			out << s_tmp.toAscii().data() << endl;
		}
	}
	writeObject( out, spec );
	out << "</" << SPECTRUM << ">" << endl;
}

void Serializer::writeSpinSystem(QTextStream & out, SpinSystem * ss)
{
	s_tmp.sprintf( "<%s id='%d' ", SPINSYS, ss->getId() );
	out << s_tmp;
	if( ss->getAssig() )
		out << "ass='" << ss->getAssig()->getId() << "' ";
	if( ss->getSysType() )
		out << "systype='" << ss->getSysType()->getId() << "' ";
	if( ss->getCands().empty() && ss->getFieldValues().empty() )
		out << "/>" << endl;
	else
	{
		out << ">" << endl;
		SpinSystem::Candidates::const_iterator p;
		for( p = ss->getCands().begin(); p != ss->getCands().end(); ++p )
			out << "<" << CAND << " type='" << encode((*p)->getId() ) << "'/>" << endl;
		writeObject( out, ss );
		out << "</" << SPINSYS << ">" << endl;
	}
}

void Serializer::writeSpin(QTextStream & out, Spin * sp)
{
	if( sp == 0 )
		return; // RISK: Workaround für Problem 0624

	const SpinLabel& lbl = sp->getLabel();
	if( lbl.isNull() )
		s_tmp.sprintf( "<%s id='%d' atom='%s' home='%d' ", 
			SPIN, sp->getId(), sp->getAtom().getIsoLabel(), sp->getHome() );
	else
		s_tmp.sprintf( "<%s id='%d' atom='%s' home='%d' tag='%s' off='%d' final='%d' ", 
			SPIN, sp->getId(), sp->getAtom().getIsoLabel(), sp->getHome(),
			encode( lbl.getTag() ).toLatin1().constData(), lbl.getOffset(), lbl.getState() );
	out << s_tmp;

	if( sp->getSystem() )
		out << "sys='" << sp->getSystem()->getId() << "' ";
	else
		out << "sys='' ";

	const Location* loc = sp->getLoc();
	if( loc )
	{
		s_tmp.sprintf( "x='%f' y='%f' z='%f' r='%f' ",
			loc->d_pos[DimX], loc->d_pos[DimY], loc->d_pos[DimZ], loc->d_dev );
		out << s_tmp;
	}

	const Spin::Shifts& ali = sp->getShifts();
	if( ali.empty() )
	{
		out << "/>" << endl;
		return;
	}// else
	out << ">" << endl;

	Spin::Shifts::const_iterator p;
	for( p = ali.begin(); p != ali.end(); ++p )
	{
		s_tmp.sprintf( "<%s spec='%d' shift='%f'/>", 
			POS, (*p).first, (*p).second );
		out << s_tmp << endl;
	}

	writeObject( out, sp );
	out << "</" << SPIN << ">" << endl;
}

void Serializer::writeLink(QTextStream & out, SpinSystem * ss)
{
	if( ss->getSucc() )
	{
		s_tmp.sprintf( "<%s pred='%d' succ='%d'/>", LINK,
			ss->getId(), ss->getSucc()->getId() );
		out << s_tmp << endl;
	}
}

void Serializer::writePair(QTextStream & out, SpinBase* base, SpinLink* l )
{
	if( base->getSpin( l->getLhs() ) == 0 || base->getSpin( l->getRhs() ) == 0 )
		return; // Schreibe nur gültige Links

	s_tmp.sprintf( "<%s lhs='%d' rhs='%d'", PAIR, l->getLhs(), l->getRhs() );
	out << s_tmp;
	const SpinLink::AliasMap& am = l->getAlias();
	if( am.empty() && l->getFieldValues().isEmpty() )
		out << "/>" << endl;
	else
	{
		out << ">" << endl;
		SpinLink::AliasMap::const_iterator p;
		for( p = am.begin(); p != am.end(); ++p )
		{
			s_tmp.sprintf( "<%s spec='%d' rate='%f' code='%d' visi='%d'/>", 
				INST, (*p).first, (*p).second.d_rating, int( (*p).second.d_code ),
				(*p).second.d_visible );
			out << s_tmp << endl;
		}
		writeObject( out, l );
		out << "</" << PAIR << ">" << endl;
	}
}

void Serializer::writePeakList(QTextStream & out, PeakList * pl)
{
	if( pl->getHome() )
		s_tmp.sprintf( "<%s id='%d' name='%s' home='%d'>", 
			PEAKLIST, pl->getId(), encode( pl->getName() ).toLatin1().constData(), pl->getHome() );
	else
		s_tmp.sprintf( "<%s id='%d' name='%s'>", 
			PEAKLIST, pl->getId(), encode( pl->getName() ).toLatin1().constData() );
	out << s_tmp << endl;
	const Dimension dim = pl->getDimCount();
	Dimension d;
	for( d = 0; d < dim; d++ )
	{
		s_tmp.sprintf( "<%s atom='%s'/>", DIMENSION, pl->getColors()[ d ].getIsoLabel() );
		out << s_tmp << endl;
	}

	PeakList::Models::const_iterator p3;
	for( p3 = pl->getModels().begin(); p3 != pl->getModels().end(); ++p3 )
	{
		s_tmp.sprintf( "<%s id='%d' kind='2'>", MODEL, (*p3).first );
		out << s_tmp << endl;
		PeakModel* m = (*p3).second;
		for( d = 0; d < m->getDimCount(); d++ )
		{

			s_tmp.sprintf( "<%s gain='%f' bal='%f' width='%f' tol='%f'/>", 
				PARAM, m->getGain( d ), m->getBalance( d ), 
				m->getWidth( d ), m->getTol( d ) );
			out << s_tmp << endl;
		}
		writeObject( out, m );
		out << "</" << MODEL << ">" << endl;
	}

	PeakList::SpecList::const_iterator p4;
	for( p4 = pl->getSpecs().begin(); p4 != pl->getSpecs().end(); ++p4 )
	{
		s_tmp.sprintf( "<%s id='%d'/>", SPEC, (*p4) );
		out << s_tmp << endl;
	}

	const PeakList::Peaks& pos = pl->getPeaks();
	PeakList::Peaks::const_iterator p;
	for( p = pos.begin(); p != pos.end(); ++p )
	{
		s_tmp.sprintf( "<%s id='%d' home='%d' tag='%s' color='%d'>", PEAK, 
			(*p).second->getId(), 
			(*p).second->getHome(),
			encode( (*p).second->getTag().data() ).toLatin1().constData(),
			int( (*p).second->getColor() ) );
		out << s_tmp << endl;
		const Peak::PositionMap& am = (*p).second->getPositions();
		Peak::PositionMap::const_iterator p2;
		for( p2 = am.begin(); p2 != am.end(); ++p2 )
		{
			s_tmp.sprintf( "<%s spec='%d' amp='%f' vol='%f' mdl='%d'", 
				POSITION, (*p2).first, 
				(*p2).second.d_amp, (*p2).second.d_vol,
				((*p2).second.d_mdl)?(*p2).second.d_mdl->getId():0 );
			out << s_tmp;
			const PeakPos& point = (*p2).second.d_pos;
			if( !point.isValid() )
				out << "/>" << endl;
			else
			{
				out << ">" << endl;
				for( d = 0; d < dim; d++ )
				{
					s_tmp.sprintf( "<%s pos='%f'/>", DIMENSION, point[ d ] );
					out << s_tmp << endl;
				}
				out << "</" << POSITION << ">" << endl;
			}
		}
		Peak::GuessMap::const_iterator p5;
		for( p5 = (*p).second->getGuesses().begin(); 
			p5 != (*p).second->getGuesses().end(); ++p5 )
		{
			s_tmp.sprintf( "<%s id='%d' prob='%f'>", GUESS, 
				(*p5).second->getId(), (*p5).second->getProb() );
			out << s_tmp << endl;
			for( d = 0; d < dim; d++ )
			{
				s_tmp.sprintf( "<%s id='%d'/>", SPIN, (*p5).second->getAssig()[ d ] );
				out << s_tmp << endl;
			}
			writeObject( out, (*p5).second );
			out << "</" << GUESS << ">" << endl;
		}

		const Peak::Assig& ass = (*p).second->getAssig();
		bool doit = false;
		for( d = 0; d < dim; d++ )
			if( ass[ d ] != 0 )
			{ 
				doit = true;
				break;
			}
		if( doit )
			for( d = 0; d < dim; d++ )
			{
				s_tmp.sprintf( "<%s id='%d'/>", SPIN, ass[ d ] );
				out << s_tmp << endl;
			}
		writeObject( out, (*p).second );
		out << "</" << PEAK << ">" << endl;
	}

	writeObject( out, pl );
	out << "</" << PEAKLIST << ">" << endl;
	
}

template<class T>
static void _writeObjectImp( QTextStream & out, T * obj)
{
    Root::Object::FieldValues f = obj->getFieldValues();
	Root::Object::FieldValues::const_iterator p;
	for( p = f.begin(); p != f.end(); ++p )
	{
		s_tmp = QString( "<%1 name='%2' type='%3'>" ).arg( FIELD ).
			arg( encode( (*p).first ) ).arg( encode( (*p).second.getTypeName() ) );
		if( (*p).second.getType() == Root::Any::CStr ) // Einzeiliger String
		{
			out << s_tmp;
			out << encode( (*p).second.getCStr() );
		}else if( (*p).second.getType() == Root::Any::Memo ||
			(*p).second.getType() == Root::Any::Xml ||
			(*p).second.getType() == Root::Any::Html ) // Mehrzeiliger String
		{
			// RISK out << "<!CDATA[" << v.getCStr() << "]]>";
			// ev. genügt encode auch hier.
			out << s_tmp;
			writeScript( out, (*p).second.getCStr() );
		}else if( (*p).second.getType() == Root::Any::Date )
		{
			out << s_tmp;
			Root::Date d = (*p).second.getDate();
			out << d.getString( true ).data();
		}else if( (*p).second.getType() == Root::Any::DateTime )
		{
			out << s_tmp;
			Root::DateTime dt = (*p).second.getDateTime();
			out << dt.getString( true ).data();
		}else
		{
			out << s_tmp;
			out << (*p).second.getCStr();
		}
		out << "</" << FIELD << ">" << endl;
	}
}

void Serializer::writeObject(QTextStream & out, Root::Object * obj)
{
    _writeObjectImp( out, obj );
}

void Serializer::writeObject(QTextStream & out, Root::SmallObject * obj)
{
    _writeObjectImp( out, obj );
}
