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

#if !defined(AFX_LISTVIEW_H__ECD2B022_46B4_4D59_9A03_8FB3E85A94AF__INCLUDED_)
#define AFX_LISTVIEW_H__ECD2B022_46B4_4D59_9A03_8FB3E85A94AF__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi/Handler.h>
#include <Gui/ListView.h>
//Added by qt3to4:
#include <QMouseEvent>

namespace Lexi 
{
	class GlyphWidget;
	class ListView;

	// Diese Klasse ist provisorisch und wird sich mit Qt4 grundlegend ndern.
	class _ListViewImp : public Gui::ListView
	{
	Q_OBJECT
	public:
		_ListViewImp(Lexi::ListView*,GlyphWidget*);
		virtual ~_ListViewImp();
	protected:
		void contentsMouseDoubleClickEvent( QMouseEvent * e );
		void onCurrentChanged();
	private:
		Root::Ptr<Lexi::ListView> d_this;
	private slots:
		void handleReturnPressed( Gui::ListViewItem * );
		void handleDoubleClicked( Gui::ListViewItem * );
		void handleRightButtonClicked( Gui::ListViewItem *, const QPoint &, int );

	};

	class ListView : public Handler
	{
		_ListViewImp* d_that;
		friend class _ListViewImp;
	public:
		ListView( GlyphWidget* );
		~ListView();
		_ListViewImp* getImp() const { return d_that; }

		//* Overrides von Glyph
		void request( Requisition& );
		void allocate( const Allocation&);
		void pick( Twips x, Twips y, const Allocation&, Trace&);

	protected:
		//* To Override
		virtual void handleRightClick( Gui::ListViewItem *,const QPoint & );
		virtual void handleReturn( Gui::ListViewItem * );
		virtual void handleDblClick( Gui::ListViewItem * );
		virtual void handleSelection();
		//-
	};
}

#endif // !defined(AFX_LISTVIEW_H__ECD2B022_46B4_4D59_9A03_8FB3E85A94AF__INCLUDED_)
