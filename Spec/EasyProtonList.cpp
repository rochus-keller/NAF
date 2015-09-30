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

#include "EasyProtonList.h"
#include <stdio.h>
#include <qstring.h> // qDebug
#include <string.h>
#include <qfileinfo.h>
using namespace Spec;

GCC_IGNORE( -Wunused-result );

EasyProtonList::EasyProtonList(const char* filePath):
	d_filePath( filePath )
{
	readFromFile();
	QFileInfo info( d_filePath.data() );	// RISK
	d_filePath = info.baseName().toLatin1().data();

}

EasyProtonList::~EasyProtonList()
{

}

void EasyProtonList::readFromFile()
{
	int res;
	const int MAXLINE = 80;
	char line[MAXLINE];
	char label[16];
	float dummy1;
	Atom atom;
	FILE* fp;
	d_atoms.clear();

	fp = ::fopen( d_filePath.data(), "r");
	if( fp == NULL )
		throw Root::Exception( "Cannot open file for input" );
	
	::fgets(line, MAXLINE, fp);
	if( ::strlen( line ) > 40 )	// Es handelt sich um das alte Format
	{
		::fclose( fp );
		throw Root::Exception( "Invalid file format" );
	}
	::rewind( fp );
	while( !feof( fp ) ) 
	{
		line[0] = '\0';
		::fgets( line, MAXLINE, fp );
		// strcore( line );
		if( line[0] != '\0' && line[0] != '#' ) // Leerzeilen und Kommentare überlesen
		{
			res = ::sscanf(line, "%d %f %f %s %d",
				&atom.d_spin, &atom.d_shift, &dummy1,
				label, &atom.d_sys );
			if( res != 5 )
			{
				qWarning( "Cannot read line '%s'", line );
				continue;
			}
			atom.d_label = label;
			d_atoms.push_back( atom );
		}
	}
	::fclose( fp );
}

EasyProtonList::EasyProtonList(Root::Index count):
	d_atoms( count )
{

}

void EasyProtonList::writeToFile(const char *path)
{
	d_filePath = path;
	FILE* fp = ::fopen( path, "w");
	if( fp == NULL )
		throw Root::Exception( "Cannot open file for writing: ", path );

	for( Root::Index i = 0; i < d_atoms.size(); i++ )
	{
		if( d_atoms[ i ].isValid() )
			::fprintf( fp, "%4d %7.3f %5.3f %-5s %3d\n",
				d_atoms[ i ].d_spin, d_atoms[ i ].d_shift, 0.0,
				d_atoms[ i ].d_label.data(), d_atoms[ i ].d_sys );
	}
	::fclose( fp );
}
