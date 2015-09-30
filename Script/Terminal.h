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

#if !defined(AFX_LUATERMINAL_H__E595EEF8_F8CE_454A_B661_7A2451EC2CF4__INCLUDED_)
#define AFX_LUATERMINAL_H__E595EEF8_F8CE_454A_B661_7A2451EC2CF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <QTextEdit>
#include <QTextCursor>
#include <Script/Engine.h>
#include <Root/ActionHandler.h>


namespace Lua
{
    class Terminal : public QTextEdit, Root::Messenger
	{
	public:
		void paste();
		void clear();
		Terminal(QWidget*, Lua::Engine*);
		virtual ~Terminal();
	private:
		Lua::Engine* d_lua;
		QTextCursor d_out;
		QString d_line;
		QStringList d_histo;
		QStringList d_next;
	protected:
		void handle( Root::Message& );
		void keyPressEvent ( QKeyEvent * e );
        void inputMethodEvent(QInputMethodEvent *);
        GENERIC_MESSENGER(QTextEdit)
        QString prompt() const;
	private:
		FRIEND_ACTION_HANDLER( Terminal );
		void handlePaste( Root::Action& t );
		void handleCopy( Root::Action& t ); 
		void handleSelectAll( Root::Action& t ); 
		void handleDelete( Root::Action& t ); 
		void handleExportPdf( Root::Action& t ); 
		void handleSaveAs( Root::Action& t ); 
	};
}

#endif // !defined(AFX_LUATERMINAL_H__E595EEF8_F8CE_454A_B661_7A2451EC2CF4__INCLUDED_)
