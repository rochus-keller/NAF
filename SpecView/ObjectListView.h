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

#if !defined(AFX_OBJECTLISTVIEW_H__E64783C0_B8B2_4509_A144_DFD3FA0EDAC9__INCLUDED_)
#define AFX_OBJECTLISTVIEW_H__E64783C0_B8B2_4509_A144_DFD3FA0EDAC9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SpecView/DynValueEditor.h>
#include <Root/Vector.h>

namespace Spec
{
	class ObjectListView : public QTableWidget
	{
		Q_OBJECT
	public:
		typedef Root::Vector<Root::Object*> ObjectList;

		static bool edit( QWidget*, Spec::ObjectDef*, const ObjectList& );

		Spec::ObjectDef* getOd() const { return d_od; }
		ObjectListView(QWidget*, Spec::ObjectDef*, const ObjectList& );
	protected slots:
		void columnClicked ( int logicalIndex );
	protected:
		virtual ~ObjectListView();
		void refill( const ObjectList& );
	private:
		Root::ExRef<Spec::ObjectDef> d_od;
	};
}

#endif // !defined(AFX_OBJECTLISTVIEW_H__E64783C0_B8B2_4509_A144_DFD3FA0EDAC9__INCLUDED_)
