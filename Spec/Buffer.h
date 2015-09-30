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

#if !defined(AFX_BUFFER_H__6C872550_8DE2_4B60_B1F8_5307016ABD92__INCLUDED_)
#define AFX_BUFFER_H__6C872550_8DE2_4B60_B1F8_5307016ABD92__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Spec/Units.h>
#include <Spec/Scale.h>

namespace Spec
{
	class Buffer : public Root::Matrix<Amplitude>
	{
	public:
		enum Operation { Correlate = 1, Convolute, Add, Subtract, Desitar };
		static void calculate( Operation, Buffer& res, const Buffer& lhs, const Buffer& rhs );

		typedef Root::Vector<bool> Switches;

		void amplify( Amplitude );
		void flip( const Switches&, bool scale = true );
		//. Spiegle den Buffer an der Achse, deren Switch true ist. Falls scale=true
		//. wird auch die Scale gespiegelt, ansonsten bleibt diese unverändert.
		void insert( const Buffer& buf, const Root::Cube& wo );
		//. Füge den Inhalt von buf bei wo in diesen Buffer ein, wobei wo eine Teilmenge
		//. von this sein muss.
		bool contains( const Buffer& ) const;
		void insert( const Buffer& buf );
		//. Füge den Inhalt von buf an die entsprechende Stelle in this.
		//. this bleibt dadurch in der Grösse unverändert, jedoch werden
		//. die Werte der Region gesetzt. buf muss eine Teilmenge von this sein.
		//. Richtung wird automatisch angepasst. this ist dominant.
		Amplitude calcMean() const;
		void calcMean( Amplitude& negative, Amplitude& positive ) const;
		//. Berechnet den arithmetischen Mittelwert des Buffers, getrennt grösser/kleiner Null
		void calcMeanMinMax( Amplitude& negative, Amplitude& positive, 
			Amplitude& minimum, Amplitude& maximum ) const;
		void null();
		Amplitude getAtPpm( PPM x, PPM y ) const;
		Amplitude getAtPpm( PPM x ) const;
		void clear();
		void resize( const ScaleVector& );
		const Scale& getScale( Dimension dim ) const { return d_scales[ dim ]; }
		void resize( const Extension& ext );
		void resample( const Extension&, bool interpolate = false );
		// Verringere die Anzahl Samples, ohne den Spektralbereich zu ändern.
		// Wenn interpolate true ist, wird der Durchschnitt der Punkte verwendet,
		// andernfalls jeder n-te Punkt.
		static void smoothScale( const Buffer& in, Buffer& out );
		void setColor( Dimension, AtomType );
		int getSize( Dimension d ) const { return d_ext[ d ]; }
		void adjustScale(Dimension d, PPM off ) { d_scales[ d ].adjust( off ); }
		void setFolding( Dimension, Scale::Folding );
		void accumulate( Root::Index, Amplitude );
		// Wie setAt, verwendet aber immer den grösseren Wert (Prüfung des bestehenden)

		Buffer();
		virtual ~Buffer();
	private:
		typedef Matrix<Amplitude> Parent;
		ScaleVector d_scales;
	};
}

#endif // !defined(AFX_BUFFER_H__6C872550_8DE2_4B60_B1F8_5307016ABD92__INCLUDED_)
