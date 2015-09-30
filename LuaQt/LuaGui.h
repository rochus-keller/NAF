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

#if !defined(AFX_LUAQTFACTORY_H__DB015C57_A25B_4FE7_A525_14A3B499B457__INCLUDED_)
#define AFX_LUAQTFACTORY_H__DB015C57_A25B_4FE7_A525_14A3B499B457__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Script/LuaEngine.h>

namespace Spec
{
	class LuaGui  
	{
	public:
		static lua_State* getCtx();
		static Spec::LuaEngine* getLua();
		static int install( Spec::LuaEngine* );
		
	};
}

#endif // !defined(AFX_LUAQTFACTORY_H__DB015C57_A25B_4FE7_A525_14A3B499B457__INCLUDED_)
