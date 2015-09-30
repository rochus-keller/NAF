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

#if !defined(AFX_STREAM_H__D465E201_B8C2_11D4_A4CC_00D00918E99C__INCLUDED_)
#define AFX_STREAM_H__D465E201_B8C2_11D4_A4CC_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Spec/Units.h>
#include <QFile>

namespace Spec
{
	namespace Private
	{
		class Stream : public Root::Resource // wegen dispose und requestCleanup
		{
		public:
			Stream();
			virtual bool isOk() const { return false; }
			virtual void seek( Root::Int64 pos ) {}
			virtual void read( Root::Byte* buffer, Root::Int64 pos, Root::UInt32 length )
			{
				seek( pos );
				read( buffer, length );
			}
			virtual void read( Root::Byte* buffer, Root::UInt32 length ) {}
			virtual Root::Byte getAt( Root::Int64 pos )
			{ 
				seek( pos );
				return get();
			}
			Root::Byte operator[](Root::Int64 index ) { return getAt( index ); }
			virtual Root::Int64 getSize() const { return 0; }
			virtual Root::Byte get() { return 0; }
		protected:
			virtual ~Stream();
		};

		class FileStream : public Stream
		{
		public:
            FileStream( const char* path );
			bool isOk() const 
			{ 
				return d_stream.isOpen() && d_stream.isReadable();
			}
			void seek( Root::Int64 pos )
			{
				d_stream.seek( pos );
			}
			void read( Root::Byte* buffer, Root::UInt32 length )
			{
				d_stream.read( (char*) buffer, length );
			}
			Root::Byte get();
            Root::Int64 getSize() const { return d_stream.size(); }
		protected:
			~FileStream() {}
		private:
			QFile d_stream;
		};
	}
}

#endif // !defined(AFX_STREAM_H__D465E201_B8C2_11D4_A4CC_00D00918E99C__INCLUDED_)
