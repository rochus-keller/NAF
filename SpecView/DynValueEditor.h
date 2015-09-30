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

#if !defined(AFX_DYNVALUEEDITOR_H__7345B3F8_AA8E_4103_A76E_4F99602BD613__INCLUDED_)
#define AFX_DYNVALUEEDITOR_H__7345B3F8_AA8E_4103_A76E_4F99602BD613__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Object.h>
#include <Spec/ObjectDef.h>
#include <QEvent>
#include <QTableWidget>
#include <QTableWidgetItem>

class QAbstractItemDelegate;

namespace Spec
{
	class DynValueCell : public QTableWidgetItem
	{
	public:
		enum { TypeRole = Qt::UserRole + 1, ReadOnlyRole };
		Root::SymbolString d_att;
		Root::ExRef<Root::Object> d_obj;

		virtual void setData ( int role, const QVariant & value );
		virtual QVariant data ( int role ) const;

		DynValueCell( Root::Object* o, Root::SymbolString a, Root::Any::Type t );
		~DynValueCell();
	};

	class DynValueEditor : public QTableWidget
	{
	public:
		static bool edit(QWidget * parent, Spec::ObjectDef * def, Root::Object * obj);
		DynValueEditor(QWidget*, Spec::ObjectDef*,Root::Object*);
		virtual ~DynValueEditor();
		static QAbstractItemDelegate* createDelegate(QObject*);
	private:
		Root::ExRef<Root::Object> d_obj;
	};
}

#endif // !defined(AFX_DYNVALUEEDITOR_H__7345B3F8_AA8E_4103_A76E_4F99602BD613__INCLUDED_)
