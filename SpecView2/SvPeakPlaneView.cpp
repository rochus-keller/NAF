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

#include "SvPeakPlaneView.h"
#include <Spec/PeakList.h>
#include <SpecView2/SvSpecBufferMdl.h>
#include <Lexi2/LxViewport.h>
#include <Lexi2/LxFontBoundingBox.h>
#include <stdio.h>
using namespace Spec2;
using namespace Lexi2;

static const Coord g_w = 60;	// Länge eines Kreuzsegments
static const Coord g_tol = 160;
static const Coord g_p = 20;	
static const Coord g_d = 40;	// Länge eines diag Kreuzsegments


static char s_buf[32];

const char* PeakPlaneView::menuText[] =
{
	"None", // NONE, 
	"Peak Number", // ID, 
	"Label", // TAG, 
	"Number or Label", // ID_OR_TAG, 
	"Model Number", // MODEL, 
	"Amplitude", // AMP, 
	"Volume", // VOL, 
	"Amp./Model/Vol.", // AMP_MODEL_VOL, 
	"Assignment", // ASSIG
	"Color", // COLOR
	"" // END
};
 
QFont PeakPlaneView::s_default = QFont( "Helvetica", 160 );
static QColor s_posClr = Qt::cyan;
static QColor s_negClr = Qt::magenta;

PeakPlaneView::PeakPlaneView(SpecBufferMdl* b, PeakSpace* mdl,
							 QColor color, QFont* font,
							 PeakColorList* colors ):
	d_model( mdl ), d_label( ID_OR_TAG ), d_colors( colors ),
	d_found( 0 ), d_buf( b )
{
	if( font == nil )
		d_font = s_default;
	else
		d_font = *font;
	FontBoundingBox box;
	box.stringBox( d_font, "0" );
	d_lw = box.getWidth();
	d_lh = box.getHeight();
	d_color = color;
	if( d_model )
		d_model->addObserver( this );
	if( d_colors )
		d_colors->addObserver( this );
	if( d_buf )
		d_buf->addObserver( this );
	d_off = 0;
	d_angle = 45;
	d_pw = 20;
	d_show = true;
}

PeakPlaneView::~PeakPlaneView()
{
	if( d_model )
		d_model->removeObserver( this );
	if( d_colors )
		d_colors->removeObserver( this );
	if( d_buf )
		d_buf->removeObserver( this );
}

void PeakPlaneView::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( PeakSpace::Update, a, msg )
	{
		PeakPos pos;
		if( a->getType() == PeakSpace::Update::Add )
		{
			d_model->getPos( a->getId(), pos, d_buf->getSpectrum() );
			damage( pos );
		}else if( a->getType() == PeakSpace::Update::Remove )
		{
			d_sel.remove( a->getId() );
			d_model->getPos( a->getId(), pos, d_buf->getSpectrum() );
			damage( pos );
		}else if( a->getType() == PeakSpace::Update::Pos )
		{
			damage( a->getOldPos() );
			d_model->getPos( a->getId(), pos, d_buf->getSpectrum() );
			damage( pos );
		}else if( a->getType() == PeakSpace::Update::PeakData )
		{
			d_model->getPos( a->getId(), pos, d_buf->getSpectrum() );
			damage( pos );
		}else if( a->getType() == PeakSpace::Update::All )
		{
			d_sel.clear();
			damageAll();
		}
	}
	/*
	MESSAGE( PeakColorListUpdate, a, msg )
	{
		damageAll();
		msg.consume();
	}
	*/
	END_HANDLER();
}

void PeakPlaneView::draw(Canvas& c, const Lexi2::Allocation& a)
{
	if( d_model.isNull() || d_buf.isNull() || !d_show )
		return;
	ViewAreaMdl* area = d_buf->getViewArea();

	PeakSpace::Selection peaks;
	d_model->findAll( peaks, d_buf->getSpectrum() );
	PeakSpace::PeakData peak;

	Coord x, y, lx, ly;
	PeakModel* mdl = 0;
	// NOTE Alias wird von PeakSpace automatisch berücksichtigt.
	QColor clr = d_color;
	if( d_label != NONE )
	{
		for( int pos = 0; pos < peaks.size(); ++pos )
		{
			d_model->getPeak( peaks[pos], peak, d_buf->getSpectrum() );
			if( d_colors )
			{
				clr = d_colors->getColor( peak.d_color );
				if( !clr.isValid() )
					clr = d_color;
			}else
				clr = d_color;
			if( peak.isGhost() )
			{
				if( peak.isHigher() )
					clr = s_posClr;
				if( peak.isLower() )
					clr = s_negClr;
			}
			ly = y = area->toTwip( peak.d_pos[ DimY ], a.getTop(), DimY );
			lx = x = area->toTwip( peak.d_pos[ DimX ], a.getLeft(), DimX );

			if( d_off == 0 )
			{
				if( d_pw )
				{
					if( peak.isAlias() )
					{
						c.drawLine( x - g_d, y - g_d, x + g_d + g_p, y + g_d + g_p, clr, d_pw );
						c.drawLine( x - g_d, y + g_d, x + g_d + g_p, y - g_d - g_p, clr, d_pw );
					}else
					{
						c.drawLine( x - g_w, y, x + g_w + g_p, y, clr, d_pw );
						c.drawLine( x, y - g_w, x, y + g_w + g_p, clr, d_pw );
					}
				}
			}else
			{
				const float a = d_angle * 2.0 * Root::Math::Pi / 360.0;
				lx = Roundoff + float( x ) + float( d_off ) * ::cos( a );
				ly = Roundoff + float( y ) - float( d_off ) * ::sin( a );
				if( d_pw )
					c.drawLine( x, y, lx, ly, clr, d_pw );
				ly += g_w;
			}
			switch( d_label )
			{
			case ID_OR_TAG:
				if( peak.d_tag.isNull() )
					::sprintf( s_buf, "%d", peaks[pos] );
				else
					::sprintf( s_buf, "%s", peak.d_tag.c_str() );
				break;
			case TAG:
				::sprintf( s_buf, "%s", peak.d_tag.c_str() );
				break;
			case MODEL:
				::sprintf( s_buf, "%d", peak.d_model );
				break;
			case AMP:
				::sprintf( s_buf, "%0.1f", peak.d_amp );
				break;
			case VOL:
				::sprintf( s_buf, "%0.1f", peak.d_vol );
				break;
			case AMP_MODEL_VOL:
				::sprintf( s_buf, "%0.0f/%d/%0.0f", peak.d_amp, peak.d_model, peak.d_vol );
				break;
			case ASSIG:
				{
					std::string tmp;
					for( int i = 0; i < d_model->getDimCount(); i++ )
					{
						::sprintf( s_buf, "%d", peak.d_assig[i] );
						if( i != 0 )
							tmp += "/";
						tmp += s_buf;
					}
					::strcpy( s_buf, tmp.c_str() );
				}
				break;
			case COLOR:
				::sprintf( s_buf, "%d", peak.d_color );
				break;
			case ID:
			default:
				::sprintf( s_buf, "%d", peaks[pos] );
				break;
			}
			if( !peak.isGhost() )
			{
				c.drawText( lx + g_w, ly - g_w, s_buf, &d_font, clr );
				if( d_sel.contains( peaks[pos] ) )
					c.drawRect( lx, ly, 
						d_lw * float( ::strlen( s_buf ) + 1 ), -d_lh, clr, d_pw );
			}
		}
	}else
	{
		for( int pos = 0; pos < peaks.size(); ++pos )
		{
			d_model->getPeak( peaks[pos], peak, d_buf->getSpectrum() );
			if( d_colors )
			{
				clr = d_colors->getColor( peak.d_color );
				if( !clr.isValid() )
					clr = d_color;
			}else
				clr = d_color;
			if( peak.isGhost() )
			{
				if( peak.isHigher() )
					clr = s_posClr;
				if( peak.isLower() )
					clr = s_negClr;
			}
			y = area->toTwip( peak.d_pos[ DimY ], a.getTop(), DimY );
			x = area->toTwip( peak.d_pos[ DimX ], a.getLeft(), DimX );

			c.drawLine( x - g_w, y, x + g_w, y, clr, d_pw );
			c.drawLine( x, y - g_w, x, y + g_w, clr, d_pw );

			if( !peak.isGhost() && d_sel.contains( peaks[pos] ) )
				c.drawEllipse( x - 80.0, y - 80.0, 180.0, 180.0, clr );
		}
	}
}

void PeakPlaneView::showLabels(bool on)
{
	if( on )
		d_label = ID_OR_TAG;
	else
		d_label = NONE;
	damageAll();
}

void PeakPlaneView::setModel(PeakSpace * mdl)
{
	if( d_model == mdl )
		return;
	if( mdl && mdl->getDimCount() < 2 )
		throw Root::Exception( "Invalid number of Dimensions" );;
	if( d_model )
		d_model->removeObserver( this );
	d_model = mdl;
	if( d_model )
		d_model->addObserver( this );
	d_sel.clear();
	damageAll();
}

void PeakPlaneView::selectPeak(const PpmCube & c)
{
	if( d_model.isNull() )
		return;
	if( c[ DimX ].getWidth() == 0.0 && c[ DimY ].getWidth() == 0.0 )
		selectSingle( c[ DimX ].first, c[ DimY ].first, false );
	else
	{
		PeakSpace::Selection s;
		d_model->find( c, s, d_buf->getSpectrum(), false ); // Kein Ghost
		PeakPos q;
		for( int i = 0; i < s.size(); ++i )
		{
			d_model->getPos( s[i], q );
			d_sel.insert( s[i] );
			damage( q );
		}
	}
}

void PeakPlaneView::selectPeak(PPM x, PPM y, bool replace)
{
	if( d_model.isNull() )
		return;
	selectSingle( x, y, replace );
}

void PeakPlaneView::selectSingle(PPM x, PPM y, bool replace)
{
	assert( d_buf );

	PpmCube cube;
	cube.assign( 2, PpmRange() );
	ViewAreaMdl* area = d_buf->getViewArea();
	cube[ DimX ].allocate( x, area->toPpmDiff( g_tol, 0, DimX ) );
	cube[ DimY ].allocate( y, area->toPpmDiff( g_tol, 0, DimY ) );

	PeakPos q;
	int i;
	Selection::const_iterator j;
	PeakSpace::Selection s;
	d_model->find( cube, s, d_buf->getSpectrum(), false );	// Keine Ghosts
	if( replace )
	{
		d_found = s.size();
		if( d_sel.size() <= 1 && s.size() > 1 )
		{
			// Wenn wir auf einem Peak-Cluster sind, gehe alle der Reihe nach durch
			Index t;
			if( d_sel.size() == 1 )
			{
				i = s.find( *d_sel.begin() );
				++i;
				if( i >= s.size() )
					i = 0;
				t = s[i];
			}else
				t = s.first();
			s.reset();
			s.push_back( t );
		}
		for( j = d_sel.begin(); j != d_sel.end(); ++j )
		{
			d_model->getPos( (*j), q, d_buf->getSpectrum() );
			damage( q );
		}
		d_sel.clear();
		for( i = 0; i < s.size(); i++ )
			d_sel.insert( s[i] );
		for( j = d_sel.begin(); j != d_sel.end(); ++j )
		{
			d_model->getPos( (*j), q, d_buf->getSpectrum() );
			damage( q );
		}
	}else
	{
		for( i = 0; i < s.size(); i++ )
		{
			d_model->getPos( s[i], q, d_buf->getSpectrum() );
			d_sel.insert( s[i] );
			damage( q );
		}
	}
}

void PeakPlaneView::damage(const PeakPos & p)
{
	// TODO: Damage müsste noch d_off und d_angle berücksichtigen
	assert( d_buf );
	ViewAreaMdl* area = d_buf->getViewArea();
	Coord y = area->toTwip( p[ DimY ], DimY );
	Coord x = area->toTwip( p[ DimX ], DimX );
	if( d_label != NONE )
		getViewport()->damage( Allocation( x - g_w, y - d_lh - 60.0, 
			( Root::Tag::MaxLen + 1 ) * d_lw + g_w, d_lh + g_w + 20.0 ) );
	else
		getViewport()->damage( Allocation( x - 80.0, y - 80.0, 180.0, 180.0 ) );
}


Index PeakPlaneView::getHit(PPM x, PPM y) const
{
	if( d_model.isNull() || d_buf.isNull() )
		return 0;

	PpmCube cube;
	cube.assign( 2, PpmRange() );
	ViewAreaMdl* area = d_buf->getViewArea();
	cube[ DimX ].allocate( x, area->toPpmDiff( g_tol, 0, DimX ) );
	cube[ DimY ].allocate( y, area->toPpmDiff( g_tol, 0, DimY ) );

	PeakSpace::Selection s;
	d_model->find( cube, s, d_buf->getSpectrum(), false ); // Kein Ghost
	if( s.empty() )
		return 0;
	else
		return s.first();
}

void PeakPlaneView::setLabel(Label l)
{
	if( d_label == l )
		return;
	d_label = l;
	damageAll();
}

void PeakPlaneView::select(Index p)
{
	PeakPos q;
	Selection::iterator p1;
	for( p1 = d_sel.begin(); p1 != d_sel.end(); ++p1 )
	{
		// Deselect old.
		d_model->getPos( (*p1), q, d_buf->getSpectrum() );
		damage( q );
	}
	d_sel.clear();
	try
	{
		PeakSpace::PeakData pd;
		d_model->getPeak( p, pd, d_buf->getSpectrum() );
		if( !pd.isGhost() )
		{
			d_sel.insert( p );
			damage( pd.d_pos );
		}
	}catch(...)
	{
	}
}

void PeakPlaneView::setAngle(float a)
{
	d_angle = a;
	damageAll();
}

void PeakPlaneView::setOff(Coord o)
{
	d_off = o;
	damageAll();
}

void PeakPlaneView::setWidth( Coord w)
{
	if( w >= 0 )
		d_pw = w;
	damageAll();
}

void PeakPlaneView::setColor(QColor clr)
{
	d_color = clr;
	damageAll();
}

void PeakPlaneView::setFont(const QFont& f)
{
	d_font = f;
	FontBoundingBox box;
	box.stringBox( d_font, "0" );
	d_lw = box.getWidth();
	d_lh = box.getHeight();
	damageAll();
}

void PeakPlaneView::setVisi(bool on)
{
	if( on == d_show )
		return;
	d_show = on;
	damageAll();
}

bool PeakPlaneView::formatSelection( std::string& s, PeakPlaneView* v, int lim )
{
	const PeakPlaneView::Selection& sel = v->getSel();
	if( sel.empty() )
		return false; // RISK
	QString str;
	if( v->d_found != 0 && v->d_found != sel.size() && sel.size() == 1 )
		str.sprintf( "Found %d peaks, one selected: ", v->d_found, sel.size() );
	else
		str.sprintf( "Selected %d peaks: ", sel.size() );
	int i = 0;
	PeakPlaneView::Selection::const_iterator p;
	PeakSpace* ps = v->getModel();
	Root::SymbolString tag;
	for( p = sel.begin(); p != sel.end(); ++p, i++ )
	{
		if( i < lim )
		{
			tag = ps->getTag( (*p) );
			if( tag.isNull() )
				::sprintf( s_buf, "%d", (*p) );
			else
				::sprintf( s_buf, "%s", tag.c_str() );
			str += s_buf;
			if( i < sel.size() - 1 )
				str += ", ";
		}else if( lim < sel.size() )
		{
			str += "...";
			break;
		}else
			break;
	}
	s = str.toLatin1().data();
	return true;
}
