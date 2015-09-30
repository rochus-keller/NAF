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

#include "PeakSpace.h"
#include <qstring.h> // qDebug
using namespace Spec;

COP PeakSpace::s_cop;

PeakSpace::PeakSpace()
{
}

PeakSpace::~PeakSpace()
{

}

Index PeakSpace::addPeak(const PeakData &, Spectrum*, COP& )
{
	return 0;
}

void PeakSpace::removePeak(Index, COP&)
{

}

void PeakSpace::setPos(Index, const PeakPos &, Amplitude, Spectrum*, COP&)
{

}

void PeakSpace::getPos(Index, PeakPos &, Spectrum*) const
{

}

SymbolString PeakSpace::getTag(Index) const
{
	return "";
}

void PeakSpace::setTag(Index, SymbolString, COP&)
{

}

void PeakSpace::setAmpVol(Index, Amplitude,Amplitude, Spectrum*, COP&)
{

}

Amplitude PeakSpace::getVol(Index, Spectrum*) const
{
	return 0;
}

Amplitude PeakSpace::getAmp(Index, Spectrum*) const
{
	return 0;
}

void PeakSpace::setColor(Index, Root::UInt8, COP&)
{

}

Root::UInt8 PeakSpace::getColor(Index) const
{
	return 0;
}

void PeakSpace::setAssig(Index, const Assig &, COP&)
{

}

void PeakSpace::getAssig(Index, Assig &) const
{

}

Dimension PeakSpace::getDimCount() const
{
	return 0;
}

void PeakSpace::find(const PpmCube &, Selection &, Spectrum*, bool with ) const
{
	// qDebug( "WARNING: PeakSpace::find not implemented" );
}

void PeakSpace::findAll(Selection &, Spectrum*) const
{
	// qDebug( "WARNING: PeakSpace::findAll not implemented" );
}

void PeakSpace::getValue(Index, Field, Root::Any &) const
{

}

void PeakSpace::setValue(Index, Field, const Root::Any &)
{

}

Index PeakSpace::getModel(Index, Spectrum*) const
{
	return 0;
}

void PeakSpace::setModel(Index peak, Index mdl, Spectrum*, COP&)
{

}

void PeakSpace::setParams(Index mdl, const ModelParam &)
{

}

void PeakSpace::getParams(Index mdl, ModelParam &) const
{

}

Index PeakSpace::getHome() const
{
	return 0;
}

Index PeakSpace::addModel(const ModelParam &)
{
	return 0;
}

void PeakSpace::getPeak(Index, PeakData &, Spectrum*) const
{

}

void PeakSpace::getModels(Selection &) const
{

}

void PeakSpace::setOrigThick(Dimension, PPM orig, PPM tol)
{

}

AtomType PeakSpace::getAtomType(Dimension) const
{
	return AtomType();
}

Index PeakSpace::getId() const
{
	return 0;
}

SymbolString PeakSpace::getName() const
{
	return "";
}
