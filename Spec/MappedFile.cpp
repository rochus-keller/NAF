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

// Dies ist die Implementation für Win32

#include "MappedFile.h"
using namespace Spec;
using namespace Spec::Private;

/****************************************************************/

#ifdef _WIN32

#include <windows.h>

class _MappedFileImp
{
public:
	~_MappedFileImp();
	_MappedFileImp( const char* path, bool readwrite );

	HANDLE d_file;
	HANDLE d_mapping;
	Root::Byte* d_start;
	DWORD d_size;
	DWORD d_cursor;
	bool d_ok;
};

_MappedFileImp::_MappedFileImp(const char * path, bool readwrite)
{
	DWORD access = GENERIC_READ;
	DWORD protect = PAGE_READONLY;
	DWORD access2 = FILE_MAP_READ;
	if( readwrite )
	{
		access |= GENERIC_WRITE;
		protect = PAGE_READWRITE;
		access2 |= FILE_MAP_WRITE;
	}

	d_file = ::CreateFile( path, access, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );

	if( d_file == INVALID_HANDLE_VALUE )
	{
		throw Root::Exception( "Cannot open file ", path );
	}
	d_mapping = ::CreateFileMapping( d_file, NULL, protect, 0, 0, NULL );
	if( d_mapping == NULL )
		throw Root::Exception( "Cannot create memory mapping for ", path );

	SYSTEM_INFO si;
	::GetSystemInfo( &si );
	d_size = ::GetFileSize( d_file, NULL );

	DWORD bytesToMap = 0; // 0: Mappe gesamte Datei
	if( d_size < si.dwAllocationGranularity )
		bytesToMap = d_size;
	else
		bytesToMap = 0; //si.dwAllocationGranularity; // * 5; // Eine Allocation entspricht 64KByte.
			//. RISK: unter Win98 läufts nur so. Andernfalls gibts eine Exception ab 64k.
	d_start = (Root::Byte*) ::MapViewOfFile( d_mapping, access2 | PAGE_WRITECOPY, 0, 0, bytesToMap ); 
	if( d_start == 0 )
	{
		throw Root::Exception( "Not enough memory available for mapping file ", path );
	}
	d_cursor = 0;
	d_ok = true;
}

_MappedFileImp::~_MappedFileImp()
{
	::UnmapViewOfFile( d_start );
	::CloseHandle( d_mapping );
	::CloseHandle( d_file );
}

/****************************************************************/

#else // _WIN32

#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "MappedFile.h"

class _MappedFileImp
{
public:
	~_MappedFileImp();
	_MappedFileImp( const char* path, bool readwrite );

	FILE* d_file;
	Root::Byte* d_start;
	int d_size;
	int d_cursor;
	bool d_ok;
};

_MappedFileImp::_MappedFileImp(const char * path, bool readwrite)
{
	d_file = ::fopen( path, readwrite ? "r+b" : "rb" );
	if( d_file == 0 )
		throw Root::Exception( "Cannot open file ", path );
    ::fseek( d_file, 0, 2 );
    d_size = ::ftell( d_file );

    d_start = (Root::Byte*) ::mmap( 0, d_size, PROT_READ, MAP_PRIVATE, fileno( d_file ), 0 );
	if( d_start == (void*) -1L )
	{
		::fclose( d_file );
		throw Root::Exception( "Cannot create memory mapping for ", path );
	}
	d_ok = true;
}

_MappedFileImp::~_MappedFileImp()
{
    ::munmap( (char*)d_start, d_size );		// Cast wegen Solaris
	::fclose( d_file );
}

#endif // _WIN32

/****************************************************************/

MappedFile::MappedFile( const char* path, bool readwrite )
{
	// Wird bereits in Stream aufgerufen: requestCleanup();
	d_this = new _MappedFileImp( path, readwrite );
}

MappedFile::~MappedFile()
{
	if( d_this )
		delete d_this;
}

Root::Int64 MappedFile::getSize() const
{
	return d_this->d_size;
}

Root::Byte MappedFile::getAt(Root::Int64 pos)
{
	if( pos >= d_this->d_size )
	{
		d_this->d_ok = false; // Error: Index überschreitet Dateigrösse
		return 0;
	}
	return *( d_this->d_start + pos );
}

void MappedFile::seek(Root::Int64 pos)	// not used
{
	if( pos >= d_this->d_size )
		d_this->d_ok = false; // Error: Index überschreitet Dateigrösse
	else
	{
		d_this->d_ok = true;
		d_this->d_cursor = pos;
	}
}

bool MappedFile::isOk() const
{
	return d_this->d_ok;	// used
}

void MappedFile::read(Root::Byte * buffer, Root::UInt32 length) // not used
{
    if( ( d_this->d_cursor + int(length) ) > d_this->d_size )
		d_this->d_ok = false; // Error: Index überschreitet Dateigrösse
	else
	{
		d_this->d_ok = true;
		::memcpy( buffer, d_this->d_start + d_this->d_cursor, length );
		d_this->d_cursor += length;
	}
}

void MappedFile::read(Root::Byte * buffer, Root::Int64 pos, Root::UInt32 length) // not used
{
	if( pos + length > d_this->d_size )
		d_this->d_ok = false; // Error: Index überschreitet Dateigrösse
	else
	{
		d_this->d_ok = true;
		::memcpy( buffer, d_this->d_start + pos, length );
		d_this->d_cursor = pos;
	}
}

Root::Byte MappedFile::get() // not used
{
	Root::Byte res = *( d_this->d_start + d_this->d_cursor );
	d_this->d_cursor++;
	return res;
}

const Root::Byte* MappedFile::getPtr() const
{
	return d_this->d_start;
}
