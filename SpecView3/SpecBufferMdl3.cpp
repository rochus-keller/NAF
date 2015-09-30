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

#include "SpecBufferMdl3.h"
using namespace Spec;

SpecBufferMdl3::SpecBufferMdl3(ViewAreaMdl3* a, Spectrum* spec, bool load ):
	d_area( a ), d_scale( false ), d_folding( false ), d_down( true ),
	d_resolution( 1 )
{
    Q_ASSERT( a != 0 );
	d_area->addObserver( this );
	setSpectrum( spec, load );
	if( !load )
	{
		PpmCube cube;
		d_area->getCube( cube );
		ScaleVector sv;
		for( Dimension d = 0; d < cube.size(); d++ )
			sv.push_back( Scale( cube[ d ].first, cube[ d ].second ) );
		d_buffer.resize( sv );
		d_buffer.null();
	}
}

SpecBufferMdl3::~SpecBufferMdl3()
{
    d_area->removeObserver( this );
    if( d_spec )
        d_spec->removeObserver( this );
}

void SpecBufferMdl3::reload()
{
	// Wir gehen davon aus, dass d_spec immer über genügend Dimensionen verfügt.
	PpmCube cube;
	d_area->getCube( cube );
    Q_ASSERT( cube.size() <= 2 );

	// cube.dump( "Reload" ); // TEST
	try
	{
		if( cube.size() == 2 )	// 2D
		{
			// Y-Achse invertieren.
			// Nein, da bereits in Area richtig gedreht: cube[ DimY ].invert();

			if( !d_spec.isNull() )
			{
                // Q_ASSERT( d_spec->getDimCount() == 2 );
				if( d_down )
					d_spec->fillPlane( d_buffer, DimX, DimY, cube, true, d_folding,
                        qRound( d_area->getSpan( DimX ) ), qRound( d_area->getSpan( DimY ) ) );
				else
					d_spec->fillPlane( d_buffer, DimX, DimY, cube, true, d_folding );
				// TEST
				/*
				cube[ DimX ] = d_buffer.getScale( DimX ).getRange();
				cube[ DimY ] = d_buffer.getScale( DimY ).getRange();
				cube.dump( "Result" ); // TEST
				*/
			}else
			{
				ScaleVector sv;
				for( Dimension d = 0; d < cube.size(); d++ )
					sv.push_back( Scale( cube[ d ].first, cube[ d ].second ) );
				d_buffer.resize( sv );
			}
			if( d_scale )
			{
				Extension ex;
                ex.push_back( qRound( d_area->getSpan( DimX ) / d_resolution ) );
                ex.push_back( qRound( d_area->getSpan( DimY ) / d_resolution ) );
				d_buffer.resample( ex, true );
			}
		}else // 1D
		{
			// cube.dump( "Reload" ); // TEST
			// Wird bereits in FitToArea gemacht: cube[ DimX ].invert();
			if( !d_spec.isNull() )
			{
                // Q_ASSERT( d_spec->getDimCount() == 1 );
				if( d_down )
					d_spec->fillSlice( d_buffer, DimX, cube, true, d_folding,
                        qRound( d_area->getSpan( d_area->getDim() ) ) );
				else
					d_spec->fillSlice( d_buffer, DimX, cube, true, d_folding );
				// spec hat nur eine Dim
			}else
			{
				// fülle 1D-ScaleVector mit cube und Phantasieauflösung.
				ScaleVector sv;
				sv.push_back( Scale( cube[ DimX ].first, cube[ DimX ].second ) );
				d_buffer.resize( sv );
			}
			if( d_scale )
			{
				Extension ex;
                ex.push_back( qRound( d_area->getSpan( d_area->getDim() ) / d_resolution ) );
				d_buffer.resample( ex, true );
			}
		}
	}catch(Root::Exception& e)
	{
		if( d_buffer.getDimCount() == 0 )
		{
			ScaleVector sv;
			for( Dimension d = 0; d < cube.size(); d++ )
				sv.push_back( Scale( cube[ d ].first, cube[ d ].second ) );
			d_buffer.resize( sv );
		}
		d_buffer.null();
#ifdef _DEBUG
		qDebug( "SpecBufferMdl3: %s", e.what() );
#endif
	}
    UpdBuffer m(this);
	notifyObservers( m );
}

void SpecBufferMdl3::setFolding(bool on, bool load)
{
	if( d_folding == on )
		return;
	d_folding = on;
	if( load )
		reload();
}

void SpecBufferMdl3::setScaling(bool on, bool load)
{
	if( d_scale == on )
		return;
	d_scale = on;
	if( load )
		reload();
}

void SpecBufferMdl3::setResolution(Root::Byte r, bool load )
{
	if( d_resolution == r && d_scale )
		return;
    Q_ASSERT( r > 0 );
	d_resolution = r;
	d_scale = true;	// TEST
	if( load && d_scale )
		reload();
}

void SpecBufferMdl3::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
    MESSAGE( ViewAreaMdl3::UpdZoom, a, msg )
	{
        Q_UNUSED(a)
        UpdZoom m(this);
        notifyObservers( m );
        if( d_down ) // Mit dem Zoom ändert auch die Auflösung
            reload();
		msg.consume();
	}
    MESSAGE( ViewAreaMdl3::UpdRange, a, msg )
    {
        Q_UNUSED(a)
        if( rangeChanged() )
            reload();
        msg.consume();
    }
    MESSAGE( Spectrum::Update, a, msg )
	{
		if( a->getOrigin() == d_spec )
		{
			switch( a->getType() )
			{
			case Spectrum::Update::Scale:
			case Spectrum::Update::Rotation:
			case Spectrum::Update::Data:
			case Spectrum::Update::All:
				reload(); // RISK
				// d_area->redraw(); // RISK
				break;
            case Spectrum::Update::Threshold:
                // TODO
                break;
            default:
                break;
			}
			msg.consume();
		}
	}
	END_HANDLER();
}


void SpecBufferMdl3::setSpectrum(Spectrum * spec, bool load )
{
	if( spec )
    {
        //qDebug() << d_area->getDimCount() << spec->getDimCount();
        Q_ASSERT( d_area->getDimCount() == spec->getDimCount() );
    }
	if( d_spec )
		d_spec->removeObserver( this );

	d_spec = spec;
	if( d_spec )
	{
		d_spec->addObserver( this );
		if( d_spec->getDimCount() >= 2 )
			d_area->setRfFreq( d_spec->getRfFreq( DimX ), d_spec->getRfFreq( DimY ) );
		else
			d_area->setRfFreq( d_spec->getRfFreq( DimX ), d_spec->getRfFreq( DimX ) );
	}
	if( load )
		reload();
}

void SpecBufferMdl3::fitToArea(bool notify)
{
	// Wir invertieren hier, da Spektrum Original-Range hat, jedoch
	// alle View-bezogenen Klassen die Y-Achse invertieren.
	if( d_spec.isNull() )
		return;
	if( d_area->getDimCount() == 2 )
	{
		PpmRange r = d_spec->getScale( DimY ).getRange();
		r.invert();
		d_area->setRange( d_spec->getScale( DimX ).getRange(), r, notify );
	}else if( d_area->inUse( DimX ) )
		d_area->setRange( DimX, d_spec->getScale( DimX ).getRange(), notify );
	else if( d_area->inUse( DimY ) )
	{
		PpmRange r = d_spec->getScale( DimX ).getRange();	// Spec hat nur eine Dim
		r.invert();
		d_area->setRange( DimY, r, notify );
	}
}

void SpecBufferMdl3::fitToDim(Dimension d, bool notify)
{
	// Wir invertieren hier, da Spektrum Original-Range hat, jedoch
	// alle View-bezogenen Klassen die Y-Achse invertieren.
	if( d_spec.isNull() )
		return;
	if( d == DimX && d_area->inUse( DimX ) )
		d_area->setRange( DimX, d_spec->getScale( DimX ).getRange(), notify );
	else if( d == DimY && d_area->inUse( DimY ) )
	{
		PpmRange r = d_spec->getScale( DimX ).getRange();	// Spec hat nur eine Dim
		r.invert();
		d_area->setRange( DimY, r, notify );
	}
}

bool SpecBufferMdl3::rangeChanged() const
{
	if( d_area->getDimCount() == 2 )
		return d_buffer.getScale( DimX ).getRange() != d_area->getRange( DimX ) ||
			d_buffer.getScale( DimY ).getRange() != d_area->getRange( DimY );
	else if( d_area->inUse( DimX ) )
		return d_buffer.getScale( DimX ).getRange() != d_area->getRange( DimX );
	else if( d_area->inUse( DimY ) )
		return d_buffer.getScale( DimX ).getRange() != d_area->getRange( DimY );
	else 
		return false;
}

void SpecBufferMdl3::copy(SpecBufferMdl3 * mdl)
{
    Q_ASSERT( mdl->getDimCount() == getDimCount() );
	d_buffer = mdl->d_buffer;
	if( d_buffer.getDimCount() == 1 )
	{
		d_area->setRange( d_area->getDim(), d_buffer.getScale( DimX ).getRange(), false );
	}else
	{
		d_area->setRange( d_buffer.getScale( DimX ).getRange(),
			d_buffer.getScale( DimY ).getRange(), false );
	}
    UpdBuffer m(this);
	notifyObservers( m );
}

void SpecBufferMdl3::setBuffer(const Buffer & buf)
{
	if( d_area->getDimCount() != buf.getDimCount() )
		throw Root::Exception( "Buffer has wrong number of dimensions" );
	d_buffer = buf;
	if( d_area->getDimCount() == 2 )
	{
		d_area->setRange( d_buffer.getScale( DimX ).getRange(), d_buffer.getScale( DimY ).getRange(), true );
	}else if( d_area->inUse( DimX ) )
		d_area->setRange( DimX, d_buffer.getScale( DimX ).getRange(), true );
	else if( d_area->inUse( DimY ) )
		d_area->setRange( DimY, d_buffer.getScale( DimX ).getRange(), true );
    UpdBuffer m(this);
	notifyObservers( m );
}

void SpecBufferMdl3::setDownsize(bool on, bool load)
{
	if( d_down == on )
		return;
	d_down = on;
	if( load )
		reload();
}
