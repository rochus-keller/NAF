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

#include "Label.h"
#include <QColor>
#include <Lexi/Font.h>
#include <Lexi/Viewport.h>
using namespace Lexi;

Label::Label(const char* str, const QFont* f, QColor c, Alignment hori, Alignment verti):
	d_verti( verti ), d_hori( hori )
{
    d_text = str;
	if( f != 0 )
		d_font = *f;
    d_color = c;
}

Label::~Label()
{
}

void Label::request( Requisition& requisition) 
{
	if( !d_text.isEmpty() )
	{
		Font::BoundingBox box;
		box.stringBox( d_font, d_text );
		box.request( requisition );
	}
	Requirement& rx = requisition.getRequirement( DimensionX );
	Requirement& ry = requisition.getRequirement( DimensionY );
	// Es schadet nichts, das Label zu vergrssern oder zu verkleinern
	rx.setStretch( MaxTwips );
	rx.setShrink( rx.getNatural() ); // RISK
	// TEST ry.setStretch( CoordMax );
	rx.setAlignment( 0 );
	ry.setAlignment( 0 ); // Draw kontrolliert Alignment selber.
}

void Label::pick( Twips x, Twips y, const Allocation& a, Trace& t) 
{
    if( a.isHit( x, y ) ) 
	{
		t.push_back( this );
    }
}

void Label::draw( Canvas& c, const Allocation& a) 
{
	Allocation aa( a );
	aa.setAlignment( d_hori, d_verti );
    c.drawText( aa, d_text, &d_font, d_color );
}

void Label::allocate( const Allocation & a)
{
	// TEST
}

void Label::setFont( const QFont& f )
{
	d_font = f;
}

void Label::setColor( QColor c )
{
	d_color = c;
}
