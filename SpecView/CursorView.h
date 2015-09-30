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

#if !defined(AFX_CURSORVIEW_H__CDEBC9C1_5EAE_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_CURSORVIEW_H__CDEBC9C1_5EAE_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <SpecView/SpecView.h>
#include <SpecView/CursorMdl.h>
#include <QColor>
#include <Root/Vector.h>
 
namespace Spec
{
	class CursorView : public SpecView
	{
	public:
		static const PPM EMPTY;
		typedef Root::Int8 ColorIndex;
		struct Cursor
		{
			PPM d_pos[ 2 ];
			ColorIndex d_color;
		public:
			Cursor();
		};
		typedef std::map<Root::Index,Cursor> Cursors;

		CursorView( SpecViewer*, CursorMdl* = nil, 
			bool useX = true, bool useY = true, QColor def = Qt::yellow );

		void setIdColor( bool on ) { d_idColor = on; }
		void setColor( QColor, ColorIndex = 0 );
		QColor getDefaultColor() const { return (* d_colors.find(0) ).second; }
		PPM getCursor( Dimension d ) { return d_cursors[ 0 ].d_pos[ d ]; }
		CursorMdl* getModel() const { return d_model; }
		void setCursor( PPM x, PPM y );
		void setCursor( Dimension, PPM );
		bool inUse( Dimension d ) const { return d_use[d]; }
		void use( Dimension, bool = true );
		QColor getColor( ColorIndex ) const;

		//* Overrides von Glyph
		void draw( Canvas&, const Allocation& );
	protected:
		virtual ~CursorView();
		void handle( Root::Message & );
	private:
		void damage( Dimension, PPM );

		typedef std::map< ColorIndex, QColor> Colors;
		Colors d_colors; // 0 ist default
		Cursors d_cursors;
		bool d_showId;
		bool d_idColor; // Die Cursor-ID bestimmt die Farbe
		Root::Ref<CursorMdl> d_model;
		bool d_use[ 2 ];
	};
}

#endif // !defined(AFX_SIMPCURSORVIEW_H__CDEBC9C1_5EAE_11D5_8DB0_00D00918E99C__INCLUDED_)
