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

#include "LxEvent.h"
#include <qnamespace.h>
using namespace Lexi2;


MouseEvent::MouseEvent( Coord x, Coord y, const ButtonSet& b, Action a):
	d_buttons( b ), d_action( a )
{
	d_point[ 0 ] = x;
	d_point[ 1 ] = y;
}

KeyEvent::KeyEvent( int key, const char* ascii, const ButtonSet& b, Action a):
	d_buttons( b ), d_key( key ), d_ascii( ascii ), d_action( a )
{

}

bool KeyEvent::isBackspace() const
{
	return d_key == Qt::Key_Backspace;
}

bool KeyEvent::isReturn() const
{
	return d_key == Qt::Key_Return;
}

bool KeyEvent::isSpace() const
{
	return !d_ascii.empty() && d_ascii[0] == ' ';
}

bool KeyEvent::isTab() const
{
	return d_key == Qt::Key_Tab;
}

bool KeyEvent::isEscape() const
{
	return d_key == Qt::Key_Escape;
}

bool KeyEvent::isDelete() const
{
	return d_key == Qt::Key_Delete;
}

bool KeyEvent::isHome() const
{
	return d_key == Qt::Key_Home;
}

bool KeyEvent::isEnd() const
{
	return d_key == Qt::Key_End;
}

bool KeyEvent::isLeft() const
{
	return d_key == Qt::Key_Left;
}

bool KeyEvent::isUp() const
{
	return d_key == Qt::Key_Up;
}

bool KeyEvent::isRight() const
{
	return d_key == Qt::Key_Right;
}

bool KeyEvent::isDown() const
{
	return d_key == Qt::Key_Down;
}

bool KeyEvent::isPageDown() const
{
	return d_key == Qt::Key_PageDown;
}

bool KeyEvent::isPageUp() const
{
	return d_key == Qt::Key_PageUp;
}

bool KeyEvent::isChar() const
{
	return !d_ascii.empty();
}

DragEvent::DragEvent( Coord x, Coord y, const ButtonSet& b, Coord sx, Coord sy,
					 Coord lx, Coord ly): 
	MouseEvent( x, y, b )
{
	d_start[ DimX ] = sx;
	d_start[ DimY ] = sy;
	d_last[ DimX ] = lx;
	d_last[ DimY ] = ly;
}

void FocusEvent::reset(bool in)
{
	d_focusIn = in;
	d_target = 0;
}

bool KeyEvent::isMenu() const
{
	return d_key == Qt::Key_Menu;
}

bool KeyEvent::isHelp() const
{
	return d_key == Qt::Key_Help;
}
