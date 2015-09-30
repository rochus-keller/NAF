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

#include "ResidueParamView.h"
#include <SpecView/SpecViewer.h>
#include <SpecView/ViewAreaMdl.h>
#include <Lexi/Font.h>
#include <stdio.h>
#include <qstring.h>
using namespace Spec;
using namespace Lexi;

static const int s_pointSize = 160;

ResidueParamView::ResidueParamView(SpecViewer* vw, QColor clr):
	SpecView( vw ), d_show( true )
{
	d_clr = clr;
}

ResidueParamView::~ResidueParamView()
{
}

void ResidueParamView::setResidue(Residue * r, bool autoOn )
{
	if( r == d_resi )
		return;
	d_resi = r;
	d_type = 0;
	if( r && autoOn )
	{
		d_show = true;
	}
}

void ResidueParamView::show(bool on)
{
	d_show = on;
}

void ResidueParamView::draw( Canvas& v, const Allocation& a )
{
	if( !d_show )
		return;
	Canvas& c = v;
	ViewAreaMdl* area = getViewArea();
	Twips y;
	QString str;
	DisPar val;
	if( d_resi )
	{
		Root::SymbolSet ss = d_resi->getAllDisPar();
		Root::SymbolSet::const_iterator pos;
		for( pos = ss.begin(); pos != ss.end(); ++pos )
		{
			val = d_resi->getDisPar( (*pos) );
			if( val.isValid() )
			{
				y = area->toTwip( val.d_mean + val.d_dev, a.getTop(), DimY );
				c.drawLine( a.getLeft(), y, a.getRight(), y, d_clr );
				str.sprintf( "+ %s", (*pos).data() );
				QFont f;
				f.setPointSize( s_pointSize / TwipsPerPoint );
				c.drawText( a.getLeft(), y, str, &f, d_clr );

				y = area->toTwip( val.d_mean - val.d_dev, a.getTop(), DimY  );
				c.drawLine( a.getLeft(), y, a.getRight(), y, d_clr );
				str.sprintf( "- %s", (*pos).data() );
				c.drawText( a.getLeft(), y, str, &f, d_clr );
			}
		}
	}else if( d_type )
	{
		const ResidueType::AtomMap& ss = d_type->getAtoms();
		ResidueType::AtomMap::const_iterator pos;
		for( pos = ss.begin(); pos != ss.end(); ++pos )
		{
			val = (*pos).second->getDisPar();
			if( val.isValid() )
			{
				y = area->toTwip( val.d_mean + val.d_dev, a.getTop(), DimY );
				c.drawLine( a.getLeft(), y, a.getRight(), y, d_clr );
				str.sprintf( "+ %s", (*pos).first.data() );
				QFont f;
				f.setPointSize( s_pointSize / TwipsPerPoint );
				c.drawText( a.getLeft(), y, str, &f, d_clr );

				y = area->toTwip( val.d_mean - val.d_dev, a.getTop(), DimY  );
				c.drawLine( a.getLeft(), y, a.getRight(), y, d_clr );
				str.sprintf( "- %s", (*pos).first.data() );
				c.drawText( a.getLeft(), y, str, &f, d_clr );
			}
		}
	}
}

void ResidueParamView::setType(ResidueType * t, bool autoOn)
{
	if( t == d_type )
		return;
	d_type = t;
	d_resi = 0;
	if( t && autoOn )
		d_show = true;
}
