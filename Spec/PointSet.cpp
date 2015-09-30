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

#include "PointSet.h"
#include <Root/Vector.h>
using namespace Spec;

PointSet::PointSet()
{

}

PointSet::~PointSet()
{

}

const char* PointSet::getComment(Index id) const
{
	return "";
}

Index PointSet::getNextId()
{
	return 0;
}

bool PointSet::isDefined(Index id) const
{
	return false;
}

void PointSet::setPoint(Index id, const PeakPos & )
{

}

void PointSet::setComment(Index id, const char *)
{

}

PointSet::Selection PointSet::find(const PpmCube &) const
{
	return Selection();
}

void PointSet::deletePoint(Index id)
{

}

PointSet::Selection PointSet::findAll() const
{
	return Selection();
}

Amplitude PointSet::getAmplitude(Index) const
{
	return 0;
}

Amplitude PointSet::getVolume(Index) const
{
	return 0;
}

void PointSet::setVolume(Index,Amplitude)
{

}

void PointSet::setAmplitude(Index,Amplitude)
{

}

void PointSet::getColors(ColorMap & m) const
{
	m.assign( getDimCount(), AtomType() );
	for( Dimension d = 0; d < m.size(); d++ )
		m[ d ] = getColor(d);
}

static PointSet::Assig s_dummy;

const PointSet::Assig& PointSet::getAssig( Index ) const
{
	// TEST s_dummy.assign( getDimCount(), 0 );
	return s_dummy;
}

void PointSet::setAssig(Index, const Assig &)
{

}

Root::UInt8 PointSet::getCode( Index ) const
{
	return 0;
}

void PointSet::setCode(Index, Root::UInt8)
{

}
