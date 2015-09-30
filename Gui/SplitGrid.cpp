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

#include "SplitGrid.h"
#include <cassert>
#include <QVariant>
using namespace Gui;

static const int s_hw = 4;

class _Column : public QSplitter
{
public:
	_Column( QWidget* p ):
	  QSplitter( p ) { }
        friend class Gui::SplitGrid;
};

SplitGrid::SplitGrid(QWidget* p):
	QSplitter( p ), d_block( false )
{
	setOrientation( Qt::Horizontal );
	setHandleWidth( s_hw );
	setOpaqueResize( false );

	/*
		Horizontaler Splitter als Spalten enthält n vertikale Splitter als Zeilen.
	*/
}

SplitGrid::~SplitGrid()
{

}

void SplitGrid::addColumn(const Rows & rows)
{
	_Column* s = new _Column( this );
	s->setOrientation( Qt::Vertical );
	s->setHandleWidth( s_hw );
	s->setOpaqueResize( false );
	for( int i = 0; i < rows.size(); i++ )
	{
		assert( rows[i] );
		s->addWidget( rows[i] );
	}
	if( colCount() > 0 )
	{
		_Column* first = static_cast<_Column*>( QSplitter::widget( 0 ) );
		s->setSizes( first->sizes() );
	}
	addWidget( s );
	connect( s, SIGNAL( splitterMoved ( int, int ) ), this, SLOT( rowMoved( int, int ) ) );
}

void SplitGrid::rowMoved(int pos, int index)
{
	if( d_block )
		return;
	QObject* s = sender();
	_Column* rows;
	d_block = true;
	for( int i = 0; i < colCount(); i++ )
	{
		rows = static_cast<_Column*>( QSplitter::widget( i ) );
		if( rows == s )
			continue;
		if( index < rows->count() )
			rows->moveSplitter( pos, index );

	}
	d_block = false;
}

QWidget* SplitGrid::widget(int row, int col) const
{
	if( col < 0 || col >= colCount() )
		return 0;
	QSplitter* s = static_cast<QSplitter*>( QSplitter::widget( col ) );
	if( row < 0 || row >= s->count() )
		return 0;
	else
		return s->widget( row );
}

void SplitGrid::setColSizes(const QList<int>& s)
{
	setSizes( s );
}


QList<int> SplitGrid::getColSizes() const
{
	return sizes();
}

QVariant SplitGrid::getSizesCoded() const
{
	QByteArray ba;
	QDataStream s( &ba, QIODevice::WriteOnly );
	s << getColSizes() << getRowSizes();
	return ba;
}

void SplitGrid::setSizesCoded( const QVariant& v )
{
	QDataStream s( v.toByteArray() );
	QList<int> l;
	s >> l;
	setColSizes( l );
	s >> l;
	setRowSizes( l );
}

void SplitGrid::setRowSizes(const QList<int>& s )
{
	for( int i = 0; i < count(); i++ )
	{
		_Column* col = static_cast<_Column*>( QSplitter::widget( i ) );
		col->setSizes( s );
	}
}

QList<int> SplitGrid::getRowSizes() const
{
	if( count() == 0 )
		return QList<int>();

	_Column* col = static_cast<_Column*>( QSplitter::widget( 0 ) );
	return col->sizes();
}

void SplitGrid::setColStretchFactor( int col, int factor )
{
	assert( col < colCount() );
	setStretchFactor( col, factor );
}

void SplitGrid::setRowStretchFactor( int row, int factor )
{
	for( int i = 0; i < colCount(); i++ )
	{
		_Column* rows = static_cast<_Column*>( QSplitter::widget( i ) );
			rows->setStretchFactor( row, factor );
	}
}
