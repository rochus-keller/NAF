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

#include "EasyPeakList.h"
#include <Spec/Spectrum.h>
#include <Spec/Scale.h>
#include <fstream>
#include <stdio.h>
using namespace Spec;
using namespace Spec::Private;

GCC_IGNORE( -Wunused-result );

static PeakPos s_empty;
static EasyPeak s_dummy;

EasyPeakList::EasyPeakList(const char* filePath):
	d_filePath( filePath ), d_dirty( false )
{
	readPeakList();
}

EasyPeakList::EasyPeakList():
	d_nextId( 1 ), d_dirty( false )
{

}

EasyPeakList::~EasyPeakList()
{

}

void EasyPeakList::setComment(Index id, const char * comment)
{
	bool add = d_peaks.find( id ) == d_peaks.end();
	EasyPeak& ep = d_peaks[ id ];
	if( add )
		ep.init( d_colors.size() );
	ep.d_comment = comment;

	d_dirty = true;

	PointSet::Update::Type t;
	if( add )
		t = PointSet::Update::Add;
	else
		t = PointSet::Update::Change;
	PointSet::Update msg( this, t, id );
	notifyObservers( msg );
}

void EasyPeakList::setPoint(Index id, const PeakPos & p )
{
	//if( p.size() != d_colors.size() )
	//	throw Root::Exception( "dimension missmatch" );

	bool add = d_peaks.find( id ) == d_peaks.end();
	if( id > d_nextId )
		d_nextId = id + 1;
	EasyPeak& ep = d_peaks[ id ];
	if( add )
		ep.init( d_colors.size() );
	ep.d_point = p;
	ep.d_id = id;

	d_dirty = true;

	PointSet::Update::Type t;
	if( add )
		t = PointSet::Update::Add;
	else
		t = PointSet::Update::Change;
	PointSet::Update msg( this, t, id );
	notifyObservers( msg );
}

bool EasyPeakList::isDefined(Index id) const
{
	return d_peaks.find( id ) != d_peaks.end();
}

Index EasyPeakList::getNextId()
{
	return d_nextId++;
}

const char* EasyPeakList::getComment(Index id) const
{
	EasyPeakList::PeakList::const_iterator pos = d_peaks.find( id );
	if( pos != d_peaks.end() )
		return (*pos).second.getComment();
	else
		return "";
}

const PeakPos& EasyPeakList::getPoint(Index id) const
{
	EasyPeakList::PeakList::const_iterator pos = d_peaks.find( id );
	if( pos != d_peaks.end() )
		return (*pos).second.getPoint();
	else
		throw Root::Exception( "Peak not known" );
	return s_empty;
}

void EasyPeakList::rotate(const Rotation & rot)
{
	d_dirty = true;
	Root::Vector<QByteArray > temp = d_labels;
	Dimension d;
	for( d = 0; d < temp.size(); d++ )
		temp[ d ] = d_labels[ rot[ d ] ];
	d_labels = temp;
	EasyPeakList::PeakList::iterator pos;
	for( pos = d_peaks.begin(); pos != d_peaks.end(); ++pos )
		(*pos).second.rotate( rot );

	Root::Vector<AtomType> temp2 = d_colors;
	for( d = 0; d < temp2.size(); d++ )
		temp2[ d ] = d_colors[ rot[ d ] ];
	d_colors = temp2;
	PointSet::Update msg( this, PointSet::Update::Rotate );
	notifyObservers( msg );
}

void EasyPeakList::readPeakList()
{
	d_peaks.clear();
	d_nextId = 1;

	FILE *fp;
	fp = ::fopen( d_filePath.data(), "r" );
	if( fp == NULL )
		throw Root::Exception( "Can't open file for input" );

	const int lineSize = 240;
	char lineBuffer[ lineSize ];

	// Lese Header
	int dimCount = 0;
	Root::Vector<double> sum;
	if( ::fscanf(fp, "# Number of dimensions %1d\n", &dimCount ) != 1 ) 
	{
		dimCount = 2; // Altes Easy-Format oder Dateifehler
	}
	sum.assign( dimCount, 0.0 );
	d_labels.assign( dimCount, "" );
	d_colors.assign( dimCount, AtomType() );
	char ch = 0;
	while( ( ch = getc( fp ) ) == '#' )	// NOTE: GCC auf Irix verträgt kein :: vor getc.
	{
		char parName[ 50 ];
		int dim;
		// Lese die Dimensionslabels, falls vorhanden
		::fgets( lineBuffer, sizeof( lineBuffer ), fp );
		if( ::sscanf( lineBuffer, "INAME%d", &dim ) )
		{
			dim--; // Zählung beginnt mit Null
			if( dim >= dimCount || dim < 0 )
			{
				::fclose( fp );
				throw Root::Exception( "Invalid dimension" );
			}
			if( ::sscanf( lineBuffer, "INAME%*d%7s", parName ) != 1 )
			{
				::fclose( fp );
				throw Root::Exception( "Invalid dimension label" );
			}
			else
				d_labels[ dim ] = parName;

		}
		// else: weitere Felder ignorieren
	}                                
	::ungetc( ch, fp);

	Index last = 0;
	while( feof( fp ) == 0 ) 
	{
		do
		{
			// Überlese Whitespace
			ch = ::fgetc( fp );
		}while( ch == ' ' || ch == '\t' );
		if( ch == '#' )
		{
			// Lese Kommentar zum vorangegangenen Peak
			::fgets( lineBuffer, sizeof( lineBuffer ), fp ); 
			int i;
			for( i = 0; i < lineSize; i++ )
				if( lineBuffer[ i ] == 13 || lineBuffer[ i ] == 10 )
				{
					lineBuffer[ i ] = 0; // fgets liefert CR zurück.
					break;
				}
			char* start = lineBuffer;
			while( *start == ' ' && *start != 0 ) // Trim left
				start++;
			if( d_peaks.size() == 0 )
			{
				::fclose( fp );
				throw Root::Exception( "Invalid comment position" );
			}else
				d_peaks[ last ].d_comment = start;
		}else if( feof( fp ) == 0 )
		{
			float parFloat;
			char parName[ 50 ];
			int parInt;
			EasyPeak peak;
			peak.init( dimCount );
			bool success = true;
			int i;	// wegen gcc

			::ungetc( ch, fp );
			success &= ::fscanf( fp, "%d", &peak.d_id ) == 1;
			// Lese die Shifts
			for( i = 0; i < dimCount; i++ )
			{
				success &= ::fscanf( fp, "%f", &parFloat ) == 1;
				sum[ i ] += parFloat;
				peak.d_point[ i ] = parFloat;
			}
			// Lese dimensionslose Parameter
#ifdef _EasyPeak_options_
			success &= ::fscanf( fp, "%d %s %e %e %c", &parInt, parName, 
				&peak.d_volume, &peak.d_volError, &peak.d_integrationMethod ) == 5;
			peak.d_specType = parName;
#else
			success &= ::fscanf( fp, "%d %s %e %e %c", &parInt, parName, 
				&peak.d_volume, &parFloat, parName ) == 5;
#endif
			peak.d_color = parInt;

			// Überlese die Null
			success &= ( ::fscanf( fp, "%*d" ) == 0 );
			// Lese die Atomnummern
			for( i = 0; i < dimCount; i++ )
			{
				success &= ::fscanf( fp, "%d", &parInt ) == 1;
#ifdef _EasyPeak_atoms_
				peak.d_atoms[ i ] = parInt;
#endif
			}
			// Fresse Rest der Zeile
			::fgets( lineBuffer, sizeof( lineBuffer ), fp );
			last = peak.d_id;
			if( success )
			{
				if( d_peaks.find( peak.d_id ) != d_peaks.end() )
				{
					::fclose( fp );
					throw Root::Exception( "Duplicate ID" );
				}
				d_peaks[ peak.d_id ] = peak;
			}else
			{
				::fclose( fp );
				throw Root::Exception( "Invalid Line" );
			}
			if( peak.d_id >= d_nextId )
				d_nextId = peak.d_id + 1;
		}
	}
	::fclose( fp );
	float num = d_peaks.size();
	for( Dimension d = 0; d < dimCount; d++ )
		d_colors[ d ] = AtomType( AtomType::parse( d_labels[ d ].data(), sum[ d ] / num ) );
}

void EasyPeakList::clear()
{
	d_peaks.clear();
	d_colors.clear();
	d_labels.clear();
	d_nextId = 1;
	PointSet::Update msg( this, PointSet::Update::Clear );
	notifyObservers( msg );
}

void EasyPeak::rotate(const Rotation & rot )
{
	PeakPos temp = d_point;
	Dimension d;
	for( d = 0; d < rot.size(); d++ )
		temp[ d ] = d_point[ rot[ d ] ];
	d_point = temp;
#ifdef _EasyPeak_atoms_
	PointSet::Assig temp2 = d_atoms;
	for( d = 0; d < rot.size(); d++ ) // TEST
		temp2[ d ] = d_atoms[ rot[ d ] ];
	d_atoms = temp2;
#endif
}

EasyPeak::EasyPeak()
{
	init( 2 );
}

void EasyPeakList::addDimension(const AtomType & c, PPM p)
{
	const Dimension dim = d_colors.size();
	d_colors.push_back( c );
	d_labels.push_back( c.getIsoLabel() );
	EasyPeakList::PeakList::iterator pos;
	if( p == 0.0 )
		p = c.getMedium();
	for( pos = d_peaks.begin(); pos != d_peaks.end(); ++pos )
		(*pos).second.d_point[ dim ] = p;
}

EasyPeakList::Selection EasyPeakList::find(const PpmCube & cube) const
{
#ifdef _DEBUG
	if( d_colors.size() != cube.size() )
		throw Root::Exception( "Dimension missmatch" );
#endif

	// TODO: ev. Performance verbessern durch Indizierung
	Selection res;
	EasyPeakList::PeakList::const_iterator pos;
	for( pos = d_peaks.begin(); pos != d_peaks.end(); ++pos )
	{
		if( cube.contains( (*pos).second.d_point ) )
			res.push_back( (*pos).second.d_id );
	}
	return res;
}

const EasyPeak& EasyPeakList::getPeak(Index id) const
{
	EasyPeakList::PeakList::const_iterator pos = d_peaks.find( id );
	if( pos != d_peaks.end() )
		return (*pos).second;
	else
		throw Root::Exception( "Peak not known" );
	return s_dummy;
}

void EasyPeakList::deletePoint(Index id)
{
	if( d_peaks.find( id ) == d_peaks.end() )
		return;
	d_dirty = true;
	d_peaks.erase( id );	// Die ID bleibt reserviert.
	PointSet::Update msg( this, PointSet::Update::Delete, id );
	notifyObservers( msg );
}

EasyPeakList::Selection EasyPeakList::findAll() const
{
	Selection res( d_peaks.size() );
	EasyPeakList::PeakList::const_iterator pos;
	Index i = 0;
	for( pos = d_peaks.begin(); pos != d_peaks.end(); ++pos )
	{
		res[ i++ ] = (*pos).second.d_id;
	}
	return res;
}

void EasyPeakList::saveToFile(const char * p)
{
	if( p != 0 && *p != 0 )
		d_filePath = p;

	writePeakList();

	d_dirty = false;
}

void EasyPeakList::writePeakList()
{
	// NOTE: Routine aus XEASY read_peaks.c mit Anpassungen.

	FILE *fp;
	fp = ::fopen( d_filePath.data(), "w" );
	if( fp == NULL )
		throw Root::Exception( "Can't open file for output" );

	Dimension dimCount = d_colors.size();
	Dimension d;

	::fprintf( fp, "# Number of dimensions %1d\n", dimCount );
	for( d = 0; d < dimCount; d++ )
		fprintf(fp, "#INAME %1d %s\n", d + 1, d_colors[ d ].getIsoLabel() );

	PeakList::const_iterator pos;
	for( pos = d_peaks.begin(); pos != d_peaks.end(); ++pos )
	{
		const EasyPeak& peak = (*pos).second;

		::fprintf( fp, "%4d", (*pos).first );	// ID

		for( d = 0; d < dimCount; d++ )
			::fprintf( fp, " %7.3f", peak.getFreq( d ) );	

#ifdef _EasyPeak_options_
		::fprintf( fp, " %1d %-9s %10.3e %9.2e %c %3d",
            int( peak.d_color ), peak.d_specType.data(), peak.d_volume,
            peak.d_volError, peak.d_integrationMethod, 0 );

#else
		::fprintf( fp, " %1d %-9s %10.3e %9.2e %c %3d",
            int( peak.d_color ), "U", peak.d_volume,
            0.0, '-', 0 );

#endif

#ifdef _EasyPeak_atoms_
		for( d = 0; d < dimCount; d++ )
			::fprintf( fp, " %4d", peak.d_atoms[ d ] );
#else
		for( d = 0; d < dimCount; d++ )
			::fprintf( fp, " %4d", 0 );
#endif

		::fprintf( fp, " 0\n" );

		if( !peak.d_comment.isEmpty() )
			::fprintf( fp, "# %s\n", peak.d_comment.data() );
	}

	::fclose( fp );
}

void EasyPeak::init(Dimension d)
{
	d_id = 0;
	d_volume = 0;
	d_color = 0;
#ifdef _EasyPeak_options_
	d_volError = 0;
	d_integrationMethod = 0;
	d_specType = "U";	// RISK
#endif
#ifdef _EasyPeak_atoms_
	// TEST d_atoms.assign( d, 0 );
#endif
#ifdef _EasyPeak_amplitude_
	d_level = 0;
#endif
	PpmPoint::checkDimCount( d );
	// d_point.assign( d, 0 );
}

Dimension EasyPeakList::getDimCount() const
{
	return d_colors.size();
}

AtomType EasyPeakList::getColor( Dimension d ) const
{
	return d_colors[ d ];
}

// TODO: merge

Amplitude EasyPeakList::getVolume(Index id) const
{
	EasyPeakList::PeakList::const_iterator pos = d_peaks.find( id );
	if( pos != d_peaks.end() )
		return (*pos).second.d_volume;
	else
		throw Root::Exception( "Peak not known" );
	return 0;
}

void EasyPeakList::setVolume( Index id, Amplitude a)
{
	EasyPeakList::PeakList::iterator pos = d_peaks.find( id );
	if( pos != d_peaks.end() )
		(*pos).second.d_volume = a;
	else
		throw Root::Exception( "Peak not known" );
	d_dirty = true;
	PointSet::Update msg( this, PointSet::Update::Change, id );
	notifyObservers( msg );
}

#ifdef _EasyPeak_amplitude_
Amplitude EasyPeakList::getAmplitude(Index id) const
{
	EasyPeakList::PeakList::const_iterator pos = d_peaks.find( id );
	if( pos != d_peaks.end() )
		return (*pos).second.d_level;
	else
		throw Root::Exception( "Peak not known" );
	return 0;
}

void EasyPeakList::setAmplitude( Index id, Amplitude a)
{
	EasyPeakList::PeakList::iterator pos = d_peaks.find( id );
	if( pos != d_peaks.end() )
		(*pos).second.d_level = a;
	else
		throw Root::Exception( "Peak not known" );
	d_dirty = true;
	PointSet::Update msg( this, PointSet::Update::Change, id );
	notifyObservers( msg );
}
#endif

const PointSet::Assig& EasyPeakList::getAssig(Index id ) const
{
#ifdef _EasyPeak_atoms_
	EasyPeakList::PeakList::const_iterator pos = d_peaks.find( id );
	if( pos != d_peaks.end() )
	{
		if( true ) // TEST !(*pos).second.d_atoms.empty() )
			return (*pos).second.d_atoms;
	}else
		throw Root::Exception( "Peak not known" );
#endif
	return PointSet::getAssig( id );
}

void EasyPeakList::setAssig( Index id, const Assig& a )
{
#ifdef _EasyPeak_atoms_
	if( false ) // TEST a.size() != d_colors.size() )
		throw Root::Exception( "Wrong number of dimensions!" );
	EasyPeakList::PeakList::iterator pos = d_peaks.find( id );
	if( pos != d_peaks.end() )
	{
		(*pos).second.d_atoms = a;
	}else
		throw Root::Exception( "Peak not known" );
	PointSet::Update msg( this, PointSet::Update::Change, id );
	notifyObservers( msg );
#endif
}

Root::UInt8 EasyPeakList::getCode( Index id ) const
{
	EasyPeakList::PeakList::const_iterator pos = d_peaks.find( id );
	if( pos != d_peaks.end() )
	{
		return (*pos).second.d_color;
	}else
		throw Root::Exception( "Peak not known" );
	return 0;
}

void EasyPeakList::setCode(Index id , Root::UInt8 c)
{
	EasyPeakList::PeakList::iterator pos = d_peaks.find( id );
	if( pos != d_peaks.end() )
	{
		(*pos).second.d_color = c;
	}else
		throw Root::Exception( "Peak not known" );
	PointSet::Update msg( this, PointSet::Update::Change, id );
	notifyObservers( msg );
}
