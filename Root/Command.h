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

#if !defined(AFX_COMMAND_H__3F9E0D07_EEED_44D3_9617_A4AF864E633C__INCLUDED_)
#define AFX_COMMAND_H__3F9E0D07_EEED_44D3_9617_A4AF864E633C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Root/Transaction.h>

namespace Root
{
	//* Command
	//. Ein Command ist im wesentlichen eine Transaktion, d.h. eine zusammengehörende
	//. Menge von Aktionen, welche als Ganzes ausgeführt oder verworfen werden.
	//. Im allgemeinen entsteht ein Command in einem Controller aufgrund eines Stimulus
	//. (z.B. Input-Events oder Menü-Actions). Es wird vom Controller angelegt, ausgeführt
	//. und anschliessend einem Undo-Manager zur Aufbewahrung übergeben.

	class Command : public Transaction 
	{
	public:
		enum State { Unknown, Initialized, Feedback, Executed, Unexecuted };

		virtual bool hasRedo() const;
		virtual bool hasUndo() const;

		virtual State getState() const;
		void setState( State );
		virtual const char* getPrettyName() const;

		virtual void unexecute(); // throw(Exception);
		virtual void execute(); // throw(Exception);
		virtual void reexecute(); // throw(Exception);

		static bool handle( Agent*, Command*, bool undo );
		bool handle( Agent* );
		bool handleNoUndo( Agent* );

		bool isFeedback() const { return d_state == Feedback; }
		bool isExecuted() const { return d_state == Executed; }
		bool isUnexecuted() const { return d_state == Unexecuted; }
		bool isInitialized() const { return d_state == Initialized; }
	protected:
		Command(State = Initialized);
		virtual ~Command();
	private:
		State d_state;
	};
}

#endif // !defined(AFX_COMMAND_H__3F9E0D07_EEED_44D3_9617_A4AF864E633C__INCLUDED_)
