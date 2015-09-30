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

#if !defined(AFX_TRANSACTION_H__9D8D1BB9_E308_455A_8E61_7E2FC66FD131__INCLUDED_)
#define AFX_TRANSACTION_H__9D8D1BB9_E308_455A_8E61_7E2FC66FD131__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Object.h>
#include <Root/Sequence.h>

namespace Root
{
	class Agent;
	class Command;

	//* Transaction
	//. Eine Transaction kapselt eine Menge von Aktionen und somit einen
	//. inkrementellen Zustandswechsel. Damit ist sie in der Lage, diesen
	//. mittels Undo wieder rückgängig zu machen.
	//. Eine Transaktion (bzw. TransactionRef) kann von einem abstrakten
	//. Modell als Ergebnis einer modifizierenden Methode an den Aufrufer
	//. zurückgegeben werden. Dieser kann dann die Transaktion direkt
	//. an den Undo-Manager weiterleiten oder mehrere Transaktionen in einer
	//. MakroTransaktion zusammenfassen.

	class Transaction : public Resource 
	{
	public:
		Transaction();
		void registerForUndo( Agent* );

		virtual bool hasRedo() const;
		virtual bool hasUndo() const;

		virtual void unexecute(); // throw(Exception);
		virtual void reexecute(); // throw(Exception);

		virtual const char* getPrettyName() const;
	protected:
		virtual ~Transaction();
	};

	// CallSpec:
	// Kann den Methoden eines Modells als Argument mitgegeben werden, um 
	// zu steuern, ob die Methode mit Undo rückgängig gemacht werden kann und
	// ob unmittelbar mit dem Call ein Notify stattfinden soll. Das resultierende
	// Transaction-Objekt muss dann noch explizit an den UndoManager gesendet werden.
	struct CallOptions // Value
	{
	public:
		bool d_notify;		// Benachrichtige Observer
		bool d_undoable;	// Kapsle die Änderung in einer Transaktion -> d_done
		Ref<Transaction> d_done;

		bool isUd() const { return d_undoable; }
		void exec( Command* );
		CallOptions( bool n = true, bool u = false ):d_undoable( u ), d_notify( n ) {}
	};
	typedef CallOptions COP;

	class RegisterForUndo : public InternalMessage
	{
	public:
		RegisterForUndo( Transaction* cmd ):d_command( cmd ) {}
		Transaction* getTransaction() const { return d_command; }
	private:
		Ref<Transaction> d_command;
	};

	class KillUndo : public InternalMessage
	{
	};

	class MakroTransaction : public Transaction
	{
	public:
		void add( Transaction* );

		//* Overrides
		bool hasRedo() const;
		bool hasUndo() const;
		const char* getPrettyName() const;
		void unexecute();
		void reexecute();
		//-

		MakroTransaction(const char* name = "");
	protected:
		virtual ~MakroTransaction();
	private:
		typedef UniqueSeq< Ref<Transaction> > Transactions;
		Transactions d_content;
		QByteArray  d_name;
	};
}

#endif // !defined(AFX_TRANSACTION_H__9D8D1BB9_E308_455A_8E61_7E2FC66FD131__INCLUDED_)
