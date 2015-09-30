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

#if !defined(AFX_CANVAS_H__6A660544_DB29_11D4_B847_00D00918E99C__INCLUDED_)
#define AFX_CANVAS_H__6A660544_DB29_11D4_B847_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi/Units.h>
#include <Lexi/Allocation.h>
#include <QPainter>

namespace Lexi
{
	//* Canvas
	//. Dies ist ein simpler Adapter zu QPainter

	class Font;
	class Allocation;
	class Image;
	class Icon;
	class Viewport;

	class Canvas // Value Only
	{
	public:
		void drawText( const Allocation&, const char* str, const QFont* f, QColor c );
		//. Zeichne str innerhalb Allocation, wobei Alignment bercksichtigt wird:
		//. Min=Left/Top, 0.5=Center, Max=Right/Bottom.
		void drawFocusRect( Twips x, Twips y, Twips w, Twips h, QColor c = Qt::gray );
		void drawPoint( Twips x, Twips y, QColor, Twips = 20 );
		void drawText( Twips x, Twips y, const char* str, const QFont* f, QColor c );
		//. Zeichne str, wobei y der Basislinie und x dem logischen Ursprung entspricht.
		void drawIcon( Twips x, Twips y, Icon* );
		void drawEllipse( Twips x, Twips y, Twips w, Twips h, QColor, Twips = 20 );
		void lineTo( Twips x, Twips y, QColor, Twips = 20 );
		void moveTo( Twips x, Twips y );
		void drawRect( Twips x, Twips y, Twips w, Twips h, QColor c, Twips = 20 );
		void bevelFrame( Twips x, Twips y, Twips w, Twips h, bool filled = false, 
			bool raised = true, Twips = 20 );
		void drawLine( Twips x1, Twips y1, Twips x2, Twips y2, QColor, Twips = 20 );
		void drawChar( Twips x, Twips y, CharCode ch, const QFont* f, QColor c );
		//. Zeichne ch, wobei y der Basislinie und x dem logischen Ursprung entspricht.
		void fillRect( Twips x, Twips y, Twips w, Twips h, QColor c);
		//. Zeichne ein geflltes Rechteck mit (x,y)=(left,top).
		void drawImage( Twips x, Twips y, Image* img, QColor bg = Qt::gray ) const;

		bool isReady();
		void rollback();
		void commit();
		void begin( Viewport*, QPainter*, const QRegion& damage, bool exact = false );

		void getDamage( AllocationVector& ) const;
		void popClip();
		void clip( const Allocation& );
		Viewport* getViewport() const { return d_vp; }

		Canvas();
		~Canvas();
	protected:
		void setWidth( Twips w );
		void setColor( QColor c );
		float t2p( Twips x ) const;
	private:
		QPainter* d_painter;
		QList<QRegion> d_clips;
		QRegion d_damage;
		QPen d_pen;
		QPointF d_point;
		Root::Ptr<Viewport> d_vp;
		bool d_exact;
		DONT_CREATE_ON_HEAP;
	};
}

#endif // !defined(AFX_CANVAS_H__6A660544_DB29_11D4_B847_00D00918E99C__INCLUDED_)
