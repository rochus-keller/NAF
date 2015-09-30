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

#pragma once
typedef struct lua_State lua_State;

class Enums
{
private:
lua_State * L;

public:
	Enums(lua_State * X);
	~Enums(void);
	int ClipOperation(int position);
	int CompositionMode(int position);
	int LayoutDirection(int position);
	int PenStyle(int position);
	int RenderHint(int position);
	int FormattingOption(int position);
	int ColorMode(int position);
	int ParsingMode(int position);
	int Orientation(int position);
	int UnderlineStyle(int position);
	int VerticalAlignment(int position);
	int OutputFormat(int position);
	int PageOrder(int position);
	int PageSize(int position);
	int PaperSource(int position);
	int PrintRange(int position);
	int Alignment(int position);
	int ItemFlags(int position);
	int CheckState(int position);
	int CaseSensitivity(int position);
	int PageBreakFlags(int position);
	int Flags(int position);
	int wrapMode(int position);
	int Corner (int position);
	int TextElideMode(int position);
	int TabPosition(int position);
	int DialogLabel(int position);
	int TabShape(int position);
	int Style(int position);
	int MoveOperation(int position);
	int MoveMode(int position);
	int SelectionType(int position);
	int Position(int position);
	int FindFlags(int position);
	int MetaInformation(int position);
	int DateFormat(int position);
	

};
