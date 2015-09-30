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

#if !defined(AFX_VARIANT_H__9698B45F_12D9_4DC2_8A40_D2CC004D1184__INCLUDED_)
#define AFX_VARIANT_H__9698B45F_12D9_4DC2_8A40_D2CC004D1184__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Units.h>

typedef struct lua_State lua_State;

namespace Qtl
{
	class Variant  
	{
	public:
		static const char* multiply;
		static const char* divide;
		static const char* add;
		static const char* subtract;
		static const char* intersectedAssign;
		static const char* unitedAssign;
		static const char* notEquals;
		static const char* subtractedAssign;
		static const char* xoredAssign;
		static const char* lessThan;
		static const char* lessThanEqualTo;
		static const char* greaterThan;
		static const char* greaterThanEqualTo;
		static QVariant toVariant( lua_State *L, int n );
		static void push( lua_State * L, const QVariant& v );
		static void install(lua_State * L);

	};

	/* Keine Vererbung zulassen
	class PaintDevice
	{
	public:
		static int depth(lua_State * L); // () const : int 
		static int height(lua_State * L); // () const : int 
		static int heightMM(lua_State * L); // () const : int 
		static int logicalDpiX(lua_State * L); // () const : int 
		static int logicalDpiY(lua_State * L); // () const : int 
		static int numColors(lua_State * L); // () const : int 
		static int paintEngine(lua_State * L); // () const : QPaintEngine * 
		static int paintingActive(lua_State * L); // () const : bool 
		static int width(lua_State * L); // () const : int 
		static int widthMM(lua_State * L); // () const : int 
	};
	*/



}

#endif // !defined(AFX_VARIANT_H__9698B45F_12D9_4DC2_8A40_D2CC004D1184__INCLUDED_)
