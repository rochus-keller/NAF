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

#if !defined(SplitGrid2__INCLUDED_)
#define SplitGrid2__INCLUDED_

#include <QSplitter>
#include <QVector>

class QSplitter;
class QStackedWidget;

namespace Gui2
{
    /*
        Horizontaler Splitter als Spalten enthält n vertikale Splitter als Zeilen.
    */
    class SplitGrid2 : public QSplitter
	{
		Q_OBJECT
	public:
		void setRowSizes( const QList<int>& );
		QList<int> getRowSizes() const;
		void setColSizes( const QList<int>& );
		QList<int> getColSizes() const;
		QVariant getSizesCoded() const;
		void setSizesCoded( const QVariant& );
        QWidget* getCell( int row, int col ) const;
        void setCell( QWidget *w, int row, int col );
        void setColStretchFactors( const QList<int>& );
        void setRowStretchFactors( const QList<int>& );

        SplitGrid2(QWidget* parent, int rowCount, int colCount );
		virtual ~SplitGrid2();
	private:
		bool d_block;
        void addColumn( int rowCount );
        QStackedWidget* getCellImp( int row, int col ) const;
    protected slots:
		void rowMoved ( int pos, int index );
	};
}
#endif // !defined(SplitGrid2__INCLUDED_)
