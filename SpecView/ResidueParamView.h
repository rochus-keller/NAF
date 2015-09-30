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

#if !defined(AFX_RESIDUEPARAMVIEW_H__6AAA538F_CC8D_422C_AE2A_72E396649A5E__INCLUDED_)
#define AFX_RESIDUEPARAMVIEW_H__6AAA538F_CC8D_422C_AE2A_72E396649A5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SpecView/SpecView.h>
#include <Spec/PeakList.h>
#include <Spec/Residue.h>
#include <QColor>

namespace Spec
{
	class ResidueParamView : public SpecView 
	{
	public:
		void show( bool );
		void setResidue( Residue*, bool autoOn = true );
		Residue* getResidue() const { return d_resi; }
		void setType( ResidueType*, bool autoOn = true );
		ResidueType* getType() const { return d_type; }
		bool show() const { return d_show; }
		ResidueParamView(SpecViewer*, QColor = Qt::darkYellow);

		//* Override
		void draw( Canvas&, const Lexi::Allocation& );
	protected:
		virtual ~ResidueParamView();
	private:
		Root::Ref<Residue> d_resi;
		Root::Ref<ResidueType> d_type;
		QColor d_clr;
		bool d_show;
	};
}

#endif // !defined(AFX_RESIDUEPARAMVIEW_H__6AAA538F_CC8D_422C_AE2A_72E396649A5E__INCLUDED_)
