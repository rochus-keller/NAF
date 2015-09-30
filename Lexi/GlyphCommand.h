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
#include <Lexi/Glyph.h>
#include <Lexi/Units.h>
#include <Lexi/Event.h>

namespace Lexi
{
	using Root::Command;

	//* GlyphCommand
	//. Dies ist ein Command, das innerhalb der Glyph-Hierarchie entsteht und
	//. normalerweise über visuelles Feedback verfügt. Während der Feedback-Ausführung 
	//. verhält sich das Command wie ein Controller.

	class GlyphCommand : public Command
	{
	public:
		GlyphCommand(Glyph* controller = nil);

		enum Result { 
			Rejected,	// Die Message wurde vom Command nicht angenommen
			Continue,	// Die Message wurde akzeptiert und man wartet auf weitere
			Commit,		// Die Message führte zu einem execute().
			Rollback	// Die Message führte zu einem Abbruch.
		};

		Glyph* getController() const { return d_controller; }
		//* Alibi von Glyph
		virtual void draw( Canvas&, const Allocation&);
		virtual Result handle( const Allocation&, GlyphTraversal&);
	protected:
		virtual ~GlyphCommand();

		void startFeedback();

		//* To Override
		virtual Result handleMousePress( Viewport&, const Allocation&, const MouseEvent& );
		virtual Result handleMouseRelease( Viewport&, const Allocation&, const MouseEvent& ); 
		virtual Result handleDoubleClick( Viewport&, const Allocation&, const MouseEvent& );
		virtual Result handleMouseMove( Viewport&, const Allocation&, const MouseEvent& );
		virtual Result handleMouseDrag( Viewport&, const Allocation&, const DragEvent& );
		virtual Result handleKeyPress( Viewport&, const Allocation&, const KeyEvent& );
		virtual Result handleKeyRelease( Viewport&, const Allocation&, const KeyEvent& );
	private:
		Root::Ref<Glyph> d_controller;	// Hier Ref erlaubt, da Commander Command freigibt.
	};
}

#endif // !defined(AFX_GLYPHCOMMAND_H__95A496CC_F4FC_43F6_B6E8_B820A41A9F56__INCLUDED_)
