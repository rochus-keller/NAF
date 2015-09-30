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

#if !defined(_QTL_FILEDIALOG)
#define _QTL_FILEDIALOG

typedef struct lua_State lua_State;




namespace Qtl
{
	class FileDialog
	{
	public:	
		static int init(lua_State * L);
		static int directory(lua_State * L); // const : QDir
		static int filters(lua_State * L); // const : QStringList
		static int history(lua_State * L); // const : QStringList
		static int iconProvider(lua_State * L); // const : QFileIconProvider *
		static int isReadOnly(lua_State * L); // const : bool
		//static int itemDelegate(lua_State * L); // const : QAbstractItemDelegate *
		static int labelText(lua_State * L); // (DialogLabel label)const : QString
		static int selectFile(lua_State * L); // const QString & filename
		static int selectFilter(lua_State * L); // const QString & filter
		static int selectedFiles(lua_State * L); // const : QStringList
		static int selectedFilter(lua_State * L); // const : QString
		static int setDirectory(lua_State * L); // const QString & directory
		static int setFilter(lua_State * L); // const QString & filter
		static int setFilters(lua_State * L); // const QStringList & filters
		static int setHistory(lua_State * L); // const QStringList & paths
		static int setIconProvider(lua_State * L); // QFileIconProvider * provider
		static int setItemDelegate(lua_State * L); // QAbstractItemDelegate * delegate
		static int setLabelText(lua_State * L); // DialogLabel label, const QString & text
		static void install(lua_State * L);
		
	};

}

#endif // !defined(_QTL_FILEDIALOG)
