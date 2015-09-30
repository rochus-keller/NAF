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

#if !defined(AFX_CENTERLINE_H__59E0EA05_86A2_4A68_AAB4_4FE99CA3E64C__INCLUDED_)
#define AFX_CENTERLINE_H__59E0EA05_86A2_4A68_AAB4_4FE99CA3E64C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SpecView/SpecView.h>
#include <QColor>

namespace Spec
{
	class CenterLine : public SpecView
	{
	public:
		CenterLine(SpecViewer*, Dimension = DimY, QColor = Qt::darkGray);
		//* Overrides von Glyph
		void draw( Canvas&, const Allocation& );
	protected:
		virtual ~CenterLine();
	private:
		Dimension d_dim;
		QColor d_color;
	};
}

#endif // !defined(AFX_CENTERLINE_H__59E0EA05_86A2_4A68_AAB4_4FE99CA3E64C__INCLUDED_)
