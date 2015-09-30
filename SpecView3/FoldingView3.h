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

#if !defined(FoldingView3__INCLUDED_)
#define FoldingView3__INCLUDED_

#include <SpecView3/SpecView3.h>
#include <SpecView3/SpecBufferMdl3.h>
#include <QColor>

namespace Spec
{
    class FoldingView3  : public SpecView3
	{
	public:
		SpecBufferMdl3* getModel() const { return d_model; }
        FoldingView3( SpecBufferMdl3*, QColor = Qt::darkGray);
        //* Overrides
        void paint( QPainter& );
	protected:
		virtual ~FoldingView3();
	private:
		Root::Ref<SpecBufferMdl3> d_model;
		QColor d_clr;
	};
}

#endif // !defined(FoldingView3__INCLUDED_)
