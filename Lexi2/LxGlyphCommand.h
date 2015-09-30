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

#if !defined(AFX_GLYPHCOMMAND_H__95A496CC_F4FC_43F6_B6E8_B820A41A9F56__INCLUDED_)
#define AFX_GLYPHCOMMAND_H__95A496CC_F4FC_43F6_B6E8_B820A41A9F56__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Root/Command.h>
#include <Lexi2/LxGlyph.h>
#include <Lexi2/LxUnits.h>
#include <Lexi2/LxEvent.h>

namespace Lexi2
{
	//* GlyphCommand
	//. Dies ist ein Command, das innerhalb der Glyph-Hierarchie entsteht und
	//. normalerweise über visuelles Feedback verfügt. Während der Feedback-Ausführung 
	//. verhält sich das Command wie ein Controller.

	class GlyphCommand : public Root::Command
	{
	public:
		GlyphCommand(Glyph* handler = nil);

		enum Result { 
			Rejected,	// Die Message wurde vom Command nicht angenommen
			Continue,	// Die Message wurde akzeptiert und man wartet auf weitere
			Commit,		// Die Message führte zu einem execute().
			Rollback	// Die Message führte zu einem Abbruch.
		};

		//* Alibi von Glyph
		virtual void draw(Canvas&, const Allocation&);
		virtual Result handleEvent( Root::Message&);
	protected:
		virtual ~GlyphCommand();

		Glyph* getHandler() const { return d_handler; }
		void startFeedback();

		//* To Override
		virtual Result handleMousePress( MouseEvent& );
		virtual Result handleMouseRelease(MouseEvent& ); 
		virtual Result handleDoubleClick(MouseEvent& );
		virtual Result handleMouseMove(MouseEvent& );
		virtual Result handleMouseDrag(DragEvent& );
		virtual Result handleKeyPress(KeyEvent& );
		virtual Result handleKeyRelease(KeyEvent& );
	private:
		Root::Ref<Glyph> d_handler;	// Hier Ref erlaubt, da Commander Command freigibt.
	};
}

#endif // !defined(AFX_GLYPHCOMMAND_H__95A496CC_F4FC_43F6_B6E8_B820A41A9F56__INCLUDED_)
