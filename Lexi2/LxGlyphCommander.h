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

#if !defined(AFX_GLYPHCOMMANDER_H__AF3E0D0A_7ADE_4D56_84DE_97B57856FAA1__INCLUDED_)
#define AFX_GLYPHCOMMANDER_H__AF3E0D0A_7ADE_4D56_84DE_97B57856FAA1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi2/LxHandler.h>
#include <Lexi2/LxGlyphCommand.h>
#include <Lexi2/LxUndoContext.h>

namespace Lexi2
{
	class GlyphCommander : public Lexi2::Handler
	{
	public:
		GlyphCommander(UndoContext* = nil, Glyph* body = nil);
		virtual ~GlyphCommander();

		bool installCommand( GlyphCommand*, Root::Message* = 0 );
		//. Diese Methode wird in den abgeleiteten Klassen im Sinne einer Factory aufgerufen.
		//. Dem neuen Command wird d_alloc und d_trav präsentiert falls forward
		void registerForUndo( Root::Command* ) const;
		//. Diese Methode sendet das übergebene Command an den Undo-Manager, falls es
		//. undoable ist.

		//* Overrides von Glyph
		void draw(Canvas&, const Allocation&);
		bool allocation( Allocation& ) const;
		void handle( Root::Message & msg );
		void allocate( const Allocation& a ) { d_alloc = a; }
	private:
		Root::Ref<GlyphCommand> d_command;
		Root::Ref<UndoContext> d_undo;
		Allocation d_alloc;
	protected:
		bool feedCommand(Root::Message & msg);
		GlyphCommand* getCommand() const { return d_command; }
	};
}

#endif // !defined(AFX_GLYPHCOMMANDER_H__AF3E0D0A_7ADE_4D56_84DE_97B57856FAA1__INCLUDED_)
