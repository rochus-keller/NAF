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

#if !defined(AFX_MAINWINDOW_H__45DE2C03_0C12_4F8E_A105_769B0E6D459D__INCLUDED_)
#define AFX_MAINWINDOW_H__45DE2C03_0C12_4F8E_A105_769B0E6D459D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Lexi2/LxUnits.h>
#include <QMainWindow>
#include <Root/Messenger.h>
#include <Gui/Reactor.h>
#include <Gui/Menu.h>
#include <Lexi2/LxCommandLine.h>

namespace Lexi2
{
	class MainWindow : public QMainWindow
	{
	public:
		MainWindow(bool showStatus = true);
		virtual ~MainWindow();
		bool event(QEvent *e);
	protected:
		CommandLine* getCl() const { return d_cl; }
		void setAgent( Root::Resource* a ) { d_agent = a; }
	private:
		CommandLine* d_cl;
		Root::Ref<Root::Resource> d_agent;
	};
}

#endif // !defined(AFX_MAINWINDOW_H__45DE2C03_0C12_4F8E_A105_769B0E6D459D__INCLUDED_)
