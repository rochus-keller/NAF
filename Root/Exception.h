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

#if !defined(AFX_EXCEPTION_H__0A995D20_B1CE_11D5_8DB1_00D00918E99C__INCLUDED_)
#define AFX_EXCEPTION_H__0A995D20_B1CE_11D5_8DB1_00D00918E99C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Units.h>
#include <exception>

namespace Root
{
	class Exception : public std::exception
	{
	public:
		// TODO Exception( const char* msg, ... );
		Exception();
		Exception(const char* what):d_what( what ) {}
		Exception(const char* what, const char* str ):d_what( what ) 
			{ d_what += str; }
		virtual ~Exception() throw();
		const char* what() const throw() { return d_what.data(); }
		const char* getMessage() const { return d_what.data(); }
	protected:
		QByteArray  d_what;
	};
}

#endif // !defined(AFX_EXCEPTION_H__0A995D20_B1CE_11D5_8DB1_00D00918E99C__INCLUDED_)
