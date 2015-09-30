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

#if !defined(AFX_TREECTRL_H__B79398E7_A9BD_43A9_8921_4D635ADE00B7__INCLUDED_)
#define AFX_TREECTRL_H__B79398E7_A9BD_43A9_8921_4D635ADE00B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Gui/EventListener.h>

class QTreeView;

namespace Gui
{
	class Menu;

	class TreeCtrl : public Gui::EventListener  
	{
	public:
		QTreeView* wnd() const;

		IMPLEMENT_REACTOR( TreeCtrl );

		TreeCtrl(QTreeView*, bool = true);
		virtual ~TreeCtrl();

		static Menu* createDefaultPopup( QWidget* target );

		static const char* Expand;
		static const char* ExpandAll;
		static const char* ExpandSubs;
		static const char* CollapseAll;
		static const char* Reload;
	private:
		void handleCopy( Root::Action& );
		void handleReload( Root::Action& );
		void handleExpand( Root::Action& );
		void handleCollapseAll( Root::Action& );
		void handleExpandSubs( Root::Action & );
		void handleExpandAll( Root::Action& );
	};
}

#endif // !defined(AFX_TREECTRL_H__B79398E7_A9BD_43A9_8921_4D635ADE00B7__INCLUDED_)
