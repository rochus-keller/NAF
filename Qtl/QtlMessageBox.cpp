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

#include <QMessageBox>
#include <QAbstractButton>
#include <QPushButton>
#include "QtlMessageBox.h"
#include <Script/ObjectInstaller.h>
#include <Script/ValueInstaller.h>
using namespace Qtl;
using namespace Lua;
int MessageBox::addButton(lua_State * L) // ( QAbstractButton * button, ButtonRole role )
// ( StandardButton button ) QPushButton *
// ( const QString & text, ButtonRole role ) QPushButton *
{
	QMessageBox* obj = ObjectHelper<QMessageBox>::check( L, 1);
	if( QAbstractButton* button = ObjectHelper<QAbstractButton>::cast( L, 2) )
	{
		QMessageBox::ButtonRole f;
		int fInt = Util::toInt( L, 3 );
		if( (fInt >= -1 && fInt <= 9) )
		{
			f = (QMessageBox::ButtonRole) fInt;
			obj->addButton( button, f );
		}
		else
			Lua::Util::error( L, "expecting a valid QMessageBox::ButtonRole" );
	}
	else if(Util::isNum( L, 2 ))
	{
		QMessageBox::StandardButton f;
		switch( Util::toInt( L, 2 ) )
		{
		case QMessageBox::NoButton: 
			f = QMessageBox::NoButton;
			ObjectPeer::pushPeer( L, obj->addButton( f ), true );
			break;
		case QMessageBox::Ok: 
			f = QMessageBox::Ok;
			ObjectPeer::pushPeer( L, obj->addButton( f ), true );
			break;
		case QMessageBox::Save: 
			f = QMessageBox::Save;
			ObjectPeer::pushPeer( L, obj->addButton( f ), true );
			break;
		case QMessageBox::SaveAll: 
			f = QMessageBox::SaveAll;
			ObjectPeer::pushPeer( L, obj->addButton( f ), true );
			break;
		case QMessageBox::Open: 
			f = QMessageBox::Open;
			ObjectPeer::pushPeer( L, obj->addButton( f ), true );
			break;
		case QMessageBox::Yes: 
			f = QMessageBox::Yes;
			ObjectPeer::pushPeer( L, obj->addButton( f ), true );
			break;
		case QMessageBox::YesToAll: 
			f = QMessageBox::YesToAll;
			ObjectPeer::pushPeer( L, obj->addButton( f ), true );
			break;
		case QMessageBox::No: 
			f = QMessageBox::No;
			ObjectPeer::pushPeer( L, obj->addButton( f ), true );
			break;
		case QMessageBox::NoToAll: 
			f = QMessageBox::NoToAll;
			ObjectPeer::pushPeer( L, obj->addButton( f ), true );
			break;
		case QMessageBox::Abort: 
			f = QMessageBox::Abort;
			ObjectPeer::pushPeer( L, obj->addButton( f ), true );
			break;
		case QMessageBox::Retry: 
			f = QMessageBox::Retry;
			ObjectPeer::pushPeer( L, obj->addButton( f ), true );
			break;
		case QMessageBox::Ignore: 
			f = QMessageBox::Ignore;
			ObjectPeer::pushPeer( L, obj->addButton( f ), true );
			break;
		case QMessageBox::Close: 
			f = QMessageBox::Close;
			ObjectPeer::pushPeer( L, obj->addButton( f ), true );
			break;
		case QMessageBox::Cancel: 
			f = QMessageBox::Cancel;
			ObjectPeer::pushPeer( L, obj->addButton( f ), true );
			break;
		case QMessageBox::Discard: 
			f = QMessageBox::Discard;
			ObjectPeer::pushPeer( L, obj->addButton( f ), true );
			break;
		case QMessageBox::Help: 
			f = QMessageBox::Help;
			ObjectPeer::pushPeer( L, obj->addButton( f ), true );
			break;
		case QMessageBox::Apply: 
			f = QMessageBox::Apply;
			ObjectPeer::pushPeer( L, obj->addButton( f ), true );
			break;
		case QMessageBox::Reset: 
			f = QMessageBox::Reset;
			ObjectPeer::pushPeer( L, obj->addButton( f ), true );
			break;
		case QMessageBox::RestoreDefaults: 
			f = QMessageBox::RestoreDefaults;
			ObjectPeer::pushPeer( L, obj->addButton( f ), true );
			break;
		default :
			Lua::Util::error( L, "expecting a valid QMessageBox::StandardButton" );
		}
	}
	else if(Util::isStr( L, 2 ))
	{
		QMessageBox::ButtonRole f;
		int fInt = Util::toInt( L, 3 );
		if( (fInt >= -1 && fInt <= 9) )
		{
			f = (QMessageBox::ButtonRole) fInt;
			ObjectPeer::pushPeer( L, obj->addButton( Util::toStr( L, 2) , f ), true );
		}
		else
			Lua::Util::error( L, "expecting a valid QMessageBox::ButtonRole" );
	}
	return 1;
}
int MessageBox::button(lua_State * L) // ( StandardButton which ) const QAbstractButton * 
{
	QMessageBox* obj = ObjectHelper<QMessageBox>::check( L, 1);
	QMessageBox::StandardButton f;
	switch( Util::toInt( L, 2 ) )
	{
	case QMessageBox::NoButton: 
		f = QMessageBox::NoButton;
		ObjectPeer::pushPeer( L, obj->button( f ), true );
		break;
	case QMessageBox::Ok: 
		f = QMessageBox::Ok;
		ObjectPeer::pushPeer( L, obj->button( f ), true );
		break;
	case QMessageBox::Save: 
		f = QMessageBox::Save;
		ObjectPeer::pushPeer( L, obj->button( f ), true );
		break;
	case QMessageBox::SaveAll: 
		f = QMessageBox::SaveAll;
		ObjectPeer::pushPeer( L, obj->button( f ), true );
		break;
	case QMessageBox::Open: 
		f = QMessageBox::Open;
		ObjectPeer::pushPeer( L, obj->button( f ), true );
		break;
	case QMessageBox::Yes: 
		f = QMessageBox::Yes;
		ObjectPeer::pushPeer( L, obj->button( f ), true );
		break;
	case QMessageBox::YesToAll: 
		f = QMessageBox::YesToAll;
		ObjectPeer::pushPeer( L, obj->button( f ), true );
		break;
	case QMessageBox::No: 
		f = QMessageBox::No;
		ObjectPeer::pushPeer( L, obj->button( f ), true );
		break;
	case QMessageBox::NoToAll: 
		f = QMessageBox::NoToAll;
		ObjectPeer::pushPeer( L, obj->button( f ), true );
		break;
	case QMessageBox::Abort: 
		f = QMessageBox::Abort;
		ObjectPeer::pushPeer( L, obj->button( f ), true );
		break;
	case QMessageBox::Retry: 
		f = QMessageBox::Retry;
		ObjectPeer::pushPeer( L, obj->button( f ), true );
		break;
	case QMessageBox::Ignore: 
		f = QMessageBox::Ignore;
		ObjectPeer::pushPeer( L, obj->button( f ), true );
		break;
	case QMessageBox::Close: 
		f = QMessageBox::Close;
		ObjectPeer::pushPeer( L, obj->button( f ), true );
		break;
	case QMessageBox::Cancel: 
		f = QMessageBox::Cancel;
		ObjectPeer::pushPeer( L, obj->button( f ), true );
		break;
	case QMessageBox::Discard: 
		f = QMessageBox::Discard;
		ObjectPeer::pushPeer( L, obj->button( f ), true );
		break;
	case QMessageBox::Help: 
		f = QMessageBox::Help;
		ObjectPeer::pushPeer( L, obj->button( f ), true );
		break;
	case QMessageBox::Apply: 
		f = QMessageBox::Apply;
		ObjectPeer::pushPeer( L, obj->button( f ), true );
		break;
	case QMessageBox::Reset: 
		f = QMessageBox::Reset;
		ObjectPeer::pushPeer( L, obj->button( f ), true );
		break;
	case QMessageBox::RestoreDefaults: 
		f = QMessageBox::RestoreDefaults;
		ObjectPeer::pushPeer( L, obj->button( f ), true );
		break;
	default :
		Lua::Util::error( L, "expecting a valid QMessageBox::StandardButton" );
	}
	return 1;
}
int MessageBox::clickedButton(lua_State * L) // const : QAbstractButton *
{
	QMessageBox* obj = ObjectHelper<QMessageBox>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->clickedButton(), true );
	return 1;
}
int MessageBox::defaultButton(lua_State * L) // const : QPushButton *
{
	QMessageBox* obj = ObjectHelper<QMessageBox>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->defaultButton(), true );
	return 1;
}
int MessageBox::escapeButton(lua_State * L) // const : QAbstractButton * 
{
	QMessageBox* obj = ObjectHelper<QMessageBox>::check( L, 1);
	ObjectPeer::pushPeer( L, obj->escapeButton(), true );
	return 1;
}
int MessageBox::removeButton(lua_State * L) // ( QAbstractButton * button )
{
	QMessageBox* obj = ObjectHelper<QMessageBox>::check( L, 1);
	QAbstractButton* button = ObjectHelper<QAbstractButton>::check( L, 2);
	obj->removeButton( button );
	return 0;
}
int MessageBox::setDefaultButton(lua_State * L) // ( QPushButton * button )
{
	QMessageBox* obj = ObjectHelper<QMessageBox>::check( L, 1);
	QPushButton* button = ObjectHelper<QPushButton>::check( L, 2);
	obj->setDefaultButton( button );
	return 0;
}
int MessageBox::setEscapeButton(lua_State * L) // ( QAbstractButton * button )
{
	QMessageBox* obj = ObjectHelper<QMessageBox>::check( L, 1);
	QAbstractButton* button = ObjectHelper<QAbstractButton>::check( L, 2);
	obj->setEscapeButton( button );
	return 0;
}
int MessageBox::setWindowModality(lua_State * L) // ( Qt::WindowModality windowModality )
{
	QMessageBox* obj = ObjectHelper<QMessageBox>::check( L, 1);
	Qt::WindowModality f;
	int fInt = Util::toInt( L, 2 );
	if( (fInt >= 0 && fInt <= 2) )
	{
		f = (Qt::WindowModality) fInt;
		obj->setWindowModality( f );
	}
	else
		Lua::Util::error( L, "expecting a valid Qt::WindowModality" );
	return 0;
}
int MessageBox::setWindowTitle(lua_State * L) // ( const QString & title )
{
	QMessageBox* obj = ObjectHelper<QMessageBox>::check( L, 1);
	obj->setWindowTitle( Util::toStr( L, 2 ) );
	return 0;
}
int MessageBox::standardButton(lua_State * L) // ( QAbstractButton * button ) const : StandardButton
{
	QMessageBox* obj = ObjectHelper<QMessageBox>::check( L, 1);
	QAbstractButton* button = ObjectHelper<QAbstractButton>::check( L, 2);
	Util::push( L, obj->standardButton( button ) );
	return 1;
}
static const luaL_reg _MessageBox[] = 
{
	{ "addButton", MessageBox::addButton },
	{ "button", MessageBox::button },
	{ "clickedButton", MessageBox::clickedButton },
	{ "defaultButton", MessageBox::defaultButton },
	{ "escapeButton", MessageBox::escapeButton },
	{ "removeButton", MessageBox::removeButton },
	{ "setDefaultButton", MessageBox::setDefaultButton },
	{ "setEscapeButton", MessageBox::setEscapeButton },
	{ "setWindowModality", MessageBox::setWindowModality },
	{ "setWindowTitle", MessageBox::setWindowTitle },
	{ "standardButton", MessageBox::standardButton },
	{ 0, 0 }
};
void MessageBox::install(lua_State * L){
	ObjectInstaller<QMessageBox>::install( L, _MessageBox, ObjectInstaller<QDialog>::className());
}
