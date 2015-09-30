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

#ifndef _LUA_LUASCRIPT
#define _LUA_LUASCRIPT

#include <Root/Resource.h>
#include <Root/SymbolString.h>

namespace Lua
{
    class Script : public Root::Resource
	{
	public:
        Script( const char* name );

		virtual const QByteArray& getCode() const { return d_code; }
		virtual void setCode( const char*, bool compiled = false );

		const QByteArray& getName() const { return d_name; }
		bool isCompiled() const { return d_compiled; }
		void setCompiled();

	protected:
        virtual ~Script();
	private:
		QByteArray d_code;
		QByteArray d_name;
		bool d_compiled;
	};
}

#endif // _QTL_LUASCRIPT
