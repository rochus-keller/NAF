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

#include "Factory.h"
#include <Spec/EasySpectrum.h>
#include <Spec/EasyPeakList.h>
#include <Spec/Scale.h>
#include <Spec/BrukerSpectrum.h>
#include <Spec/CaraSpectrum.h>
#include <Spec/SitarSpectrum.h>
#include <Spec/PipeSpectrum.h>
#include <Spec/UcsfSpectrum.h>
#include <Spec/FelixSpectrum.h>
#include <Spec/PeakList.h>
#include <qfileinfo.h> 
#include <qregexp.h> 
using namespace Spec;
using namespace Spec::Private;
 
const char* Spectrum::s_fileFilter = "Spectrum (*.nmr *.param *.ucsf *.pipe *.felix *.mat );;"
    "Bruker Spectrum (1r 1i 2rr 2ri 2ir 2ii 3rrr 3irr 3rir 3rri 4rrrr);;"
	"CARA Spectrum (*.nmr);;"
	"EASY Spectrum (*.param);;"
	"Felix Spectrum (*.felix *.mat);;"
	"NNR Pipe Spectrum (*.pipe *.ft1 *.ft2 *.ft3 *.ft4 );;"
	"Sitar Spectrum (*.sitar);;"
	"Sparky Spectrum (*.ucsf);;"
	"All Formats (* *.*)";


Spectrum* Factory::createEasySpectrum(const char * path, bool mapped)
{
	return new EasySpectrum( path ); // TEST
}

PointSet* Factory::createEasyPeakList(const char * path)
{
	return new EasyPeakList( path );
}

PointSet* Factory::createEasyPeakList(Spectrum * s)
{
	if( s == 0 )
		throw Root::Exception( "need spectrum" );
	EasyPeakList* pl = new EasyPeakList();
	for( Dimension d = 0; d < s->getDimCount(); d++ )
		pl->addDimension( s->getColor( d ) );
	return pl;
}

PointSet* Factory::createEasyPeakList(const ColorMap& m)
{
	EasyPeakList* pl = new EasyPeakList();
	for( Dimension d = 0; d < m.size(); d++ )
		pl->addDimension( m[ d ] );
	return pl;
}

Spectrum* Factory::createBrukerSpectrum(const char *path)
{
	return new BrukerSpectrum( path );
}

Spectrum* Factory::createSpectrum(const char *path)
{
	QFileInfo info( path );

	QRegExp bruker( "[0-9][ir]+", false );

	const QString pat = info.extension(false).upper();
	if( pat == "PARAM" )
		return createEasySpectrum( path );
	else if( pat == "NMR" )
		return new CaraSpectrum( path );
	else if( pat == "SITAR" )
		return new SitarSpectrum( path );
	else if( pat == "PIPE" || pat == "FT1" || pat == "FT2" || pat == "FT3" || pat == "FT4" )
		return new PipeSpectrum( path );
	else if( pat == "UCSF" )
		return new UcsfSpectrum( path );
	else if( pat == "FELIX" || pat == "MAT" )
		return new FelixSpectrum( path );
	else if( bruker.exactMatch( info.suffix() ) ||
		( info.suffix().isEmpty() && 
		  bruker.exactMatch( info.fileName() ) ) )
		return createBrukerSpectrum( path );
	else
		return 0;

}

Root::Ref<PointSet> Factory::createEasyPeakList(PeakList * pl, Spectrum* spec )
{
	Root::Ref<EasyPeakList> e = new EasyPeakList();
	Dimension dim = pl->getDimCount();
	for( Dimension d = 0; d < dim; d++ )
		e->addDimension( pl->getColors()[ d ] );
	const PeakList::Peaks& pos = pl->getPeaks();
	PeakList::Peaks::const_iterator i;
	Index id;
	Peak* p;
	for( i = pos.begin(); i != pos.end(); ++i )
	{
		p = (*i).second;
		id = p->getId(); // e->getNextId();
		e->setPoint( id, pl->getPos( p, spec ) );
		e->setAmplitude( id, p->getAmp( spec ) );
		e->setCode( id, p->getColor() );
		e->setVolume( id, p->getVol( spec ) );
		e->setComment( id, p->getTag().data() );
		// TEST if( p->getAssig().size() == dim )
			e->setAssig( id, p->getAssig() );
	}
	return Root::Ref<PointSet>( e.deref() );
}


// RISK: hier definiert, da keine Units.cpp
static const Dimension s_max = 6;

static const char* s_letterUc[] =
{
	"X", "Y", "Z", "W", "D5", "D6"
};

static const char* s_letterLc[] =
{
	"x", "y", "z", "w", "d5", "d6"
};

static const char* s_symUc[] =
{
	"D1", "D2", "D3", "D4", "D5", "D6"
};

static const char* s_symLc[] =
{
	"d1", "d2", "d3", "d4", "d5", "d6"
};

static const char* s_symLetUc[] =
{
	"D1 (x)", "D2 (y)", "D3 (z)", "D4 (w)", "D5", "D6"
};

static const char* s_symLetLc[] =
{
	"d1 (x)", "d2 (y)", "d3 (z)", "d4 (w)", "d5", "d6"
};

static const char* s_number[] =
{
	"1", "2", "3", "4", "5", "6"
};


const char* Spec::getDimLetter( Dimension d, bool ucase )
{
	if( d < 0 || d >= s_max )
		return "?";
	else if( ucase )
		return s_letterUc[ d ];
	else
		return s_letterLc[ d ];
}

const char* Spec::getDimSymbol( Dimension d, bool ucase )
{
	if( d < 0 || d >= s_max )
		return "?";
	else if( ucase )
		return s_symUc[ d ];
	else
		return s_symLc[ d ];
}

Dimension Spec::parseDimString( const char* str )
{
	for( int i = 0; i < s_max; i++ )
	{
		if( ::strcmp( str, s_number[ i ] ) == 0 )
			return i;
		if( ::strcmp( str, s_letterUc[ i ] ) == 0 )
			return i;
		if( ::strcmp( str, s_letterLc[ i ] ) == 0 )
			return i;
		if( ::strcmp( str, s_symUc[ i ] ) == 0 )
			return i;
		if( ::strcmp( str, s_symLc[ i ] ) == 0 )
			return i;
	}
	return DimUndefined;
}

const char* Spec::getDimSymbolLetter( Dimension d, bool ucase )
{
	if( d < 0 || d >= s_max )
		return "?";
	else if( ucase )
		return s_symLetUc[ d ];
	else
		return s_symLetLc[ d ];
}

