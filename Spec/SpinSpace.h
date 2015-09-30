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

#if !defined(AFX_SPINSPACE_H__856555DA_9D91_46FC_94F1_613EDA91D38A__INCLUDED_)
#define AFX_SPINSPACE_H__856555DA_9D91_46FC_94F1_613EDA91D38A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <Root/UniqueList.h>
#include <Root/Subject.h>
#include <Spec/SpinSystem.h>
#include <Spec/Spectrum.h>
#include <bitset>

namespace Spec
{
	//* SpinSpace
	//. Dies ist die abstrakte Basisklasse fr Mengen von CrossPeaks,
	//. die aus der Menge aller Spins bzw. SpinSysteme berechnet werden.
	//. Der SpinSpace kann eine beliebige Anzahl Dimensionen haben.

	class SpinSpace : public Root::Subject
	{
	public:
		class Element 
		{
			enum { L12, L13, L23, L14, L24, L34, // Durch Link-Auflsung gefunden 1-2, 1-3, 2-3
				INFER,	  // Durch Peak-Inference gefunden (Pfadsim oder Kreuzprod)
				LOWER,	 // Geist ist bei tieferen PPM-Werten
				HIGHER,	 // Geist ist bei hheren PPM-Werten
				HIDDEN,	 // Aufgrund eines versteckten Links nicht sichtbar.
				_COUNT
			};
		public:
			SpinPoint d_point;

			Element( const SpinPoint& p ):
				d_point( p ) 
			{
			}
			Element() {}
			bool operator<( const Element& rhs ) const { return d_point < rhs.d_point; }
			bool operator==( const Element& rhs ) const { return d_point == rhs.d_point; }

			void unite( const Element& );
			bool isLink( Dimension, Dimension ) const;
			bool isLink() const { return d_flags.test( L12 ) || d_flags.test( L13 ) || d_flags.test( L23 ) ||
                        d_flags.test( L14 ) || d_flags.test( L24 ) || d_flags.test( L34 ); }
			void setLink( Dimension, Dimension, bool = true );
			bool isInfer() const { return d_flags.test( INFER ); }
			void setInfer( bool on = true ) { d_flags.set( INFER, on ); }
			bool isGhost() const { return d_flags.test( LOWER ) || d_flags.test( HIGHER ); }
			bool isLower() const { return d_flags.test( LOWER ) && !d_flags.test( HIGHER ); }
			void setLower( bool on = true ) { d_flags.set( LOWER, on ); }
			bool isHigher() const { return d_flags.test( HIGHER ) && !d_flags.test( LOWER ); }
			void setHigher( bool on = true ) { d_flags.set( HIGHER, on ); }
			void setHidden( bool on = true ) { d_flags.set( HIDDEN, on ); }
			bool isHidden() const { return d_flags.test( HIDDEN ); }
			void reset() { d_flags.reset(); }
			void assign( const Element&, const Rotation& );
		private:
			std::bitset<_COUNT> d_flags;
		};

		typedef Root::UniqueList<Element> Result;
		typedef std::set<Element> ElementSet;

		class Iterator
		{
		public:
			Iterator( Result::const_iterator c ):d_cur( c ) {}
			Iterator() {}
			/*
			bool next()
			{
				if( d_cur.isNull() )
					return false;
				++d_cur;
				return true;
			}
			bool isEof() const
			{
				return d_cur.isNull();
			}
			*/
			bool operator<( const Iterator& rhs ) const { return d_cur < rhs.d_cur; }
			bool operator!=( const Iterator& rhs ) const { return d_cur != rhs.d_cur; }
			void operator++() { ++d_cur; }
		private:
			Result::const_iterator d_cur;
			friend class SpinSpace;
		};

		// Muss ein set sein, da Updates irgendwelche Elemente in der Mitte 
		// lschen und einfgen knnen.
		SpinSpace();

        //* To Override
        virtual Iterator begin() const;
        virtual Iterator end() const;
        virtual Spectrum* getSpec() const { return 0; }
        //. Abgeleitete Klassen können konkret auf ein Spektrum referenzieren. Kann
        //. unabhngig von getSpecType() null oder rotiert sein.
        virtual SpectrumType* getSpecType() const { return 0; }
        //-

		//* Downcalls mit Default-Implementationen
        virtual Dimension getDimCount() const;
        virtual Dimension mapToSpec( Dimension d ) const;
		virtual int find( Result&, Spin*, Spin* ) const;
		virtual int find( Result&, const PpmCube&, Spectrum* = 0 ) const;
		virtual int find( Result&, Spin* ) const;
		virtual int find( Result&, SpinSystem* ) const;
        virtual int fetchAll( Result& ) const;
        virtual bool fetch(Iterator, Element&) const;
        //. Gebe das Element zurck, auf welches der Iterator zeigt.
		//-

		class Update : public Root::UpdateMessage
		{
		public:
			enum Type { Create, Delete, Move, Param, All };

			Update( Root::Object* m, Type t, 
				SpinSpace::Element p = SpinSpace::Element(), 
				PPM old = 0, Dimension d = DimUndefined, Spectrum* s = 0 ):
				Root::UpdateMessage( m ), d_type( t ), d_elem( p ), 
				d_old( old ), d_dim( d ) {}

			Type getType() const { return d_type; }
			const SpinSpace::Element& getElem() const { return d_elem; }
			PPM getOld() const { return d_old; }
			Dimension getDim() const { return d_dim; }
			Spectrum* getSpec() const { return d_spec; }
		private:
			Type d_type;
			SpinSpace::Element d_elem;
			Spectrum* d_spec;	// Es handelt sich um das Spec, aus welchem d_old stammt.
			PPM d_old;
			Dimension d_dim;
		};

	protected:
		virtual ~SpinSpace();
	};

    //* RotatedSpace
    //. Ein Adapter, der eine simple Rotation des SpinSpace vornimmt
    class RotatedSpace : public SpinSpace
    {
    public:
        RotatedSpace( SpinSpace*, const Rotation& );
        RotatedSpace( SpinSpace* );

        void setHost( SpinSpace* );
        SpinSpace* getHost() const { return d_host; }
        void setRotation( const Rotation& );

        //* Overrides
        bool fetch(Iterator, Element&) const;
        Iterator begin() const { return d_host->begin(); }
        Iterator end() const { return d_host->end(); }
        SpectrumType* getSpecType() const { return d_host->getSpecType(); }
        Spectrum* getSpec() const { return d_host->getSpec(); }
        Dimension getDimCount() const { return d_rot.size(); }
        Dimension mapToSpec(Dimension d ) const;
        //-
    protected:
        ~RotatedSpace();
        void handle( Root::Message& );
    protected:
        Root::Ref<SpinSpace> d_host;
        Rotation d_rot;
    };

    // Wie RotatedSpace, ausser dass die Rotation von Spec bzw. SpecType bestimmt wird.
    // Im Ergebnis hat der Space dadurch dieselbe Rotation wie Spec. Es werden nur Specs
    // akzeptiert, die denselben SpecType haben wie der Host.
    class SpecRotatedSpace : public RotatedSpace
    {
    public:
        SpecRotatedSpace( SpinSpace*, Spectrum* = 0 );
        void setSpec( Spectrum* );

        //* Overrides
        Spectrum* getSpec() const { return d_spec; }
        // Es braucht kein mapToSpec, da hier die Quelle von Spec
        //-
    private:
        Root::Ref<Spectrum> d_spec;
    };

    class LinkFilterSpace : public SpinSpace
    {
    public:
        LinkFilterSpace( SpinSpace* );
        bool showAll() const { return d_showAll; }
        void showAll( bool on );
        void setHost( SpinSpace* );
        SpinSpace* getHost() const { return d_host; }

        //* Overrides
        bool fetch(Iterator, Element&) const;
        Iterator begin() const { return d_host->begin(); }
        Iterator end() const { return d_host->end(); }
        SpectrumType* getSpecType() const { return d_host->getSpecType(); }
        Spectrum* getSpec() const { return d_host->getSpec(); }
        Dimension mapToSpec(Dimension d ) const { return d_host->mapToSpec( d ); }
        //-
    protected:
        ~LinkFilterSpace();
        void adjustParams( Element& ) const;
        void handle( Root::Message& );
    private:
        Root::Ref<SpinSpace> d_host;
        bool d_showAll;
    };

    //* RotatedLinkFilterSpace, ehemals SpecSpinSpace
    // Einerseits wie RotatedSpace, ausser dass die Rotation von Spec bzw. SpecType bestimmt wird.
	//. Ein Adapter, der eine Rotation des SpinSpace entsprechend dem gegebenen Spektrum
    //. auf dessen SpectrumType vornimmt. Vereinigung von LinkFilterSpace und SpecRotatedSpace.
    class LinkFilterRotSpace : public RotatedSpace
	{
	public:
        LinkFilterRotSpace( SpinSpace*, Spectrum* = 0 );
        void setSpec( Spectrum* );
		bool showAll() const { return d_showAll; }
		void showAll( bool on );

		//* Overrides
		bool fetch(Iterator, Element&) const;
		Iterator begin() const;
		Spectrum* getSpec() const { return d_spec; }
        // Es braucht hier kein mapToSpec, dito
		//-
	protected:
		void adjustParams( Element& ) const;
		void handle( Root::Message& );
	private:
		Root::Ref<Spectrum> d_spec;
        bool d_showAll;
	};

    // Ein SpinSpace ohne Automatismus, wo man die SpinPoints manuell einfgt
    // und entfernt, bzw. diese nicht automatisch generiert werden. Man braucht
    // diese Klasse primr, um Cursors oder Lineale zu verwalten.
    // In dieser Klasse haben getSpec und getSpecType nicht die übliche Bedeutung.
	class ManualSpinSpace : public SpinSpace
	{
	public:
		bool isEmpty() const { return d_res.empty(); }
        void setSpec( Spectrum* ) {} // dummy
        void removeAll();
		void removePoint( const SpinPoint& );
		void addPoint( const SpinPoint& );
        ManualSpinSpace( Dimension count );
		//* Overrides
        Dimension getDimCount() const { return d_count; }
        Iterator begin() const { return d_res.begin(); }
		Iterator end() const { return d_res.end(); }
	protected:
		~ManualSpinSpace() {}
		void handle( Root::Message & );
	private:
		Result d_res;
		Dimension d_count;
	};

}

#endif // !defined(AFX_SPINSPACE_H__856555DA_9D91_46FC_94F1_613EDA91D38A__INCLUDED_)
