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

#include "SplitGrid2.h"
#include <QVariant>
#include <QStackedWidget>
#include <QtDebug>
using namespace Gui2;

static const int s_hw = 4;

class _Column : public QSplitter
{
public:
	_Column( QWidget* p ):
	  QSplitter( p ) { }
        friend class Gui2::SplitGrid2;
};

SplitGrid2::SplitGrid2(QWidget* parent, int rowCount, int colCount):
    QSplitter( parent ), d_block( false )
{
	setOrientation( Qt::Horizontal );
	setHandleWidth( s_hw );
	setOpaqueResize( false );

    for( int col = 0; col < colCount; col++ )
        addColumn( rowCount );
}

SplitGrid2::~SplitGrid2()
{

}

void SplitGrid2::addColumn(int rowCount)
{
    _Column* newColumn = new _Column( this );
    newColumn->setOrientation( Qt::Vertical );
    newColumn->setHandleWidth( s_hw );
    newColumn->setOpaqueResize( false );
    for( int i = 0; i < rowCount; i++ )
	{
        newColumn->addWidget( new QStackedWidget( this ) );
	}
    if( count() > 0 )
	{
        _Column* firstColumn = static_cast<_Column*>( QSplitter::widget( 0 ) );
        newColumn->setSizes( firstColumn->sizes() );
	}
    addWidget( newColumn );
    connect( newColumn, SIGNAL( splitterMoved ( int, int ) ), this, SLOT( rowMoved( int, int ) ) );
}

void SplitGrid2::rowMoved(int pos, int index)
{
	if( d_block )
		return;
	QObject* s = sender();
	_Column* rows;
	d_block = true;
    for( int i = 0; i < count(); i++ )
	{
		rows = static_cast<_Column*>( QSplitter::widget( i ) );
		if( rows == s )
			continue;
		if( index < rows->count() )
			rows->moveSplitter( pos, index );

	}
	d_block = false;
}

QStackedWidget* SplitGrid2::getCellImp(int row, int col) const
{
    if( col < 0 || col >= count() )
		return 0;
	QSplitter* s = static_cast<QSplitter*>( QSplitter::widget( col ) );
	if( row < 0 || row >= s->count() )
		return 0;
	else
        return static_cast<QStackedWidget*>( s->widget( row ) );
}

void SplitGrid2::setCell(QWidget * w, int row, int col)
{
    Q_ASSERT( w != 0 );

    QStackedWidget* cell = getCellImp( row, col );
    Q_ASSERT( cell != 0 );
    if( cell->count() > 0 )
        cell->removeWidget( cell->widget(0) );
    cell->addWidget( w );
}

void SplitGrid2::setColSizes(const QList<int>& s)
{
	setSizes( s );
}


QList<int> SplitGrid2::getColSizes() const
{
	return sizes();
}

QVariant SplitGrid2::getSizesCoded() const
{
	QByteArray ba;
	QDataStream s( &ba, QIODevice::WriteOnly );
	s << getColSizes() << getRowSizes();
	return ba;
}

void SplitGrid2::setSizesCoded( const QVariant& v )
{
	QDataStream s( v.toByteArray() );
	QList<int> l;
	s >> l;
	setColSizes( l );
	s >> l;
    setRowSizes( l );
}

QWidget *SplitGrid2::getCell(int row, int col) const
{
    QStackedWidget* cell = getCellImp( row, col );
    Q_ASSERT( cell != 0 );
    return cell->currentWidget();
}

void SplitGrid2::setRowSizes(const QList<int>& s )
{
	for( int i = 0; i < count(); i++ )
	{
		_Column* col = static_cast<_Column*>( QSplitter::widget( i ) );
		col->setSizes( s );
	}
}

QList<int> SplitGrid2::getRowSizes() const
{
	if( count() == 0 )
		return QList<int>();

	_Column* col = static_cast<_Column*>( QSplitter::widget( 0 ) );
	return col->sizes();
}

void SplitGrid2::setColStretchFactors( const QList<int>& f )
{
    Q_ASSERT( f.size() == count() );

    for( int col = 0; col < count(); col++ )
    {
        QWidget* w = widget(col);
        QSizePolicy policy = w->sizePolicy();
        policy.setHorizontalStretch(f[col]);
        w->setSizePolicy(policy);
    }
}

void SplitGrid2::setRowStretchFactors( const QList<int>& f )
{
    for( int col = 0; col < count(); col++ )
    {
        _Column* rows = static_cast<_Column*>( QSplitter::widget( col ) );
        Q_ASSERT( f.size() == rows->count() );
        for( int row = 0; row < rows->count(); row++ )
        {
            rows->setStretchFactor( row, f[row] );
//            QWidget* w = rows->widget( row );
//            QSizePolicy policy = w->sizePolicy();
//            policy.setVerticalStretch(f[row]);
//            w->setSizePolicy(policy);
        }
    }
}
