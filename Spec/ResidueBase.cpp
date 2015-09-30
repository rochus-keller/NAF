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

#include "ResidueBase.h"
#include <fstream>
#include <qdir.h>
#include <qfileinfo.h> 
#include <Root/Vector.h>
#include <Xml/XmlHandler.h>
using namespace Spec;
using namespace Xml;

//////////////////////////////////////////////////////////////////////

class _ResidueBaseParser : public XmlHandler
{
public:
	//* Overrides
	bool endElement( Root::SymbolString name );
	bool startElement( Root::SymbolString name, const Atts& atts );
	//-

	_ResidueBaseParser(Repository* rep, Sequence* seq):
		d_rep( rep ), d_seq( seq ), d_state( Init ) {}

	struct Value
	{
		DisPar d_dp;
		Root::SymbolString d_name;
		Root::SymbolString d_atom;
		Root::Index d_id;
		Value( Root::SymbolString s, Root::Index id, Root::SymbolString a, PPM m, PPM d ):
			d_name( s ), d_id( id ), d_atom( a ), d_dp( m, d ) {}
	};
	typedef Root::Deque<Value> Values;
	Values d_values;

	enum State { Init, Base, Residue };
	Root::SymbolString d_resi;
	Root::Index d_id;
	State d_state;
	Root::ExRef<Repository> d_rep;
	Root::ExRef<Sequence> d_seq;
};

bool _ResidueBaseParser::startElement( Root::SymbolString qName, const Atts& atts )
{
	bool ok = true;
	if( qName == "residue-base" && d_state == Init )
		d_state = Base;
	else if( qName == "residue" && d_state == Base )
	{
		if( !atts.value( "sym" ).isEmpty() )
		{
			d_resi = atts.value( "sym" ).data();
			if( d_rep->findResidueType( d_resi.data() ) == 0 )
			{
				d_error.sprintf( "Invalid residue type %s", atts.value( "sym" ).data() );
				return false;
			}
		}else
			d_resi = "";
		if( !atts.value( "id" ).isEmpty() )
		{
			if( d_seq == 0 )
			{
				d_error.sprintf( "Residue numbers not allowed in this context" );
				return false;
			}
			bool ok;
			d_id = atts.value( "id" ).toULong( &ok );
			if( !ok || d_id <= 0 || d_seq->getResi().count( d_id ) == 0 )
			{
				d_error.sprintf( "Invalid id %s", atts.value( "id" ).data() );
				return false;
			}
		}else
			d_id = 0;
		d_state = Residue;
	}else if( qName == "entry" && d_state == Residue )
	{
		PPM mean;
		PPM dev;
		Root::SymbolString atom;
		bool done = true;
		atom = atts.value( "atom" ).data();
		if( atom.isNull() || atts.value( "atom" ).length() > 4 )
		{
			d_error.sprintf( "Invalid label %s", atts.value( "atom" ).data() );
			return false;
		}
		mean = atts.value( "mean" ).toFloat( &ok ); done &= ok;
		if( !atts.value( "dev" ).isEmpty() )
		{
			dev = atts.value( "dev" ).toFloat( &ok ); done &= ok;
		}else
			dev = 0.0;
		if( !done )
		{
			d_error.sprintf( "Invalid value for %s", atts.value( "atom" ).data() );
			return false;
		}
		d_values.push_back( Value( d_resi, d_id, atom, mean, dev ) );
	}else
		return false; // Error
	return true; // Success
}

bool _ResidueBaseParser::endElement(Root::SymbolString qName)
{
	if( qName == "residue-base" && d_state == Base )
		d_state = Init;
	else if( qName == "residue" && d_state == Residue )
		d_state = Base;
	else if( qName == "entry" && d_state == Residue )
		return true;
	else
		return false; // Error
	return true; // Success
}


//////////////////////////////////////////////////////////////////////

static void _apply( Sequence* seq, ResidueType* rt, Root::SymbolString atom, const DisPar& dp )
{
	assert( rt );
	Sequence::ResidueMap::const_iterator p;
	for( p = seq->getResi().begin(); p != seq->getResi().end(); ++p )
	{
		if( (*p).second->getType() == rt )
		{
			seq->setDisPar( (*p).second, atom, dp );
		}
	}
}

bool ResidueBase::read(const char* path, Repository* rep, Sequence* seq )
{
	assert( seq );
	assert( rep );
	assert( path );
	_ResidueBaseParser handler( rep, seq );
    bool ok = handler.parse( path );
    if( !ok ) 
	{
		d_result = handler.getError().toLatin1().data();
		return false;
	}
	Residue* r;
	for( int i = 0; i < handler.d_values.size(); i++ )
	{
		const _ResidueBaseParser::Value& v = handler.d_values[ i ];
		if( v.d_id )
		{
			r = seq->getResidue( v.d_id );
			assert( r );
			seq->setDisPar( r, v.d_atom, v.d_dp );
		}
		if( !v.d_name.isNull() )
		{
			_apply( seq, rep->findResidueType( v.d_name.data() ), v.d_atom, v.d_dp );
		}
	}
	return true;
}

bool ResidueBase::read(const char* path, Repository* rep )
{
	assert( rep );
	assert( path );
	_ResidueBaseParser handler( rep, 0 );
    bool ok = handler.parse( path );
    if( !ok ) 
	{
		d_result = handler.getError().toLatin1().data();
		return false;
	}
	ResidueType* rt;
	Atom* at;
	for( int i = 0; i < handler.d_values.size(); i++ )
	{
		const _ResidueBaseParser::Value& v = handler.d_values[ i ];
		if( !v.d_name.isNull() )
		{
			rt = rep->findResidueType( v.d_name.data() );
			if( rt == 0 )
			{
				d_result = "Unknown residue type ";
				d_result += v.d_name.data();
				return false;
			}
			at = rt->getAtom( v.d_atom.data() );
			if( at == 0 )
			{
				d_result = "Unknown atom ";
				d_result += v.d_atom.data();
				d_result += " in residue type ";
				d_result += v.d_name.data();
				return false;
			}
			rt->setDisPar( at, v.d_dp );
		}
	}
	return true;
}

struct _StatsLine 
{
	// Eine reguläre Zeile ist 76 Zeichen lang.
	int d_line;
	QByteArray  d_rt;
	QByteArray  d_label;
	PPM d_mean;
	PPM d_dev;
	_StatsLine* d_next;

	_StatsLine( const _StatsLine& l):d_next(0)
	{
		d_line = l.d_line;
		d_rt = l.d_rt;
		d_label = l.d_label;
		d_mean = l.d_mean;
		d_dev = l.d_dev;
	}
	_StatsLine()
	{
		d_line = 0;
		d_next = 0;
	}
	~_StatsLine()
	{
		if( d_next )
			delete d_next;
	}
};

static _StatsLine* readFile( const char* path, QByteArray & err )
{
	assert( path );
    QFile file( path );
	if( !file.open( QIODevice::ReadOnly ) )
	{
		err = "Cannot open file ";
		err += path;
		return 0;
	}
	char buf[ 128 ];
	
	int res = file.readLine( buf, sizeof( buf ) );
	// -1 bei Error
	if( res == -1 )
	{
		err = "Error reading file";
		return 0;
	}
	_StatsLine* first = 0;
	_StatsLine* last = 0;
	bool ok = true;
	_StatsLine tmp;
	tmp.d_line = 1;
	QString str;
    //const char* test;
	while( res != -1 )
	{
		if( res >=76 && res <= 78 )
		{
			// Es handelt sich um eine reguläre Zeile.
			str = QByteArray ( buf, 7 ).data();
			tmp.d_rt = str.stripWhiteSpace().toLatin1();
			str = QByteArray ( buf + 8, 8 ).data();
			tmp.d_label = str.stripWhiteSpace().toLatin1();
			str = QByteArray ( buf + 55, 11 ).data();
            //test = str.toLatin1();
			tmp.d_mean = str.stripWhiteSpace().toFloat( &ok );
			if( !ok )
			{
				err = "Invalid mean PPM shift on line ";
				err += QString().setNum( tmp.d_line ).toLatin1();
				goto error;
			}
			str = QByteArray ( buf + 66, 11 ).data();
            //test = str.toLatin1();
			tmp.d_dev = str.stripWhiteSpace().toFloat( &ok );
			if( !ok || tmp.d_dev < 0.0 )
			{
				err = "Invalid deviation value on line ";
				err += QString().setNum( tmp.d_line ).toLatin1();
				goto error;
			}
			if( last )
			{
				last->d_next = new _StatsLine( tmp );
				last = last->d_next;
			}else
			{
				first = last = new _StatsLine( tmp );
			}
		}
		res = file.readLine( buf, sizeof( buf ) );
		tmp.d_line++;
	}
	return first;
error:
	if( first )
		delete first;
	return 0;
}

bool ResidueBase::readStats(const char* path, Repository* rep )
{
	assert( rep );
	_StatsLine* l = readFile( path, d_result );
	if( l == 0 )
		return false;
	_StatsLine* i = l;
	ResidueType* rt;
	Atom* at;
	while( i )
	{
		rt = rep->findResidueType( i->d_rt.data() );
		if( rt == 0 )
		{
			d_result = "Unknown residue type ";
			d_result += i->d_rt;
			d_result += " on line ";
			d_result += QString().setNum( i->d_line ).toLatin1();
			goto error;
		}
		at = rt->getAtom( i->d_label.data() );
		if( at == 0 )
		{
			d_result = "Unknown atom ";
			d_result += i->d_label;
			d_result += " in residue type ";
			d_result += i->d_rt;
			d_result += " on line ";
			d_result += QString().setNum( i->d_line ).toLatin1();
			goto error;
		}
		rt->setDisPar( at, DisPar( i->d_mean, i->d_dev ) );
		i = i->d_next;
	}
	delete l;
	return true;
error:
	if( l )
		delete l;
	return false;
}

bool ResidueBase::readStats(const char* path, Repository* rep, Sequence* seq )
{
	assert( seq );
	assert( rep );
	assert( path );
	_StatsLine* l = readFile( path, d_result );
	if( l == 0 )
		return false;
	_StatsLine* i = l;
	while( i )
	{
		_apply( seq, rep->findResidueType( i->d_rt.data() ), 
			i->d_label.data(), DisPar( i->d_mean, i->d_dev ) );
		i = i->d_next;
	}
	delete l;
	return true;
}


bool ResidueBase::write(const char * path, Sequence * seq)
{
	std::ofstream out( path );
	if( !out )
	{
		d_result = "Cannot open file for writing";
		return false;
	}
	out << "<?xml version='1.0' encoding='ISO-8859-1'?>" << std::endl;	// RISK
	
	out << "<residue-base>" << std::endl;
	QString tmp;
	Sequence::ResidueMap::const_iterator i;
	Residue::Parameters::const_iterator j;
	for( i = seq->getResi().begin(); i != seq->getResi().end(); ++i )
	{
		const Residue::Parameters& p = (*i).second->getParams();
		if( !p.empty() )
		{
			tmp.sprintf( "<residue id='%d' sym='%s'>", (*i).second->getId(),
				(*i).second->getType()->getShort().data() );
            out << tmp.toLatin1().data() << std::endl;
			for( j = p.begin(); j != p.end(); ++j )
			{
				if( (*j).second.isValid() )
				{
					tmp.sprintf( "<entry atom='%s' mean='%f' dev='%f'/>", 
						(*j).first.data(), (*j).second.d_mean, (*j).second.d_dev );
                    out << tmp.toLatin1().data() << std::endl;
				}
			}
			out << "</residue>" << std::endl;
		}
	}
	out << "</residue-base>" << std::endl;
	return true;
}

bool ResidueBase::write(const char *path, TypeBase * tb)
{
	std::ofstream out( path );
	if( !out )
	{
		d_result = "Cannot open file for writing";
		return false;
	}
	out << "<?xml version='1.0' encoding='ISO-8859-1'?>" << std::endl;	// RISK
	
	out << "<residue-base>" << std::endl;
	QString tmp;
	TypeBase::ResidueTypeMap::const_iterator i;
	ResidueType::AtomMap::const_iterator j;
	for( i = tb->getResidueTypes().begin(); i != tb->getResidueTypes().end(); ++i )
	{
		const ResidueType::AtomMap& p = (*i).second->getAtoms();
		if( !p.empty() )
		{
			tmp.sprintf( "<residue sym='%s'>", (*i).second->getShort().data() );
            out << tmp.toLatin1().data() << std::endl;
			for( j = p.begin(); j != p.end(); ++j )
			{
				if( (*j).second->getDisPar().isValid() )
				{
					tmp.sprintf( "<entry atom='%s' mean='%f' dev='%f'/>", 
						(*j).first.data(), 
						(*j).second->getDisPar().d_mean, (*j).second->getDisPar().d_dev );
                    out << tmp.toLatin1().data() << std::endl;
				}
			}
			out << "</residue>" << std::endl;
		}
	}
	out << "</residue-base>" << std::endl;
	return true;
}
