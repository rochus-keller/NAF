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

#include "Structure.h"
#include "Project.h"
using namespace Spec;

static Location s_loc;

QByteArray  Structure::getInstanceName(bool) const
{
	return getName(); // RISK
}


void Structure::removeConformer(Conformer * c)
{
	assert( c );
	Root::Ref<Conformer> tmp = c;
	d_confs.erase( c->getId() );
	Conformer::Removed m( c );
	d_owner->notifyObservers( m );
}

void Structure::setLoc(Root::Index id, const Location & loc)
{
	d_locs[ id ] = loc;
	Changed m( this, Loc );
	d_owner->notifyObservers( m );
}

void Structure::setName(const char * name)
{
	d_name = name;
}

Conformer* Structure::addConformer()
{
	Root::Index id = 1;
	if( !d_confs.empty() )
	{
		Conformers::const_iterator i = d_confs.end();
		--i;
		id = (*i).first + 1;
	}
	Conformer* c = new Conformer();
	c->d_owner = this;
	c->d_id = id;
	c->d_nr = id;
	d_confs[ id ] = c;
	Conformer::Added m( c );
	d_owner->notifyObservers( m );
	return c;
}

const Location& Structure::getLoc(Root::Index id) const
{
	Locations::const_iterator i = d_locs.find( id );
	if( i == d_locs.end() )
		return s_loc;
	else
		return (*i).second;
}

void Structure::setOrig(const Location::Position& o)
{
	d_orig = o;
	Changed m( this, Orig );
	d_owner->notifyObservers( m );
}
