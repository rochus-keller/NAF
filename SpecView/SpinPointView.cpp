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

#include "SpinPointView.h"
#include <Spec/Residue.h>
#include <Spec/PeakList.h> // wegen PeakColorList
#include <SpecView/SpecViewer.h>
#include <SpecView/ViewAreaMdl.h>
#include <stdio.h>
using namespace Spec;
using namespace Lexi;

const char* SpinPointView::menuText[] = 
{
"None", 
"Main System Number", // SystemId, 
"Main Sys./Res. Number", // SysResiId, 
"Main Spin Number", // SpinId, 
"Main Tag", // TagOnly, 
"Main Tag && Offset", // TagOffset, 
"Main Label", // TagAll, 
"Main Number && Label", // IdTagAll,
"Main Label && System", // SysTagAll
"Main Label && Sys./Resi.", // SysOrResiTagAll
"Labels with Residue", // PairLabelResi, 
"Labels with System", // PairLabelSystem, 
"Labels with Sys. or Resi.", // PairLabelSysOrResi, 
"Lbl. or Num. with Sys. or Resi.", // PairIdLabelSysOrResi
"Spin Numbers", // PairSpinId, 
"System Numbers", // PairSystemId
"Spin and System Numbers", // PairSpinSystemId
"Numbers and Labels", // PairAll
};

static const Twips g_w = 60;	// Lnge eines Kreuzsegments
static const Twips g_p = 20;	
static const Twips g_d = 40;	// Lnge eines diag Kreuzsegments
static const Twips g_tol = 160;

static char s_buf[32];
static char s_ly[32];
static char s_lx[32];
static char s_sx[32];
static char s_sy[32];

QFont SpinPointView::s_defaultFont( "", 8 );
static QColor s_posClr = Qt::cyan;
static QColor s_negClr = Qt::magenta;
static QColor s_midClr = Qt::gray;
Twips SpinPointView::s_defaultOff = 0;
float SpinPointView::s_defaultAngle = 45;
bool SpinPointView::s_defaultCross = true;
static Twips s_lw = 1, s_lh = 1; // Label-Width und Height

SpinPointView::SpinPointView(SpecViewer* v, SpinSpace* stp, 
							 QColor clr, const QFont* f):
	SpecView( v ), d_model( stp ), d_label( SystemId ), d_refDim( 0 ), 
    d_found( 0 ), d_showGhost( true ), d_ghostLabel( false ), d_peakColors( 0 )
{
	if( f )
		d_font = *f;
	else
		d_font = s_defaultFont;
	Font::BoundingBox box;
	box.stringBox( d_font );
	s_lw = box.getWidth(); 
	s_lh = box.getHeight(); 
	d_color = clr;
	if( d_model )
		d_model->addObserver( this );
	d_off = s_defaultOff;
	d_angle = s_defaultAngle;
	d_showCross = s_defaultCross;
	d_pw = 20;
	d_show = true;
}

SpinPointView::~SpinPointView()
{
	if( d_model )
		d_model->removeObserver( this );
    Root::Resource::release( d_peakColors );
}

// static
bool SpinPointView::formatSelection( QByteArray & s, SpinPointView* v, Label l, int lim )
{
	const SpinPointView::Selection& sel = v->getSel();
	if( sel.empty() )
		return false; // RISK
	QString str;
    if( v->d_found != 0 && v->d_found != int(sel.size()) && sel.size() == 1 )
        str.sprintf( "Found %d peaks, one selected: ", v->d_found );
	else
		str.sprintf( "Selected %d peaks: ", sel.size() );
	int i = 0;
	SpinPointView::Selection::const_iterator p;
	for( p = sel.begin(); p != sel.end(); ++p, i++ )
	{
		if( i < lim )
		{
			SpinPointView::formatLabel( s_buf, sizeof( s_buf ), (*p),
				(l == None)?PairAll:l, v->getRefDim() );
			str += s_buf;
            if( i < int(sel.size() - 1) )
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

static inline void dump( const SpinPoint& p )
{
	int i = 0;
	qDebug( "**** Dump Point" );
	while( i < p.maxSize() && p[i] )
	{
        qDebug( "adr=%x id=%d", (unsigned int)p[i], p[i]->getId() );
		i++;
	}
}

void SpinPointView::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( SpinSpace::Update, a, msg )
	{
		PpmPoint p;
		switch( a->getType() )
		{
		case SpinSpace::Update::Create:
			d_sel.insert( a->getElem().d_point );
			// dump( a->getElem().d_point ); // TEST
			damage( a->getElem().d_point );
			break;
		case SpinSpace::Update::Delete:
			d_sel.erase( a->getElem().d_point );
			damage( a->getElem().d_point );
			break;
		case SpinSpace::Update::Move:
			//if( a->getOld().empty() )
				damageMe(); // TODO
			/*else // TODO
			{
				damage( a->getOld() ); 
				d_model->toPoint( a->getTuple(), p );
				damage( a->getElem().d_point );
			}*/
			break;
		/*case SpinSpace::Update::Flags:
		case SpinSpace::Update::Label:
			damage( a->getElem().d_point );
			break;*/
		case SpinSpace::Update::Param:
			damage( a->getElem().d_point );
			break;
		case SpinSpace::Update::All:
			checkSelection(); 
			damageMe();
			break;
		}
		// qDebug( "View %x cmd %d sel. count %d", this, a->getType(), d_sel.size() ); // TEST
		msg.consume();
	}
	END_HANDLER();
}

void SpinPointView::draw( Canvas& v, const Lexi::Allocation& a)
{
	if( d_model.isNull() || !d_show )
		return;
	if( d_model->getDimCount() < 2 )
		throw Root::Exception( "Invalid number of Dimensions" );;
	ViewAreaMdl* area = getViewArea();
	Canvas& c = v;

	PpmPoint p( 0, 0 );
	SpinSpace::Iterator pos, _end = d_model->end();
	SpinSpace::Element e;
	Twips x, y, lx, ly;
	QColor clr;
	bool alias, tmp, ghost;
	for( pos = d_model->begin(); pos != _end; ++pos )
	{
		alias = false;
		d_model->fetch( pos, e );
		ghost = e.isGhost();
		if( ghost && !d_showGhost )
			continue;
		if( !e.isInfer() && e.isHidden() )
			continue;
		p[ DimX ] = e.d_point[ DimX ]->getShift( d_model->getSpec(), tmp );
		if( tmp )
			alias = true;
		p[ DimY ] = e.d_point[ DimY ]->getShift( d_model->getSpec(), tmp );
		if( tmp )
			alias = true;
		ly = y = area->toTwip( p[ DimY ], a.getTop(), DimY );
		lx = x = area->toTwip( p[ DimX ], a.getLeft(), DimX );

        if( d_peakColors )
		{
            if( !ghost )
            {
                SpinLink* link = e.d_point[ DimX ]->findLink( e.d_point[ DimY ] );
                if( link != 0 )
                {
                    clr = d_peakColors->getColor( link->getCode( d_model->getSpec() ) );
                    if( !clr.isValid() )
                        clr = d_color;
                }else
                    clr = d_color;
            }else
                clr = s_midClr;
        }else if( ghost )
		{
			clr = s_midClr;
			if( e.isHigher() )
				clr = s_posClr;
			if( e.isLower() )
				clr = s_negClr;
		}else
			clr = d_color;
		if( d_showCross && d_pw )
		{
            if( d_peakColors == 0 )
            {
                // Normalfall, zeichne Kreuz und Farben für Ghosts
                if( alias )
                {
                    c.drawLine( x - g_d, y - g_d, x + g_d + g_p - 20, y + g_d + g_p - 20, clr, d_pw );
                    c.drawLine( x - g_d, y + g_d, x + g_d + g_p - 20, y - g_d - g_p + 20, clr, d_pw );
                }else
                {
                    c.drawLine( x - g_w, y, x + g_w + g_p - 20, y, clr, d_pw );
                    c.drawLine( x, y - g_w, x, y + g_w + g_p - 20, clr, d_pw );
                }
            }else
            {
                // Spezialfall; zeichne Symbole gem. Email von Fred von 2015-04-01 19:18
                if( alias )
                {
                    if( !e.isHigher() && !e.isLower() )
                    {
                        // schräges Kreuz
                        c.drawLine( x - g_d, y - g_d, x + g_d + 20, y + g_d + 20, clr, d_pw );
                        c.drawLine( x - g_d, y + g_d, x + g_d + 20, y - g_d - 20, clr, d_pw );
                    }else if( e.isHigher() )
                    {
                        // Peaks in einer hoeheren Ebene wuerde man mit der unteren Haelfte des x, also mit ^ darstellen
                        c.drawLine( x, y, x + g_d + 40, y + g_d + 40, clr, d_pw ); // mitte->rechts
                        c.drawLine( x, y, x - g_d - 40, y + g_d + 40, clr, d_pw ); // mitte->links
                    }else
                    {
                        c.drawLine( x, y, x + g_d + 40, y - g_d - 40, clr, d_pw ); // mitte->rechts
                        c.drawLine( x, y, x - g_d - 40, y - g_d - 40, clr, d_pw ); // mitte->links
                    }
                }else
                {
                    if( !e.isHigher() && !e.isLower() )
                    {
                        // normales Kreuz
                        c.drawLine( x - g_w, y, x + g_w, y, clr, d_pw );
                        c.drawLine( x, y - g_w, x, y + g_w, clr, d_pw );
                    }else if( e.isHigher() )
                    {
                        // Peaks in einer hoeheren Ebene mit  _|_  (also + ohne die untere Haelfte des senkrechten Strichs)
                        c.drawLine( x - g_w, y, x + g_w, y, clr, d_pw ); // hori
                        c.drawLine( x, y - g_w - 20, x, y, clr, d_pw ); // verti

                    }else
                    {
                        // Peaks in einer tieferen Ebene mit T (also + ohne die obere Haelfte des senkrechten Strichs)
                        c.drawLine( x - g_w, y, x + g_w, y, clr, d_pw ); // hori
                        c.drawLine( x, y, x, y + g_w + 20, clr, d_pw ); // verti
                    }
                }
            }
        }
		if( d_off )
		{
			const float a = d_angle * 2.0 * Root::Math::Pi / 360.0;
			lx = 0.5f + float( x ) + float( d_off ) * ::cos( a );
			ly = 0.5f + float( y ) - float( d_off ) * ::sin( a );
			if( !d_showCross && d_pw )
				c.drawLine( x, y, lx, ly, clr, d_pw );
			ly += g_w;
		}
		if( d_label != None && ( !ghost || d_ghostLabel ) )
		{
			char buf[32];
			formatLabel( buf, sizeof( buf ), e.d_point );
			if( ghost )
				lx += g_w;

			c.drawText( lx + g_w, ly - g_w, buf, &d_font, clr );
			Font::BoundingBox box;
			box.stringBox( d_font, buf );
			if( !ghost && d_sel.count( e.d_point ) != 0 )
				c.drawRect( lx, ly, box.getWidth() + 2 * g_w, 
					-( box.getHeight() + 2 * g_w ), clr, d_pw );
			else if( ghost )
			{
				ly -= box.getHeight() / 2;
				if( e.isHigher() )
				{
					c.drawEllipse( lx, ly - g_w - 20, g_w / 2, g_w / 2, clr, d_pw );
				}else if( e.isLower() )
				{
					c.drawEllipse( lx, ly + g_w + 20, g_w / 2 , g_w / 2, clr, d_pw );
				}
			}
		}else if( d_sel.count( e.d_point ) != 0 )
		{
			c.drawEllipse( x - 80, y - 80, 180, 180, clr, d_pw );
		}
	}
}

void SpinPointView::setModel(SpinSpace * mdl)
{
	if( d_model == mdl )
		return;
	if( d_model )
		d_model->removeObserver( this );
	d_model = mdl;
	if( d_model )
		d_model->addObserver( this );
	d_sel.clear();
	damageMe();
}

void SpinPointView::selectPeak(const PpmCube & c)
{
	if( d_model.isNull() )
		return;
	if( d_model->getDimCount() < 2 )
		throw Root::Exception( "Invalid number of Dimensions" );;
	if( c[ DimX ].getWidth() == 0.0 && c[ DimY ].getWidth() == 0.0 )
		selectSingle( c[ DimX ].first, c[ DimY ].first, false );
	else
	{
		SpinSpace::Result s;
		d_model->find( s, c, d_model->getSpec() );
		d_found = 0;
		SpinSpace::Result::const_iterator p;
		for( p = s.begin(); p != s.end(); ++p )
		{
			if( !(*p).isGhost() && ( (*p).isInfer() || !(*p).isHidden() ) )
			{
				d_sel.insert( (*p).d_point );
				damage( (*p).d_point );
			}
		}
	}
}

void SpinPointView::selectPeak(PPM x, PPM y)
{
	if( d_model.isNull() )
		return;
	selectSingle( x, y, true );
}

void SpinPointView::selectSingle(PPM x, PPM y, bool replace)
{
	PpmCube cube;
	cube.assign( 2, PpmRange() );
	cube[ DimX ].allocate( x, getViewArea()->toPpmDiff( g_tol, 0, DimX ) );
	cube[ DimY ].allocate( y, getViewArea()->toPpmDiff( g_tol, 0, DimY ) );

	selectSingle( cube, replace );
}

void SpinPointView::selectSingle( const PpmCube& cube, bool replace)
{
	SpinSpace::Result tmp;
	d_model->find( tmp, cube, d_model->getSpec() );
	Selection::const_iterator p;
	Selection s;
	SpinSpace::Result::const_iterator i;
	for( i = tmp.begin(); i != tmp.end(); ++i )
	{
		if( !(*i).isGhost() && ( (*i).isInfer() || !(*i).isHidden() ) )
			s.insert( (*i).d_point );
	}
	if( replace )
	{
		d_found = s.size();
		if( d_sel.size() <= 1 && s.size() > 1 )
		{
			// Wenn wir auf einem Peak-Cluster sind, gehe alle der Reihe nach durch
			SpinPoint t;
			if( d_sel.size() == 1 )
			{
				p = s.find( *d_sel.begin() );
				if( p != s.end() )
					++p;
				if( p == s.end() )
					p = s.begin();
				t = (*p);
			}else
				t = *s.begin();
			s.clear();
			s.insert( t );
		}
		for( p = d_sel.begin(); p != d_sel.end(); ++p )
			damage( (*p) );
		d_sel = s;
		for( p = d_sel.begin(); p != d_sel.end(); ++p )
			damage( (*p) );
	}else
	{
		d_found = 0;
		Selection::iterator p;
		for( p = s.begin(); p != s.end(); ++p )
		{
			d_sel.insert( (*p) );
			damage( (*p) );
		}
	}
}

void SpinPointView::damage(const SpinPoint & sp)
{
    if( d_model.isNull() )
		return;
	PpmPoint p( 0, 0 );
	p[ DimX ] = sp[ DimX ]->getShift( d_model->getSpec() );
	p[ DimY ] = sp[ DimY ]->getShift( d_model->getSpec() );
	ViewAreaMdl* area = getViewArea();
	const Twips y = area->toTwip( p[ DimY ], DimY );
	const Twips x = area->toTwip( p[ DimX ], DimX );
	const Twips h = s_lh + 2 * g_w + TwipsPerPoint;
	const Twips w = ( Root::Tag::MaxLen + 3 ) * s_lw + 2 * g_w + TwipsPerPoint;

	if( d_label != None )
		getViewport()->damage( Allocation( x, y - h + TwipsPerPoint, w, h ) );
	else
		getViewport()->damage( Allocation( x - g_w - 20, y - g_w - 20, 
			s_lw + g_w + 60, s_lw + g_w + 60 ) );
}


SpinPoint SpinPointView::getHit(PPM x, PPM y) const
{
	if( d_model.isNull() )
		return SpinPoint();

	PpmCube cube;
	cube.assign( 2, PpmRange() );
	cube[ DimX ].allocate( x, getViewArea()->toPpmDiff( g_tol, 0, DimX ) );
	cube[ DimY ].allocate( y, getViewArea()->toPpmDiff( g_tol, 0, DimY ) );

	SpinSpace::Result s;
	d_model->find( s, cube, d_model->getSpec() );
	if( s.empty() )
		return SpinPoint();
	else 
	{
		SpinSpace::Element e = ( *s.begin() );
		if( !e.isGhost() && ( e.isInfer() || !e.isHidden() ) )
			return e.d_point;
		else
			return SpinPoint();
	}
}

void SpinPointView::setLabel(Label l, Dimension d)
{
	if( d_model && d_model->getDimCount() < 2 && 
		( l > SysOrResiTagAll || d > DimX ) )
		qWarning( "SpinPointView::setLabel: Invalid label or ref dim for 1D view" );
	if( d_label == l && d_refDim == d )
		return;
	d_label = l; 
	d_refDim = d;
	damageMe();
}

void SpinPointView::select(const SpinPoint & t)
{
	Selection::iterator p;
	for( p = d_sel.begin(); p != d_sel.end(); ++p )
		damage( (*p) );
	d_sel.clear();
	d_sel.insert( t );
	damage( t );
}

void SpinPointView::select(const Selection & s)
{
	Selection::iterator p;
	for( p = d_sel.begin(); p != d_sel.end(); ++p )
		damage( (*p) );
	d_sel.clear();
	d_sel = s;
	for( p = d_sel.begin(); p != d_sel.end(); ++p )
		damage( (*p) );
}

void SpinPointView::formatLabel(char* buf, int len, const SpinPoint& tuple,
								Label label, Dimension refDim )
{
		// TODO: was ist mit Final?
#ifdef _DEBUG
	assert( tuple[ DimX ] != 0 );
	if( label > SysOrResiTagAll )
		assert( tuple[ DimY ] != 0 );
#endif
	switch( label )
	{
	case SystemId:
		if( tuple[ refDim ]->getSystem() )
			::sprintf( buf, "%d", tuple[ refDim ]->getSystem()->getId() );
		else
			::strcpy( buf, "-" );
		break;
	case SysResiId:
		if( tuple[ refDim ]->getSystem()->getAssig() )
			tuple[ refDim ]->getSystem()->formatLabel( buf, len );
		else if( tuple[ refDim ]->getSystem() )
			tuple[ refDim ]->getSystem()->formatLabel( buf, len );
		else
			::strcpy( buf, "-" );
		break;
	case SpinId:
		::sprintf( buf, "%d", tuple[ refDim ]->getId() );
		break;
	case TagOnly:
		if( tuple[ refDim ]->getLabel().getTag().isNull() )
			::strcpy( buf, "?" );
		else
			::strcpy( buf, tuple[ refDim ]->getLabel().getTag().data() );
		break;
	case TagOffset:
		if( tuple[ refDim ]->getLabel().getTag().isNull() )
			::sprintf( buf, "?" );
		else if( tuple[ refDim ]->getLabel().getOffset() )
			::sprintf( buf, "%s%+d", tuple[ refDim ]->getLabel().getTag().data(),
				tuple[ refDim ]->getLabel().getOffset() );
		else
			::sprintf( buf, "%s", tuple[ refDim ]->getLabel().getTag().data() );
		break;
	case TagAll:
		tuple[ refDim ]->getLabel().format( buf, len );
		break;
	case IdTagAll:
		tuple[ refDim ]->getLabel().format( s_ly, sizeof( s_ly ) );
		::sprintf( buf, "%s %d", s_ly, tuple[ refDim ]->getId() );
		break;
	case SysTagAll:
		tuple[ refDim ]->getLabel().format( s_ly, sizeof( s_ly ) );
		if( tuple[ refDim ]->getSystem() )
			::sprintf( buf, "%s %d", s_ly, tuple[ refDim ]->getSystem()->getId() );
		else
			::sprintf( buf, "%s -", s_ly );
		break;
	case SysOrResiTagAll:
		tuple[ refDim ]->getLabel().format( s_ly, sizeof( s_ly ) );
		if( tuple[ refDim ]->getSystem() && tuple[ refDim ]->getSystem()->getAssig() )
		{
			tuple[ refDim ]->getSystem()->formatLabel( s_buf, sizeof(s_buf) );
			::sprintf( buf, "%s %s", s_ly, s_buf );
		}else if( tuple[ refDim ]->getSystem() )
		{
			tuple[ refDim ]->getSystem()->formatLabel( s_buf, sizeof(s_buf) );
			::sprintf( buf, "%s %s", s_ly, s_buf );
		}else
			::sprintf( buf, "%s -", s_ly );
		break;
	case PairLabelResi:
	case PairLabelSystem:
	case PairLabelSysOrResi:
	case PairIdLabelSysOrResi:
	case PairAll:
		{
			tuple[ DimX ]->getLabel().format( s_lx, sizeof( s_lx ) );
			tuple[ DimY ]->getLabel().format( s_ly, sizeof( s_ly ) );
			*s_sx = 0;
			*s_sy = 0;
			SpinSystem* sx = tuple[ DimX ]->getSystem();
			SpinSystem* sy = tuple[ DimY ]->getSystem();
			switch( label )
			{
			case PairLabelResi:
				if( sy && sy->getAssig() )
					sy->formatLabel( s_sy, sizeof(s_sy) );
				if( sx && sx != sy && sx->getAssig() )
					sx->formatLabel( s_sx, sizeof(s_sx) );
				break;
			case PairLabelSystem:
				if( sy )
					::sprintf( s_sy, "%d", sy->getId() );
				if( sx && sx != sy )
					::sprintf( s_sx, "%d", sx->getId() );
				break;
			case PairLabelSysOrResi:
				if( sy && sy->getAssig() )
					sy->formatLabel( s_sy, sizeof(s_sy) );
				else if( sy )
					sy->formatLabel( s_sy, sizeof(s_sy) );
				if( sx && sx != sy && sx->getAssig() )
					sx->formatLabel( s_sx, sizeof(s_sx) );
				else if( sx && sx != sy )
					sx->formatLabel( s_sx, sizeof(s_sx) );
				break;
			case PairIdLabelSysOrResi:
				if( tuple[ DimX ]->getLabel().isNull() )
					::sprintf( s_lx, "%d", tuple[ DimX ]->getId() );
				if( tuple[ DimY ]->getLabel().isNull() )
					::sprintf( s_ly, "%d", tuple[ DimY ]->getId() );
				if( sy && sy->getAssig() )
					sy->formatLabel( s_sy, sizeof(s_sy) );
				else if( sy )
					sy->formatLabel( s_sy, sizeof(s_sy) );
				if( sx && sx != sy && sx->getAssig() )
					sx->formatLabel( s_sx, sizeof(s_sx) );
				else if( sx && sx != sy )
					sx->formatLabel( s_sx, sizeof(s_sx) );
				break;
			case PairAll:
				if( tuple[ DimX ]->getLabel().isNull() )
					::sprintf( s_lx, "%d", tuple[ DimX ]->getId() );
				else
				{
					::sprintf( s_buf, "%d:%s", tuple[ DimX ]->getId(), s_lx );
					::strcpy( s_lx, s_buf );
				}
				if( tuple[ DimY ]->getLabel().isNull() )
					::sprintf( s_ly, "%d", tuple[ DimY ]->getId() );
				else
				{
					::sprintf( s_buf, "%d:%s", tuple[ DimY ]->getId(), s_ly );
					::strcpy( s_ly, s_buf );
				}
				if( sy && sy->getAssig() )
				{
					sy->formatLabel( s_buf, sizeof(s_buf) );
					::sprintf( s_sy, "%d:%s", sy->getId(), s_buf );
				}else if( sy )
					sy->formatLabel( s_sy, sizeof(s_sy) );
				if( sx && sx != sy && sx->getAssig() )
				{
					sx->formatLabel( s_buf, sizeof(s_buf) );
					::sprintf( s_sx, "%d:%s", sx->getId(), s_buf );
				}else if( sx && sx != sy )
					sx->formatLabel( s_sx, sizeof(s_sx) );
				break;
            default:
                break;
			}
			if( *s_sx != 0 )
				::sprintf( buf, "%s %s/%s %s", s_lx, s_sx, s_ly, s_sy );
			else
				::sprintf( buf, "%s/%s %s", s_lx, s_ly, s_sy );
		}
		break;
	case PairSpinId:
		::sprintf( buf, "%d/%d", tuple[ DimX ]->getId(), tuple[ DimY ]->getId() );
		break;
	case PairSystemId:
		{
			*s_sx = 0;
			*s_sy = 0;
			if( tuple[ DimX ]->getSystem() )
				::sprintf( s_sx, "%d", tuple[ DimX ]->getSystem()->getId() );
			if( tuple[ DimY ]->getSystem() )
				::sprintf( s_sy, "%d", tuple[ DimY ]->getSystem()->getId() );
			::sprintf( buf, "%s/%s", s_sx, s_sy ); 
		}
		break;
	case PairSpinSystemId:
		{
			::sprintf( s_lx, "%d", tuple[ DimX ]->getId() );
			::sprintf( s_ly, "%d", tuple[ DimY ]->getId() );
			*s_sx = 0;
			*s_sy = 0;
			if( tuple[ DimX ]->getSystem() )
				::sprintf( s_sx, " %d", tuple[ DimX ]->getSystem()->getId() );
			if( tuple[ DimY ]->getSystem() )
				::sprintf( s_sy, " %d", tuple[ DimY ]->getSystem()->getId() );
			::sprintf( buf, "%s%s/%s%s", s_lx, s_sx, s_ly, s_sy ); 
		}
		break;
	case None:
		::strcpy( buf, "" );
		break;
	default:
		assert( FALSE );
	}
}

void SpinPointView::setAngle(float a)
{
	d_angle = a;
	damageMe();
}

void SpinPointView::setOff(Twips o)
{
	d_off = o;
	damageMe();
}

void SpinPointView::setWidth( Twips w)
{
	if( w >= 0 )
		d_pw = w;
	damageMe();
}

void SpinPointView::setColor(QColor clr)
{
	d_color = clr;
	damageMe();
}

void SpinPointView::setFont(const QFont& f)
{
	d_font = f;
	Font::BoundingBox box;
	box.stringBox( f, "0" );
	s_lw = box.getWidth();
	s_lh = box.getHeight();
	damageMe();
}

void SpinPointView::show(bool on)
{
	d_show = on;
	damageMe();
}

void SpinPointView::checkSelection()
{
	Selection del;
	Selection::const_iterator p;
	Selection s;
	SpinSpace::Element e;
	SpinSpace::Iterator i, _end = d_model->end();
	for( i = d_model->begin(); i != _end; ++i )
	{
		d_model->fetch( i, e );
		if( !e.isGhost() && ( e.isInfer() || !e.isHidden() ) )
			s.insert( e.d_point );
	}
	for( p = d_sel.begin(); p != d_sel.end(); ++p )
	{
		if( s.count( (*p) ) == 0 )
			del.insert( (*p) );
	}
	for( p = del.begin(); p != del.end(); ++p )
		d_sel.erase( (*p) );
}

void SpinPointView::showGhost(bool on)
{
	if( d_showGhost == on )
		return;
	d_showGhost = on;
	damageMe();
}

void SpinPointView::ghostLabel(bool on)
{
	if( d_ghostLabel == on )
		return;
	d_ghostLabel = on;
	damageMe();
}

void SpinPointView::showCross(bool on)
{
	if( d_showCross == on )
		return;
	d_showCross = on;
	damageMe();
}


void SpinPointView::setColors(PeakColorList * l)
{
    Root::Resource::release( d_peakColors );
    d_peakColors = l;
    Root::Resource::addRef( d_peakColors );
	damageMe();
}

//////////////////////////////////////////////////////////////////////

SpinPoint1DView::SpinPoint1DView(SpecViewer* v, Dimension dim, SpinSpace* stp, 
							 QColor clr, QFont* font):
	SpinPointView( v, stp, clr, font )
{
	d_dir = dim;
}

SpinPoint1DView::~SpinPoint1DView()
{
}

void SpinPoint1DView::draw( Canvas& v, const Lexi::Allocation& a)
{
	if( d_model.isNull() || !d_show )
		return;
	ViewAreaMdl* area = getViewArea();
	Canvas& c = v;

	PpmPoint p( 0 );
	SpinSpace::Iterator pos, _end = d_model->end();
	SpinSpace::Element e;
    Twips x = 0, lx = 0;
	QColor clr;
    bool tmp, ghost;
	for( pos = d_model->begin(); pos != _end; ++pos )
	{
        //alias = false;
		d_model->fetch( pos, e );
		ghost = e.isGhost();
		if( ghost && !d_showGhost )
			continue;
		if( !e.isInfer() && e.isHidden() )
			continue;
		p[ DimX ] = e.d_point[ DimX ]->getShift( d_model->getSpec(), tmp );
//		if( tmp )
//			alias = true;

		if( ghost )
		{
			clr = s_midClr;
			if( e.isHigher() ) 
				clr = s_posClr;
			if( e.isLower() )
				clr = s_negClr;
		}else
			clr = d_color;
		if( d_showCross && d_pw )
		{
			// TODO: alias
			if( d_dir == DimX )
			{
				lx = x = area->toTwip( p[ DimX ], a.getLeft(), d_dir );
				c.drawLine( x, a.getTop(), x, a.getBottom(), clr, d_pw );
			}else
			{
				lx = x = area->toTwip( p[ DimX ], a.getTop(), d_dir );
				c.drawLine( a.getLeft(), x, a.getRight(), x, clr, d_pw );
			}
		}
		if( d_label != None && ( !ghost || d_ghostLabel ) )
		{
			formatLabel( s_buf, sizeof( s_buf ), e.d_point );
			if( ghost )
				lx += g_w;

			Font::BoundingBox box;
			box.stringBox( d_font, s_buf );
			if( d_dir == DimX )
			{
				c.drawText( lx + g_w, a.getBottom() - g_w, s_buf, &d_font, clr );
				if( !ghost && d_sel.count( e.d_point ) != 0 )
					c.drawRect( x, a.getBottom() - s_lh, 
						box.getWidth() + 2 * g_w, box.getHeight() + 2 * g_w, clr, d_pw );
			}else
			{
				c.drawText( a.getLeft() + g_w, lx - g_w, s_buf, &d_font, clr );
				if( !ghost && d_sel.count( e.d_point ) != 0 )
					c.drawRect( a.getLeft(), x - s_lh + 20, 
						box.getWidth() + 2 * g_w, box.getHeight() + 2 * g_w, clr, d_pw );
			}
		}
	}
}

void SpinPoint1DView::selectPeak(const PpmCube & c)
{
	if( d_model.isNull() )
		return;
	// c hat nur eine Dimension
	assert( c.size() == 1 );
	if( c[ DimX ].getWidth() == 0.0 )
		selectSingle( c[ DimX ].first, false );
	else
	{
		SpinSpace::Result s;
		d_model->find( s, c, d_model->getSpec() );
		d_found = 0;
		SpinSpace::Result::const_iterator p;
		for( p = s.begin(); p != s.end(); ++p )
		{
			if( !(*p).isGhost() && ( (*p).isInfer() || !(*p).isHidden() ) )
			{
				d_sel.insert( (*p).d_point );
				damage( (*p).d_point );
			}
		}
	}
}

void SpinPoint1DView::selectPeak(PPM x )
{
	if( d_model.isNull() )
		return;
	selectSingle( x, true );
}

void SpinPoint1DView::selectSingle(PPM x, bool replace)
{
	PpmCube cube;
	cube.assign( 1, PpmRange() );
	cube[ DimX ].allocate( x, getViewArea()->toPpmDiff( g_tol, 0, d_dir ) );
	SpinPointView::selectSingle( cube, replace );
}

void SpinPoint1DView::damage(const SpinPoint & sp)
{
    if( d_model.isNull() )
		return;
	ViewAreaMdl* area = getViewArea();
	Twips x = area->toTwip( sp[ DimX ]->getShift( d_model->getSpec() ), d_dir );
	const Allocation& a = area->getAllocation();
	if( d_dir == DimX )
	{
		getViewport()->damage( Allocation( x, a.getBottom() - s_lh - g_w, 
			( Root::Tag::MaxLen + 1 ) * s_lw + g_w, s_lh + g_w ) );
		getViewport()->damage( Allocation( x, a.getTop(), s_lw, a.getHeight() ) );
	}else
	{
		getViewport()->damage( Allocation( a.getLeft(), x - s_lh + 20, 
					s_lw * ( Root::Tag::MaxLen + 1 ), s_lh ) );
		getViewport()->damage( Allocation( a.getLeft(), x, a.getWidth(), g_w ) );
	}
}


SpinPoint SpinPoint1DView::getHit(PPM x ) const
{
	if( d_model.isNull() )
		return SpinPoint();

	PpmCube cube;
	cube.assign( 1, PpmRange() );
	cube[ DimX ].allocate( x, getViewArea()->toPpmDiff( g_tol, 0, d_dir ) );

	SpinSpace::Result s;
	d_model->find( s, cube, d_model->getSpec() );
	if( s.empty() )
		return SpinPoint();
	else 
	{
		SpinSpace::Element e = ( *s.begin() );
		if( !e.isGhost() && ( e.isInfer() || !e.isHidden() ) )
			return e.d_point;
		else
			return SpinPoint();
	}
}



