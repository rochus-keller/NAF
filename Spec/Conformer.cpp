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

#include "Conformer.h"
#include "Project.h"
#include "Structure.h"
using namespace Spec;

static Location s_loc;

QByteArray  Conformer::getInstanceName(bool) const
{
	QString str;
	str.sprintf( "Conformer %d", d_id );
	return str.toLatin1(); // RISK
}

const Location::Position& Conformer::getCoord(Root::Index id) const
{
	Coords::const_iterator i = d_coords.find( id );
	if( i == d_coords.end() )
		return s_loc.d_pos;
	else
		return (*i).second;
}

void Conformer::setCoord(Root::Index id, const Location::Position & c)
{
	d_coords[ id ] = c;
    Changed m( this, Position );
	d_owner->getOwner()->notifyObservers( m );
}

void Conformer::setNr(Root::Index nr)
{
	d_nr = nr;
	Changed m( this, Nr );
	d_owner->getOwner()->notifyObservers( m );
}

