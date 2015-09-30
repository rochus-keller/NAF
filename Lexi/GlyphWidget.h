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

#if !defined(AFX_GLYPHWIDGET_H__63E5B6A1_D84C_11D4_B823_00D00918E99C__INCLUDED_)
#define AFX_GLYPHWIDGET_H__63E5B6A1_D84C_11D4_B823_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <qwidget.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QEvent>
#include <Root/Units.h>
#include <Lexi/Viewport.h>

namespace Root
{
	class Agent;
}

namespace Lexi
{
	struct _GlyphWidgetImp;

	//* GlyphWidget
	//. Diese Klasse ist das Verbindungsstück zwischen Qt und Lexi.
	//. Es handelt sich um ein Widget, in dessen innerem eine Glyph-Hierarchie dargestellt wird.
	//. Die Klasse enthält gleichzeitig eine Implementation von Agent bzw. Viewer, welche von
	//. den Glyphs gesehen wird und als Kommunikationsknoten für den Message-Transfer dient.

	class GlyphWidget : public QWidget
	{
		Q_OBJECT
	public:
		bool isDoubleBuffering() const;
		void setDoubleBuffering( bool on );
		QSize minimumSizeHint () const;
		QSizePolicy sizePolicy() const;
		QSize sizeHint() const;

		GlyphWidget(QWidget * parent1, Glyph* body = 0, 
			Root::Agent* parent2 = 0, bool transparent = false );
		virtual ~GlyphWidget();

		void reallocate();
		Glyph* getBody() const;
		void setBody( Glyph* body );
        void setFocusGlyph( Glyph* );
		// Folgende beiden Routinen sind identisch:
		Viewport* getViewport() const;
	private:
		Root::Ptr<_GlyphWidgetImp> d_this;
	protected:
		friend struct _GlyphWidgetImp;
		bool event( QEvent * );
		void paintEvent ( QPaintEvent * );
		void resizeEvent ( QResizeEvent * );
		void mouseDoubleClickEvent ( QMouseEvent * );
		void focusOutEvent ( QFocusEvent * );
		void focusInEvent ( QFocusEvent * );
		void keyReleaseEvent ( QKeyEvent * );
		void keyPressEvent ( QKeyEvent * );
		void mouseMoveEvent ( QMouseEvent * );
		void mouseReleaseEvent ( QMouseEvent * );
		void mousePressEvent ( QMouseEvent * );
	};
}

#endif // !defined(AFX_GLYPHWIDGET_H__63E5B6A1_D84C_11D4_B823_00D00918E99C__INCLUDED_)
