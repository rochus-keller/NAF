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

#include "SpecBufferMdl.h"
using namespace Spec;

SpecBufferMdl::SpecBufferMdl(ViewAreaMdl* a, Spectrum* spec, bool load ):
	d_area( a ), d_scale( false ), d_folding( false ), d_down( true ),
	d_resolution( 1 )
{
	assert( a );
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

SpecBufferMdl::~SpecBufferMdl()
{
	if( d_spec || d_area )
	{
		d_area->removeObserver( this );
		d_area = 0;
		if( d_spec )
			d_spec->removeObserver( this );
		d_spec = 0;
		d_buffer.clear();
	}
}

void SpecBufferMdl::reload()
{
	// Wir gehen davon aus, dass d_spec immer über genügend Dimensionen verfügt.
	PpmCube cube;
	d_area->getCube( cube );
	assert( cube.size() <= 2 );

	// cube.dump( "Reload" ); // TEST
	try
	{
		if( cube.size() == 2 )	// 2D
		{
			// Y-Achse invertieren.
			// Nein, da bereits in Area richtig gedreht: cube[ DimY ].invert();

			if( !d_spec.isNull() )
			{
				// assert( d_spec->getDimCount() == 2 );
				if( d_down )
					d_spec->fillPlane( d_buffer, DimX, DimY, cube, true, d_folding,
						d_area->getSpan( DimX ) / 20, d_area->getSpan( DimY ) / 20 );
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
				ex.push_back( d_area->getSpan( DimX ) / ( d_resolution * 20 ) );
				ex.push_back( d_area->getSpan( DimY ) / ( d_resolution * 20 ) );
				d_buffer.resample( ex, true );
			}
		}else // 1D
		{
			// cube.dump( "Reload" ); // TEST
			// Wird bereits in FitToArea gemacht: cube[ DimX ].invert();
			if( !d_spec.isNull() )
			{
				// assert( d_spec->getDimCount() == 1 );
				if( d_down )
					d_spec->fillSlice( d_buffer, DimX, cube, true, d_folding,
						d_area->getSpan( d_area->getDim() ) / 20 );
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
				ex.push_back( d_area->getSpan( d_area->getDim() ) / ( d_resolution * 20 ) );
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
		qDebug( "SpecBufferMdl: %s", e.what() );
#endif
	}
	SpecBufferMdl::Update m(this,SpecBufferMdl::Update::Buffer);
	notifyObservers( m );
}

void SpecBufferMdl::setFolding(bool on, bool load)
{
	if( d_folding == on )
		return;
	d_folding = on;
	if( load )
		reload();
}

void SpecBufferMdl::setScaling(bool on, bool load)
{
	if( d_scale == on )
		return;
	d_scale = on;
	if( load )
		reload();
}

void SpecBufferMdl::setResolution(Root::Byte r, bool load )
{
	if( d_resolution == r && d_scale )
		return;
	assert( r > 0 );
	d_resolution = r;
	d_scale = true;	// TEST
	if( load && d_scale )
		reload();
}

void SpecBufferMdl::handle( Root::Message & msg )
{
	BEGIN_HANDLER();
	MESSAGE( ViewAreaMdl::Update, a, msg )
	{
		switch( a->getType() )
		{
		case ViewAreaMdl::Update::Range:
			if( rangeChanged() )
				reload();
			break;
		case ViewAreaMdl::Update::Zoom:
			{
				SpecBufferMdl::Update m(this,SpecBufferMdl::Update::Zoom);
				notifyObservers( m );
			}
			break;
		}
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
            default:
                break;
			}
			msg.consume();
		}
	}
	END_HANDLER();
}


void SpecBufferMdl::setSpectrum(Spectrum * spec, bool load )
{
	if( spec )
		assert( d_area->getDimCount() == spec->getDimCount() );
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

void SpecBufferMdl::fitToArea(bool notify)
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

void SpecBufferMdl::fitToDim(Dimension d, bool notify)
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

bool SpecBufferMdl::rangeChanged() const
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

void SpecBufferMdl::copy(SpecBufferMdl * mdl)
{
	assert( mdl->getDimCount() == getDimCount() );
	d_buffer = mdl->d_buffer;
	if( d_buffer.getDimCount() == 1 )
	{
		d_area->setRange( d_area->getDim(), d_buffer.getScale( DimX ).getRange(), false );
	}else
	{
		d_area->setRange( d_buffer.getScale( DimX ).getRange(),
			d_buffer.getScale( DimY ).getRange(), false );
	}
	SpecBufferMdl::Update m(this,SpecBufferMdl::Update::Buffer);
	notifyObservers( m );
}

void SpecBufferMdl::setBuffer(const Buffer & buf)
{
	if( d_area->getDimCount() != buf.getDimCount() )
		throw Root::Exception( "Buffer has wrong number of dimensions" );
	d_buffer = buf;
	if( d_area->getDimCount() == 2 )
	{
		d_area->setRange( d_buffer.getScale( DimX ).getRange(), 
			d_buffer.getScale( DimY ).getRange(), true );
	}else if( d_area->inUse( DimX ) )
		d_area->setRange( DimX, d_buffer.getScale( DimX ).getRange(), true );
	else if( d_area->inUse( DimY ) )
		d_area->setRange( DimY, d_buffer.getScale( DimX ).getRange(), true );
	SpecBufferMdl::Update m(this,SpecBufferMdl::Update::Buffer);
	notifyObservers( m );
}

void SpecBufferMdl::setDownsize(bool on, bool load)
{
	if( d_down == on )
		return;
	d_down = on;
	if( load )
		reload();
}
