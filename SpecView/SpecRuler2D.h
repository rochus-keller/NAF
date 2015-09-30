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

#if !defined(AFX_SPECRULER2D_H__696F952E_A7D6_49D6_BB73_528CFEBCA153__INCLUDED_)
#define AFX_SPECRULER2D_H__696F952E_A7D6_49D6_BB73_528CFEBCA153__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Lexi/MonoGlyph.h>
#include <QColor>
#include <Lexi/Font.h>
#include <SpecView/SpecBufferMdl.h>
#include <Spec/SpinSystem.h>

namespace Spec
{
	using namespace Lexi;

	class SpecRuler2D : public Lexi::MonoGlyph
	{
	public:
		void setModel( SpecBufferMdl* );
		void show( Dimension, bool );
		void setThick( Lexi::Twips );
		Lexi::Twips getThick() const { return d_thick; }

		Lexi::Twips getSpace( Dimension d ) const { return d_space[ d ]; }
		void setSpace( Dimension d, Lexi::Twips s ) { d_space[ d ] = s; }

		Root::UInt8 getCount( Dimension d ) const { return d_count[ d ]; }
		void setCount( Dimension d, Root::UInt8 s ) { d_count[ d ] = s; }

		void setColor( QColor );
		QColor getColor() const { return d_color; }

		void setFont( const QFont& );
		const QFont& getFont() const { return d_font; }

		SpecRuler2D(Glyph* body, SpecBufferMdl*, const QFont* = 0, QColor = Qt::black, bool showH = true);
		void allocateBody( Glyph*, Allocation& );

		Lexi::Twips getWidth() const { return d_width; }
		Lexi::Twips getHeight() const { return d_height; }
		Lexi::Twips getSmall() const { return d_small; }
		Lexi::Twips getLarge() const { return d_large; }
		bool show( Dimension d ) const { return d_show[ d ]; }

		//* Overrides from MonoGlyph
		void draw( Canvas&, const Allocation&);
		void request( Requisition& );
		void allocate( const Allocation& a );
		void pick( Twips x, Twips y, const Allocation&, Trace&);
		void traverse(const Allocation & a, GlyphTraversal & t);
	protected:
		void calcSize();
		virtual ~SpecRuler2D();
	private:
		QColor d_color;
		QFont d_font;
		Root::Ref<SpecBufferMdl> d_model;
		Lexi::Twips d_width, d_height, d_small, d_large;
		Lexi::Twips d_space[ 2 ];
		Root::UInt8 d_count[ 2 ];
		bool d_show[ 2 ];
		Lexi::Twips d_thick;
	};

	class StripRuler2D : public Lexi::MonoGlyph
	{
	public:
		void anchLabel( bool on );
		bool anchLabel() const { return d_anchLabel; }
		void show( bool on );
		void setThick( Lexi::Twips );
		Lexi::Twips getThick() const { return d_thick; }
		void showCenter( bool );
		bool showCenter() const { return d_showCenter; }
		void addStrip( SpecBufferMdl*, Spin* x, Spin* y );
		void setColor( QColor );
		QColor getColor() const { return d_color; }
		void setCenterColor( QColor );
		QColor getCenterColor() const { return d_centerClr; }
		void setFont( const QFont& );
		const QFont& getFont() const { return d_font; }
		void allocateBody( Glyph*, Allocation& );
		Lexi::Twips getHUpper() const { return d_hUpper; }
		Lexi::Twips getHLower() const { return d_hLower; }
		bool show() const { return d_show; }

		//* Overrides from MonoGlyph
		void draw( Canvas&, const Allocation&);
		void request( Requisition& );
		void allocate( const Allocation& a );
		void pick( Twips x, Twips y, const Allocation&, Trace&);
		void traverse(const Allocation & a, GlyphTraversal & t);

		StripRuler2D(Glyph* body, const QFont* = 0, QColor = Qt::black, QColor = Qt::gray);
	private:
		QColor d_color;
		QColor d_centerClr;
		QFont d_font;
		Lexi::Twips d_thick;
		Lexi::Twips d_hUpper, d_hLower;
		struct Strip
		{
			Root::Ref<SpecBufferMdl> d_model;
			Root::Ref<Spin> d_x;
			Root::Ref<Spin> d_y;
			Strip( SpecBufferMdl* mdl, Spin* x, Spin* y ):
				d_model( mdl ), d_x( x ), d_y( y ) {}
			Strip():
				d_model( 0 ), d_x( 0 ), d_y( 0 ) {}
		};
		Root::Vector<Strip> d_strips;
		bool d_showCenter;
		bool d_show;
		bool d_anchLabel;
	protected:
		void calcSize();
	};
}

#endif // !defined(AFX_SPECRULER2D_H__696F952E_A7D6_49D6_BB73_528CFEBCA153__INCLUDED_)
