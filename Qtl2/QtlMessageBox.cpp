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
#include <QInputDialog>
#include <QStringList>
#include "QtlMessageBox.h"
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;

int MessageBox::addButton(lua_State * L) // ( QAbstractButton * button, ButtonRole role )
// ( StandardButton button ) QPushButton *
// ( const QString & text, ButtonRole role ) QPushButton *
{
	QMessageBox* obj = QtObject<QMessageBox>::check( L, 1);
	if( QAbstractButton* button = QtObject<QAbstractButton>::cast( L, 2) )
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
        QMessageBox::StandardButton f = (QMessageBox::StandardButton)Util::toInt( L, 2 ); // TODO
        QtObject<QPushButton>::create( L, obj->addButton( f ) );
    }
    else if(QtValueBase::isString( L, 2 ))
	{
		QMessageBox::ButtonRole f;
		int fInt = Util::toInt( L, 3 );
		if( (fInt >= -1 && fInt <= 9) )
		{
			f = (QMessageBox::ButtonRole) fInt;
            QtObject<QPushButton>::create( L, obj->addButton( QtValueBase::toString( L, 2) , f ) );
		}
		else
			Lua::Util::error( L, "expecting a valid QMessageBox::ButtonRole" );
	}
	return 1;
}

int MessageBox::button(lua_State * L) // ( StandardButton which ) const QAbstractButton * 
{
	QMessageBox* obj = QtObject<QMessageBox>::check( L, 1);
    QMessageBox::StandardButton f = (QMessageBox::StandardButton)Util::toInt( L, 2 ); // TODO
    QtObject<QAbstractButton>::create( L, obj->button( f ) );
    return 1;
}

int MessageBox::clickedButton(lua_State * L) // const : QAbstractButton *
{
	QMessageBox* obj = QtObject<QMessageBox>::check( L, 1);
    QtObject<QAbstractButton>::create( L, obj->clickedButton() );
	return 1;
}

int MessageBox::defaultButton(lua_State * L) // const : QPushButton *
{
	QMessageBox* obj = QtObject<QMessageBox>::check( L, 1);
    QtObject<QPushButton>::create( L, obj->defaultButton() );
	return 1;
}

int MessageBox::escapeButton(lua_State * L) // const : QAbstractButton * 
{
	QMessageBox* obj = QtObject<QMessageBox>::check( L, 1);
    QtObject<QAbstractButton>::create( L, obj->escapeButton() );
	return 1;
}

int MessageBox::removeButton(lua_State * L) // ( QAbstractButton * button )
{
	QMessageBox* obj = QtObject<QMessageBox>::check( L, 1);
	QAbstractButton* button = QtObject<QAbstractButton>::check( L, 2);
	obj->removeButton( button );
	return 0;
}

int MessageBox::setDefaultButton(lua_State * L) // ( QPushButton * button )
{
	QMessageBox* obj = QtObject<QMessageBox>::check( L, 1);
	QPushButton* button = QtObject<QPushButton>::check( L, 2);
	obj->setDefaultButton( button );
	return 0;
}

int MessageBox::setEscapeButton(lua_State * L) // ( QAbstractButton * button )
{
	QMessageBox* obj = QtObject<QMessageBox>::check( L, 1);
	QAbstractButton* button = QtObject<QAbstractButton>::check( L, 2);
	obj->setEscapeButton( button );
	return 0;
}

int MessageBox::setWindowModality(lua_State * L) // ( Qt::WindowModality windowModality )
{
	QMessageBox* obj = QtObject<QMessageBox>::check( L, 1);
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
	QMessageBox* obj = QtObject<QMessageBox>::check( L, 1);
    obj->setWindowTitle( QtValueBase::toString( L, 2 ) );
	return 0;
}

int MessageBox::standardButton(lua_State * L) // ( QAbstractButton * button ) const : StandardButton
{
	QMessageBox* obj = QtObject<QMessageBox>::check( L, 1);
	QAbstractButton* button = QtObject<QAbstractButton>::check( L, 2);
	Util::push( L, obj->standardButton( button ) );
	return 1;
}

//void about ( QWidget * parent, const QString & title, const QString & text )
int about(lua_State * L)
{
	QWidget* parent = QtObject<QWidget>::cast( L, 1 );
	QString title = QtValueBase::toString( L, 2 );
	QString text = QtValueBase::toString( L, 3 );
	QMessageBox::about( parent, title, text );
	return 0;
}

//void aboutQt ( QWidget * parent, const QString & title = QString() )
int aboutQt(lua_State * L)
{
	QWidget* parent = QtObject<QWidget>::cast( L, 1 );
	QString title = QtValueBase::toString( L, 2, false );
	QMessageBox::aboutQt( parent, title );
	return 0;
}

//StandardButton critical ( QWidget * parent, const QString & title, const QString & text, StandardButtons buttons = Ok, StandardButton defaultButton = NoButton )
//StandardButton information ( QWidget * parent, const QString & title, const QString & text, StandardButtons buttons = Ok, StandardButton defaultButton = NoButton )
//StandardButton question ( QWidget * parent, const QString & title, const QString & text, StandardButtons buttons = Ok, StandardButton defaultButton = NoButton )
//StandardButton warning ( QWidget * parent, const QString & title, const QString & text, StandardButtons buttons = Ok, StandardButton defaultButton = NoButton )
template<char which>
int ciqw(lua_State * L)
{
	QWidget* parent = QtObject<QWidget>::cast( L, 1 );
	QString title = QtValueBase::toString( L, 2 );
	QString text = QtValueBase::toString( L, 3 );
	QMessageBox::StandardButtons buttons = QMessageBox::Ok;
	if( Util::top(L) > 3 )
		buttons &= Util::toInt( L, 4 );
	QMessageBox::StandardButton defaultButton = QMessageBox::NoButton;
	if( Util::top(L) > 4 )
		buttons = (QMessageBox::StandardButton)Util::toInt( L, 5 );
	int res = 0;
	switch( which )
	{
	case 'c':
		res = QMessageBox::critical( parent, title, text, buttons, defaultButton );
		break;
	case 'i':
		res = QMessageBox::information( parent, title, text, buttons, defaultButton );
		break;
	case 'q':
		res = QMessageBox::question( parent, title, text, buttons, defaultButton );
		break;
	case 'w':
		res = QMessageBox::warning( parent, title, text, buttons, defaultButton );
		break;
	}
	lua_pushinteger( L, res );
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
	// statics:
	{ "about", about },
	{ "aboutQt", aboutQt },
	{ "critical", ciqw<'c'> },
	{ "information", ciqw<'i'> },
	{ "question", ciqw<'q'> },
	{ "warning", ciqw<'w'> },
	{ 0, 0 }
};

void MessageBox::install(lua_State * L){
	QtObject<QMessageBox,QDialog>::install( L, "QMessageBox", _MessageBox );
}

int InputDialog::getDouble(lua_State *L)
{
	double value = 0;
	if( Util::top(L) > 3 )
		value = Util::toDbl( L, 4 );
	double minValue = -2147483647;
	if( Util::top(L) > 4 )
		minValue = Util::toDbl( L, 5 );
	double maxValue = 2147483647;
	if( Util::top(L) > 5 )
		maxValue = Util::toDbl( L, 6 );
	int decimals = 1;
	if( Util::top(L) > 6 )
		decimals = Util::toInt( L, 7 );
	bool ok = true;
	Qt::WindowFlags f = 0;
	if( Util::top(L) > 7 )
		f &= Util::toInt( L, 8 );
	Util::push( L, QInputDialog::getDouble( QtObject<QWidget>::cast( L, 1 ), QtValueBase::toString( L, 2, false ),
											QtValueBase::toString( L, 3, false ), value, minValue, maxValue,
											decimals, &ok, f ) );
	Util::push( L, ok );
	return 2;
}

int InputDialog::getInteger(lua_State *L)
{
	int value = 0;
	if( Util::top(L) > 3 )
		value = Util::toInt( L, 4 );
	int minValue = -2147483647;
	if( Util::top(L) > 4 )
		minValue = Util::toInt( L, 5 );
	int maxValue = 2147483647;
	if( Util::top(L) > 5 )
		maxValue = Util::toInt( L, 6 );
	int step = 1;
	if( Util::top(L) > 6 )
		step = Util::toInt( L, 7 );
	bool ok = true;
	Qt::WindowFlags f = 0;
	if( Util::top(L) > 7 )
		f &= Util::toInt( L, 8 );
	Util::push( L, QInputDialog::getDouble( QtObject<QWidget>::cast( L, 1 ), QtValueBase::toString( L, 2, false ),
											QtValueBase::toString( L, 3, false ), value, minValue, maxValue,
											step, &ok, f ) );
	Util::push( L, ok );
	return 2;
}

int InputDialog::getItem(lua_State *L)
{
	int current = 0;
	if( Util::top(L) > 4 )
		current = Util::toInt( L, 5 );
	bool editable = true;
	if( Util::top(L) > 5 )
		editable = Util::toBool( L, 6 );
	bool ok = false;
	Qt::WindowFlags f = 0;
	if( Util::top(L) > 6 )
		f &= Util::toInt( L, 7 );
	const QString res = QInputDialog::getItem( QtObject<QWidget>::cast( L, 1 ), QtValueBase::toString( L, 2, false ),
										 QtValueBase::toString( L, 3, false ), *QtValue<QStringList>::check( L, 4 ),
										 current, editable, &ok, f );
	*QtValue<QString>::create( L ) = res;
	Util::push( L, ok );
	return 2;
}

int InputDialog::getText(lua_State *L)
{
	QLineEdit::EchoMode mode = QLineEdit::Normal;
	if( Util::top(L) > 3 )
		mode = (QLineEdit::EchoMode)Util::toInt( L, 4 );
	bool ok = false;
	Qt::WindowFlags f = 0;
	if( Util::top(L) > 5 )
		f &= Util::toInt( L, 6 );
	const QString res = QInputDialog::getText( QtObject<QWidget>::cast( L, 1 ), QtValueBase::toString( L, 2, false ),
										 QtValueBase::toString( L, 3, false ), mode,
										 QtValueBase::toString( L, 5, false ), &ok, f );
	*QtValue<QString>::create( L ) = res;
	Util::push( L, ok );
	return 2;
}

static const luaL_reg _InputDialog[] =
{
	{ "getDouble", InputDialog::getDouble },
	{ "getInteger", InputDialog::getInteger },
	{ "getItem", InputDialog::getItem },
	{ "getText", InputDialog::getText },
	{ 0, 0 }
};

void InputDialog::install(lua_State *L)
{
	QtObject<QInputDialog,QDialog,NotCreatable>::install( L, "QInputDialog", _InputDialog );
}
