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

#if !defined(AFX_UNDOMANAGER_H__C781FE69_4456_41AD_A3B2_3168E83A3711__INCLUDED_)
#define AFX_UNDOMANAGER_H__C781FE69_4456_41AD_A3B2_3168E83A3711__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Root/Agent.h>
#include <Root/Action.h>
#include <Root/Transaction.h>
#include <QList>

namespace Root
{
	//* UndoManager
	//* Dies ist ein Agent, der irgendwo in der Agent-Hierarchie sitzt und von oben
	//* oder unten Commands (mittels RegisterForUndo) erhält.
	//* Der Manager reagiert auf die Actions Edit.Undo, Edit.Redo und System.KillUndo.

	class UndoManager : public Agent
	{
	public:
		static void kill();
		void setMax( Index );
		Index getMax() const { return d_max; }

		UndoManager(Agent* parent = 0, Index max = 10 );
	protected:
		void handleRedo();
		void handleKillUndo();
		void handleUndo();
		void handle( Message& );
		virtual ~UndoManager();
	private:
		void limit();
		QList< Ref<Transaction> > d_dones;
		QList< Ref<Transaction> > d_undones;
		Index d_max;
	};
}

#endif // !defined(AFX_UNDOMANAGER_H__C781FE69_4456_41AD_A3B2_3168E83A3711__INCLUDED_)
