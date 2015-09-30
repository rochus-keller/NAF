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

#include <QTemporaryFile>
#include <QString>
#include "QtlTemporaryFile.h"
#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>
#include "Variant.h"
using namespace std;
using namespace Qtl;
using namespace Lua;


int TemporaryFile::init(lua_State * L)
{
	return 0;
}
static const luaL_reg _TemporaryFile[] = 
{
	{ Util::s_init, TemporaryFile::init },
	{ 0, 0 }
};
void TemporaryFile::install(lua_State * L){
	ValueInstaller2<TemporaryFile>::install( L, _TemporaryFile, "QTemporaryFile" );
}
