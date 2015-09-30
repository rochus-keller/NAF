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

#include "SvSpinLinkView.h"
#include <SpecView2/SvSpecViewer.h>
#include <SpecView2/SvViewAreaMdl.h>
#include <Lexi2/LxViewport.h>
#include <map>
#include <vector>
using namespace Spec2;
using namespace Lexi2;

SpinLinkView::SpinLinkView(ViewAreaMdl* vw, SpinSpace* ts, QColor clr):
	d_area( vw ), d_model( ts ), d_thick( 20 ), d_showVerti( false )
{
	assert( ts );
	d_clr = clr;
	d_model->addObserver( this );
}

SpinLinkView::~SpinLinkView()
{
	if( d_model )
		d_model->removeObserver( this );
}

void SpinLinkView::draw( Canvas& c, const Allocation& a )
{
	ViewAreaMdl* area = d_area;
	SpinSpace::Element e;
	SpinSpace::Iterator pos, _end = d_model->end();
	Coord y, x = 0;
	typedef std::map<Root::SymbolString,std::vector<Lexi2::Coord> > Verti;
	Verti verti;
	bool first = true;
	for( pos = d_model->begin(); pos != _end; ++pos )
	{
		d_model->fetch( pos, e );
		if( e.isGhost() || e.isHidden() )
			continue;
		if( first )
			x = area->toTwip( e.d_point[DimX]->getShift( d_model->getSpec() ), 
				a.getLeft(), DimX );
		first = false;
		const SpinLabel& l = e.d_point[DimY]->getLabel();
		if( d_labels.find( l.getTag() ) != d_labels.end() )
		{
			y = area->toTwip( e.d_point[DimY]->getShift( d_model->getSpec() ), 
				a.getTop(), DimY );
			if( l.getOffset() >= 0 )
				c.drawLine( x, y, a.getRight(), y, d_clr, d_thick );
			else
				c.drawLine( a.getLeft(), y, x, y, d_clr, d_thick );
			if( d_showVerti )
				verti[ l.getTag() ].push_back( y );
		}
	}
	if( d_showVerti )
	{
		// TODO: effizienter
		Verti::const_iterator p2;
		int i,j;
		for( p2 = verti.begin(); p2 != verti.end(); ++p2 )
		{
			for( i = 0; i < (*p2).second.size(); i++ )
			{
				for( j = 0; j < (*p2).second.size(); j++ )
				{
					c.drawLine( x, (*p2).second[ i ], x, 
						(*p2).second[ j ], d_clr, d_thick );
				}
			}
		}
	}
}

void SpinLinkView::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( SpinSpace::Update, a, msg )
	{
		// TODO
		// if( !d_model->getTuples().empty() )
		damageAll();
		msg.consume();
	}
	END_HANDLER();
}

void SpinLinkView::setLabels(const SymbolSet & ss)
{
	d_labels = ss;
	damageAll();
}

void SpinLinkView::addLabel(Root::SymbolString s)
{
	d_labels.insert( s );
	damageAll();
}

void SpinLinkView::setThick(Coord c)
{	
	assert( c > 0 );
	d_thick = c;
	damageAll();
}

void SpinLinkView::setColor(QColor c)
{
	d_clr = c;
	damageAll();
}

void SpinLinkView::clear()
{
	d_labels.clear();
	damageAll();
}

void SpinLinkView::showVerti(bool on)
{
	d_showVerti = on;
	damageAll();
}
