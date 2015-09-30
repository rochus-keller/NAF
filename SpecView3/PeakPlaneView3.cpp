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

#include "PeakPlaneView3.h"
#include <QPainter>
#include <QApplication>
#include <Spec/PointMdl.h>
#include <stdio.h>
using namespace Spec;

static const Points g_w = 3.0;	// Lnge eines Kreuzsegments
static const Points g_tol = 8.0;
static const Points g_p = 1.0;
static const Points g_d = 2.0;	// Lnge eines diag Kreuzsegments


static char s_buf[32];

const char* PeakPlaneView3::menuText[] =
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
 
QFont PeakPlaneView3::s_default( "", 8 ); // RISK wegen statischer Initialisierung
static QColor s_posClr = Qt::cyan;
static QColor s_negClr = Qt::magenta;

PeakPlaneView3::PeakPlaneView3(ViewAreaMdl3 *vr, PeakSpace* mdl,
							 QColor color, const QFont* font,
							 PeakColorList* colors ):
    SpecView3( vr ), d_model( mdl ), d_label( ID_OR_TAG ), d_colors( colors ),
    d_found( 0 ), d_spec( 0 ), d_curX(DimNone), d_curY(DimNone)
{
    Q_ASSERT( vr != 0 );
	if( font )
		d_font = *font;
	d_color = color;
	if( d_model )
		d_model->addObserver( this );
	if( d_colors )
		d_colors->addObserver( this );
	d_off = 0;
	d_angle = 45;
    d_pw = 1.0;
}

PeakPlaneView3::~PeakPlaneView3()
{
	if( d_model )
		d_model->removeObserver( this );
	if( d_colors )
		d_colors->removeObserver( this );
}

void PeakPlaneView3::handle( Root::Message & msg )
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
            getArea()->damageAll();
			break;
        default:
            break;
		}
		msg.consume();
	}
	MESSAGE( PeakColorList::Update, a, msg )
	{
        Q_UNUSED(a)
        getArea()->damageAll();
		msg.consume();
	}
    MESSAGE( PointMdl::Updating, a, msg )
    {
        if( !( a->hasDim( d_curX ) && a->hasDim( d_curY ) ) )
            return;
        const Root::Index peak = getHit( a->getNew( d_curX ), a->getNew( d_curY ) );
        if( peak != 0 )
        {
            PeakPos p;
            d_model->getPos( peak, p, d_spec );
            a->override( d_curX, p[ DimX ], d_curY, p[ DimY ] );
            msg.consume();
        }
    }
    END_HANDLER();
}

void PeakPlaneView3::paint( QPainter& painter )
{
    if( d_model.isNull() )
		return;
    ViewAreaMdl3* area = getArea();

	PeakSpace::Selection peaks;
	d_model->findAll( peaks, d_spec );
	PeakSpace::PeakData peak;

	// NOTE Alias wird von PeakSpace automatisch bercksichtigt.
	if( d_label != NONE )
	{
		for( int pos = 0; pos < peaks.size(); ++pos )
		{
			d_model->getPeak( peaks[pos], peak, d_spec );
            if( peak.isOutOfRange() )
                continue;
            QColor clr;
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
            const Points y = area->toPoints( peak.d_pos[ DimY ], DimY );
            const Points x = area->toPoints( peak.d_pos[ DimX ], DimX );

            painter.setPen( QPen( clr, d_pw ) );
            Points lx = x; // Position des Labels
            Points ly = y;
            if( d_off > 0.0 )
            {
                const double a = d_angle * 2.0 * Root::Math::Pi / 360.0;
                lx = 0.5 + x + d_off * ::cos( a );
                ly = 0.5 + y - d_off * ::sin( a );
                if( d_pw > 0.0 )
                {
                    // Fahnenstange
                    painter.setPen( QPen( clr, d_pw ) );
                    painter.drawLine( QPointF( x, y ), QPointF( lx, ly ) );
                }
                ly += g_w;
            }else
			{
                if( d_pw > 0.0 )
				{
					if( peak.isAlias() )
					{
                        // schräges Kreuz
                        painter.drawLine( QPointF( x - g_d, y - g_d ), QPointF( x + g_d + 1.0, y + g_d + 1.0 ) );
                        painter.drawLine( QPointF( x - g_d, y + g_d ), QPointF( x + g_d + 1.0, y - g_d - 1.0 ) );
                    }else
					{
                        // normales Kreuz
                        painter.drawLine( QPointF( x - g_w, y ), QPointF( x + g_w, y ) );
                        painter.drawLine( QPointF( x, y - g_w ), QPointF( x, y + g_w ) );
                    }
				}
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
                painter.setFont( d_font );
                painter.setPen( QPen( clr, d_pw ) );
                painter.drawText( QPointF( lx + g_w, ly - g_w ), s_buf );
                const QRectF bound = QFontMetricsF( painter.font() ).boundingRect( s_buf );
                if( d_sel.count( peaks[pos] ) != 0 )
                    // Selektionsrahmen
                    painter.drawRect( QRectF( lx, ly, bound.width() + 2.0 * g_w, -bound.height() ) );
			}
		}
	}else
	{
		for( int pos = 0; pos < peaks.size(); ++pos )
		{
			d_model->getPeak( peaks[pos], peak, d_spec );
            if( peak.isOutOfRange() )
                continue;
            QColor clr;
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
            const Points y = area->toPoints( peak.d_pos[ DimY ], DimY );
            const Points x = area->toPoints( peak.d_pos[ DimX ], DimX );

            painter.setPen( QPen( clr, d_pw ) );
            painter.drawLine( QPointF( x - g_w, y ), QPointF( x + g_w, y ) );
            painter.drawLine( QPointF( x, y - g_w ), QPointF( x, y + g_w ) );

			if( !peak.isGhost() && d_sel.count( peaks[pos] ) != 0 )
                painter.drawRect( QRectF( x - g_w - 1.0, y - g_w - 1.0, 2.0 * g_w + 2.0, 2.0 * g_w + 2.0 ) ); // Quadrat um Kreuz
        }
	}
}

void PeakPlaneView3::showLabels(bool on)
{
	if( on )
		d_label = ID_OR_TAG;
	else
		d_label = NONE;
    getArea()->damageAll();
}

void PeakPlaneView3::setModel(PeakSpace * mdl)
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
    getArea()->damageAll();
}

void PeakPlaneView3::selectPeak(const PpmCube & c, bool toggle )
{
	if( d_model.isNull() )
		return;
	if( c[ DimX ].getWidth() == 0.0 && c[ DimY ].getWidth() == 0.0 )
        selectSingle( c[ DimX ].first, c[ DimY ].first, true, toggle );
	else
	{
		PeakSpace::Selection s;
		d_model->find( c, s, d_spec, false ); // Kein Ghost
		PeakPos q;
		for( int i = 0; i < s.size(); ++i )
		{
			d_model->getPos( s[i], q );
            if( toggle && d_sel.count( s[i] ) > 0 )
                d_sel.erase( s[i] );
            else
                d_sel.insert( s[i] );
			damage( q );
		}
	}
}

void PeakPlaneView3::selectPeak(PPM x, PPM y, bool iterate, bool toggle)
{
	if( d_model.isNull() )
		return;
    selectSingle( x, y, iterate, toggle );
}

void PeakPlaneView3::selectSingle(PPM x, PPM y, bool iterate, bool toggle)
{
	PpmCube cube;
	cube.assign( 2, PpmRange() );
    cube[ DimX ].allocate( x, getArea()->toPpmDiff( g_tol, 0, DimX ) );
    cube[ DimY ].allocate( y, getArea()->toPpmDiff( g_tol, 0, DimY ) );

	PeakPos q;
	int i;
	Selection::const_iterator j;
	PeakSpace::Selection s;
	d_model->find( cube, s, d_spec, false );	// Keine Ghosts
    if( iterate )
	{
		d_found = s.size();
		if( d_sel.size() <= 1 && s.size() > 1 )
		{
			// Wenn wir auf einem Peak-Cluster sind, gehe alle der Reihe nach durch
            Index t = -1;
			if( d_sel.size() == 1 )
			{
				i = s.find( *d_sel.begin() );
				++i;
                if( i >= s.size() && !toggle )
					i = 0;
                if( i < s.size() )
                    t = s[i];
			}else
				t = s.first();
			s.reset();
            if( t >= 0 )
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
            if( toggle && d_sel.count( s[i] ) > 0 )
                d_sel.erase( s[i] );
            else
                d_sel.insert( s[i] );
			damage( q );
		}
	}
}

void PeakPlaneView3::damage(const PeakPos & p)
{
    if( d_model.isNull() )
        return;
    ViewAreaMdl3* area = getArea();
    const Points y = area->toPoints( p[ DimY ], DimY );
    const Points x = area->toPoints( p[ DimX ], DimX );

    // Kreuz
    getArea()->damage( QRectF( x - g_w, y - g_w, 2.0 * g_w, 2.0 * g_w ) );

    if( d_label != NONE )
    {
        Points lx = x; // Position des Labels
        Points ly = y;
        if( d_off )
        {
            const double a = d_angle * 2.0 * Root::Math::Pi / 360.0;
            lx = 0.5 + x + d_off * ::cos( a );
            ly = 0.5 + y - d_off * ::sin( a );
            getArea()->damage( QRectF( x, y, lx - x, ly - y ) ); // Fahne
        }
        QFontMetrics fm( d_font );
        const QRectF box = fm.boundingRect( "12345678901234567890" );
        getArea()->damage( QRectF( lx, ly - box.height() - 2.0 * g_w, box.width() + 2.0 * g_w, box.height() + 4.0 * g_w ) );
    }else
        getArea()->damage( QRectF( x - g_w - 1.0, y - g_w - 1.0, 4.0 * g_w, 4.0 * g_w ) ); // wie Quadrat um Kreuz
}


Index PeakPlaneView3::getHit(PPM x, PPM y) const
{
	if( d_model.isNull() )
		return 0;

	PpmCube cube;
	cube.assign( 2, PpmRange() );
    cube[ DimX ].allocate( x, getArea()->toPpmDiff( g_tol, 0, DimX ) );
    cube[ DimY ].allocate( y, getArea()->toPpmDiff( g_tol, 0, DimY ) );

	PeakSpace::Selection s;
	d_model->find( cube, s, d_spec, false ); // Kein Ghost
	if( s.empty() )
		return 0;
	else
		return s.first();
}

void PeakPlaneView3::setLabel(Label l)
{
	if( d_label == l )
		return;
	d_label = l;
    getArea()->damageAll();
}

void PeakPlaneView3::select(Index p)
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

void PeakPlaneView3::setAngle(float a)
{
	d_angle = a;
    getArea()->damageAll();
}

void PeakPlaneView3::setOff(Points o)
{
	d_off = o;
    getArea()->damageAll();
}

void PeakPlaneView3::setWidth( Points w)
{
	if( w >= 0 )
		d_pw = w;
    getArea()->damageAll();
}

void PeakPlaneView3::setColor(QColor clr)
{
	d_color = clr;
    getArea()->damageAll();
}

void PeakPlaneView3::setFont(const QFont& f)
{
	d_font = f;
    getArea()->damageAll();
}

bool PeakPlaneView3::formatSelection( QByteArray & s, PeakPlaneView3* v, int lim )
{
	const PeakPlaneView3::Selection& sel = v->getSel();
	if( sel.empty() )
		return false; // RISK
	QString str;
    if( v->d_found != 0 && v->d_found != int(sel.size()) && sel.size() == 1 )
        str.sprintf( "Found %d peaks, one selected: ", v->d_found );
	else
		str.sprintf( "Selected %d peaks: ", sel.size() );
	int i = 0;
	PeakPlaneView3::Selection::const_iterator p;
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
				::sprintf( s_buf, "%s", tag.data() );
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

void PeakPlaneView3::setSpec(Spectrum * s)
{
	d_spec = s;
    getArea()->damageAll();
}

void PeakPlaneView3::adjustPositions(PointMdl * mdl, Dimension dx, Dimension dy)
{
    Q_ASSERT( mdl != 0 );
    Q_ASSERT( dy != DimNone );
    Q_ASSERT( dy != DimNone );
    mdl->addObserver( this );
    d_curX = dx;
    d_curY = dy;
}

//////////////////////////////////////////////////////////////////////

PeakSliceView3::PeakSliceView3(ViewAreaMdl3* v, PeakSpace* pl, QColor c,
							 const QFont* f, PeakColorList* cl):
	PeakPlaneView3( v, pl, c, f, cl )
{

}

void PeakSliceView3::paint( QPainter& )
{
    if( d_model.isNull() )
		return;
    // TODO
//    ViewAreaMdl3* area = getArea();
//	Canvas& c = v;

//	PeakSpace::Selection peaks;
//	d_model->findAll( peaks );
//	PeakSpace::PeakData peak;
	
//	// NOTE Alias wird von PeakSpace automatisch bercksichtigt.

//	for( int pos = 0; pos < peaks.size(); ++pos )
//	{
//		d_model->getPeak( peaks[pos], peak, d_spec );
//        QColor clr = d_color;
//        if( d_colors )
//		{
//			clr = d_colors->getColor( peak.d_color );
//			if( !clr.isValid() )
//				clr = d_color;
//		}
//        const Points x = area->toPoints( peak.d_pos[ DimX ], DimX );
//        painter.setPen( QPen( clr, d_pw ) );
//        painter.drawLine( QPointF( x, area->getOrig( DimY ) ), QPointF( x, area->getEnd( DimY ) ) );

//		switch( d_label )
//		{
//		case ID_OR_TAG:
//			if( peak.d_tag.isNull() )
//				::sprintf( s_buf, "%d", peaks[pos] );
//			else
//				::sprintf( s_buf, "%s", peak.d_tag.data() );
//			break;
//		case TAG:
//			::sprintf( s_buf, "%s", peak.d_tag.data() );
//			break;
//		case MODEL:
//			::sprintf( s_buf, "%d", peak.d_model );
//			break;
//		case AMP:
//			::sprintf( s_buf, "%0.1f", peak.d_amp );
//			break;
//		case VOL:
//			::sprintf( s_buf, "%0.1f", peak.d_vol );
//			break;
//		case AMP_MODEL_VOL:
//			::sprintf( s_buf, "%0.0f/%d/%0.0f", peak.d_amp, peak.d_model, peak.d_vol );
//			break;
//		case ID:
//		default:
//			::sprintf( s_buf, "%d", peaks[pos] );
//			break;
//		}
//		c.drawText( lx + g_w, a.getBottom() - g_w, s_buf, &d_font, clr );
//		if( d_sel.count( peaks[pos] ) != 0 )
//			c.drawRect( lx, a.getBottom() - d_lh,
//				d_lw * ( ::strlen( s_buf ) + 1 ), d_lh, clr, d_pw );
//	}
}

void PeakSliceView3::selectPeak(const PpmCube & c)
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

void PeakSliceView3::selectPeak(PPM x )
{
	if( d_model.isNull() )
		return;
	selectPeak( x, true );
}

void PeakSliceView3::selectPeak(PPM x, bool replace)
{
	PpmCube cube;
	cube.assign( 1, PpmRange() );
    cube[ DimX ].allocate( x, getArea()->toPpmDiff( g_tol, 0, DimX ) );

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

void PeakSliceView3::damage(const PeakPos & p)
{
    // TODO
}

