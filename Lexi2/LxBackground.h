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

#if !defined(AFX_BACKGROUND_H__04C2F278_08A5_11D4_AB64_00C04FAD063B__INCLUDED_)
#define AFX_BACKGROUND_H__04C2F278_08A5_11D4_AB64_00C04FAD063B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Lexi2/LxMonoGlyph.h>

namespace Lexi2
{
	//* Background
	//. Diese Klasse zeichnet einfach den Hintergrund in der gewünschten Farbe,
	//. wobei einfach immer die gesamte Allocation gefärbt wird.

	class Background : public MonoGlyph 
	{
	public:
		Background( Glyph* body = 0, QColor = Qt::black , bool all = false );
		//. wenn all wird immer ganze Damage-Area gezeichnet, egal wie gross Allocation.
		virtual ~Background();

		void setOpaque( bool on ) { d_opaque = on; }

		//* Overrides von Glyph
		virtual void draw(Canvas&, const Allocation&);
	private:
		QColor d_color;
		bool d_all;
		bool d_opaque;
	};
}

#endif // !defined(AFX_BACKGROUND_H__04C2F278_08A5_11D4_AB64_00C04FAD063B__INCLUDED_)
