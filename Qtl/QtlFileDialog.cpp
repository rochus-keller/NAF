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

#include <QFileDialog>
#include <QDir>
#include <QString>
#include <QFileIconProvider>
#include <QAbstractItemDelegate>
#include "QtlFileDialog.h"
#include "Enums.h"
#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;

GCC_IGNORE(-Wunused-variable);

int FileDialog::init(lua_State * L)
{
	return 0;
}
int FileDialog::directory(lua_State * L) // const : QDir
{
	QFileDialog* obj = ObjectHelper<QFileDialog>::check( L, 1);
	QDir* res = ValueInstaller2<QDir>::create( L );
	*res = 	obj->directory();
	return 1;
}
int FileDialog::filters(lua_State * L) // const : QStringList
{
	QFileDialog* obj = ObjectHelper<QFileDialog>::check( L, 1);
	QStringList* res = ValueInstaller2<QStringList>::create( L );
	*res = 	obj->filters();
	return 1;
}
int FileDialog::history(lua_State * L) // const : QStringList
{
	QFileDialog* obj = ObjectHelper<QFileDialog>::check( L, 1);
	QStringList* res = ValueInstaller2<QStringList>::create( L );
	*res = 	obj->history();
	return 1;
}
int FileDialog::iconProvider(lua_State * L) // const : QFileIconProvider *
{
	QFileDialog* obj = ObjectHelper<QFileDialog>::check( L, 1);
    // TODO
//	QFileIconProvider* res = ValueInstaller2<QFileIconProvider>::create( L );
//	res = obj->iconProvider();
    Util::push(L);
	return 1;

}
int FileDialog::isReadOnly(lua_State * L) // const : bool
{
	QFileDialog* obj = ObjectHelper<QFileDialog>::check( L, 1);
	Lua::Util::push( L, obj->isReadOnly() );
	return 1;
}/*
int FileDialog::itemDelegate(lua_State * L) // const : QAbstractItemDelegate *
{
	QFileDialog* obj = ObjectHelper<QFileDialog>::check( L, 1);
	QAbstractItemDelegate* res = ObjectHelper<QAbstractItemDelegate>::create( L );
	res = obj->itemDelegate();
	return 1;

}*/
int FileDialog::labelText(lua_State * L) // (DialogLabel label)const : QString
{
	QFileDialog* obj = ObjectHelper<QFileDialog>::check( L, 1);
	Enums enums(L);
	QFileDialog::DialogLabel label = (QFileDialog::DialogLabel)enums.DialogLabel( 2 );
	//QString* res = ValueInstaller2<QString>::create( L );
	//*res = obj->labelText( label );
	Util::push( L, obj->labelText( label ) );
	return 1;
}
int FileDialog::selectFile(lua_State * L) // const QString & filename
{
	QFileDialog* obj = ObjectHelper<QFileDialog>::check( L, 1);
	//QString* filename = ValueInstaller2<QString>::check( L, 2);
	obj->selectFile( Util::toString( L, 2 ) );
	return 0; 
}
int FileDialog::selectFilter(lua_State * L) // const QString & filter
{
	QFileDialog* obj = ObjectHelper<QFileDialog>::check( L, 1);
	//QString* filter = ValueInstaller2<QString>::check( L, 2);
	obj->selectFilter( Util::toString( L, 2 ) );
	return 0; 
}
int FileDialog::selectedFiles(lua_State * L) // const : QStringList
{
	QFileDialog* obj = ObjectHelper<QFileDialog>::check( L, 1);
	QStringList* res = ValueInstaller2<QStringList>::create( L );
	*res = obj->selectedFiles();
	return 1; 
}
int FileDialog::selectedFilter(lua_State * L) // const : QString
{
	QFileDialog* obj = ObjectHelper<QFileDialog>::check( L, 1);
	//QString* res = ValueInstaller2<QString>::create( L );
	//*res = obj->selectedFilter();
	Util::push( L, obj->selectedFilter() );
	return 1; 
}

int FileDialog::setDirectory(lua_State * L) // const QString & directory, const QDir & directory
{
	QFileDialog* obj = ObjectHelper<QFileDialog>::check( L, 1);
	//if(	QString* directory = ValueInstaller2<QString>::cast( L, 2))
	if( Util::isStr( L, 2 ) )
	{
		obj->setDirectory( Util::toString( L, 2 ) );
	}else if(QDir* directory = ValueInstaller2<QDir>::cast( L, 2))
	{
		obj->setDirectory( *directory );
	}
	return 0; 
}
int FileDialog::setFilter(lua_State * L) // const QString & filter
{
	QFileDialog* obj = ObjectHelper<QFileDialog>::check( L, 1);
	//QString* filter = ValueInstaller2<QString>::check( L, 2);
	obj->setFilter( Util::toString( L, 2 ) );
	return 0; 
}
int FileDialog::setFilters(lua_State * L) // const QStringList & filters
{
	QFileDialog* obj = ObjectHelper<QFileDialog>::check( L, 1);
	QStringList* filters = ValueInstaller2<QStringList>::check( L, 2);
	obj->setFilters( *filters );
	return 0; 
}
int FileDialog::setHistory(lua_State * L) // const QStringList & paths
{
	QFileDialog* obj = ObjectHelper<QFileDialog>::check( L, 1);
	QStringList* paths = ValueInstaller2<QStringList>::check( L, 2);
	obj->setHistory( *paths );
	return 0; 
}
int FileDialog::setIconProvider(lua_State * L) // QFileIconProvider * provider
{
	QFileDialog* obj = ObjectHelper<QFileDialog>::check( L, 1);
	QFileIconProvider* provider = ValueInstaller2<QFileIconProvider>::check( L, 2);
	obj->setIconProvider( provider );
	return 0; 
}
int FileDialog::setItemDelegate(lua_State * L) // QAbstractItemDelegate * delegate
{
	QFileDialog* obj = ObjectHelper<QFileDialog>::check( L, 1);
	QAbstractItemDelegate* delegate = ValueInstaller2<QAbstractItemDelegate>::check( L, 2);
	obj->setItemDelegate( delegate );
	return 0; 
}
int FileDialog::setLabelText(lua_State * L) // DialogLabel label, const QString & text
{
	QFileDialog* obj = ObjectHelper<QFileDialog>::check( L, 1);
	Enums enums(L);
	QFileDialog::DialogLabel label = (QFileDialog::DialogLabel)enums.DialogLabel( 2 );
	//QString* text = ValueInstaller2<QString>::check( L, 2);
	obj->setLabelText( label , Util::toString( L, 2 ) );
	return 0;
}

static const luaL_reg _FileDialog[] = 
{
	{ Util::s_init, FileDialog::init },
	{ "directory", FileDialog::directory },
	{ "filters", FileDialog::filters },
	{ "history", FileDialog::history },
	{ "iconProvider", FileDialog::iconProvider },
	{ "isReadOnly", FileDialog::isReadOnly },
	//{ "itemDelegate", FileDialog::itemDelegate },
	{ "labelText", FileDialog::labelText },
	{ "selectFile", FileDialog::selectFile },
	{ "selectFilter", FileDialog::selectFilter },
	{ "selectedFiles", FileDialog::selectedFiles },
	{ "selectedFilter", FileDialog::selectedFilter },
	{ "setDirectory", FileDialog::setDirectory },
	{ "setFilter", FileDialog::setFilter },
	{ "setFilters", FileDialog::setFilters },
	{ "setHistory", FileDialog::setHistory },
	{ "setIconProvider", FileDialog::setIconProvider },
	{ "setItemDelegate", FileDialog::setItemDelegate },
	{ "setLabelText", FileDialog::setLabelText },
	{ 0, 0 }
};
void FileDialog::install(lua_State * L){
	ObjectInstaller<QFileDialog>::install( L, _FileDialog, ObjectInstaller<QDialog>::className());
}
