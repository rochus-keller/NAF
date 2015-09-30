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

#if !defined(AFX_SPECBATCHLIST_H__DF0CC0E7_8B04_4AD5_8FF5_511ED87C46F4__INCLUDED_)
#define AFX_SPECBATCHLIST_H__DF0CC0E7_8B04_4AD5_8FF5_511ED87C46F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <QDialog>
#include <Gui/Menu.h>
#include <Root/ActionHandler.h>
#include <Spec/Project.h>

class QTreeWidget;

namespace Spec
{
    class SpecBatchList : public QDialog, public Root::Messenger
	{
		Q_OBJECT
	public:
		static Root::Action::CmdStr AddSpectrum;
		static Root::Action::CmdStr RemoveSpectrum;
		static Root::Action::CmdStr MoveUp;
		static Root::Action::CmdStr MoveDown;
		static Root::Action::CmdStr EditFields;
		static Root::Action::CmdStr AddAllAlpha;
		static Root::Action::CmdStr RemoveAll;
		static Root::Action::CmdStr ShowTable;

		bool doit();
		SpecBatchList(QWidget *, PeakList*, Project*);
		virtual ~SpecBatchList();
	protected:
		GENERIC_MESSENGER(QDialog)
		void handle( Root::Message& );
	private:
		void handleShowTable( Root::Action& );
		void handleRemoveAll( Root::Action& );
		void handleAddAllAlpha( Root::Action& );
		void handleEditFields( Root::Action& );
		void handleMoveDown( Root::Action& );
		void handleMoveUp( Root::Action& );
		void handleRemoveSpectrum( Root::Action& );
		void handleAddSpectrum( Root::Action& );
		FRIEND_ACTION_HANDLER( SpecBatchList );
		QTreeWidget* d_list;
		Root::ExRef<Spectrum> d_master;
		Root::ExRef<Project> d_pro;
		Root::ExRef<PeakList> d_pl;
	};
}

#endif // !defined(AFX_SPECBATCHLIST_H__DF0CC0E7_8B04_4AD5_8FF5_511ED87C46F4__INCLUDED_)
