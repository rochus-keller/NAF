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

#include "Repository.h"
#include <Spec/Serializer.h>
#include <QtDebug>
using namespace Spec;

const char* Repository::keyRepository = "Repository";
const char* Repository::keySpectrumType = "SpectrumType";
const char* Repository::keyResidueType = "ResidueType";
const char* Repository::keySystemType = "SystemType";
const char* Repository::keyProject = "Project";
const char* Repository::keySpectrum = "Spectrum";
const char* Repository::keyPeaklist = "Peaklist";
const char* Repository::keyPeak = "Peak";
const char* Repository::keyResidue = "Residue";
const char* Repository::keySpinSystem = "SpinSystem";
const char* Repository::keySpin = "Spin";
const char* Repository::keyAuthor = "Author";
const char* Repository::keyScript = "Script";
const char* Repository::keyCreationDate = "Creation Date";
const char* Repository::keyLink = "SpinLink";
const char* Repository::keyGuess = "AssigGuess";
const char* Repository::keyDesc = "Description";
const char* Repository::keyTplDesc = "TemplateDesc";
const char* Repository::keyTplAuthor = "TemplateAuthors";
const char* Repository::keyTplPath = "TemplateName";
const char* Repository::keyOrigTplAuthor = "OrigTemplateAuthor";
const char* Repository::keyOrigTplDesc = "OrigTemplateDesc";
const char* Repository::keyTplDate = "TemplateDate";
const char* Repository::keyOrigTplDate = "OrigTemplateDate";
const char* Repository::keyLabelingScheme = "LabelingScheme";
const char* Repository::keySample = "Sample";
const char* Repository::keyAtom = "Atom";

Repository::Repository():d_dirty(false)
{
	d_base = new ObjectBase( this );
	d_types = new TypeBase( this );
}

void Repository::init()
{
	copyDynValuesFrom( 0 );
	d_projects.clear();
	d_classes.clear();
	d_scripts.clear();
	d_screenClr.clear();
	d_printClr.clear();
	d_path = "";
	d_tplAuthor = "";
	d_author = "";

	d_base = new ObjectBase( this );
	d_types = new TypeBase( this );

	d_colors = new PeakColorList();
    d_colors->addObserver(this);
	d_colors->setColor( 1, Qt::cyan );
	d_colors->setColor( 2, Qt::magenta );
	d_colors->setColor( 3, Qt::yellow );
	d_colors->setColor( 4, Qt::darkCyan );
	d_colors->setColor( 5, Qt::darkMagenta );
	d_colors->setColor( 6, Qt::darkYellow );
	// XEASY: white magenta red yellow green zyan blue

	Root::Ref<ObjectDef> obj;

	obj = new ObjectDef( this, keyRepository );
	d_classes[ keyRepository ] = obj;
	obj->setField( keyAuthor, Root::Any::CStr, "" );
	obj->setField( keyCreationDate, Root::Any::Date, "" );
	setFieldValue( keyCreationDate, Root::Date::today() );
	obj->setField( keyDesc, Root::Any::Memo, "" );
	// obj->setField( keyTplDesc, Root::Any::Memo, "" );
	// obj->setField( keyTplAuthor, Root::Any::CStr, "" );
	// obj->setField( keyTplPath, Root::Any::CStr, "" );

	/*
	str = "Atom";
	obj = new ObjectDef( str );
	d_classes[ str ] = obj;

	str = "AtomGroup";
	obj = new ObjectDef( str );
	d_classes[ str ] = obj;
	*/

	obj = new ObjectDef( this, keySpectrumType );
	d_classes[ keySpectrumType ] = obj;

	obj = new ObjectDef( this, keyResidueType );
	d_classes[ keyResidueType ] = obj;

	obj = new ObjectDef( this, keySystemType );
	d_classes[ keySystemType ] = obj;

	obj = new ObjectDef( this, keyLabelingScheme );
	d_classes[ keyLabelingScheme ] = obj;

	obj = new ObjectDef( this, keyProject );
	d_classes[ keyProject ] = obj;

	obj = new ObjectDef( this, keySpectrum );
	d_classes[ keySpectrum ] = obj;

	obj = new ObjectDef( this, keyPeaklist );
	d_classes[ keyPeaklist ] = obj;

	obj = new ObjectDef( this, keyPeak );
	d_classes[ keyPeak ] = obj;

	obj = new ObjectDef( this, keyResidue );
	d_classes[ keyResidue ] = obj;

	obj = new ObjectDef( this, keySample );
	d_classes[ keySample ] = obj;

	obj = new ObjectDef( this, keyAtom );
	d_classes[ keyAtom ] = obj;

	obj = new ObjectDef( this, keySpinSystem );
	d_classes[ keySpinSystem ] = obj;

	obj = new ObjectDef( this, keySpin );
	d_classes[ keySpin ] = obj;

	obj = new ObjectDef( this, keyLink );
	d_classes[ keyLink ] = obj;

	obj = new ObjectDef( this, keyGuess );
	d_classes[ keyGuess ] = obj;

	obj = new ObjectDef( this, keyScript );
	d_classes[ keyScript ] = obj;
	obj->setField( keyAuthor, Root::Any::CStr, "" );
	obj->setField( keyCreationDate, Root::Any::Date, "" );
	obj->setField( keyDesc, Root::Any::Memo, "" );

}

Repository::~Repository()
{
    if( d_colors )
        d_colors->removeObserver(this);
}

void Repository::saveToFile(const char *path, const char* creator)
{
	if( path != 0 && *path != 0 )
	{
        const bool changed = ( d_path != path );
        d_path = path;
        if( changed )
        {
            ProjectMap::const_iterator i;
            for( i = d_projects.begin(); i != d_projects.end(); ++i )
                (*i).second->pathAdjusted();
        }
    }
	Repository::Changed m1( this, Repository::AboutToSave );
	Subject::notifyObservers( m1 );
	Serializer s( this );
	s.serialize( d_path.data(), creator );
	d_dirty = false;
	Repository::Changed m2( this, Repository::Saved );
	Subject::notifyObservers( m2 );
}

void Repository::addProject(Project* p)
{
	if( d_projects.find( p->getName() ) != d_projects.end() )
		throw Root::Exception( "Project name is not unique within repository" );
	d_projects[ p->getName() ] = p;
	p->d_owner = this;
	Project::Added m( p );
	notifyObservers( m );
	d_dirty = true;
}

Project* Repository::findProject( const char* name ) const
{
	ProjectMap::const_iterator i = d_projects.find( name );
	if( i != d_projects.end() )
		return (*i).second;
	else
		return 0;
}

bool Repository::removeProject(Project* pro)
{
	if( pro == 0 || d_projects.count( pro->getName() ) == 0 )
		return false;
	d_dirty = true;
	Root::Ref<Project> tmp = pro;
	d_projects.erase( pro->getName() ); // Muss vor Update kommen, sonst wird liste falsch aufgebaut.
	Project::Removed m( pro );
	notifyObservers( m );
	return true;
}

void Repository::notifyObservers( Root::UpdateMessage& msg )
{
	touch();
	Subject::notifyObservers( msg );
}

void Repository::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( Root::UpdateMessage, a, msg )
	{
        Q_UNUSED(a)
		d_dirty = true;
		touch();
		msg.consume();
	}
	END_HANDLER();
}

bool Repository::inUse(ResidueType * rt) const
{
	ProjectMap::const_iterator p1;
	Sequence* seq;
	Sequence::ResidueMap::const_iterator p2;
	for( p1 = d_projects.begin(); p1 != d_projects.end(); ++p1 )
	{
		seq = (*p1).second->getSequence();
		for( p2 = seq->getResi().begin(); p2 != seq->getResi().end(); ++p2 )
		{
			if( (*p2).second->getType() == rt )
				return true;
		}
	}
	ProjectMap::const_iterator p3;
	SpinBase::SpinSystemMap::const_iterator p4;
	for( p3 = d_projects.begin(); p3 != d_projects.end(); ++p3 )
	{
		for( p4 = (*p3).second->getSpins()->getSystems().begin(); 
			p4 != (*p3).second->getSpins()->getSystems().end(); ++p4 )
		{
			if( (*p4).second->getCands().count( rt ) != 0 )
				return true;
		}
	}
	return d_types->inUse( rt );
}

bool Repository::inUse(SpectrumType * t) const
{
	ProjectMap::const_iterator p1;
	Project::SpectrumMap::const_iterator p2;
	for( p1 = d_projects.begin(); p1 != d_projects.end(); ++p1 )
	{
		for( p2 = (*p1).second->getSpectra().begin(); p2 != (*p1).second->getSpectra().end(); ++p2 )
		{
			if( (*p2).second->getType() == t )
				return true;
		}
	}
	return false;
}

bool Repository::inUse(SystemType * st) const
{
	assert( st );
	ProjectMap::const_iterator p1;
	SpinBase::SpinSystemMap::const_iterator p2;
	for( p1 = d_projects.begin(); p1 != d_projects.end(); ++p1 )
	{
		for( p2 = (*p1).second->getSpins()->getSystems().begin(); 
			p2 != (*p1).second->getSpins()->getSystems().end(); ++p2 )
		{
			if( (*p2).second->getSysType() == st )
				return true;
		}
	}
	return d_types->inUse( st );
}

bool Repository::remove(SystemType * st)
{
	if( inUse( st ) )
		return false;
	else
		return d_types->remove( st );
}

bool Repository::remove(SpectrumType * st)
{
	if( inUse( st ) )
		return false;
	else
		return d_types->remove( st );
}

bool Repository::remove(ResidueType * rt)
{
	if( inUse( rt ) )
		return false;
	else
		return d_types->remove( rt );
}

ResidueType* Repository::findResidueType(const char * name, bool caseInsens) const
{
	return d_types->findResidueType( name, caseInsens );
}

ResidueType* Repository::addResidueType(Root::SymbolString n, 
			Root::SymbolString s, Root::SymbolString l)
{
	return d_types->addResidueType( n, s, l);
}

bool Repository::addSpectrumType(SpectrumType * t)
{
	return d_types->addSpectrumType( t );
}

bool Repository::rename(SpectrumType * st, const char * name)
{
	return d_types->rename( st, name );
}

bool Repository::rename(ResidueType * rt, const char * name)
{
	return d_types->rename( rt, name );
}

SystemType* Repository::addSystemType(const char *name)
{
	return d_types->addSystemType( name );
}

bool Repository::rename(SystemType * st, const char * name)
{
	return d_types->rename( st, name );
}

ObjectDef* Repository::findObjectDef(Root::SymbolString s) const
{
	ObjectDefs::const_iterator i = d_classes.find( s );
	if( i == d_classes.end() )
		return 0;
	else
		return (*i).second;
}

Root::Object::FieldInfo Repository::getFieldInfo(Field f) const
{
	if( f == keyTplAuthor || f == keyTplPath || f == keyOrigTplAuthor || f == keyOrigTplDesc || f == keyOrigTplDate )
		return Root::Object::ReadOnlyField;
	else
		return Subject::getFieldInfo( f );
}

Root::Object::FieldValues Repository::getFieldValues() const
{
	FieldValues v = Subject::getFieldValues();
	v.setFieldValue( keyAuthor, d_author.data() );
	v.setFieldValue( keyTplPath, d_tplPath.data() );
	v.setFieldValue( keyTplAuthor, d_tplAuthor.data() );
	v.setFieldValue( keyOrigTplAuthor, d_origTplAuthor.data() );
	return v;
}

bool Repository::getDynValue( Field f, Root::Any& v ) const
{
	if( f == keyAuthor )
	{
		v.setCStr( d_author.data() );
		return true;
	}else if( f == keyTplPath )
	{
		v.setCStr( d_tplPath.data() );
		return true;
	}else if( f == keyTplAuthor )
	{
		v.setCStr( d_tplAuthor.data() );
		return true;
	}else if( f == keyOrigTplAuthor )
	{
		v.setCStr( d_origTplAuthor.data() );
		return true;
	}
	return Object::getFieldValue( f, v );
}

void Repository::setFieldValue( Field f, const Root::Any& v )
{
	if( f == keyAuthor )
		d_author = v.getCStr();
	else
		Object::setFieldValue( f, v );
	d_dirty = true;
	Root::UpdateDynValue m( this, f );
	notifyObservers( m );
}

bool Repository::rename(Project * p, const char *name)
{
	assert( p );
	if( p == 0 || d_projects.count( p->getName() ) == 0 || 
		d_projects.count( name ) != 0 || name == 0 || *name == 0 )
		return false;
	Root::Ref<Project> pro = p;
	d_projects.erase( pro->getName() );
	pro->d_name = name;
	d_projects[ pro->d_name ] = pro;
	d_dirty = true;
	Project::Changed m( pro, Project::Name );
	notifyObservers( m );
	return true;
}

const char* Script::s_lang[] =
{
	"Lua"
};

Script::Script(QByteArray name, Lang l):
	d_name( name ), d_lang( l ), d_compiled( false ), d_rep( 0 )
{

}

Script::~Script()
{

}

Script* Repository::addScript(Root::SymbolString name, Script::Lang l)
{
	if( d_scripts.find( name ) != d_scripts.end() )
		return 0;
	
	Root::Ref<Script> s = new Script( name, l );
	s->d_rep = this;
	d_scripts[ name ] = s;
	s->setFieldValue( keyCreationDate, Root::Date::today() );
	d_dirty = true;
	Script::Added m( s );
	notifyObservers( m );
	return s;
}

void Repository::remove(Script * s)
{
	assert( s );
	if( d_scripts.find( s->getName() ) == d_scripts.end() )
		return;
	Root::Ref<Script> tmp = s;
	d_scripts.erase( s->getName() );
	d_dirty = true;
	Script::Removed m( s );
	notifyObservers( m );
}

bool Repository::rename(Script * s, const char * name)
{
	if( d_scripts.find( name ) != d_scripts.end() )
		return false;
	Root::Ref<Script> t = s;
	d_scripts.erase( s->getName() );
	s->d_name = name;
	d_scripts[ name ] = s;
	d_dirty = true;
	Script::Changed m( s, Script::Name );
	notifyObservers( m );
	return true;
}

void Script::setCode(const char * txt)
{
	d_code = txt;
	d_compiled = false;
	assert( d_rep );
	Script::Changed m( this, Script::Code );
	d_rep->notifyObservers( m );
}

void Script::setCompiled()
{
	d_compiled = true;
	assert( d_rep );
	// Unnötig. d_rep->notifyObservers( Script::Changed( this, Script::Compiled ) );
}

QByteArray  Script::getInstanceName(bool) const
{
	return d_name.data(); // RISK
}

void Repository::setScreenClr(const SlotColors & clr)
{
	if( d_screenClr.size() <= clr.size() )
		d_screenClr = clr;
	else
	{
		for( int i = 0; i < clr.size(); i++ )
			d_screenClr[ i ] = clr[ i ];
	}
	d_dirty = true;
	touch();
}

void Repository::copyFrom(const Repository & r)
{
	init();
	d_types = r.d_types;
	d_types->d_owner = this;

	if( r.d_tplAuthor.isEmpty() )
		d_tplAuthor = r.d_author;
	else
	{
		d_tplAuthor = r.d_tplAuthor;
		d_tplAuthor += ", ";
		d_tplAuthor += r.d_author;
	}
	d_origTplAuthor = r.d_origTplAuthor;
	d_tplPath = r.getFilePath();
	copyDynValuesFrom( &r );

	d_classes = r.d_classes;
	ObjectDefs::const_iterator p1;
	for( p1 = d_classes.begin(); p1 != d_classes.end(); ++p1 )
		(*p1).second->d_owner = this;

	d_scripts = r.d_scripts;
	ScriptMap::const_iterator p3;
	for( p3 = d_scripts.begin(); p3 != d_scripts.end(); ++p3 )
		(*p3).second->d_rep = this;

	d_colors = r.d_colors;
	d_screenClr = r.d_screenClr;
	d_printClr = r.d_printClr;
	setFieldValue( keyCreationDate, Root::Date::today() );
	d_dirty = false;
}

void Repository::load(const char *path, bool asTemplate )
{
	init();
    d_dirty = false;
	if( path != 0 && *path != 0 )
	{
		d_path = path;
		Serializer s( this );
		s.deserialize( d_path.data(), asTemplate );
	}
}

void Repository::reload(const char *path, bool asTemplate )
{
	clear();
	if( path != 0 && *path != 0 )
	{
		d_path = path;
		Serializer s( this );
		s.deserialize( d_path.data(), asTemplate );
	}
	Repository::Changed m( this, Repository::Ready );
	Subject::notifyObservers( m );
}

void Repository::clear()
{
	Repository::Changed m( this, Repository::Closing );
	Subject::notifyObservers( m );
	init();
}

void Repository::reloadEmpty()
{
	clear();
	d_dirty = false;
	Repository::Changed m( this, Repository::Ready );
	Subject::notifyObservers( m );
}

void Repository::touch()
{
	d_dirty = true;
	Repository::Changed m( this, Repository::Dirty );
	Subject::notifyObservers( m );
}

QByteArray  Repository::getInstanceName(bool) const
{
	return d_path; // RISK
}

void Repository::reloadSpecs()
{
	bool old = d_dirty;
	ProjectMap::const_iterator i;
	for( i = d_projects.begin(); i != d_projects.end(); ++i )
		(*i).second->reloadSpecs();
    d_dirty = old;
}

Script *Repository::getScript(const char *name) const
{
    ScriptMap::const_iterator i = d_scripts.find( name );
    if( i != d_scripts.end() )
        return (*i).second;
    else
        return 0;
}


void Repository::addRef() const
{
	Resource::addRef();
}
