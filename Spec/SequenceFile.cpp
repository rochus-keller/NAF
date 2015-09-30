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

#include "SequenceFile.h"
#include <Spec/Repository.h>
#include <qstring.h> // qDebug
#include <fstream>
#include <stdio.h>
#include <Root/Vector.h>
using namespace Spec;

void SequenceFile::loadFromFile(Repository* rep, const char *path, Root::Index start)
{
	const int MAXLINE = 80;
	FILE* fp;
	char line[ MAXLINE ];
	char name[ 16 ];
	char comment[ MAXLINE ];

	fp = ::fopen( path, "r");
	if( fp == NULL )
		throw Root::Exception( "Cannot open file for input" );

	assert( rep );
	ResidueType* rt = 0;
	Slot resi;
	
	int id = (start)?start:1; 
	int nr = 0, lastNr = -999999999; // RISK
	int hit;
	d_hasSys = false;
	d_hasId = false;

	while( true ) 
	{
		if( feof( fp ) )
			break;
		if( ::fgets( line, MAXLINE, fp ) == 0 )
			break;
		if( line[0] != '#' ) 
		{
			hit = ::sscanf( line, "%s %d %d \"%[^\"\r\n]\"", 
				name, &nr, &resi.d_sys, comment );
			if( hit < 3 )
				resi.d_sys = 0; // XEASY verwendet -1 stattdessen
			else
				d_hasSys = true;

			if( hit >= 2 ) 
			{
				if( start == 0 )
					id = nr;
				d_hasId = true;
			}else if( start == 0 )
			{
				// hit < 2: Die Zeile enthält keine IDs
				// Caller hat mit start==0 mitgeteilt, dass id irgend einen Wert haben darf.
				nr = id;
			}else
				nr++;

			if( nr <= lastNr )
				throw Root::Exception( "Residue numbers not in ascending order: ", line );
			lastNr = nr;

			if( d_seq.find( id ) != d_seq.end() )
			{
				::fclose( fp );
				throw Root::Exception( "Residue number not unique within sequence: ", line );
			}
			if( hit > 0 )
			{
				rt = rep->findResidueType( name );
				if( rt == 0 )
				{
					::fclose( fp ); 
					throw Root::Exception( "Invalid residue type: ", name );
				}
				resi.d_resi = new Residue( id, rt, nr );
				if( hit == 4 )
					resi.d_resi->setFieldValue( "Comment", comment );
				d_seq[ id ] = resi;
				id++;
			}else
				qDebug( "SequenceFile Warning: Empty Line at Residue %d", id );
		}
	}                         
	::fclose( fp );

}

SequenceFile::SequenceFile()
{
}

bool SequenceFile::checkSystems() const
{
	Slots::const_iterator p;
	std::set<Root::Index> test;
	for( p = d_seq.begin(); p != d_seq.end(); ++p )
	{
		if( (*p).second.d_sys != 0 )
		{
			if( test.count( (*p).second.d_sys ) != 0 )
				return false;
			else
				test.insert( (*p).second.d_sys );
		}
	}
	return true;
}

bool SequenceFile::addResidue(Residue * r, Root::Index i)
{
	assert( r );
	if( d_seq.count( r->getId() ) != 0 )
		return false;
	d_seq[ r->getId() ].d_resi = r;
	if( i > 0 )
		d_seq[ r->getId() ].d_sys = i;
	return true;
}

void SequenceFile::writeToFile(const char * path, bool useNr)
{
	std::ofstream out( path );
	if( !out )
		throw Root::Exception( "Cannot open file for output" );

	Slots::const_iterator p;
	for( p = d_seq.begin(); p != d_seq.end(); ++p )
	{
		out << (*p).second.d_resi->getType()->getShort().data() << "  ";
		if( useNr )
			out << (*p).second.d_resi->getNr() << "   ";
		else
			out << (*p).second.d_resi->getId() << "   ";
		if( (*p).second.d_sys )
			out << (*p).second.d_sys;
		out << std::endl;
	}
}

void SequenceFile::renumber()
{
	Slots tmp = d_seq;
	d_seq.clear();
	Slots::iterator p;
	Root::Index i = 1;
	for( p = tmp.begin(); p != tmp.end(); ++p, i++ )
	{
		(*p).second.d_sys = (*p).first;
		d_seq[ i ] = (*p).second;
	}
	d_hasSys = true;
}

void SequenceFile::removeResidue(Root::Index i)
{
	d_seq.erase( i );
}

void SequenceFile::addResidue(Repository * rep, Root::Index id, Root::SymbolString name )
{
	if( d_seq.find( id ) != d_seq.end() )
		throw Root::Exception( "Residue numbers not unique within sequence" );
	ResidueType* rt = rep->findResidueType( name );
	if( rt == 0 )
		throw Root::Exception( "Invalid residue type: ", name );
	d_seq[ id ].d_resi = new Residue( id, rt );
}
