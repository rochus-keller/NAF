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

#if !defined(AFX_MEM_H__0BBFB21E_2101_4FC2_B995_7DF77B482075__INCLUDED_)
#define AFX_MEM_H__0BBFB21E_2101_4FC2_B995_7DF77B482075__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Units.h>

namespace Root
{
	class Mem  
	{
		Mem() {}
	public:
		static void init();
		static void* alloc( size_t );
		static void* realloc( void*, size_t );
		static void* allocAtomic( size_t );
		// Für Objekte, die keine GC-Pointer enthalten
		static void* allocExtern( size_t );	
		// Für Objekte, die GC-Pointer enthalten, aber selber nicht von GC gelöscht werden.
		static void free( void* );
		static void collect();
		static void enableGc();
		static void disableGc();

		static void addGuard(void **ptr); // registriere Variable, in der Pointer gespeichert
		static void removeGuard(void **ptr);
		static void changeGuard(void **ptr, void *obj);
		static void notifyGuard(void* obj); // Das Objekt teilt dem Guard mit, dass es gelöscht wird
											// Damit werden alle Variablen, die darauf zeigen, auf Null gesetzt.
	};
}

#endif // !defined(AFX_MEM_H__0BBFB21E_2101_4FC2_B995_7DF77B482075__INCLUDED_)
