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

#include "SpectrumType.h"
#include <Spec/Repository.h>
#include <qstring.h> // qDebug
#include <Root/Vector.h>
#include <stdio.h>
using namespace Spec;
using namespace Root;

SpectrumType::SpectrumType(const char* name, Dimension count):
	d_name( name ), d_dims( count ), d_owner( 0 )
{
	assert( count > 0 );
	assert( name );
	d_dims.assign( count, SpecDim() );
}

SpectrumType::SpectrumType(const char* name, const SpectrumType& st ):
	d_name( name )
{
	assert( name );
	d_dims = st.d_dims;
	copyDynValuesFrom( &st );
	d_proc = st.d_proc;
	d_step = st.d_step;
	d_owner = st.d_owner;
}


SpectrumType::~SpectrumType()
{
	d_dims.clear();
	d_proc.clear();
	d_step.clear();
	d_owner = 0;
}

void SpectrumType::setColor(Dimension d, AtomType c)
{
	SpecDim* dim = &d_dims[ d ];
	if( dim->d_atom != c )
	{
		dim->d_atom = c;
		SpectrumType::Changed m( this, SpectrumType::Color, d );
		notifyObservers( m );
	}
}

bool SpectrumType::addLabel(Dimension d, SpinLabel l)
{
	SpecDim* dim = &d_dims[ d ];
	if( dim->d_labels.count( l ) == 0 )
	{
		dim->d_labels.insert( l );
		SpectrumType::Changed m( this, SpectrumType::Labels, d );
		notifyObservers( m );
		return true;
	}else
		return false;
}

bool SpectrumType::removeLabel(Dimension d, SpinLabel l)
{
	SpecDim* dim = &d_dims[ d ];
	if( dim->d_labels.count( l ) != 0 )
	{
		dim->d_labels.erase( l );
		SpectrumType::Changed m( this, SpectrumType::Labels, d );
		notifyObservers( m );
		return true;
	}else
		return false;
}

void SpectrumType::setName(Dimension d, const char * str)
{
	SpecDim* dim = &d_dims[ d ];
	Root::SymbolString ss( str );
	if( dim->d_name != ss )
	{
		dim->d_name = ss;
		SpectrumType::Changed m( this, SpectrumType::DimName, d );
		notifyObservers( m );
	}
}

bool SpectrumType::isCompatible(Spectrum * spec) const
{
	if( spec == 0 )
		return false;
	if( getDimCount() != spec->getDimCount() )
		return false;
	for( Dimension d = 0; d < spec->getDimCount(); d++ )
	{
		if( getColor( d ) != spec->getScale( d ).getColor() )
			return false;
	}
	return true;
}

SpinLabel SpectrumType::getKeyLabel(Dimension d) const
{
	const SpinLabelSet& sls = d_dims[d].d_labels;
	if( sls.size() == 1 )
		// Es gibt nur ein Label, dieses wird als Final verwendet
		return ( *sls.begin() );
	SpinLabelSet::const_iterator pos;
	SpinLabel res;
	for( pos = sls.begin(); pos != sls.end(); ++pos )
	{
		if( res.isFinal() )
			return SpinLabel();
		else if( (*pos).isFinal() )
			res = (*pos);
	}
	return res;
}

Root::SymbolString SpectrumType::getName( Dimension d ) const 
{ 
	if( d_dims[d].d_name.isNull() )
		return d_dims[d].d_atom.getIsoLabel();
	else if( !d_step.isEmpty() && !d_proc.isEmpty() && !d_proc[ d_step[ d ] ].d_text.isEmpty() )
		return d_proc[ d_step[ d ] ].d_text.data();
	else
		return d_dims[d].d_name; 
}

bool SpectrumType::setProc(const Procedure & p)
{
	try
	{
		checkValid( p );
	}catch( ... )
	{
		return false; 
	}
	d_proc = p;
	update();
	SpectrumType::Changed m( this, SpectrumType::ProcStep );
	notifyObservers( m );
	return true;
}

void SpectrumType::update()
{
	d_step.assign( d_dims.size(), -1 );

	for( int i = 0; i < d_proc.size(); i++ )
	{
		if( d_proc[ i ].d_dim != DimUndefined )
			d_step[ d_proc[ i ].d_dim ] = i;
	}
}

bool SpectrumType::isNoesy( Dimension d ) const
{
	if( d_proc.empty() || d_step.empty() )
		return false;
	else
		return d_proc[ d_step[ d ] ].d_hops < 0;
}

void SpectrumType::checkValid(const Procedure & p) const
{
	if( !p.empty() )
	{
		std::set<Dimension> test;
		for( Dimension d = 0; d < p.size(); d++ )
		{
			if( p[ d ].d_dim != DimUndefined )
			{
				if( test.count( p[ d ].d_dim ) != 0 )
					throw Root::Exception( "Dimensions not unique!" );
				else if( p[ d ].d_dim < 0 || p[ d ].d_dim >= d_dims.size() )
					throw Root::Exception( "Dimension out of valid range!" );
				else if( getColor( p[ d ].d_dim ) != p[ d ].d_atom )
					throw Root::Exception( "Referenced dimension must be of same atom type!" );
				else
					test.insert( p[ d ].d_dim );
			}
			if( p[ d ].d_atom.isNone() )
				throw Root::Exception( "Invalid atom type!" );
			if( p[ d ].d_hops < -1 )
				throw Root::Exception( "Invalid hops value!" );
		}
        if( int(test.size()) != d_dims.size() )
			throw Root::Exception( "Not all spectrum dimensions defined by a step!" );
	}
}

bool SpectrumType::autoRotate( Spectrum* spec, Rotation& out, bool optReso )
{
	ColorMap cm( d_dims.size() );
	for( Dimension i = 0; i < d_dims.size(); i++ )
		cm[ i ] = d_dims[i].d_atom;
	return autoRotate( cm, spec, out, optReso );
}

bool SpectrumType::autoRotate( const ColorMap& soll, Spectrum* spec, Rotation& out, bool optReso )
{
	if( spec == 0 || spec->getDimCount() != soll.size() )
		return false;

	typedef std::vector<bool> Flags;
	typedef std::map<AtomType,int> ColorCount;
	typedef std::map< std::pair<float,Sample>,std::vector<Dimension> > Order; 

	const Dimension dim = spec->getDimCount();
	std::set<AtomType> ambigSoll, ambigIst;
	bool equal = true;
	int availJokers = 0, neededJokers = 0;
	Flags used( dim );

	ColorMap ist( dim );
	// soll kann Dimensionen mit isNone haben. Diese sind "Joker", d.h. ein
	// Vergleich mit jeder anderen Color ist true.

	ColorCount countSoll, countIst, translateTo;

	Dimension d,k;
	// qDebug( "Ist: %s", spec->getName() ); // TEST
	for( d = 0; d < dim; d++ )
	{
		// qDebug( "soll[%d]=%s", d, soll[ d ].getLabel() ); // TEST

		if( !soll[ d ].isNone() )
		{
			if( countSoll[ soll[ d ] ] > 0 )
			{
				ambigSoll.insert( soll[ d ] );
			}else
				countSoll[ soll[ d ] ]++;
		}else
			availJokers++;
		ist[ d ] = spec->getColor( d );
		// qDebug( "ist[%d]=%s", d, ist[ d ].getLabel() );	// TEST
		// Der Fall ist[d].isNone() wird unten abgefangen
		if( countIst[ ist[ d ] ] > 0 )
		{
			ambigIst.insert( ist[ d ] );
		}else
		{
			countIst[ ist[ d ] ]++;
			translateTo[ ist[ d ] ] = d;
		}
		if( !soll[ d ].isNone() && ist[ d ] != soll[ d ] )
			equal = false;
	}
	for( d = 0; d < dim; d++ )
	{
		if( !soll[ d ].isNone() && countIst[ soll[ d ] ] == 0 )
			// Ein non-joker-Soll ist nicht in der Ist-Menge vorhanden
			return false;
		if(	countSoll[ ist[ d ] ] == 0 )
			// Wir haben eine Ist-Farbe gefunden, die von Soll nur mit einem
			// Joker geliefert werden kann.
			neededJokers++;	
	}
	Rotation rot( dim );
	if( neededJokers > availJokers )
		return false; // In Spec gibt es eine Farbe, die nicht auf Type passt.
	else if( availJokers == 0 && countSoll != countIst )
		return false; // Nicht gleichviel Farben bzw. Anzahl. Rotation unmöglich.
		// TODO: diese Prüfung für availJokers != 0 auch vornehmen
	else if( equal && ambigSoll.empty() )
	{
		// Zufällig gleich und keine mehrdeutigen Farben
		for( d = 0; d < dim; d++ )
			rot[ d ] = d;
		out = rot;
		return true;
	}else if( ambigSoll.empty() )
	{
		// Keine 1:1-Abbildung, aber dennoch keine mehrdeutigen Farben
		for( d = 0; d < dim; d++ )
		{
			if( !soll[ d ].isNone() )
			{
				rot[ d ] = translateTo[ soll[ d ] ];
				used[ rot[ d ] ] = true;
			}
		}
	}else
	{
		// Zuerst die eindeutigen
		for( d = 0; d < dim; d++ )
		{
			if( ambigSoll.count( soll[ d ] ) == 0 && !soll[ d ].isNone() )
			{
				assert( countIst[ ist[ d ] ] == 1 ); // Bereits oben geprüft
				// Farbe im Typ eindeutig und auch im Spektrum
				rot[ d ] = translateTo[ soll[ d ] ];
				used[ rot[ d ] ] = true;
			}
		}
		// Dann alle mehrdeutigen
		for( d = 0; d < dim; d++ )
		{
			if( ambigSoll.count( soll[ d ] ) != 0 )
			{
				Order order;
				for( k = 0; k < dim; k++ )
				{
					if( ist[ k ] == soll[ d ] )
					{
						if( optReso )
							order[ std::make_pair( spec->getScale( k ).getResol(),
								spec->getScale( k ).getSampleCount() ) ].push_back( k );
							// Es entscheidet 1) Resolution und 2) Anz. Sample
							// (bei gleicher Resolution), und dann Dim-Reihenfolge
						else
							order[ std::make_pair( float(0), Sample(0) ) ].push_back( k );
							// Es entscheidet nur Dim-Reihenfolge

					}
				}
				ambigSoll.erase( soll[ d ] ); // gerade alle der Farbe zurücksetzen.
				// Dies ist nötig, da gerade alle ambigSoll auf einmal erledigt
				Order::reverse_iterator q = order.rbegin();
				k = 0; // Gehe rückwärts, um die besten zuerst zuzuordnen
				int l = 0;
				while( k < dim && q != order.rend() )
				{
					if( soll[ k ] == soll[ d ] )
					{
						rot[ k ] = (*q).second[ l++ ];
						used[ rot[ k ] ] = true;
                        if( l >= int((*q).second.size()) )
						{
							++q;
							l = 0;
						}
					}
					k++;
				}
			}
		}
	}
	// Dann alle Joker
	if( availJokers > 0 )
	{
		for( d = 0; d < dim; d++ )
		{
			if( soll[ d ].isNone() )
			{
				for( k = 0; k < dim; k++ )
				{
					if( !used[ k ] )
					{
						rot[ d ] = k;
						used[ k ] = true;
						neededJokers--;
					}
				}
			}
		}
		if( neededJokers > 0 )
			return false;	// Konnte nicht ganzen Bedarf stillen.
	}
	std::set<Dimension> test;
	for( d = 0; d < dim; d++ )
		test.insert( rot[ d ] );
    if( int(test.size()) != rot.size() )
	{
		qDebug( "SpectrumType::autoRotate: I'm confused" );
		return false;
	}

	out = rot;

	// rot[0]==1 bedeutet: SpecPeer.getScale(0) -> spec.getScale(1)

	return true;
}

QByteArray  SpectrumType::getInstanceName(bool) const
{
	//char buf[16];
	//::sprintf( buf, "%d ", getId() );
	//QByteArray  res( buf );
	//res += getName();
	return getName().data(); // RISK
}

void SpectrumType::setPeakWidth(Dimension d, PPM w)
{
	SpecDim* dim = &d_dims[ d ];
	if( dim->d_width == w || w < 0.0 )
		return;
	dim->d_width = w;
	SpectrumType::Changed m( this, SpectrumType::Width, d );
	notifyObservers( m );
}

void SpectrumType::notifyObservers( Root::UpdateMessage& msg )
{
	if( d_owner )
		d_owner->getOwner()->notifyObservers( msg );
}

bool SpectrumType::isNoesy(Dimension a, Dimension b) const
{
	if( d_proc.empty() || d_step.empty() )
		return false;
	else if( Root::Math::abs( d_step[ a ] - d_step[ b ] ) == 1 )
        return ( ( d_step[ a ] > d_step[ b ] && isNoesy( a ) ) ||
            ( d_step[ a ] < d_step[ b ] && isNoesy( b ) ) );
	else
		return false;
}

bool SpectrumType::hasNoesy() const
{
	if( d_proc.empty() || d_step.empty() )
		return false;
	for( int i = 0; i < d_proc.size(); i++ )
		if( d_proc[ i ].d_hops < 0 )
			return true;
	return false;
}

int SpectrumType::getNoesyDir(Dimension a, Dimension b) const
{
	if( d_proc.empty() || d_step.empty() )
		return NoNoesy;
	else if( Root::Math::abs( d_step[ a ] - d_step[ b ] ) == 1 )
	{
		if( d_step[ a ] > d_step[ b ] && isNoesy( a ) )
			return LeftNoesy;
		else if( d_step[ a ] < d_step[ b ] && isNoesy( b ) )
			return RightNoesy;
	}
	return NoNoesy;
}

bool SpectrumType::rotateToUniqueKeys(Spectrum * spec, Rotation & res, bool reduce )
{
	// Code aus SystemTupleSet.
	assert( spec );
	assert( spec->getType() );

	// Diese Routine findet alle unique Keys des Spektrums. Wenn diese
	// nicht bei 0 beginnen und zusammenhängend sind, wird das Spektrum
	// rotiert. Die Reihenfolge der Dim wird dabei relativ gewahrt.
	KeyLabels kl( spec->getDimCount() );
	Dimension d;
	int nn = 0;
	for( d = 0; d < spec->getDimCount(); d++ )
	{
		kl[ d ] = spec->getType()->getKeyLabel( spec->mapToType( d ) );
		if( !kl[ d ].isNull() )
			nn++;
	}
	if( nn == 0 )
		throw Root::Exception( "Empty key set: ",
			"at least two dimensions with a unique final label each required.");
	if( nn < ( kl.size() - 1 ) )
		throw Root::Exception( "At least dim - 1 key labels needed." );

	if( reduce && nn == kl.size() )
	{
		kl[ DimY ] = SpinLabel();
		nn--;
		// Wenn alle dim einen eindeutigen Key haben, verwende die X- und Z-Achse als Anchor
	}

	std::set<SpinLabel> test;
	for( d = 0; d < kl.size(); d++ )
		test.insert( kl[ d ] );
    if( int(test.size()) != kl.size() )
		throw Root::Exception( "Duplicate label in key set: ",
		"at least two dimensions with a unique final label each required." );

	Rotation rot( kl.size() );
	int i = 0, j = 0;
	bool doRot = false;
	KeyLabels k( nn );
	for( d = 0; d < kl.size(); d++ )
	{
		// Rotiere nun so, dass alle nn zusammenhängend und bei 0 beginnend.
		if( !kl[ d ].isNull() )
		{
			rot[ i ] = d;
			k[ i ] = kl[ d ];
			if( i != d )
				doRot = true;
			i++;
		}else
		{
			rot[ j + nn ] = d;
			j++;
		}
	}

	if( doRot )
		res = rot;
	return doRot;
}
