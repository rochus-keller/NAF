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

#if !defined(AFX_PEAKMATHMODEL_H__DE216785_0A4B_4C0F_8EB4_E25899C48336__INCLUDED_)
#define AFX_PEAKMATHMODEL_H__DE216785_0A4B_4C0F_8EB4_E25899C48336__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Subject.h>
#include <Spec/Units.h>

namespace Spec
{
	// Diese Klasse definiert den Funktor, der für die Berechnung des Intensitätsprofils
	// eines Peaks verwendet werden kann. Die Funktion ist grundsätzlich stetig, hat das
	// Maximum bei Punkt P0 mit Wert 1 und fällt mit wachsendem Radius stetig ab.
	// Es handelt sich somit um eine Sphäre um den Mittelpunkt P0, auf der sich
	// P bewegen kann. Für jedes P lässt sich eine Amplitude berechnen.

	class PeakModel : public Root::Subject 
	{
	public:
		struct Params 
		{
			PeakPos d_tol;	
			// Dies ist der Toleranzfaktor (initial 0), der angibt, in welchem Teil
			// des Cubus aus d_width nach dem Maximum gesucht wird.
			PeakPos d_width;
			// Dies ist die Öffnungsweite von p0, bzw. die PPM-Breite für die 
			// Varianz 1 in Gauss.
			PeakPos d_gain;	// LorentzGain
			PeakPos d_bal;	// Gauss/Lorentz-Balance

			void init()
			{
				for( int i = 0; i < PeakPos::MAX_DIM; i++ )
				{
					d_width[i] = 0.5;
					d_gain[i] = 0.0;
					d_bal[i] = 1.0;
					d_tol[i] = 0.0;
				}
			}
			Amplitude calcBell( Dimension dim, 
				const PeakPos& p, const PeakPos& p0, Amplitude a0 ) const
			{
				double res = 1;
				for( Dimension d = 0; d < dim; d++ )
					res *= calcShape( d, p[ d ], p0[ d ] );	// Berechne kartesisches Produkt.
				return a0 * res;

			}
			double calcShape( Dimension d, PPM p, PPM p0 ) const
			{
				double r = ( 1.0 - d_gain[ d ] ) * 
						2.0 * Root::Math::_fabs( p - p0 ) / d_width[ d ];
						
				return ( 1.0 - d_bal[ d ] ) * 
							
							1.0 / ( 1.0 + r * r ) + 
						
						d_bal[ d ] * 
							::exp( - ( r * r ) / 2.0 );
			}
			PPM calcMaxWidth( Dimension d ) const
			{ 
				return ( d_bal[ d ] * 3.0 + ( 1.0 - d_bal[ d ] ) * 9.0 * 
								1.0 / ( 1.0 - d_gain[ d ] ) ) * d_width[ d ]; 
			}
		};

		void setParams( const Params&, bool update = false );
		void getParams( Params& );

		float getGain( Dimension d ) const { return d_par.d_gain[ d ]; }
		void setGain( Dimension, float );
		float getBalance( Dimension d ) const { return d_par.d_bal[ d ]; }
		void setBalance( Dimension, float );

		void setWidth( Dimension, PPM );
		PPM getWidth( Dimension d ) const { return d_par.d_width[ d ]; }

		Dimension getDimCount() const { return d_dim; }

		void setTol( Dimension d, float t );
		float getTol( Dimension d ) const { return d_par.d_tol[ d ]; }

		PeakModel( Dimension d, Root::Index id );
		PeakModel();
		PeakModel( const PeakModel& );

		Root::Index getId() const { return d_id; }

		double getRadius( const PeakPos& p, const PeakPos& p0 ) const;
		// Berechne den Radius von p nach p0, wobei als Grenze, an der die Funktion
		// Null wird, der Radius eins angenommen wird. Die in d_nullGrenze angegebene
		// Distanz entspricht somit Radius 1.
		PPM getMaxWidth( Dimension d ) const
			{ return d_par.calcMaxWidth( d ); }
		//. Die Breite, in der noch mit Intensität zu rechnen ist.
		Amplitude getAmplitude( const PeakPos& p, const PeakPos& p0, Amplitude a0 ) const
			{ return d_par.calcBell( d_dim, p, p0, a0 ); }
		// Berechne Amplitude bei p, wobei p0 der Mittelpunkt ist und Amplitude a0 hat.
		double getShape( Dimension d, PPM p, PPM p0 ) const
			{ return d_par.calcShape( d, p, p0 ); }

		// Folgende Routinen werden von Backcalculation verwendet:
		// getAmplitude
		// getMaxWidth
		// getWidth

		class Update : public Root::UpdateMessage
		{
		public:
			enum Type { Tol, Width, Gain, Balance, All };

			Update( PeakModel* m, Type t, Root::Index id ):
				Root::UpdateMessage( m ), d_type( t ), d_id( id ) {}

			Type getType() const { return d_type; }
			Root::Index getId() const { return d_id; }
		private:
			Type d_type;
			Root::Index d_id;
		};
	protected:
		~PeakModel() {}
	private:
		friend class _XmlParser;

		void init( Dimension );

		Dimension d_dim;	// Anzahl Dimensionen des Modells
		Root::Index d_id;
		Params d_par;
	};

	// Gauss: Dieses Modell ist eine Gaussglocke um den Peak P0. Es handelt sich um eine
	// normierte Gaussfunktion, welche jedoch horizontal durch die Angabe des
	// Sockelradius in PPM verzerrt wird.

	// GaussLorentz: Dieses Modell ist eine Superposition von Gaussglocke und Lorentz-Funktion
	// um den Peak P0. Die Balance legt vertikal den Anteil von
	// Gauss und Lorentz fest, wobei 1.0 ganz Gauss und 0.0 ganz Lorentz ist.
	// HGain verbreitert die Lorentz-Kurve, was vorallem die Basis verbreitert.

}

#endif // !defined(AFX_PEAKMATHMODEL_H__DE216785_0A4B_4C0F_8EB4_E25899C48336__INCLUDED_)
