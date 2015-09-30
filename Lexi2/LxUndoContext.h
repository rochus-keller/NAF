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

#if !defined(AFX_TXNCONTEXT_H__A918EBD2_4CD5_4257_812C_BF4D3CF26242__INCLUDED_)
#define AFX_TXNCONTEXT_H__A918EBD2_4CD5_4257_812C_BF4D3CF26242__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Resource.h>
#include <Root/MakroCommand.h>
#include <Root/Sequence.h>

namespace Lexi2
{
	//* UndoContext
	//. Kontext einer Transaktion und gleichzeitig UndoManager.

	class UndoContext : public Root::Resource
	{
	public:
		static UndoContext* global();
		static bool exec( UndoContext*, Root::Command* );

		void redo();
		void undo();

		/**
			Submit and execute a command (only execute if unexecuted).
		*/
		virtual void exec( Root::Command*, bool immediate = true );

		/**
			Submit a command without executing (with or without commit).
		*/
		virtual void submit( Root::Command* );

		/**
			Create a new macro command. Following submit and exec will
			be added to it.
		*/
		virtual void begin(const char* title = "");

		/**
			Execute the macro command (only unexecuted)
		*/
		virtual void commit();

		/**
			Unexecute and drop the macro command.
		*/
		virtual void rollback();

		UndoContext();
	protected:
		virtual ~UndoContext();
	private:
		Root::Sequence< Root::Own<Root::Command> > d_done;
		Root::Sequence< Root::Own<Root::Command> > d_undone;
		Root::Sequence< Root::Own<Root::MakroCommand> > d_trans;
	};
}

#endif // !defined(AFX_TXNCONTEXT_H__A918EBD2_4CD5_4257_812C_BF4D3CF26242__INCLUDED_)
