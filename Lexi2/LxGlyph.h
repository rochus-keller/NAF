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

#include <Lexi2/LxUnits.h>
#include <Lexi2/LxCanvas.h>
#include <Lexi2/LxAllocation.h>
#include <Lexi2/LxRequisition.h>
#include <Root/Sequence.h>
#include <Root/Message.h>
#include <Root/Exception.h>

namespace Lexi2
{
	class Viewport;

	//* Glyph
	//. Ein Glyph ist die Oberklasse für alles, was sich in einem Bildschirmfenster
	//. (bzw. Canvas oder Viewport) darstellen lässt. 

	class Glyph : public Root::Resource
	{
	public:
		typedef Root::Sequence<Glyph*> Traversal;
		typedef Traversal GlyphTraversal;

		virtual Viewport* getViewport(bool check=true) const;
		Glyph* getOwner(bool check=false) const 
			{ if( check && d_owner.isNull() ) throw NotOwned(); return d_owner; }
		void reparent( Glyph* );

		void damageAll(bool immediate = false);
		void damage(Coord left, Coord top, Coord width, Coord height, bool immediate = false);

		//* Resource
		void traverse( Root::Message& );
		//-

		//* Component management
		virtual Glyph* getComponent(GlyphIndex) const;
		virtual GlyphIndex getCount() const;
		virtual void remove(GlyphIndex);
		virtual void insert(GlyphIndex, Glyph*);
		virtual void prepend(Glyph*);
		virtual void append(Glyph*);
		virtual void replace(GlyphIndex, Glyph*);

		//* Allocation-Methoden
		//. Diese sorgen dafür, dass allfällige lokale Allocations aktualisiert werden.
		//. Der changed-State wird durch diese Methoden zurückgesetzt.
		virtual void pick( Coord x, Coord y, const Allocation&, GlyphTraversal&);
		//. Prüft, ob der Glyph vom Punkt getroffen wurde.
		//. Falls ja wird der Glyph im Traversal registriert.
		//. Methode kann lokal persistente Allocation setzen.
		virtual void draw(Canvas&, const Allocation&);
		//. Zeichnet den Glyph auf dem Viewport, wobei Position, Breite und Höhe des
		//. des Glyphs relativ zum Viewport als Allocation mitgegeben wird. 
		//. Methode kann lokal persistente Allocation setzen.
		virtual void allocate(const Allocation&);
		//. Teile dem Glyph seinen Viewport und darin die zugeteilte Allocation mit.
		//. Methode kann lokal persistente Allocation setzen.
		//. Ein Polyglyph ruft normalerweise allocate für all seine Komponenten auf,
		//. während er sich in pick oder draw befindet und sich die mitgeteilte
		//. Allocation gegenüber dem letzten Aufruf geändert hat oder eine Komponente
		//. sich anlässlich changed als ungültig erklärt hat.

		virtual void deallocate();
		//. Teile dem Glyph mit, dass er nicht mehr Teil eines Viewport ist (ehemals undraw).
		//. Gegenstück zu Allocation-Methoden
		virtual void modified(Glyph* me);
		//. This notifies a Container that its component me has 
		//. changed its requisition, and therefore it might be necessary to reallocate it. 

		//* Frage nach Geometrie oder Geometrieänderungen
		virtual void request( Requisition& );
		//. Erfrage vom Glyph die gewünschte Höhe und Breite, die ihn selber und
		//. allenfalls enthaltene Glyphs umfasst.
		//. Nur Character könnte auch Viewport brauchen wegen Character-Metrics.
		virtual bool allocation( Allocation& ) const;
		//. Gebe die lokal gehaltene Allocation des Component-Glyphs zurück.
		//. Gibt false zurück, wenn keine lokalen Informationen verfügbar.


		Glyph(Glyph* = 0);
		
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

	protected:
		virtual ~Glyph();
	private:
		Root::Ptr<Glyph> d_owner;
	};

}

#endif // !defined(AFX_GLYPH_H__D1546CC1_C7EB_11D4_831C_CECE9BFFF574__INCLUDED_)
