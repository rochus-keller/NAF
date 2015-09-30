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

#include "Shapes.h"
using namespace Lexi;

void Shape::request(Requisition & r)
{
	r = d_size;
}

Shape::Shape( const Requisition& r, QColor c ):
	d_size( r ), d_color( c )
{
}

Shape::Shape(QColor c):
	d_color( c )
{
}

Rectangle::Rectangle(const Requisition & r, QColor c):
	Shape( r, c )
{

}

void Rectangle::draw( Canvas & c, const Allocation & a)
{
	c.fillRect( a.getLeft(), a.getTop(), a.getWidth(), a.getHeight(), d_color );
}

Rule::Rule( Dimension d, QColor c, Twips t )
{
    d_dimension = d;
    d_thickness = t;
	d_color = c;
}

void Rule::request( Requisition& r ) 
{
	Requirement& rx = r.getRequirement( d_dimension );
	rx.setNatural( 0 );
	rx.setStretch( MaxTwips );
	r.getRequirement( transpose( d_dimension ) ).setNatural( d_thickness );
}

void Rule::draw( Canvas& c, const Allocation& a) 
{
	c.fillRect( a.getLeft(), a.getTop(), a.getWidth(), a.getHeight(), d_color );
}

