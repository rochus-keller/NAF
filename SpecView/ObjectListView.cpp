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

#include "ObjectListView.h"
#include <q3multilineedit.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <QHeaderView>
using namespace Root;
using namespace Spec;

ObjectListView::ObjectListView(QWidget* par, Spec::ObjectDef* od, const ObjectList& o ):
	QTableWidget( par ), d_od( od )
{
	assert( od );
	setAlternatingRowColors( true );
	setSelectionMode( QAbstractItemView::NoSelection );
    horizontalHeader()->setMovable(true);
    verticalHeader()->setMovable(true);
	verticalHeader()->setResizeMode ( QHeaderView::Interactive );
	setEditTriggers( QAbstractItemView::AllEditTriggers );
	horizontalHeader()->setResizeMode ( QHeaderView::Interactive );
    horizontalHeader()->setStretchLastSection(true);

	connect( horizontalHeader(), SIGNAL( sectionClicked ( int ) ),
		this, SLOT( columnClicked ( int ) ) );

	refill( o );
	setItemDelegate( DynValueEditor::createDelegate( this ) );
}

ObjectListView::~ObjectListView()
{
	setCurrentIndex( QModelIndex() ); 
}

bool ObjectListView::edit(QWidget * parent, Spec::ObjectDef * def, const ObjectList& objs)
{
	QDialog dlg( parent, "", true );
	QString str;
	str.sprintf( "Edit Attributes of %s Objects", def->getClass().data() );
	dlg.setCaption( str );


	Q3VBoxLayout* top = new Q3VBoxLayout( &dlg, 5 );
	Q3HBoxLayout* buttons = new Q3HBoxLayout();

	ObjectListView* tv = new ObjectListView( &dlg, def, objs );
	top->addWidget( tv );

	QPushButton* ok = new QPushButton( "&Close", &dlg );
	QObject::connect( ok, SIGNAL( clicked() ), &dlg, SLOT( accept() ) );
	buttons->addWidget( ok );
	ok->setDefault( true );

	top->addLayout( buttons );

	dlg.resize( 600, 400 ); // RISK
	dlg.exec();
	ok->setFocus();
	return true;
}

typedef std::set< std::pair<Root::Any, Object* > > Sorter;

static void fillSorted( QTableWidget* tab, ObjectDef* od, const 
					   Sorter& data, Root::SymbolString of )
{
	typedef std::map< QByteArray ,Root::Any::Type > Sort;
	Sort sort;
	ObjectDef::Attributes::const_iterator p1;
	for( p1 = od->getAttributes().begin(); p1 != od->getAttributes().end(); ++p1 )
		sort[ (*p1).first.data() ] = (*p1).second.d_type;

	tab->setColumnCount( sort.size() );
	tab->setRowCount( data.size() );
	QString str;
	Sort::const_iterator p2;
	int i = 0, j = 0;
	QStringList labels;
	for( p2 = sort.begin(); p2 != sort.end(); ++p2, ++j )
	{
		str = (*p2).first.data();
		if( (*p2).first == of.data() )
			str = str + " (sorted)";
		labels << str;
	}
	tab->setHorizontalHeaderLabels( labels );
	QFontMetrics fm = tab->verticalHeader()->fontMetrics();
	int w = 0, tmp;
	Sorter::const_iterator p3;
	labels.clear();
	for( p3 = data.begin(); p3 != data.end(); ++p3, ++i )
	{
		j = 0;
		str = (*p3).second->getInstanceName().data();
		tmp = fm.width( str );
		if( tmp > w )
			w = tmp;
		labels << str;
		for( p2 = sort.begin(); p2 != sort.end(); ++p2, ++j )
		{
			tab->setItem( i, j, new DynValueCell( (*p3).second, (*p2).first.data(), (*p2).second ) );
		}
	}
	tab->setVerticalHeaderLabels( labels );
	// tab->setLeftMargin( 10 + w + w * 0.3 );
}

void ObjectListView::refill(const ObjectList & o)
{
	Sorter data;
	for( int i = 0; i < o.size(); i++ )
	{
		if( o[ i ] )
			data.insert( std::make_pair( 
				Root::Any( o[ i ]->getInstanceName( true ).data() ), 
			o[ i ] ) );
	}
	fillSorted( this, d_od, data, "" );
}

void ObjectListView::columnClicked(int col)
{
	setCurrentIndex( QModelIndex() ); 
	Root::Object::Field f = horizontalHeaderItem( col )->text().toLatin1();

	Root::Any a;
	Sorter data;
	DynValueCell* cell;
	for( int i = 0; i < rowCount(); i++ )
	{
		cell = (DynValueCell*) item( i, 0 );
		if( !cell->d_obj->getFieldValue( f, a ) )
			a.setNull();
		data.insert( std::make_pair( a, cell->d_obj.deref() ) );
	}
	fillSorted( this, d_od, data, f );
}
