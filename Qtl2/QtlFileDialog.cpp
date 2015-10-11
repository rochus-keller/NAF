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
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;

int FileDialog::init(lua_State * L)
{
    return DefaultCreateObject<QFileDialog>::init( L );
}
int FileDialog::directory(lua_State * L) // const : QDir
{
	QFileDialog* obj = QtObject<QFileDialog>::check( L, 1);
	QDir* res = QtValue<QDir>::create( L );
	*res = 	obj->directory();
	return 1;
}
int FileDialog::filters(lua_State * L) // const : QStringList
{
	QFileDialog* obj = QtObject<QFileDialog>::check( L, 1);
	QStringList* res = QtValue<QStringList>::create( L );
	*res = 	obj->filters();
	return 1;
}
int FileDialog::history(lua_State * L) // const : QStringList
{
	QFileDialog* obj = QtObject<QFileDialog>::check( L, 1);
	QStringList* res = QtValue<QStringList>::create( L );
	*res = 	obj->history();
	return 1;
}
int FileDialog::iconProvider(lua_State * L) // const : QFileIconProvider *
{
	//QFileDialog* obj = QtObject<QFileDialog>::check( L, 1);
    // TODO
//	QFileIconProvider* res = QtValue<QFileIconProvider>::create( L );
//	res = obj->iconProvider();
    Util::push(L);
	return 1;

}
int FileDialog::isReadOnly(lua_State * L) // const : bool
{
	QFileDialog* obj = QtObject<QFileDialog>::check( L, 1);
	Lua::Util::push( L, obj->isReadOnly() );
	return 1;
}/*
int FileDialog::itemDelegate(lua_State * L) // const : QAbstractItemDelegate *
{
	QFileDialog* obj = QtObject<QFileDialog>::check( L, 1);
	QAbstractItemDelegate* res = QtObject<QAbstractItemDelegate>::create( L );
	res = obj->itemDelegate();
	return 1;

}*/
int FileDialog::labelText(lua_State * L) // (DialogLabel label)const : QString
{
	QFileDialog* obj = QtObject<QFileDialog>::check( L, 1);
    QFileDialog::DialogLabel label = (QFileDialog::DialogLabel)Util::toInt( L, 2 );
    *QtValue<QString>::create( L ) = obj->labelText( label );
	return 1;
}
int FileDialog::selectFile(lua_State * L) // const QString & filename
{
	QFileDialog* obj = QtObject<QFileDialog>::check( L, 1);
	//QString* filename = QtValue<QString>::check( L, 2);
    obj->selectFile( QtValueBase::toString( L, 2 ) );
	return 0; 
}
int FileDialog::selectFilter(lua_State * L) // const QString & filter
{
	QFileDialog* obj = QtObject<QFileDialog>::check( L, 1);
	//QString* filter = QtValue<QString>::check( L, 2);
    obj->selectFilter( QtValueBase::toString( L, 2 ) );
	return 0; 
}
int FileDialog::selectedFiles(lua_State * L) // const : QStringList
{
	QFileDialog* obj = QtObject<QFileDialog>::check( L, 1);
	QStringList* res = QtValue<QStringList>::create( L );
	*res = obj->selectedFiles();
	return 1; 
}
int FileDialog::selectedFilter(lua_State * L) // const : QString
{
	QFileDialog* obj = QtObject<QFileDialog>::check( L, 1);
    *QtValue<QString>::create( L ) = obj->selectedFilter();
	return 1; 
}

int FileDialog::setDirectory(lua_State * L) // const QString & directory, const QDir & directory
{
	QFileDialog* obj = QtObject<QFileDialog>::check( L, 1);
	//if(	QString* directory = QtValue<QString>::cast( L, 2))
	if( Util::isStr( L, 2 ) )
	{
        obj->setDirectory( QtValueBase::toString( L, 2 ) );
	}else if(QDir* directory = QtValue<QDir>::cast( L, 2))
	{
		obj->setDirectory( *directory );
	}
	return 0; 
}
int FileDialog::setFilter(lua_State * L) // const QString & filter
{
	QFileDialog* obj = QtObject<QFileDialog>::check( L, 1);
	//QString* filter = QtValue<QString>::check( L, 2);
    obj->setFilter( QtValueBase::toString( L, 2 ) );
	return 0; 
}
int FileDialog::setFilters(lua_State * L) // const QStringList & filters
{
	QFileDialog* obj = QtObject<QFileDialog>::check( L, 1);
	QStringList* filters = QtValue<QStringList>::check( L, 2);
	obj->setFilters( *filters );
	return 0; 
}
int FileDialog::setHistory(lua_State * L) // const QStringList & paths
{
	QFileDialog* obj = QtObject<QFileDialog>::check( L, 1);
	QStringList* paths = QtValue<QStringList>::check( L, 2);
	obj->setHistory( *paths );
	return 0; 
}
int FileDialog::setIconProvider(lua_State * L) // QFileIconProvider * provider
{
	QFileDialog* obj = QtObject<QFileDialog>::check( L, 1);
	QFileIconProvider* provider = QtValue<QFileIconProvider>::check( L, 2);
	obj->setIconProvider( provider );
	return 0; 
}
int FileDialog::setItemDelegate(lua_State * L) // QAbstractItemDelegate * delegate
{
	QFileDialog* obj = QtObject<QFileDialog>::check( L, 1);
	QAbstractItemDelegate* delegate = QtValue<QAbstractItemDelegate>::check( L, 2);
	obj->setItemDelegate( delegate );
	return 0; 
}
int FileDialog::setLabelText(lua_State * L) // DialogLabel label, const QString & text
{
	QFileDialog* obj = QtObject<QFileDialog>::check( L, 1);
    QFileDialog::DialogLabel label = (QFileDialog::DialogLabel)Util::toInt( L, 2 );
    obj->setLabelText( label , QtValueBase::toString( L, 2 ) );
	return 0;
}

static const luaL_reg _FileDialog[] = 
{
	{ "init", FileDialog::init },
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
    QtObject<QFileDialog,QDialog>::install( L, "QFileDialog", _FileDialog );
}
