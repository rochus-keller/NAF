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

#include "SpinPointView3.h"
#include <Spec/Residue.h>
#include <Spec/PeakList.h> // wegen PeakColorList
#include <Spec/PointMdl.h>
#include <QFontMetricsF>
#include <QPainter>
#include <QApplication>
#include <stdio.h>
using namespace Spec;

const char* SpinPointView3::menuText[] = 
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

static const Points g_w = 3.0;	// Lnge eines Kreuzsegments
static const Points g_d = 2.0;	// Offset eines diag Kreuzsegments
static const Points g_tol = 10.0;

QFont SpinPointView3::s_defaultFont( "", 8 ); // Wenn Application::font().family oder Default-Constructor -> Probleme
// Das Problem entsteht primär dadurch, dass bei statischer Initialisierung QApplication::font() aufgerufen wird!
// besser Q_GLOBAL_STATIC_WITH_ARGS(QFont, s_font, ( QApplication::font().family(), 8 ) )

static QColor s_posClr = Qt::cyan;
static QColor s_negClr = Qt::magenta;
static QColor s_midClr = Qt::gray;
Points SpinPointView3::s_defaultOff = 0;
float SpinPointView3::s_defaultAngle = 45;
bool SpinPointView3::s_defaultCross = true;

SpinPointView3::SpinPointView3(ViewAreaMdl3 *v, SpinSpace* stp,
							 QColor clr, const QFont* f):
	SpecView3( v ), d_model( stp ), d_labelType( SystemId ), d_refDim( 0 ),
    d_found( 0 ), d_showGhost( true ), d_ghostLabel( false ), d_peakColors( 0 ),
    d_curX(DimNone), d_curY(DimNone)
{
	if( f )
		d_font = *f;
	else
		d_font = s_defaultFont;
	d_color = clr;
	if( d_model )
		d_model->addObserver( this );
	d_off = s_defaultOff;
	d_angle = s_defaultAngle;
	d_showCross = s_defaultCross;
    d_pw = 1.0;
}

SpinPointView3::~SpinPointView3()
{
	if( d_model )
		d_model->removeObserver( this );
    Root::Resource::release( d_peakColors );
}

// static
bool SpinPointView3::formatSelection( QByteArray & s, SpinPointView3* v, LabelType l, int lim )
{
	const SpinPointView3::Selection& sel = v->getSel();
	if( sel.empty() )
		return false; // RISK
	QString str;
    if( v->d_found != 0 && v->d_found != int(sel.size()) && sel.size() == 1 )
        str.sprintf( "Found %d points, one selected: ", v->d_found );
	else
        str.sprintf( "Selected %d points: ", sel.size() );
	int i = 0;
	SpinPointView3::Selection::const_iterator p;
	for( p = sel.begin(); p != sel.end(); ++p, i++ )
	{
		if( i < lim )
		{
            str += formatLabel( (*p), (l == None)?PairAll:l, v->getRefDim() );
            if( i < int(sel.size() - 1) )
				str += ", ";
        }else if( lim < int(sel.size()) )
		{
			str += "...";
			break;
		}else
			break;
	}
    s = str.toLatin1();
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

void SpinPointView3::handle( Root::Message & msg )
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
                getArea()->damageAll(); // TODO
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
            getArea()->damageAll();
			break;
		}
		// qDebug( "View %x cmd %d sel. count %d", this, a->getType(), d_sel.size() ); // TEST
		msg.consume();
	}
    MESSAGE( PointMdl::Updating, a, msg )
    {
        if( !( a->hasDim( d_curX ) && a->hasDim( d_curY ) ) )
            return;
        SpinPoint tuple = getHit( a->getNew( d_curX ), a->getNew( d_curY ) );
        if( !tuple.isZero() )
        {
            a->override( d_curX, tuple[ DimX ]->getShift( d_model->getSpec() ),
                         d_curY, tuple[ DimY ]->getShift( d_model->getSpec() ) );
            msg.consume();
        }
    }

	END_HANDLER();
}

void SpinPointView3::paint( QPainter& painter )
{
    if( d_model.isNull() )
		return;
	if( d_model->getDimCount() < 2 )
    {
        qWarning() << "SpinPointView3::paint has invalid number of dimensions:" << d_model->getDimCount();
        return;
    }
    ViewAreaMdl3* area = getArea();

    for( SpinSpace::Iterator pos = d_model->begin(); pos != d_model->end(); ++pos )
	{
        SpinSpace::Element e;
        d_model->fetch( pos, e );
        const bool ghost = e.isGhost();
		if( ghost && !d_showGhost )
			continue;
		if( !e.isInfer() && e.isHidden() )
			continue;
        bool aliasX = false;
        bool aliasY = false;
        const Points y = area->toPoints( e.d_point[ DimY ]->getShift( d_model->getSpec(), aliasY ), DimY );
        const Points x = area->toPoints( e.d_point[ DimX ]->getShift( d_model->getSpec(), aliasX ), DimX );

        QColor clr;
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

        if( d_showCross && d_pw > 0.0 )
		{
            painter.setPen( QPen( clr, d_pw ) );
            if( d_peakColors == 0 )
            {
                // Normalfall, zeichne Kreuz und Farben für Ghosts
                if( aliasX || aliasY )
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
            }else
            {
                // Spezialfall; zeichne Symbole gem. Email von Fred von 2015-04-01 19:18
                if( aliasX || aliasY )
                {
                    if( !e.isHigher() && !e.isLower() )
                    {
                        // schräges Kreuz
                        painter.drawLine( QPointF( x - g_d, y - g_d ), QPointF( x + g_d + 1.0, y + g_d + 1.0 ) );
                        painter.drawLine( QPointF( x - g_d, y + g_d ), QPointF( x + g_d + 1.0, y - g_d - 1.0 ) );
                    }else if( e.isHigher() )
                    {
                        // Peaks in einer hoeheren Ebene wuerde man mit der unteren Haelfte des x, also mit ^ darstellen
                        painter.drawLine( QPointF( x, y ), QPointF( x + g_d + 2.0, y + g_d + 2.0 ) ); // mitte->rechts
                        painter.drawLine( QPointF( x, y ), QPointF( x - g_d - 2.0, y + g_d + 2.0 ) ); // mitte->links
                    }else
                    {
                        painter.drawLine( QPointF( x, y ), QPointF( x + g_d + 2.0, y - g_d - 2.0 ) ); // mitte->rechts
                        painter.drawLine( QPointF( x, y ), QPointF( x - g_d - 2.0, y - g_d - 2.0 ) ); // mitte->links
                    }
                }else
                {
                    if( !e.isHigher() && !e.isLower() )
                    {
                        // normales Kreuz
                        painter.drawLine( QPointF( x - g_w, y ), QPointF( x + g_w, y ) );
                        painter.drawLine( QPointF( x, y - g_w ), QPointF( x, y + g_w ) );
                    }else if( e.isHigher() )
                    {
                        // Peaks in einer hoeheren Ebene mit  _|_  (also + ohne die untere Haelfte des senkrechten Strichs)
                        painter.drawLine( QPointF( x - g_w, y ), QPointF( x + g_w, y ) ); // hori
                        painter.drawLine( QPointF( x, y - g_w - 1.0 ), QPointF( x, y ) ); // verti

                    }else
                    {
                        // Peaks in einer tieferen Ebene mit T (also + ohne die obere Haelfte des senkrechten Strichs)
                        painter.drawLine( QPointF( x - g_w, y ), QPointF( x + g_w, y ) ); // hori
                        painter.drawLine( QPointF( x, y ), QPointF( x, y + g_w + 1.0 ) ); // verti
                    }
                }
            }
        }
        Points lx = x; // Position des Labels
        Points ly = y;
        if( d_off > 0.0 )
		{
            const double a = d_angle * 2.0 * Root::Math::Pi / 360.0;
            lx = 0.5 + x + d_off * ::cos( a );
            ly = 0.5 + y - d_off * ::sin( a );
            if( !d_showCross && d_pw > 0.0 )
            {
                // Fahnenstange
                painter.setPen( QPen( clr, d_pw ) );
                painter.drawLine( QPointF( x, y ), QPointF( lx, ly ) );
            }
			ly += g_w;
		}
		if( d_labelType != None && ( !ghost || d_ghostLabel ) )
		{
            const QString text = formatLabel( e.d_point );
			if( ghost )
				lx += g_w;

            painter.setFont( d_font );
            painter.setPen( clr );
            painter.drawText( QPointF( lx + g_w, ly - g_w ), text );
            const QRectF bound = QFontMetricsF( painter.font() ).boundingRect( text );
            painter.setPen( QPen( clr, d_pw ) );
			if( !ghost && d_sel.count( e.d_point ) != 0 )
                // Selektionsrahmen
                painter.drawRect( QRectF( lx, ly, bound.width() + 2.0 * g_w, -bound.height() ) );
			else if( ghost )
			{
                ly -= bound.height() / 2.0;
				if( e.isHigher() )
				{
                    painter.drawEllipse( QRectF( lx, ly - g_w - 1.0, g_w / 2.0, g_w / 2.0 ) );
				}else if( e.isLower() )
				{
                    painter.drawEllipse( QRectF( lx, ly + g_w + 1.0, g_w / 2.0 , g_w / 2.0 ) );
				}
			}
		}else if( d_sel.count( e.d_point ) != 0 )
		{
            painter.setPen( QPen( clr, d_pw ) );
            painter.drawRect( QRectF( x - g_w - 1.0, y - g_w - 1.0, 2.0 * g_w + 2.0, 2.0 * g_w + 2.0 ) ); // Quadrat um Kreuz
		}
	}
}

void SpinPointView3::damage(const SpinPoint & sp)
{
    if( d_model.isNull() )
        return;
    ViewAreaMdl3* area = getArea();
    const Points y = area->toPoints( sp[ DimY ]->getShift( d_model->getSpec() ), DimY );
    const Points x = area->toPoints( sp[ DimX ]->getShift( d_model->getSpec() ), DimX );

    // Kreuz
    getArea()->damage( QRectF( x - g_w, y - g_w, 2.0 * g_w, 2.0 * g_w ) );

	if( d_labelType != None )
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

void SpinPointView3::setModel(SpinSpace * mdl)
{
	if( d_model == mdl )
		return;
	if( d_model )
		d_model->removeObserver( this );
	d_model = mdl;
	if( d_model )
		d_model->addObserver( this );
	d_sel.clear();
    getArea()->damageAll();
}

template <typename T>
inline T fAbs(const T &t) { return t >= 0.0 ? t : -t; }

void SpinPointView3::selectPoint(const PpmCube & c, bool toggle)
{
    // Hier werden aufeinanderliegende Peaks nicht durchgesteppt.
	if( d_model.isNull() )
		return;
	if( d_model->getDimCount() < 2 )
		throw Root::Exception( "Invalid number of Dimensions" );;
	if( c[ DimX ].getWidth() == 0.0 && c[ DimY ].getWidth() == 0.0 )
    {
        PpmCube c2 = c;
        c2[DimX].expand( fAbs(getArea()->toPpmDiff( g_tol, 0, DimX )) );
        c2[DimY].expand( fAbs(getArea()->toPpmDiff( g_tol, 0, DimY )) );
        selectSingle( c2, true, toggle );
    }else
	{
		SpinSpace::Result s;
		d_model->find( s, c, d_model->getSpec() );
		d_found = 0;
		SpinSpace::Result::const_iterator p;
		for( p = s.begin(); p != s.end(); ++p )
		{
			if( !(*p).isGhost() && ( (*p).isInfer() || !(*p).isHidden() ) )
			{
                if( toggle && d_sel.count( (*p).d_point ) > 0 )
                    d_sel.erase( (*p).d_point );
                else
                    d_sel.insert( (*p).d_point );
				damage( (*p).d_point );
			}
		}
	}
}

void SpinPointView3::selectPoint(PPM x, PPM y, bool toggle)
{
	if( d_model.isNull() )
		return;

    PpmCube cube;
    cube.assign( 2, PpmRange() );
    cube[ DimX ].allocate( x, fAbs(getArea()->toPpmDiff( g_tol, 0, DimX )) );
    cube[ DimY ].allocate( y, fAbs(getArea()->toPpmDiff( g_tol, 0, DimY )) );

    selectSingle( cube, true, toggle );
}

void SpinPointView3::selectSingle( const PpmCube& cube, bool iterate, bool toggle)
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
    if( iterate )
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
                if( p == s.end() && !toggle )
					p = s.begin();
                if( p != s.end() )
                    t = (*p);
			}else
				t = *s.begin();
			s.clear();
            if( !t.isZero() )
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
            if( toggle && d_sel.count( (*p) ) > 0 )
                d_sel.erase( (*p) );
            else
                d_sel.insert( (*p) );
			damage( (*p) );
		}
	}
}

SpinPoint SpinPointView3::getHit(PPM x, PPM y) const
{
	if( d_model.isNull() )
		return SpinPoint();

	PpmCube cube;
	cube.assign( 2, PpmRange() );
    cube[ DimX ].allocate( x, getArea()->toPpmDiff( g_tol, 0, DimX ) );
    cube[ DimY ].allocate( y, getArea()->toPpmDiff( g_tol, 0, DimY ) );

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

void SpinPointView3::setLabelType(LabelType l, Dimension d)
{
	if( d_model && d_model->getDimCount() < 2 && 
		( l > SysOrResiTagAll || d > DimX ) )
		qWarning( "SpinPointView3::setLabel: Invalid label or ref dim for 1D view" );
	if( d_labelType == l && d_refDim == d )
		return;
	d_labelType = l;
	d_refDim = d;
    getArea()->damageAll();
}

void SpinPointView3::adjustPositions(PointMdl * mdl, Dimension dx, Dimension dy)
{
    Q_ASSERT( mdl != 0 );
    Q_ASSERT( dy != DimNone );
    Q_ASSERT( dy != DimNone );
    mdl->addObserver( this );
    d_curX = dx;
    d_curY = dy;
}

void SpinPointView3::select(const SpinPoint & t)
{
	Selection::iterator p;
	for( p = d_sel.begin(); p != d_sel.end(); ++p )
		damage( (*p) );
	d_sel.clear();
	d_sel.insert( t );
	damage( t );
}

void SpinPointView3::clearSelection()
{
	Selection::iterator p;
	for( p = d_sel.begin(); p != d_sel.end(); ++p )
		damage( (*p) );
	d_sel.clear();
}

void SpinPointView3::select(const Selection & s)
{
	Selection::iterator p;
	for( p = d_sel.begin(); p != d_sel.end(); ++p )
		damage( (*p) );
	d_sel.clear();
	d_sel = s;
	for( p = d_sel.begin(); p != d_sel.end(); ++p )
		damage( (*p) );
}

QByteArray SpinPointView3::formatLabel( const SpinPoint& tuple, LabelType label, Dimension refDim )
{

    QByteArray buf( 32, 0 );
#ifdef _DEBUG
    Q_ASSERT( tuple[ DimX ] != 0 );
	if( label > SysOrResiTagAll )
        Q_ASSERT( tuple[ DimY ] != 0 );
#endif
	switch( label )
	{
	case SystemId:
		if( tuple[ refDim ]->getSystem() )
            qsnprintf( buf.data(), buf.size(), "%d", tuple[ refDim ]->getSystem()->getId() );
		else
            buf = "-";
		break;
	case SysResiId:
		if( tuple[ refDim ]->getSystem()->getAssig() )
            tuple[ refDim ]->getSystem()->formatLabel( buf.data(), buf.size() );
		else if( tuple[ refDim ]->getSystem() )
            tuple[ refDim ]->getSystem()->formatLabel( buf.data(), buf.size() );
		else
            buf = "-";
		break;
	case SpinId:
        qsnprintf( buf.data(), buf.size(), "%d", tuple[ refDim ]->getId() );
		break;
	case TagOnly:
		if( tuple[ refDim ]->getLabel().getTag().isNull() )
            buf = "?";
		else
            buf = tuple[ refDim ]->getLabel().getTag().data();
		break;
	case TagOffset:
		if( tuple[ refDim ]->getLabel().getTag().isNull() )
            buf = "?";
		else if( tuple[ refDim ]->getLabel().getOffset() )
            qsnprintf( buf.data(), buf.size(), "%s%+d", tuple[ refDim ]->getLabel().getTag().data(),
				tuple[ refDim ]->getLabel().getOffset() );
		else
            qsnprintf( buf.data(), buf.size(), "%s", tuple[ refDim ]->getLabel().getTag().data() );
		break;
	case TagAll:
        buf = tuple[ refDim ]->getLabel().format();
		break;
	case IdTagAll:
        qsnprintf( buf.data(), buf.size(), "%s %d", tuple[ refDim ]->getLabel().format().constData(), tuple[ refDim ]->getId() );
		break;
	case SysTagAll:
		if( tuple[ refDim ]->getSystem() )
            qsnprintf( buf.data(), buf.size(), "%s %d", tuple[ refDim ]->getLabel().format().constData(), tuple[ refDim ]->getSystem()->getId() );
		else
            qsnprintf( buf.data(), buf.size(), "%s -", tuple[ refDim ]->getLabel().format().constData() );
		break;
	case SysOrResiTagAll:
		if( tuple[ refDim ]->getSystem() && tuple[ refDim ]->getSystem()->getAssig() )
		{
            qsnprintf( buf.data(), buf.size(), "%s %s", tuple[ refDim ]->getLabel().format().constData(),
                       tuple[ refDim ]->getSystem()->formatLabel().constData() );
		}else if( tuple[ refDim ]->getSystem() )

		{
            qsnprintf( buf.data(), buf.size(), "%s %s", tuple[ refDim ]->getLabel().format().constData(),
                       tuple[ refDim ]->getSystem()->formatLabel().constData() );
		}else
            qsnprintf( buf.data(), buf.size(), "%s -", tuple[ refDim ]->getLabel().format().constData() );
		break;
	case PairLabelResi:
	case PairLabelSystem:
	case PairLabelSysOrResi:
	case PairIdLabelSysOrResi:
	case PairAll:
		{
            QByteArray labelX = tuple[ DimX ]->getLabel().format(); // s_lx
            QByteArray labelY = tuple[ DimY ]->getLabel().format(); // s_ly
            QByteArray sysLabelY( 32, 0 ); // s_sy
            QByteArray sysLabelX( 32, 0 ); // s_sx
            SpinSystem* sx = tuple[ DimX ]->getSystem();
			SpinSystem* sy = tuple[ DimY ]->getSystem();
			switch( label )
			{
			case PairLabelResi:
				if( sy && sy->getAssig() )
                    sysLabelY = sy->formatLabel();
				if( sx && sx != sy && sx->getAssig() )
                    sysLabelX = sx->formatLabel();
				break;
			case PairLabelSystem:
				if( sy )
                    qsnprintf( sysLabelY.data(), sysLabelY.size(), "%d", sy->getId() );
				if( sx && sx != sy )
                    qsnprintf( sysLabelX.data(), sysLabelX.size(), "%d", sx->getId() );
				break;
			case PairLabelSysOrResi:
                if( sy )
                    sysLabelY = sy->formatLabel();
                if( sx && sx != sy )
                    sysLabelX = sx->formatLabel();
				break;
			case PairIdLabelSysOrResi:
				if( tuple[ DimX ]->getLabel().isNull() )
                    labelX = QByteArray::number( tuple[ DimX ]->getId() );
				if( tuple[ DimY ]->getLabel().isNull() )
                    labelY = QByteArray::number( tuple[ DimY ]->getId() );
                if( sy )
                    sysLabelY = sy->formatLabel();
                if( sx && sx != sy )
                    sysLabelX = sx->formatLabel();
				break;
			case PairAll:
				if( tuple[ DimX ]->getLabel().isNull() )
                    labelX = QByteArray::number( tuple[ DimX ]->getId() );
				else
				{
                    QByteArray buf2( 32, 0 );
                    qsnprintf( buf2.data(), buf2.size(), "%d:%s", tuple[ DimX ]->getId(), labelX.constData() );
                    labelX = buf2;
				}
				if( tuple[ DimY ]->getLabel().isNull() )
                    labelY = QByteArray::number( tuple[ DimY ]->getId() );
				else
				{
                    QByteArray buf2( 32, 0 );
                    qsnprintf( buf2.data(), buf2.size(), "%d:%s", tuple[ DimY ]->getId(), labelY.constData() );
                    labelY = buf2;
                }
				if( sy && sy->getAssig() )
                    qsnprintf( sysLabelY.data(), sysLabelY.size(), "%d:%s", sy->getId(), sy->formatLabel().constData() );
                else if( sy )
                    sysLabelY = sy->formatLabel(); // s_sx
                if( sx && sx != sy && sx->getAssig() )
                    qsnprintf( sysLabelX.data(), sysLabelX.size(), "%d:%s", sx->getId(), sx->formatLabel().constData() );
                else if( sx && sx != sy )
                    sysLabelX = sx->formatLabel();
				break;
            default:
                break;
			}
            if( !sysLabelX.isEmpty() )
                qsnprintf( buf.data(), buf.size(), "%s %s/%s %s", labelX.constData(),
                           sysLabelX.constData(), labelY.constData(), sysLabelY.constData() );
			else
                qsnprintf( buf.data(), buf.size(), "%s/%s %s", labelX.constData(),
                           labelY.constData(), sysLabelY.constData() );
		}
		break;
	case PairSpinId:
        qsnprintf( buf.data(), buf.size(), "%d/%d", tuple[ DimX ]->getId(), tuple[ DimY ]->getId() );
		break;
	case PairSystemId:
		{
            QByteArray sysLabelY; // s_sy
            QByteArray sysLabelX; // s_sx
            if( tuple[ DimX ]->getSystem() )
                sysLabelX = QByteArray::number( tuple[ DimX ]->getSystem()->getId() );
			if( tuple[ DimY ]->getSystem() )
                sysLabelY = QByteArray::number( tuple[ DimY ]->getSystem()->getId() );
            qsnprintf( buf.data(), buf.size(), "%s/%s", sysLabelX.constData(), sysLabelY.constData() );
		}
		break;
	case PairSpinSystemId:
		{
            const QByteArray labelX = QByteArray::number( tuple[ DimX ]->getId() );
            const QByteArray labelY = QByteArray::number( tuple[ DimY ]->getId() );
            QByteArray sysLabelY; // s_sy
            QByteArray sysLabelX; // s_sx
            if( tuple[ DimX ]->getSystem() )
                sysLabelX = QByteArray::number( tuple[ DimX ]->getSystem()->getId() );
            if( tuple[ DimY ]->getSystem() )
                sysLabelY = QByteArray::number( tuple[ DimY ]->getSystem()->getId() );
            qsnprintf( buf.data(), buf.size(), "%s%s/%s%s", labelX.constData(), sysLabelX.constData(),
                       labelY.constData(), sysLabelY.constData() );
		}
		break;
	case None:
        buf.clear();
		break;
	default:
        Q_ASSERT( false );
	}
    return buf;
}

void SpinPointView3::setAngle(float a)
{
	d_angle = a;
    getArea()->damageAll();
}

void SpinPointView3::setOff(Points o)
{
	d_off = o;
    getArea()->damageAll();
}

void SpinPointView3::setWidth( Points w)
{
	if( w >= 0 )
		d_pw = w;
    getArea()->damageAll();
}

void SpinPointView3::setColor(QColor clr)
{
	d_color = clr;
    getArea()->damageAll();
}

void SpinPointView3::setFont(const QFont& f)
{
	d_font = f;
    getArea()->damageAll();
}

void SpinPointView3::checkSelection()
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

void SpinPointView3::setShowGhost(bool on)
{
	if( d_showGhost == on )
		return;
	d_showGhost = on;
    getArea()->damageAll();
}

void SpinPointView3::setGhostLabel(bool on)
{
	if( d_ghostLabel == on )
		return;
	d_ghostLabel = on;
    getArea()->damageAll();
}

void SpinPointView3::setShowCross(bool on)
{
	if( d_showCross == on )
		return;
	d_showCross = on;
    getArea()->damageAll();
}


void SpinPointView3::setColors(PeakColorList * l)
{
    Root::Resource::release( d_peakColors );
    d_peakColors = l;
    Root::Resource::addRef( d_peakColors );
    getArea()->damageAll();
}

