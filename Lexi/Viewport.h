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

#if !defined(AFX_VIEWPORT_H__86153CE1_C95C_11D4_831C_0020CB010A30__INCLUDED_)
#define AFX_VIEWPORT_H__86153CE1_C95C_11D4_831C_0020CB010A30__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Root/Agent.h>
#include <Lexi/Canvas.h>

namespace Lexi
{
	using Root::Agent;
	class Allocation;
	class Glyph;
	class Extension;
	class GlyphWidget;

	//* Viewport
	//. Der Viewport ist der Teil eines Fenster, der die Glyphs enthlt. Er 
	//. enthlt den Root-Glyph, welcher die gesamte Flche des Viewport umfasst.
	//. Der Viewport bemisst seine Grsse derart, dass sie mglichst der
	//. Requisition des Root-Glyph entspricht.
	//. Der Viewport dient gleichzeitig als (abstraktes) Canvas-Interface mit
	//. allen ntigen Zeichenoperationen.

	class Viewport : public Agent
	{
	public:
		Viewport( Agent* parent );

		//* Umrechnung von Bildschirmpunkten zu Twips
		static inline Twips toTwip( int x ) { return x * TwipsPerPoint; }
		static inline int toPoint( Twips x ) 
			{ return Root::Math::round( x / float( TwipsPerPoint ) ); }
		static inline float toPointF( Twips x ) { return x / float( TwipsPerPoint ); }
		//-

		//* Damaging
		//. Einen Teil des Viewports fr ungltig erklren, so dass er neu gezeichnet wird.
		virtual void damage(Twips left, Twips top, Twips width, Twips height, 
			Twips offX, Twips offY, bool immediate = false );
		void damage(const Allocation&, bool immediate = false);
		//. Lokaler helper, der damage( Twips..) aufruft.
		virtual void damageAll(bool immediate = false);
		//. Diese Routine passt nicht ins Offset-Konzept von Allocation und wird
		//. sowieso nirgends gebraucht.
		// virtual void damage(const Extension&, bool immediate = false);

		virtual void scrollDamage( const Allocation& a, Twips dx, Twips dy );
		//. Verschiebt den Bildschirmpuffer a um dx und dy, wobei ursprngliches a
		//. als Clip-Bereich verwendet wird. Der freiwerdende Bereich wird darauf
		//. in Damage-Liste eingetragen.

		virtual void getDamage( Allocation& ) const;
		bool isDamaged( const Allocation& ) const;
		//. Lokaler Helper, der isDamaged( Twips..) aufruft.
		virtual bool isDamaged( Twips left, Twips top, Twips width, Twips height,
			Twips offX, Twips offY ) const;

		virtual void captureDamage();
		//. Zeichne damage() auf fr spteren commit(). Ausnahme ist immediate, dass sofort ausfhrt.
		virtual void commitDamage();
		//. Fhre zuvor mit capture() aufgezeichnete Damages aus.
		//-

		//* Grabbing
		//. Der angegebene Glyph erhlt alle Mausereignisse
		virtual void grab( Glyph* );
		virtual void ungrab();
		virtual bool isGrabbing( Glyph* ) const;
		//-

		//* Cursor
		enum Cursor { CurNone = 0, CurArrow, CurUpArrow, CurCross, CurWait, CurIBeam, 
			CurSizeVer, CurSizeHor, CurSizeNEDiag, CurSizeSEDiag, 
			CurSizeAll, CurSplitV, CurSplitH, CurHand, CurForbidden };
		static void pushCursor( Cursor );
		static void pushHourglass();
		static void popCursor();
		virtual void setCursor( Cursor );
		//- 

		virtual GlyphWidget* getWindow() const { return 0; }
		//. Dient einem Glyph dazu, ein Qt-Widget als Child des GlyphWidgets zu deklarieren.
		virtual bool setFocus( Glyph* );
		//. Setze den Fokus auf den angegebenen Glyph. Gebe true zurck, wenn dieser den Fokus akzeptiert.
		virtual Twips getLastClick( Dimension ) const;
		//. Position des letzten Mouse-Press (nicht jedoch Mouse-Release).

	protected:
		~Viewport() {}
	};
}

#endif // !defined(AFX_VIEWPORT_H__86153CE1_C95C_11D4_831C_0020CB010A30__INCLUDED_)
