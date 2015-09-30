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

#include "Action.h"
#include <Root/Any.h>
using namespace Root;

Action::CmdStr Action::Nop = "System.Nop";
Action::CmdStr Action::FileNew = "File.New";
Action::CmdStr Action::FileOpen = "File.Open";
Action::CmdStr Action::FileClose = "File.Close";
Action::CmdStr Action::FileSave = "File.Save";
Action::CmdStr Action::FileSaveAs = "File.SaveAs";
Action::CmdStr Action::FilePageSetup = "File.PageSetup";
Action::CmdStr Action::FilePrintPreview = "File.PrintPreview";
Action::CmdStr Action::FilePrint = "File.Print";
Action::CmdStr Action::FileExportPdf = "File.ExportPdf";
Action::CmdStr Action::FileQuit = "File.Quit";
Action::CmdStr Action::EditUndo = "Edit.Undo";
Action::CmdStr Action::EditRedo = "Edit.Redo";
Action::CmdStr Action::EditCut = "Edit.Cut";
Action::CmdStr Action::EditCopy = "Edit.Copy";
Action::CmdStr Action::EditPaste = "Edit.Paste";
Action::CmdStr Action::EditDelete = "Edit.Delete";
Action::CmdStr Action::EditSelectAll = "Edit.SelectAll";
Action::CmdStr Action::EditFind = "Edit.Find";
Action::CmdStr Action::EditFindAgain = "Edit.FindAgain";
Action::CmdStr Action::EditReplace = "Edit.Replace";
Action::CmdStr Action::WindowClose = "Window.Close";
Action::CmdStr Action::WindowCloseAll = "Window.CloseAll";
Action::CmdStr Action::WindowCloseAllButThis = "Window.CloseAllButThis";
Action::CmdStr Action::WindowCascade = "Window.Cascade";
Action::CmdStr Action::WindowNew = "Window.New";
Action::CmdStr Action::WindowShowStatusBar = "Window.ShowStatusBar";
Action::CmdStr Action::SystemKillUndo = "System.KillUndo";
Action::CmdStr Action::HelpContents = "Help.Contents";
Action::CmdStr Action::HelpSearch = "Help.Search";
Action::CmdStr Action::HelpContext = "Help.Context";
Action::CmdStr Action::HelpAbout = "Help.About";
Action::CmdStr Action::ExecuteLine = "Execute.Line";
Action::CmdStr Action::ExecuteCmd = "Execute.Cmd";
Action::CmdStr Action::ViewBackward = "View.Backward";
Action::CmdStr Action::ViewForward = "View.Forward";
Action::CmdStr Action::ViewRefresh = "View.Refresh";
Action::CmdStr Action::ViewAbort = "View.Abort";
Action::CmdStr Action::ViewFullScreen = "View.FullScreen";
Action::CmdStr Action::RecordUpdate = "Record.Update";
Action::CmdStr Action::RecordRestore = "Record.Restore";
Action::CmdStr Action::RecordCreate = "Record.Create";
Action::CmdStr Action::RecordCreateSub = "Record.CreateSub";
Action::CmdStr Action::RecordDelete = "Record.Delete";
Action::CmdStr Action::RecordNext = "Record.Next";
Action::CmdStr Action::RecordPrev = "Record.Prev";

const char* Action::prettyName( Action::CmdStr cmd )
{
	if( cmd == Action::FileNew )
		return "New";
	else if( cmd == Action::FileOpen )
		return "Open...";
	else if( cmd == Action::FileClose )
		return "Close";
	else if( cmd == Action::FileSave )
		return "Save";
	else if( cmd == Action::FileSaveAs )
		return "Save As...";
	else if( cmd == Action::FilePageSetup )
		return "Page Setup...";
	else if( cmd == Action::FilePrintPreview )
		return "Preview";
	else if( cmd == Action::FilePrint )
		return "Print...";
	else if( cmd == Action::FileExportPdf )
		return "Export PDF...";
	else if( cmd == Action::FileQuit )
		return "Quit";
	else if( cmd == Action::EditUndo )
		return "Undo";
	else if( cmd == Action::EditRedo )
		return "Redo";
	else if( cmd == Action::EditCut )
		return "Cut";
	else if( cmd == Action::EditCopy )
		return "Copy";
	else if( cmd == Action::EditPaste )
		return "Paste";
	else if( cmd == Action::EditDelete )
		return "Delete";
	else if( cmd == Action::EditSelectAll )
		return "Select All";
	else if( cmd == Action::EditFind )
		return "Find...";
	else if( cmd == Action::EditFindAgain )
		return "Find again";
	else if( cmd == Action::EditReplace )
		return "Replace...";
	else if( cmd == Action::WindowClose )
		return "Close";
	else if( cmd == Action::WindowCascade )
		return "Cascade";
	else if( cmd == Action::HelpContents )
		return "Contents";
	else if( cmd == Action::HelpSearch )
		return "Search...";
	else if( cmd == Action::HelpContext )
		return "Context Help";
	else if( cmd == Action::HelpAbout )
		return "About...";
	else if( cmd == Action::WindowNew )
		return "New Window";
	else if( cmd == Action::WindowShowStatusBar )
		return "Show Statusbar";
	return "";
}

Action::Action()
{

}

Action::~Action()
{

}

bool Action::isEnabled() const
{
	return false;
}

void Action::execute()
{

}

const char* Action::getPrettyName() const
{
	return "";
}

Action::CmdSym Action::getCommand() const
{
	return "";
}

bool Action::isOn() const
{
	return false;
}

void Action::setOn(bool)
{

}

void Action::setEnabled(bool)
{

}

bool Action::isFlipFlop() const
{
	return false;
}

void Action::update()
{
}

bool Action::isUpdate() const
{
	return false;
}

void StateAction::setEnabled(bool on)
{
	d_enabled = on;
}

void StateAction::setOn(bool on)
{
	d_on = on;
}

StateAction::StateAction(const char * name, CmdStr cmd, bool flipflop):
	d_name( name ), d_command( cmd ), d_flipflop( flipflop ), d_on( false ), d_enabled( false )
{

}

StateAction::StateAction(CmdStr cmd):
	d_command( cmd ), d_name( prettyName( cmd ) ),
	d_flipflop( false ), d_on( false ), d_enabled( false )
{

}

bool Action::operator ==(CmdStr) const
{
	return false;
}

bool StateAction::operator ==(CmdStr cmd) const
{
	return d_command == cmd;
}

Index Action::getParamCount() const
{
	return 0;
}

const Any& Action::getParam(Index) const
{
	return Any::null;
}

bool Action::checkAndRun()
{
	update();
	if( isEnabled() )
	{
		execute();
		return true;
	}else
		return false;
}
