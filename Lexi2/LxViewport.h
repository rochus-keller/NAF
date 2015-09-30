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

#if !defined(AFX_VIEWPORT_H__FE31F28E_4837_42A9_B3F5_C65653A7FCE3__INCLUDED_)
#define AFX_VIEWPORT_H__FE31F28E_4837_42A9_B3F5_C65653A7FCE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Lexi2/LxMonoGlyph.h>
#include <Lexi2/LxAllocation.h>
#include <Root/Sequence.h>
#include <QRegion>

class QWidget;

namespace Lexi2
{
	// Der Viewport repräsentiert den Top-Level Glyph der Visual-Hierarchy und ist
	// damit der jenige, dessen Owner null ist und direkt das QWidget ausfüllt.
	class Viewport : public MonoGlyph
	{
	public:
		// Focus
		bool setFocus(Glyph *);
		Glyph* getFocus() const { return d_focus; }

		// Grabbing
		void grab(Glyph * h);
		void ungrab();
		Glyph* getGrabber() const { return d_grabber; }
		bool isGrabbing( Glyph* h ) const { return h == d_grabber; }

		// Damaging
		void damage(Coord left, Coord top, Coord width, Coord height, bool immediate = false);
		void damageAll( bool immediate = false );
		void damage(const Allocation& e, bool immediate = false)
		{
			damage( e.getLeft(), e.getTop(), e.getWidth(), e.getHeight(), immediate );
		}
		void commitDamage();
		void captureDamage();

		//* Cursor
		enum Cursor { CurNone = 0, CurArrow, CurUpArrow, CurCross, CurWait, CurIBeam, 
			CurSizeVer, CurSizeHor, CurSizeNEDiag, CurSizeSEDiag, 
			CurSizeAll, CurSplitV, CurSplitH, CurHand, CurForbidden };
		static void pushCursor( Cursor );
		static void pushHourglass();
		static void popCursor();
		//- 

		//* Overrides
		Viewport* getViewport(bool check=true) const;
		void request( Requisition& );
		void modified(Glyph* me);
		bool allocation( Allocation& a) const { a = d_area; return true; }
		void setBody(Glyph*);
		void allocate(const Allocation&);
		//-

		//* GlyphWidget-Interface
		void draw( Canvas& );
		//-

		//* Umrechnung von Bildschirmpunkten zu Twips
		static inline Coord toTwip( float x ) { return x * TwipsPerPoint; }
		static inline float toPointF( Coord x ) { return x / float( TwipsPerPoint ); }
		static inline int toPoint( Coord x ) { return Roundoff + x / float( TwipsPerPoint ); }
		//-

		Viewport(QWidget* parent);
	protected:
		~Viewport();
	private:
		struct Damage
		{
			Damage( Coord l, Coord t, Coord w, Coord h ):
				d_left( l ), d_top( t ), d_width( w ), d_height( h ) {}
			Damage():
				d_left( 0 ), d_top( 0 ), d_width( 0 ), d_height( 0 ) {}
			Coord d_left;
			Coord d_top;
			Coord d_width;
			Coord d_height;
		};

		Root::Sequence<Damage> d_damages;
		Allocation d_area;	
		Requisition d_requ;	// Letztbekannte Grössenanforderung ????
		Ref<Glyph> d_grabber;
		Ref<Glyph> d_focus;
		QWidget* d_parent;
		bool d_capture;
		bool d_requested;
	};
}

#endif // !defined(AFX_VIEWPORT_H__FE31F28E_4837_42A9_B3F5_C65653A7FCE3__INCLUDED_)
