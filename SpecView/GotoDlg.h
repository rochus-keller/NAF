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

#if !defined(AFX_GOTODLG_H__2BC01C41_C3BE_11D4_B823_00D00918E99C__INCLUDED_)
#define AFX_GOTODLG_H__2BC01C41_C3BE_11D4_B823_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <qdialog.h>
#include <Root/Vector.h>
#include <Spec/Scale.h>

class QLineEdit;

namespace Spec
{
	class GotoDlg : public QDialog
	{
	public:
		PPM getValue( Dimension ) const;
		void addDimension( const PpmRange&, AtomType, PPM );
		void setDelta( PPM d ) { d_delta = d; }
		PPM getDelta() const { return d_delta; }
		bool exec( bool delta = false );
		GotoDlg(QWidget * parent );
		virtual ~GotoDlg();
	private:
		Root::Vector<QLineEdit*> d_edits;
		struct Dim
		{
			PpmRange d_range;
			AtomType d_color;
			PPM d_value;
		};
		Root::Vector<Dim> d_dims;
		PPM d_delta;
	};
}

#endif // !defined(AFX_GOTODLG_H__2BC01C41_C3BE_11D4_B823_00D00918E99C__INCLUDED_)
