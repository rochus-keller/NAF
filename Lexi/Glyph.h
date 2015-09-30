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

#if !defined(AFX_GLYPH_H__D1546CC1_C7EB_11D4_831C_CECE9BFFF574__INCLUDED_)
#define AFX_GLYPH_H__D1546CC1_C7EB_11D4_831C_CECE9BFFF574__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi/Units.h>
#include <Lexi/Viewport.h>
#include <Lexi/Allocation.h>
#include <Lexi/Requisition.h>
#include <Lexi/GlyphTraversal.h>

namespace Lexi
{
	//* Glyph
	//. Ein Glyph ist die Oberklasse fr alles, was sich in einem Bildschirmfenster
	//. (bzw. Canvas oder Viewport) darstellen lsst. Glyphs sind Komponenten. Sie
	//. knnen umfangreich (mit Zustand) oder minimal (FlightWeight, ohne Zustand) sein. 
	//. Letztere eignen sich auch als "Shared-Components", d.h. welche gleichzeitig
	//. in mehreren verschiedenen Contexts bzw. Container vertreten sein knnen.

	class Requisition;

	class Glyph : public Root::Resource
	{
	public:
		typedef Root::Sequence<Glyph*> Trace;

		//* Component management
		virtual Glyph* getComponent(GlyphIndex) const;
		virtual GlyphIndex getCount() const;
		virtual void remove(GlyphIndex);
		virtual void insert(GlyphIndex, Glyph*);
		virtual void prepend(Glyph*);
		virtual void append(Glyph*);
		virtual void replace(GlyphIndex, Glyph*);

		//* Allocation-Methoden
		//. Diese sorgen dafr, dass allfllige lokale Allocations aktualisiert werden.
		//. Der changed-State wird durch diese Methoden zurckgesetzt.
		virtual void pick( Twips x, Twips y, const Allocation&, Trace&);
		//. Prft, ob der Glyph vom Punkt im GlyphTraversal getroffen wurde.
		//. Falls ja wird der Glyph im Traversal registriert.
		//. Methode kann lokal persistente Allocation und Viewport setzen.
		virtual void draw( Canvas&, const Allocation&);
		//. Zeichnet den Glyph auf dem Viewport, wobei Position, Breite und Hhe des
		//. des Glyphs relativ zum Viewport als Allocation mitgegeben wird. 
		//. Methode kann lokal persistente Allocation und Viewport setzen.
		virtual void allocate( const Allocation&);
		//. Teile dem Glyph seinen Viewport und darin die zugeteilte Allocation mit.
		//. Methode kann lokal persistente Allocation und Viewport setzen.
		//. Ein Polyglyph ruft normalerweise allocate fr all seine Komponenten auf,
		//. whrend er sich in pick oder draw befindet und sich die mitgeteilte
		//. Allocation gegenber dem letzten Aufruf gendert hat oder eine Komponente
		//. sich anlsslich changed als ungltig erklrt hat.
		virtual void traverse( const Allocation&, GlyphTraversal&);
		//. Trage den Traversal runter entlang der visuellen Glyph-Hierarchie. Die
		//. Default-Implementation ruft handle auf. Jeder Glyph kann somit seinen
		//. eigenen Handler definieren und den Traversal als Default belassen.

		virtual void deallocate();
		//. Teile dem Glyph mit, dass er nicht mehr Teil eines Viewport ist (ehemals undraw).
		//. Gegenstck zu Allocation-Methoden
		virtual void modified(GlyphIndex);
		//. This notifies a glyph that its component at the given index has 
		//. changed its requisition, and therefore it might be necessary to reallocate it. 
		//. This is just a notification. Real reallocation occurs with the next call 
		//. of pick, draw or allocate. Insertion and deletion at the given index can
		//. also be seen as a change of requisition. (ehemals change()).
		//. Diese Methode kann den Changed-State dieses Glyphs setzen.

		//* Frage nach Geometrie oder Geometrienderungen
		virtual void request( Requisition& );
		//. Erfrage vom Glyph die gewnschte Hhe und Breite, die ihn selber und
		//. allenfalls enthaltene Glyphs umfasst.
		//. Nur Character knnte auch Viewport brauchen wegen Character-Metrics.
		virtual bool allocation( Allocation&, GlyphIndex = NilGlyphIndex ) const;
		//. Gebe die lokal gehaltene Allocation des Component-Glyphs zurck.
		//. NilGlyphIndex gibt die gesamte Allocation des Containers zurck. 
		virtual bool changed();
		//. Mit dieser Routine teilt eine Komponente ihrem Container mit, dass ihre
		//. Requisition gendert hat. Die Standardimplementation ruft changed aller
		//. Subkomponenten auf und fr jede, welche true liefert, das lokale modified() 
		//. mit dem Index der Subkomponente. Ein allflliges Changed-Flag soll von
		//. request() wieder zurckgesetzt werden.

		//* Overrides von Object
		void traverse( Message& );
		//. Gebe Message entlang der visuellen Hierarchie runter (depth first), bis consumed.

		Glyph();

		//* Viewport und Damage-Delegates
		virtual Viewport* getViewport(bool check=true) const;
		void damageAll(bool immediate = false);
		void damage(Twips left, Twips top, Twips width, Twips height, 
			Twips offX, Twips offY, bool immediate = false);
		void damage(const Allocation& e, bool immediate = false);
		void damageMe(bool immediate = false);
		//.

		//* Glyph-Ownership-Hierarchie
		Glyph* getOwner(bool check=false) const 
		{ 
				if( check && d_owner.isNull() ) 
					throw NotOwned(); 
				return d_owner; 
		}
		void reparent( Glyph*, bool check = false );

		class AlreadyOwned : public Root::Exception
		{
		public:
			AlreadyOwned():Exception( "The glyph already belongs to a container" ) {}
		};
		class NotOwned : public Root::Exception
		{
		public:
			NotOwned():Exception( "The glyph has no owner" ) {}
		};
		//.

	protected:
		virtual ~Glyph();

		friend class GlyphTraversal;
		virtual bool handle( const Allocation&, GlyphTraversal&);
		//. Reagiere auf die Message im Traversal. Jeder Glyph kann auf eigene Art reagieren.
		//. Wenn keine Message vorhanden ist, handelt es sich um einen pick.
		//. Wenn die Message verarbeitet wurde, wird true zurckgegeben, sonst false.
		//. Diese Methode soll nicht direkt aufgerufen werden, sondern nur via traverse().

		void handle( Message& ) {}
		//. Hier nochmals wiederholen, da sonst von anderer Handle-Methode verdeckt.
	private:
		Root::Ptr<Glyph> d_owner;
	};

	typedef Root::Vector< Ref<Glyph> > GlyphVector;
}

#endif // !defined(AFX_GLYPH_H__D1546CC1_C7EB_11D4_831C_CECE9BFFF574__INCLUDED_)
