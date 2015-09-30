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

#if !defined(AFX_MOLECULEVIEWER_H__D03F2E95_82AE_438C_A236_82A7B4EA17D7__INCLUDED_)
#define AFX_MOLECULEVIEWER_H__D03F2E95_82AE_438C_A236_82A7B4EA17D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Qt/qttableview.h> 
#include <QMouseEvent>
#include <Gui/Menu.h>
#include <Spec/ResidueType.h>
#include <Root/ActionHandler.h>
#include <Root/Vector.h>

namespace Spec
{
    class MoleculeViewer : public QtTableView, public Root::Messenger
	{
		Q_OBJECT
	public:
		static Root::Action::CmdStr NewAtom;
		static Root::Action::CmdStr DeleteAtom;
		static Root::Action::CmdStr ToggleBond;
		static Root::Action::CmdStr MoveAllVerti;
		static Root::Action::CmdStr MoveAllHori;
		static Root::Action::CmdStr MoveTo;
		static Root::Action::CmdStr ClearSel;
		static Root::Action::CmdStr Select;
		static Root::Action::CmdStr SetGroup;

		void select( Atom*, AtomType to, Root::Index hops, bool rep );
		void select( const SpinLabel& start, const SpinLabelSet& targets );
        void setResidueType( ResidueType* );
		MoleculeViewer(QWidget* parent, ResidueType*, bool rw = true );
		virtual ~MoleculeViewer();
	protected:
		void mouseMoveEvent ( QMouseEvent * );
		void mouseReleaseEvent ( QMouseEvent * );
		void mousePressEvent ( QMouseEvent * );
		void paintCell(class QPainter *,int,int);
		GENERIC_MESSENGER(QtTableView)
		void handle( Root::Message& );
	private:
		void handleSetGroup( Root::Action& );
		void handleClearSel( Root::Action& );
		void handleSelect( Root::Action& );
		void handleMoveTo( Root::Action& );
		void handleMoveAllHori( Root::Action& );
		void handleMoveAllVerti( Root::Action& );
		void handleToggleBond( Root::Action& );
		void handleDeleteAtom( Root::Action& );
		void handleNewAtom( Root::Action& );
		FRIEND_ACTION_HANDLER( MoleculeViewer );

		typedef std::pair<int,int> Cell; // row, col
		typedef std::map< Cell,Root::ExRef<Atom> > ObjList;
		typedef std::set< Root::ExRef<Atom> > AtomSet;

		Cell toCell( QMouseEvent* );
		Cell toCell( Atom* );
		bool isOccupied( const Cell& );
		void createAtom( const Cell& );
		void loadAll();
		void handleBond();

		ObjList d_atoms;
		Cell d_current;
		bool d_selected;
		bool d_dragging;
		Cell d_start;
		AtomSet d_sel;
		Root::ExRef<Atom> d_main;
		Gui::Menu* d_kontext;
		bool d_rw;

		Root::ExRef<ResidueType> d_rt;
	};
}

#endif // !defined(AFX_MOLECULEVIEWER_H__D03F2E95_82AE_438C_A236_82A7B4EA17D7__INCLUDED_)
