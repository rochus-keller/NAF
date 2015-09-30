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

#if !defined(AFX_PAINTER_H__60AEC24C_2034_4473_9D70_2105ACE55350__INCLUDED_)
#define AFX_PAINTER_H__60AEC24C_2034_4473_9D70_2105ACE55350__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Lexi2/LxUnits.h>
#include <Lexi2/LxAllocation.h>
#include <QPainter>
#include <QList>

namespace Lexi2
{
	class Image;
	class Icon;

	// Canvas ist nur ein dünner Wrapper um QPainter
	class Canvas  
	{
	public:
		bool isReady();
		void drawText( const Allocation&, const char* str, const QFont* f, QColor c );
		//. Zeichne str innerhalb Allocation, wobei Alignment berücksichtigt wird:
		//. Min=Left/Top, 0.5=Center, Max=Right/Bottom.
		void drawFocusRect( Coord x, Coord y, Coord w, Coord h, QColor );
		void drawPoint( Coord x, Coord y, QColor, Coord = 20 );
		void drawText( Coord x, Coord y, const char* str, const QFont* f, QColor c );
		//. Zeichne str, wobei y der Basislinie und x dem logischen Ursprung entspricht.
		void drawIcon( Coord x, Coord y, Icon* );
		void drawEllipse( Coord x, Coord y, Coord w, Coord h, QColor, Coord = 20 );
		void lineTo( Coord x, Coord y, QColor, Coord = 20 );
		void moveTo( Coord x, Coord y );
		void drawRect( Coord x, Coord y, Coord w, Coord h, QColor c, Coord = 20 );
		void bevelFrame( Coord x, Coord y, Coord w, Coord h, bool filled = false, 
			bool raised = true, Coord = 20 );
		void drawLine( Coord x1, Coord y1, Coord x2, Coord y2, QColor, Coord = 20 );
		void drawChar( Coord x, Coord y, char ch, const QFont* f, QColor c );
		//. Zeichne ch, wobei y der Basislinie und x dem logischen Ursprung entspricht.
		void fillRect( Coord x, Coord y, Coord w, Coord h, QColor c);
		//. Zeichne ein gefülltes Rechteck mit (x,y)=(left,top).
		void drawImage( Coord x, Coord y, Image* img, QColor bg = Qt::gray ) const;

		void rollback();
		void commit();
		void begin( QPainter*, const QRegion& );

		//* Damages
		void getDamage(Allocation & a) const;
		bool isDamaged(Coord left, Coord top, Coord width, Coord height ) const;
		bool isDamaged( const Allocation& e ) const
		{
			return isDamaged( e.getLeft(), e.getTop(), e.getWidth(), e.getHeight() );
		}
		void getDamage( AllocationVector& ) const;
		//-

		void popClip();
		void clip( const Allocation& );
		Canvas();
		virtual ~Canvas();

	protected:
		void setWidth( Coord w );
		void setColor( QColor c );
		float t2p( Coord x );
	private:
		QPainter* d_painter;
		QList<QRegion> d_clips;
		QRegion d_damage;
		QPen d_pen;
		QPointF d_point;
		DONT_CREATE_ON_HEAP;
	};
}
#endif // !defined(AFX_PAINTER_H__60AEC24C_2034_4473_9D70_2105ACE55350__INCLUDED_)
