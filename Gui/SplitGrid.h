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

#if !defined(AFX_SPLITGRID_H__6D6FAFC9_AEDF_4711_981D_17077C29FC33__INCLUDED_)
#define AFX_SPLITGRID_H__6D6FAFC9_AEDF_4711_981D_17077C29FC33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <QSplitter>
#include <QVector>

class QSplitter;

namespace Gui
{
	class SplitGrid : public QSplitter 
	{
		Q_OBJECT
	public:
		typedef QVector<QWidget*> Rows;

		void setRowSizes( const QList<int>& );
		QList<int> getRowSizes() const;
		void setColSizes( const QList<int>& );
		QList<int> getColSizes() const;
		QVariant getSizesCoded() const;
		void setSizesCoded( const QVariant& );
		QWidget* widget( int row, int col ) const;
		void addColumn( const Rows& );
		int colCount() const { return count(); }
		void setColStretchFactor( int col, int factor );
		void setRowStretchFactor( int row, int factor );

		SplitGrid(QWidget*);
		virtual ~SplitGrid();
	private:
		bool d_block;
	protected slots:
		void rowMoved ( int pos, int index );
	};
}
#endif // !defined(AFX_SPLITGRID_H__6D6FAFC9_AEDF_4711_981D_17077C29FC33__INCLUDED_)
