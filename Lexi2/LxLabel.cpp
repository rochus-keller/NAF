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

#include "LxLabel.h"
#include "LxFontBoundingBox.h"
#include <Lexi2/LxViewport.h>
using namespace Lexi2;

Label::Label(const char* str, const QFont* f, QColor c, Alignment hori, Alignment verti):
	d_verti( verti ), d_hori( hori )
{
    d_text = str;
	if( f )
		d_font = *f;
	else
		d_font = QFont( "Helvetica", 200 );
    d_color = c;
}

Label::~Label()
{
}

void Label::request( Requisition& requisition) 
{
	if( !d_text.isEmpty() )
	{
		FontBoundingBox box;
		box.stringBox( d_font, d_text );
		box.request( requisition );
	}
	Requirement& rx = requisition.getRequirement( DimX );
	Requirement& ry = requisition.getRequirement( DimY );
	// Es schadet nichts, das Label zu vergrössern oder zu verkleinern
	rx.setStretch( CoordMax );
	rx.setShrink( rx.getNatural() ); // RISK
	// TEST ry.setStretch( CoordMax );
}

void Label::draw(Canvas& c, const Allocation& a) 
{
	FontBoundingBox box;
	box.stringBox( d_font, d_text );
	Allocation aa( 0, 0, box.getWidth(), box.getHeight() );

	if( d_verti == RightBottom )
		aa.getAllotment( DimY ).move( a.getHeight() - aa.getHeight() );
	if( d_hori == RightBottom )
 		aa.getAllotment( DimX ).move( a.getWidth() - aa.getWidth() );
	// TODO Center
   c.drawText( aa, d_text, &d_font, d_color.rgb() );
}

void Label::setFont( const QFont& f )
{
	d_font = f;
	damageAll();
}

void Label::setColor( QColor c )
{
	d_color = c;
	damageAll();
}
