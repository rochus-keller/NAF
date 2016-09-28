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

#include "PeakPlaneView.h"
#include <SpecView/SpecViewer.h>
#include <SpecView/ViewAreaMdl.h>
#include <stdio.h>
using namespace Spec;
using namespace Lexi;

static const Twips g_w = 60;	// Lnge eines Kreuzsegments
static const Twips g_tol = 160;
static const Twips g_p = 20;	
static const Twips g_d = 40;	// Lnge eines diag Kreuzsegments


static char s_buf[50];

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
 
QFont PeakPlaneView::s_default( "", 8 );
static QColor s_posClr = Qt::cyan;
static QColor s_negClr = Qt::magenta;

PeakPlaneView::PeakPlaneView(SpecViewer* vr, PeakSpace* mdl,
							 QColor color, const QFont* font,
							 PeakColorList* colors ):
	SpecView( vr ), d_model( mdl ), d_label( ID_OR_TAG ), d_colors( colors ),
	d_found( 0 ), d_spec( 0 )
{
	if( font )
		d_font = *font;
	Font::BoundingBox box;
	box.stringBox( d_font, "0" );
	d_lw = box.getWidth() * 1.3; // RISK
	d_lh = box.getHeight() * 1.3;
	d_color = color;
	if( d_model )
		d_model->addObserver( this );
	if( d_colors )
		d_colors->addObserver( this );
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
}

void PeakPlaneView::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( PeakSpace::Update, a, msg )
	{
		PeakPos pos;
		switch( a->getType() )
		{
		case PeakSpace::Update::Add:
			d_model->getPos( a->getId(), pos, d_spec );
			damage( pos );
			break;
		case PeakSpace::Update::Remove:
			d_sel.erase( a->getId() );
			d_model->getPos( a->getId(), pos, d_spec );
			damage( pos );
			break;
		case PeakSpace::Update::Pos:
			damage( a->getOldPos() );
			d_model->getPos( a->getId(), pos, d_spec );
			damage( pos );
			break;
		case PeakSpace::Update::Tag:
		case PeakSpace::Update::PeakData:
		case PeakSpace::Update::Assig:
		case PeakSpace::Update::Color:
		case PeakSpace::Update::AmpVol:
		case PeakSpace::Update::Model:
			d_model->getPos( a->getId(), pos, d_spec );
			damage( pos );
			break;
		case PeakSpace::Update::All:
			d_sel.clear();
			getViewArea()->redraw(getViewport(false));
			break;
        default:
            break;
		}
		msg.consume();
	}
	MESSAGE( PeakColorList::Update, a, msg )
	{
        Q_UNUSED(a)
		getViewArea()->redraw(getViewport(false));
		msg.consume();
	}
	END_HANDLER();
}

void PeakPlaneView::draw( Canvas& v, const Lexi::Allocation& a)
{
	if( d_model.isNull() || !d_show )
		return;
	ViewAreaMdl* area = getViewArea();
	Canvas& c = v;

	PeakSpace::Selection peaks;
	d_model->findAll( peaks, d_spec );
	PeakSpace::PeakData peak;

	Twips x, y, lx, ly;
    //PeakModel* mdl = 0;
	// NOTE Alias wird von PeakSpace automatisch bercksichtigt.
	QColor clr = d_color;
	if( d_label != NONE )
	{
		for( int pos = 0; pos < peaks.size(); ++pos )
		{
			d_model->getPeak( peaks[pos], peak, d_spec );
            if( peak.isOutOfRange() )
                continue;
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
						c.drawLine( x - g_d, y - g_d, x + g_d + g_p - 20, y + g_d + g_p - 20, clr, d_pw );
						c.drawLine( x - g_d, y + g_d, x + g_d + g_p - 20, y - g_d - g_p + 20, clr, d_pw );
					}else
					{
						c.drawLine( x - g_w, y, x + g_w + g_p - 20, y, clr, d_pw );
						c.drawLine( x, y - g_w, x, y + g_w + g_p - 20, clr, d_pw );
					}
				}
			}else
			{
				const float a = d_angle * 2.0 * Root::Math::Pi / 360.0;
				lx = 0.5f + float( x ) + float( d_off ) * ::cos( a );
				ly = 0.5f + float( y ) - float( d_off ) * ::sin( a );
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
					::sprintf( s_buf, "%s", peak.d_tag.data() );
				break;
			case TAG:
				::sprintf( s_buf, "%s", peak.d_tag.data() );
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
					QByteArray  tmp;
					for( int i = 0; i < d_model->getDimCount(); i++ )
					{
						::sprintf( s_buf, "%d", peak.d_assig[i] );
						if( i != 0 )
							tmp += "/";
						tmp += s_buf;
					}
					::strcpy( s_buf, tmp.data() );
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
				if( d_sel.count( peaks[pos] ) != 0 )
					c.drawRect( lx, ly, d_lw * ( ::strlen( s_buf ) + 1 ), -d_lh, clr, d_pw );
					// c.drawRect( lx, ly - d_lh + 20, d_lw * ( ::strlen( s_buf ) + 1 ), d_lh, clr, d_pw );
			}
		}
	}else
	{
		for( int pos = 0; pos < peaks.size(); ++pos )
		{
			d_model->getPeak( peaks[pos], peak, d_spec );
            if( peak.isOutOfRange() )
                continue;
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

			if( !peak.isGhost() && d_sel.count( peaks[pos] ) != 0 )
				c.drawEllipse( x - 80, y - 80, 180, 180, clr );
		}
	}
}

void PeakPlaneView::showLabels(bool on)
{
	if( on )
		d_label = ID_OR_TAG;
	else
		d_label = NONE;
	damageMe();
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
	damageMe();
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
		d_model->find( c, s, d_spec, false ); // Kein Ghost
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
	PpmCube cube;
	cube.assign( 2, PpmRange() );
	cube[ DimX ].allocate( x, getViewArea()->toPpmDiff( g_tol, 0, DimX ) );
	cube[ DimY ].allocate( y, getViewArea()->toPpmDiff( g_tol, 0, DimY ) );

	PeakPos q;
	int i;
	Selection::const_iterator j;
	PeakSpace::Selection s;
	d_model->find( cube, s, d_spec, false );	// Keine Ghosts
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
			d_model->getPos( (*j), q, d_spec );
			damage( q );
		}
		d_sel.clear();
		for( i = 0; i < s.size(); i++ )
			d_sel.insert( s[i] );
		for( j = d_sel.begin(); j != d_sel.end(); ++j )
		{
			d_model->getPos( (*j), q, d_spec );
			damage( q );
		}
	}else
	{
		for( i = 0; i < s.size(); i++ )
		{
			d_model->getPos( s[i], q, d_spec );
			d_sel.insert( s[i] );
			damage( q );
		}
	}
}

void PeakPlaneView::damage(const PeakPos & p)
{
	// TODO: Damage msste noch d_off und d_angle bercksichtigen
	ViewAreaMdl* area = getViewArea();
	Twips y = area->toTwip( p[ DimY ], DimY );
	Twips x = area->toTwip( p[ DimX ], DimX );
	if( d_label != NONE )
		getViewport()->damage( Allocation( x - g_w - 20, y - d_lh, 
			( Root::Tag::MaxLen + 3 ) * d_lw + g_w + 20, d_lh + g_w + 20 ) );
	else
		getViewport()->damage( Allocation( x - g_w - 20, y - g_w - 20, 
			d_lw + g_w + 60, d_lw + g_w + 60 ) );
}


Index PeakPlaneView::getHit(PPM x, PPM y) const
{
	if( d_model.isNull() )
		return 0;

	PpmCube cube;
	cube.assign( 2, PpmRange() );
	cube[ DimX ].allocate( x, getViewArea()->toPpmDiff( g_tol, 0, DimX ) );
	cube[ DimY ].allocate( y, getViewArea()->toPpmDiff( g_tol, 0, DimY ) );

	PeakSpace::Selection s;
	d_model->find( cube, s, d_spec, false ); // Kein Ghost
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
	damageMe();
}

void PeakPlaneView::select(Index p)
{
	PeakPos q;
	Selection::iterator p1;
	for( p1 = d_sel.begin(); p1 != d_sel.end(); ++p1 )
	{
		// Deselect old.
		d_model->getPos( (*p1), q, d_spec );
		damage( q );
	}
	d_sel.clear();
	try
	{
		PeakSpace::PeakData pd;
		d_model->getPeak( p, pd, d_spec );
        if( !pd.isGhost() && !pd.isOutOfRange() )
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
	damageMe();
}

void PeakPlaneView::setOff(Twips o)
{
	d_off = o;
	damageMe();
}

void PeakPlaneView::setWidth( Twips w)
{
	if( w >= 0 )
		d_pw = w;
	damageMe();
}

void PeakPlaneView::setColor(QColor clr)
{
	d_color = clr;
	damageMe();
}

void PeakPlaneView::setFont(const QFont& f)
{
	d_font = f;
	Font::BoundingBox box;
	box.stringBox( f, "0" );
	d_lw = box.getWidth();
	d_lh = box.getHeight();
	damageMe();
}

void PeakPlaneView::show(bool on)
{
	d_show = on;
	damageMe();
}

bool PeakPlaneView::formatSelection( QByteArray & s, PeakPlaneView* v, int lim )
{
	const PeakPlaneView::Selection& sel = v->getSel();
	if( sel.empty() )
		return false; // RISK
	QString str;
    if( v->d_found != 0 && v->d_found != int(sel.size()) && sel.size() == 1 )
        str.sprintf( "Found %d peaks, one selected: ", v->d_found );
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
				::sprintf( s_buf, "%d %s", (*p), tag.data() );
			str += s_buf;
            if( i < int(sel.size()) - 1 )
				str += ", ";
        }else if( lim < int(sel.size()) )
		{
			str += "...";
			break;
		}else
			break;
	}
	s = str.toLatin1().data();
	return true;
}

void PeakPlaneView::setSpec(Spectrum * s)
{
	d_spec = s;
	damageMe();
}

//////////////////////////////////////////////////////////////////////

PeakSliceView::PeakSliceView(SpecViewer* v, PeakSpace* pl, QColor c, 
							 const QFont* f, PeakColorList* cl):
	PeakPlaneView( v, pl, c, f, cl )
{

}

void PeakSliceView::draw( Canvas& v, const Lexi::Allocation& a)
{
	if( d_model.isNull() || !d_show )
		return;
	ViewAreaMdl* area = getViewArea();
	Canvas& c = v;

	PeakSpace::Selection peaks;
	d_model->findAll( peaks );
	PeakSpace::PeakData peak;
	
	Twips x, lx;
    //PeakModel* mdl = 0;
	// NOTE Alias wird von PeakSpace automatisch bercksichtigt.
	QColor clr = d_color;

	for( int pos = 0; pos < peaks.size(); ++pos )
	{
		d_model->getPeak( peaks[pos], peak, d_spec );
		if( d_colors )
		{
			clr = d_colors->getColor( peak.d_color );
			if( !clr.isValid() )
				clr = d_color;
		}
		lx = x = area->toTwip( peak.d_pos[ DimX ], a.getLeft(), DimX );
		c.drawLine( x, a.getTop(), x, a.getBottom(), clr );

		switch( d_label )
		{
		case ID_OR_TAG:
			if( peak.d_tag.isNull() )
				::sprintf( s_buf, "%d", peaks[pos] );
			else
				::sprintf( s_buf, "%s", peak.d_tag.data() );
			break;
		case TAG:
			::sprintf( s_buf, "%s", peak.d_tag.data() );
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
		case ID:
		default:
			::sprintf( s_buf, "%d", peaks[pos] );
			break;
		}
		c.drawText( lx + g_w, a.getBottom() - g_w, s_buf, &d_font, clr );
		if( d_sel.count( peaks[pos] ) != 0 )
			c.drawRect( lx, a.getBottom() - d_lh, 
				d_lw * ( ::strlen( s_buf ) + 1 ), d_lh, clr, d_pw );
	}
}

void PeakSliceView::selectPeak(const PpmCube & c)
{
	if( d_model.isNull() )
		return;
	if( c[ DimX ].getWidth() == 0.0 )
		selectPeak( c[ DimX ].first,  false );
	else
	{
		PeakSpace::Selection s;
		d_model->find( c, s, d_spec );
		PeakPos q;
		for( int i = 0; i < s.size(); ++i )
		{
			d_model->getPos( s[i], q );
			d_sel.insert( s[i] );
			damage( q );
		}
	}
}

void PeakSliceView::selectPeak(PPM x )
{
	if( d_model.isNull() )
		return;
	selectPeak( x, true );
}

void PeakSliceView::selectPeak(PPM x, bool replace)
{
	PpmCube cube;
	cube.assign( 1, PpmRange() );
	cube[ DimX ].allocate( x, getViewArea()->toPpmDiff( g_tol, 0, DimX ) );

	PeakPos q;
	Selection::iterator p;
	PeakSpace::Selection s;
	if( replace )
	{
		for( p = d_sel.begin(); p != d_sel.end(); ++p )
		{
			d_model->getPos( (*p), q, d_spec );
			damage( q );
		}
		d_model->find( cube, s, d_spec );
		d_sel.clear();
		for( int i = 0; i < s.size(); i++ )
			d_sel.insert( s[i] );
		for( p = d_sel.begin(); p != d_sel.end(); ++p )
		{
			d_model->getPos( (*p), q, d_spec );
			damage( q );
		}
	}else
	{
		for( int i = 0; i < s.size(); i++ )
		{
			d_model->getPos( s[i], q, d_spec );
			d_sel.insert( s[i] );
			damage( q );
		}
	}
}

void PeakSliceView::damage(const PeakPos & p)
{
	// TODO: Damage msste noch d_off und d_angle bercksichtigen
	ViewAreaMdl* area = getViewArea();
	Twips x = area->toTwip( p[ DimX ], DimX );
	const Allocation& a = area->getAllocation();
	getViewport()->damage( Allocation( x, a.getBottom() - d_lh - g_w, 
		( Root::Tag::MaxLen + 1 ) * d_lw + g_w, d_lh + g_w ) );
	getViewport()->damage( Allocation( x, a.getTop(), d_lw, a.getHeight() ) );
}

