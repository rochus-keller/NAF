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

#include "SpecBatchList.h"
#include <QTreeWidget>
#include <QLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QRadioButton> 
#include <QButtonGroup>
#include <QMessageBox> 
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <Root/Any.h>
#include <SpecView/DynValueEditor.h>
#include <SpecView/ObjectListView.h>
#include <Spec/Repository.h>
#include <Root/Vector.h>
using namespace Spec;

//////////////////////////////////////////////////////////////////////

Root::Action::CmdStr SpecBatchList::AddSpectrum = "AddSpectrum";
Root::Action::CmdStr SpecBatchList::RemoveSpectrum = "RemoveSpectrum";
Root::Action::CmdStr SpecBatchList::MoveUp = "MoveUp";
Root::Action::CmdStr SpecBatchList::MoveDown = "MoveDown";
Root::Action::CmdStr SpecBatchList::EditFields = "EditFields";
Root::Action::CmdStr SpecBatchList::AddAllAlpha = "AddAllAlpha";
Root::Action::CmdStr SpecBatchList::RemoveAll = "RemoveAll";
Root::Action::CmdStr SpecBatchList::ShowTable = "ShowTable";

ACTION_SLOTS_BEGIN( SpecBatchList )
	{ SpecBatchList::ShowTable, &SpecBatchList::handleShowTable },
	{ SpecBatchList::AddAllAlpha, &SpecBatchList::handleAddAllAlpha },
	{ SpecBatchList::RemoveAll, &SpecBatchList::handleRemoveAll },
	{ SpecBatchList::EditFields, &SpecBatchList::handleEditFields },
	{ SpecBatchList::RemoveSpectrum, &SpecBatchList::handleRemoveSpectrum },
	{ SpecBatchList::MoveUp, &SpecBatchList::handleMoveUp },
	{ SpecBatchList::MoveDown, &SpecBatchList::handleMoveDown },
	{ SpecBatchList::AddSpectrum, &SpecBatchList::handleAddSpectrum },
ACTION_SLOTS_END( SpecBatchList )

//////////////////////////////////////////////////////////////////////
class _SpecBatchListItem : public QTreeWidgetItem
{
public:
	_SpecBatchListItem( QTreeWidget* p, Spectrum* spec ):
		QTreeWidgetItem(p),d_spec( spec ) { init(); }
	_SpecBatchListItem( QTreeWidget* p, QTreeWidgetItem* after, Spectrum* spec ):
		QTreeWidgetItem(p, after),d_spec( spec ) { init(); }

	Root::ExRef<Spectrum> d_spec;

	void init() 
	{ 
		setText( 0, d_spec->getName() );
		setText( 1, d_spec->getFilePath() );
	}
};
//////////////////////////////////////////////////////////////////////

SpecBatchList::SpecBatchList(QWidget * parent, PeakList* pl, Project* pro ):	
	QDialog( parent ), d_pl( pl )
{
	assert( pl );
	assert( pro );
	setWindowTitle( "Spectrum Batch" );
	resize( 300, 500 );

	d_master = pro->getSpec( pl->getHome() );
	d_pro = pro;
	Spectrum* spec;

	typedef std::map<QByteArray ,Spectrum*> Order;
	Order order;
	const Project::SpectrumMap& st = pro->getSpectra();
	Project::SpectrumMap::const_iterator p1;
	for( p1 = st.begin(); p1 != st.end(); ++p1 )
	{
		if( d_master && (*p1).second->getType() == d_master->getType() )
			order[ (*p1).second->getName() ] = (*p1).second;
		else if( d_master == 0 && pl->getDimCount() == (*p1).second->getDimCount() )
			order[ (*p1).second->getName() ] = (*p1).second;
	}

	d_list = new QTreeWidget( this );
	d_list->setAllColumnsShowFocus( true );
	d_list->setHeaderLabels( QStringList() << "Name" << "Path" );

	PeakList::SpecList::const_iterator p2;
	_SpecBatchListItem* t = 0;
	for( p2 = pl->getSpecs().begin(); p2 != pl->getSpecs().end(); ++p2 )
	{
		spec = pro->getSpec( (*p2) );
		if( spec )
			t = new _SpecBatchListItem( d_list, t, spec );
	}

	Gui::Menu* pop = new Gui::Menu( d_list, true );
	Gui::Menu* pop2 = new Gui::Menu( this, "Add Spectrum" );
	pop->addMenu( pop2 );
	Order::const_iterator p3;
	for( p3 = order.begin(); p3 != order.end(); ++p3 )
	{
		Gui::Menu::item( pop2, this, 
			(*p3).second->getName(), AddSpectrum, false )->addParam( (*p3).second );
	}
	Gui::Menu::item( pop, this, "Add All Spectra", AddAllAlpha, false );
	Gui::Menu::item( pop, this, "Remove Spectrum", RemoveSpectrum, false );
	Gui::Menu::item( pop, this, "Remove All Spectra", RemoveAll, false );
	pop->insertSeparator();
	Gui::Menu::item( pop, this, "Move Up", MoveUp, false );
	Gui::Menu::item( pop, this, "Move Down", MoveDown, false );
	pop->insertSeparator();
	Gui::Menu::item( pop, this, "Edit Attributes...", EditFields, false );
	Gui::Menu::item( pop, this, "Open Object Table...", ShowTable, false );
}

SpecBatchList::~SpecBatchList()
{

}

bool SpecBatchList::doit()
{
	QVBoxLayout* top = new QVBoxLayout(this);
	top->addWidget( d_list ); 

	QDialogButtonBox* box = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel, 
		Qt::Horizontal, this );
	connect(box, SIGNAL(accepted()), this, SLOT(accept()));
	connect(box, SIGNAL(rejected()), this, SLOT(reject())); 
	top->addWidget( box );

	if( exec() == QDialog::Accepted )
	{
		PeakList::SpecList l( d_list->topLevelItemCount() );
		for( int i = 0; i < l.size(); i++ )
		{
			_SpecBatchListItem* t = (_SpecBatchListItem*) d_list->topLevelItem( i );
			l[ i ] = t->d_spec->getId();
		}
		d_pl->setSpecs( l );
		return true;
	}else
		return false;
}

void SpecBatchList::handle(Root::Message & msg)
{
	BEGIN_HANDLER();
	MESSAGE( Root::Action, a, msg )
	{
		EXECUTE_ACTION( SpecBatchList, *a );
	}
	END_HANDLER();
}

void SpecBatchList::handleAddSpectrum(Root::Action & a)
{
	ACTION_ENABLED_IF( a, true );

	Spectrum* s = dynamic_cast<Spectrum*>( a.getParam( 0 ).getObject() );
	assert( s );

	_SpecBatchListItem* i = (_SpecBatchListItem*) d_list->currentItem();
	_SpecBatchListItem* t = new _SpecBatchListItem( d_list, i, s );
	d_list->setCurrentItem( t, true );
}

void SpecBatchList::handleRemoveSpectrum(Root::Action & a)
{
	_SpecBatchListItem* i = (_SpecBatchListItem*) d_list->currentItem();
	ACTION_ENABLED_IF( a, i );

	delete i;
}

void SpecBatchList::handleMoveUp(Root::Action & a)
{
	_SpecBatchListItem* i = (_SpecBatchListItem*) d_list->currentItem();
	ACTION_ENABLED_IF( a, i && d_list->itemAbove( i ) );

	QTreeWidgetItem* t = d_list->itemAbove( i );
	t = d_list->itemAbove( t );
	_SpecBatchListItem* n = new _SpecBatchListItem( 
		d_list, t, i->d_spec );
	delete i;
	d_list->setCurrentItem( n, true );
}

void SpecBatchList::handleMoveDown(Root::Action & a)
{
	_SpecBatchListItem* i = (_SpecBatchListItem*) d_list->currentItem();
	ACTION_ENABLED_IF( a, i && d_list->itemBelow(i) );

	_SpecBatchListItem* n = new _SpecBatchListItem( 
		d_list, d_list->itemBelow(i), i->d_spec );
	delete i;
	d_list->setCurrentItem( n, true );
}

void SpecBatchList::handleEditFields(Root::Action & a)
{
	_SpecBatchListItem* i = (_SpecBatchListItem*) d_list->currentItem();
	ACTION_ENABLED_IF( a, i );

	DynValueEditor::edit( this, d_pro->getRepository()
		->findObjectDef( Repository::keySpectrum ),	i->d_spec );
}

void SpecBatchList::handleAddAllAlpha(Root::Action & a)
{
	ACTION_ENABLED_IF( a, true );

	typedef std::map<QByteArray ,Spectrum*> Order;
	Order order;
	const Project::SpectrumMap& st = d_pro->getSpectra();
	Project::SpectrumMap::const_iterator p1;
	for( p1 = st.begin(); p1 != st.end(); ++p1 )
	{
		if( d_master && (*p1).second->getType() == d_master->getType() )
			order[ (*p1).second->getName() ] = (*p1).second;
		else if( d_master == 0 && d_pl->getDimCount() == (*p1).second->getDimCount() )
			order[ (*p1).second->getName() ] = (*p1).second;
	}
	Order::const_iterator p3;
	_SpecBatchListItem* t = 0;
	for( p3 = order.begin(); p3 != order.end(); ++p3 )
	{
		t = new _SpecBatchListItem( d_list, t, (*p3).second );
	}
}

void SpecBatchList::handleRemoveAll(Root::Action & a)
{
	ACTION_ENABLED_IF( a, true );

	d_list->clear();
}

void SpecBatchList::handleShowTable(Root::Action & a)
{
	ACTION_ENABLED_IF( a, true );

    // iterate through all items of the listview
	ObjectListView::ObjectList o;
    for ( int i = 0; i < d_list->topLevelItemCount(); ++i ) 
	{
		_SpecBatchListItem* bi = (_SpecBatchListItem*)d_list->topLevelItem( i );;
		o.push_back( bi->d_spec );
    }
	ObjectListView::edit( this, d_pro->getRepository()->
		findObjectDef( Repository::keySpectrum ), o );
}



