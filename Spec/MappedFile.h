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

#if !defined(AFX_MAPPEDFILE_H__E924EFC0_AE6C_11D4_A4CC_00D00918E99C__INCLUDED_)
#define AFX_MAPPEDFILE_H__E924EFC0_AE6C_11D4_A4CC_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Spec/Stream.h>

class _MappedFileImp;

namespace Spec
{
	namespace Private
	{
		//* MappedFile
		//. Diese Klasse ist ein Layer über das Konzept der Memory-Mapped-Files und
		//. bietet eine plattformunabhängige Schnittstelle zu diesem Dienst.

		class MappedFile : public Stream
		{
		public:
			const Root::Byte* getPtr() const;
			Root::Byte get();
			void read( Root::Byte* buffer, Root::UInt32 length );
			void read( Root::Byte* buffer, Root::Int64 pos, Root::UInt32 length );
			bool isOk() const;
			void seek( Root::Int64 pos );
			Root::Byte getAt( Root::Int64 pos );
			Root::Int64 getSize() const;
			MappedFile( const char* path, bool readwrite = false );
			virtual ~MappedFile();
		private:
			_MappedFileImp* d_this;
		};
	}
}

#endif // !defined(AFX_MAPPEDFILE_H__E924EFC0_AE6C_11D4_A4CC_00D00918E99C__INCLUDED_)
