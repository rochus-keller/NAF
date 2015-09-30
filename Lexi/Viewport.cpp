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

#include "Viewport.h"
#include <Lexi/Extension.h>
#include <Lexi/Allocation.h>
using namespace Lexi;


bool Viewport::isDamaged(const Allocation & e) const
{
	return isDamaged( e.getLeft(), e.getTop(), e.getWidth(), e.getHeight(),
		e.getOffX(), e.getOffY() );
}

bool Viewport::setFocus(Glyph *)
{
	// to override
	return false;
}

/*
void Viewport::damage(const Extension & e, bool immediate)
{
	damage( e.getLeft(), e.getTop(), e.getWidth(), e.getHeight(), immediate );
}
*/

void Viewport::damage(Twips left, Twips top, Twips width, Twips height,
					  Twips offX, Twips offY, bool immediate )
{
	// To override;
}

void Viewport::damage(const Allocation & e, bool immediate)
{
	damage( e.getLeft(), e.getTop(), e.getWidth(), e.getHeight(), 
		e.getOffX(), e.getOffY(), immediate );
}

bool Viewport::isDamaged(Twips left, Twips top, Twips width, Twips height,
						 Twips offX, Twips offY ) const
{
	// To override
	return true;
}

void Viewport::grab(Glyph *)
{
	// to override
}

void Viewport::ungrab()
{
	// to override
}

void Viewport::scrollDamage(const Allocation &, Twips dx, Twips dy)
{
	// to override
}

void Viewport::getDamage(Allocation &) const
{ 
	// to override
}


bool Viewport::isGrabbing(Glyph *) const
{
	// to overrider
	return false;
}

Viewport::Viewport(Agent * parent):
	Agent( parent )
{
}

void Viewport::damageAll(bool immediate)
{
	// to override
}

void Viewport::captureDamage()
{
	// to override
}

void Viewport::commitDamage()
{
	// to override
}

Twips Viewport::getLastClick(Dimension) const
{
	// to override
	return 0;
}

void Viewport::setCursor(Cursor)
{
	// to override
}

void Viewport::pushHourglass()
{
	pushCursor( CurWait );
}
