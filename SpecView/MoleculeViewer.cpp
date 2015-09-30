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

#include "MoleculeViewer.h"
#include <Spec/Repository.h>
#include <qpainter.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qlineedit.h> 
#include <qlayout.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qfontmetrics.h> 
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3GridLayout>
#include <Q3Frame>
#include <QMouseEvent>
#include <Q3VBoxLayout>
using namespace Spec;

static int g_cellWidth = 40;

//////////////////////////////////////////////////////////////////////

Root::Action::CmdStr MoleculeViewer::NewAtom = "NewAtom";
Root::Action::CmdStr MoleculeViewer::DeleteAtom = "DeleteAtom";
Root::Action::CmdStr MoleculeViewer::ToggleBond = "ToggleBond";
Root::Action::CmdStr MoleculeViewer::MoveAllVerti = "MoveAllVerti";
Root::Action::CmdStr MoleculeViewer::MoveAllHori = "MoveAllHori";
Root::Action::CmdStr MoleculeViewer::MoveTo = "MoveTo";
Root::Action::CmdStr MoleculeViewer::ClearSel = "ClearSel";
Root::Action::CmdStr MoleculeViewer::Select = "Select";
Root::Action::CmdStr MoleculeViewer::SetGroup = "SetGroup";

ACTION_SLOTS_BEGIN( MoleculeViewer )
	{ MoleculeViewer::SetGroup, &MoleculeViewer::handleSetGroup },
	{ MoleculeViewer::Select, &MoleculeViewer::handleSelect },
	{ MoleculeViewer::ClearSel, &MoleculeViewer::handleClearSel },
	{ MoleculeViewer::MoveTo, &MoleculeViewer::handleMoveTo },
	{ MoleculeViewer::MoveAllHori, &MoleculeViewer::handleMoveAllHori },
	{ MoleculeViewer::MoveAllVerti, &MoleculeViewer::handleMoveAllVerti },
	{ MoleculeViewer::ToggleBond, &MoleculeViewer::handleToggleBond },
	{ MoleculeViewer::DeleteAtom, &MoleculeViewer::handleDeleteAtom },
	{ MoleculeViewer::NewAtom, &MoleculeViewer::handleNewAtom },
ACTION_SLOTS_END( MoleculeViewer )

//////////////////////////////////////////////////////////////////////

MoleculeViewer::MoleculeViewer(QWidget* parent, ResidueType* rt, bool rw ):
	QtTableView( parent ), 
	d_rt( rt ), d_main( 0 ), d_rw( rw )
{
	QString str;
    if( rt )
    {
        str.sprintf( "MoleculeViewer - Residue Type %s", rt->getShort().data() );
        setAttribute(Qt::WA_DeleteOnClose);
    }else
        str.sprintf( "MoleculeViewer" );
	setCaption( str );

	g_cellWidth = fontMetrics().width( "HHHHH" );


	d_selected = false;
	d_dragging = false;

	d_kontext = new Gui::Menu( this );
	Gui::Menu::item( d_kontext, this, "&New Atom...", NewAtom, false );
	Gui::Menu::item( d_kontext, this, "&Delete Atom...", DeleteAtom, false );
	Gui::Menu::item( d_kontext, this, "&Toggle Bond", ToggleBond, false );
	Gui::Menu::item( d_kontext, this, "&Move Atom", MoveTo, false );
	Gui::Menu::item( d_kontext, this, "&Set Group...", SetGroup, false );
	d_kontext->insertSeparator();
	Gui::Menu::item( d_kontext, this, "Move All &Vertical...", MoveAllVerti, false );
	Gui::Menu::item( d_kontext, this, "Move All &Horizontal...", MoveAllHori, false );
	d_kontext->insertSeparator();
	Gui::Menu::item( d_kontext, this, "Select Spins...", Select, false );
	Gui::Menu::item( d_kontext, this, "Reset Selection", ClearSel, false );

	setFocusPolicy( Qt::StrongFocus );
	setKeyCompression( false ); // Wir wollen ein Event fr jeden Key incl. Autorepeat.
	setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, 
		QSizePolicy::MinimumExpanding ) );
	setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
	setLineWidth( 1 );
	setNumRows( 50 );
	setNumCols( 50 );
	setCellWidth( g_cellWidth );
	setCellHeight( g_cellWidth );
	setTableFlags( Tbl_autoScrollBars | Tbl_snapToGrid ); 
	updateTableSize();

	loadAll();
	if( !d_rw )
	{
		int dx = 0, dy = 0;
		ObjList::const_iterator i;
		for( i = d_atoms.begin(); i != d_atoms.end(); ++ i )
		{
			if( (*i).first.first > dy )
				dy = (*i).first.first;
			if( (*i).first.second > dx )
				dx = (*i).first.second;
		}
		setNumRows( dy + 1 );
		setNumCols( dx + 1 );
		resize( ( dx + 2 ) * g_cellWidth, ( dy + 2 ) * g_cellWidth ); // RISK
	}
    if( d_rt )
    {
        d_rt->getOwner()->getOwner()->addObserver( this );
        resize( 640, 480 );
    }
}

MoleculeViewer::~MoleculeViewer()
{
    if( d_rt )
        d_rt->getOwner()->getOwner()->removeObserver( this );
}

void MoleculeViewer::paintCell(class QPainter *p, int row, int col)
{
	/*QFont f = p->font();
	f.setBold( true );
	p->setFont( f );*/
	Cell c = std::make_pair( row, col );
	const int r3 = g_cellWidth / 3;
	const int r2 = g_cellWidth / 2;
    //const int r4 = g_cellWidth / 4;
	p->setBrush( Qt::NoBrush );
	if( d_selected && row == d_current.first && col == d_current.second )
		p->drawRect( 0, 0, g_cellWidth, g_cellWidth );
	if( isOccupied( c ) )
	{
		Atom* a = d_atoms[ c ];
		if( a )
		{
			p->setPen( Qt::darkGray );
			Atom::Neighbours::const_iterator i;
			for( i = a->getNeighbours().begin(); i != a->getNeighbours().end(); ++i )
			{
				Cell to = toCell( (*i) );
				if( to.first == -1 || to.second == -1 )
					break;
				to.first = to.first - row + 1;
				to.second = to.second - col + 1;
				p->drawLine( r2, r2, to.second * r2, to.first * r2 );
			}
			if( a == d_main )
				p->setBrush( Qt::red );
			else if( !d_sel.empty() && d_sel.count( a ) != 0 )
				p->setBrush( Qt::yellow );
			else
				p->setBrush( Qt::white );
			p->drawEllipse( r3 / 2, r3 / 2, 2 * r3, 2 * r3 ); 
			p->setPen( Qt::black );
			p->drawText( -1, -2, g_cellWidth, g_cellWidth, Qt::AlignHCenter |
				Qt::AlignVCenter, a->getName().data() );
			if( a->getGroup() )
			{
				p->setPen( Qt::blue );
				p->drawText( -1, -2, g_cellWidth, g_cellWidth, Qt::AlignHCenter |
					Qt::AlignBottom, a->getGroup()->getName().data() );
				p->setPen( Qt::black );
			}
		}
	}
}

void MoleculeViewer::select(Atom * a, AtomType to, Root::Index hops, bool rep)
{
	d_sel.clear();
	d_main = a;
	ResidueType::Selection sel;
	if( d_main && hops >= 0 )
		sel = d_rt->find( d_main->getName(), to, hops, rep );
	else
		sel = d_rt->findAll( to );
	ResidueType::Selection::iterator i;
	for( i = sel.begin(); i != sel.end(); ++i )
		d_sel.insert( d_rt->getAtom( (*i).getTag() ) );
	update();
}

void MoleculeViewer::select( const SpinLabel& start, const SpinLabelSet& targets )
{
	d_sel.clear();
	d_main = 0;
	ResidueType::Selection sel;
	if( !start.isNull() && start.getOffset() == 0 )
	{
		d_main = d_rt->getAtom( start.getTag() );
		if( d_main )
		{
			SpinLabelSet::const_iterator pos;
			Atom* a;
			for( pos = targets.begin(); pos != targets.end(); ++pos )
			{
				if( (*pos).getOffset() == 0 )
				{
					a = d_rt->getAtom( (*pos).getTag() );
					if( a )
						d_sel.insert( a );
				}
			}
		}
	}
    update();
}

void MoleculeViewer::setResidueType(ResidueType * rt)
{
    if( d_rt )
        d_rt->getOwner()->getOwner()->removeObserver( this );
    d_rt = rt;
    if( d_rt )
        d_rt->getOwner()->getOwner()->addObserver( this );
    loadAll();
    int dx = 0, dy = 0;
    ObjList::const_iterator i;
    for( i = d_atoms.begin(); i != d_atoms.end(); ++ i )
    {
        if( (*i).first.first > dy )
            dy = (*i).first.first;
        if( (*i).first.second > dx )
            dx = (*i).first.second;
    }
    setNumRows( dy + 1 );
    setNumCols( dx + 1 );
    resize( ( dx + 3 ) * g_cellWidth, ( dy + 3 ) * g_cellWidth ); // RISK
}

void MoleculeViewer::mousePressEvent(QMouseEvent * e)
{
	d_start = toCell( e );
	if( e->button() == Qt::RightButton )
	{
		d_kontext->exec( e->globalPos() );
	}else if( d_rw && e->state() && Qt::ControlButton )
	{
		// Bond-Operation
		handleBond();
	}else
	{
		// Select-Operation
		d_current = toCell( e );
		d_selected = d_current.first >= 0 && d_current.second >= 0;
		if( d_rw )
			d_dragging = isOccupied( d_current );
		update();
	}
}

void MoleculeViewer::handle(Root::Message & msg)
{
	BEGIN_HANDLER();
	MESSAGE( Atom::Removed, a, msg )
	{
		if( a->sender()->getOwner() == d_rt )
		{
			// Hier wird nur auf Lschung geprft. Rest wird lokal direkt
			// aktualisiert. Es ist Sache des Users, pro Resi nicht mehr
			// als eine Box offenzuhaben.
			ObjList::iterator p;
			for( p = d_atoms.begin(); p != d_atoms.end(); ++p )
				if( (*p).second == a->sender() )
				{
					d_atoms.erase( p );
					break;
				}
			update();
		}
	}
	MESSAGE( Root::Action, a, msg )
	{
		EXECUTE_ACTION( MoleculeViewer, *a );
	}
	END_HANDLER();
}

void MoleculeViewer::mouseReleaseEvent(QMouseEvent * e)
{
	if( d_dragging )
	{
		d_dragging = false;
		d_current = toCell( e );
		if( isOccupied( d_current ) )
		{
			d_current = d_start;
			update();
			return;
		}
		Atom* a = d_atoms[ d_start ];
		d_rt->setPos( a, d_current.second, d_current.first );
		d_atoms.erase( d_start );
		d_atoms[ d_current ] = a;
		update();
	}
}

void MoleculeViewer::mouseMoveEvent(QMouseEvent *e)
{
	if( d_dragging )
	{
		d_current = toCell( e );
		update();
	}
}

static void _distribute( ResidueType* rt )
{
	bool doit = true;
	ResidueType::AtomMap::const_iterator i;
	for( i = rt->getAtoms().begin(); i != rt->getAtoms().end(); ++i )
	{
		if( (*i).second->getX() || (*i).second->getY() )
			doit = false;
	}
	Root::UInt8 row = 0, col = 0;
	if( doit )
	{
		for( i = rt->getAtoms().begin(); i != rt->getAtoms().end(); ++i )
		{
			rt->setPos( (*i).second, col, row );
			if( row < 15 )
				row++;
			else
			{
				row = 0;
				col++;
			}
		}
	}
}

void MoleculeViewer::loadAll()
{
	d_atoms.clear();
    if( d_rt.isNull() )
        return;
	_distribute( d_rt );
	ResidueType::AtomMap::const_iterator i;
	for( i = d_rt->getAtoms().begin(); i != d_rt->getAtoms().end(); ++i )
	{
		Cell c( (*i).second->getY(), (*i).second->getX() );
		d_atoms[ c ] = (*i).second;
	}
}

MoleculeViewer::Cell MoleculeViewer::toCell(QMouseEvent * e)
{
	Cell c;
	c.first = findRow( e->y() );
	c.second = findCol( e->x() );
	return c;
}

bool MoleculeViewer::isOccupied( const Cell& c )
{
	ObjList::iterator pos = d_atoms.find( c );
	return ( pos != d_atoms.end() );
}

MoleculeViewer::Cell MoleculeViewer::toCell(Atom* a)
{
	ObjList::iterator pos;
	for( pos = d_atoms.begin(); pos != d_atoms.end(); ++pos )
	{
		if( (*pos).second == a )
			return (*pos).first;
	}
	return Cell(-1,-1);
}

void MoleculeViewer::handleBond()
{
	// Bond-Operation
	if( isOccupied( d_current ) && isOccupied( d_start ) )
	{
		Atom* from = d_atoms[ d_current ];
		Atom* to = d_atoms[ d_start ];
		if( from == to )
			return;
		if( from->getNeighbours().count( to ) == 0 )
		{
			d_rt->link( from, to );
		}else
		{
			d_rt->unlink( from, to );
		}
		update();
	}
}

void MoleculeViewer::handleNewAtom(Root::Action & a)
{
	ACTION_ENABLED_IF( a, d_rw && !isOccupied( d_current ) );

	QDialog dlg( this, "", true );
	dlg.setCaption( "New Atom" );

	Q3VBoxLayout* top = new Q3VBoxLayout(&dlg, 10);
	Q3HBoxLayout* buttons = new Q3HBoxLayout();
	Q3GridLayout* contents = new Q3GridLayout( 7, 2 );

	contents->addWidget( new QLabel( "Symbol:", &dlg ), 1, 0 ); 
	QLineEdit* sym = new QLineEdit( &dlg );
	contents->addWidget( sym, 1, 1 );

	contents->addWidget( new QLabel( "Magnitude:", &dlg ), 2, 0 ); 
	QLineEdit* mag = new QLineEdit( &dlg );
	mag->setText( "1" );
	contents->addWidget( mag, 2, 1 );

	contents->addWidget( new QLabel( "Type:", &dlg ), 3, 0 ); 
	QComboBox* option = new QComboBox( &dlg );
	for( int i = AtomType::H1; i < AtomType::MaxIsotope; i++ )
		option->insertItem( AtomType::s_labels[ i ] );
	contents->addWidget( option, 3, 1 );
	option->setCurrentItem( 0 );

	contents->addWidget( new QLabel( "Group:", &dlg ), 4, 0 ); 
	QLineEdit* group = new QLineEdit( &dlg );
	contents->addWidget( group, 4, 1 );

	contents->addWidget( new QLabel( "Mean:", &dlg ), 5, 0 ); 
	QLineEdit* mean = new QLineEdit( &dlg );
	contents->addWidget( mean, 5, 1 );

	contents->addWidget( new QLabel( "Deviation:", &dlg ), 6, 0 ); 
	QLineEdit* dev = new QLineEdit( &dlg );
	contents->addWidget( dev, 6, 1 );

	QPushButton* ok = new QPushButton( "&OK", &dlg );
	QObject::connect( ok, SIGNAL( clicked() ), &dlg, SLOT( accept() ) );
	buttons->addWidget( ok );
	ok->setDefault( true );

	QPushButton* cancel = new QPushButton( "&Cancel", &dlg );
	QObject::connect( cancel, SIGNAL( clicked() ), &dlg, SLOT( reject() ) );
	buttons->addWidget( cancel );

	top->addLayout( contents );
	top->addLayout( buttons );

	bool doit;
	int num;
	DisPar dp;
	while( dlg.exec() == QDialog::Accepted )
	{
		num = mag->text().toUShort( &doit );
		if( !mean->text().isEmpty() )
		{
			dp.d_mean = mean->text().toFloat( &doit );
			if( !doit )
			{
				QMessageBox::critical( this, "New Atom",
						"Invalid mean value!", "&Cancel" );
				continue;
			}
		}else
			dp.d_mean = DisPar::Undefined;
		if( !dev->text().isEmpty() )
		{
			dp.d_dev = dev->text().toFloat( &doit );
			if( !doit || dp.d_dev < 0.0 )
			{
				QMessageBox::critical( this, "New Atom",
						"Invalid deviation value!", "&Cancel" );
				continue;
			}
		}else
			dp.d_dev = 0.0;

		if( sym->text().isEmpty() )
		{
			QMessageBox::critical( this, "New Atom",
					"Symbol cannot be empty!", "&Cancel" );
		}else if( d_rt->getAtom( sym->text().toLatin1() ) != 0 )
		{
			QMessageBox::critical( this, "New Atom",
					"Symbol already in use!", "&Cancel" );
		}else if( !doit || num < 1 )
		{
			QMessageBox::critical( this, "New Atom",
					"Invalid magnitude!", "&Cancel" );
		}else
		{
			AtomGroup* g = 0;
			if( !group->text().isEmpty() && group->text() != "-" )
			{
				g = d_rt->getGroup( group->text().toLatin1() );
				if( g == 0 )
					g = d_rt->addGroup( group->text().toLatin1() );
			}
			Atom* at = d_rt->addAtom( sym->text().toLatin1(),
				AtomType::Isotope( option->currentItem() + 1 ), num, g );
			if( at )
			{
				d_atoms[ d_current ] = at;
				d_rt->setDisPar( at, dp );
				d_rt->setPos( at, d_current.second, d_current.first );
				return;
			}
		}
	}
}

void MoleculeViewer::handleDeleteAtom(Root::Action & a)
{
	ACTION_ENABLED_IF( a, d_rw && isOccupied( d_current ) );

	Atom* atom = d_atoms[ d_current ];
	d_rt->removeAtom( atom->getName() );
	loadAll();
}

void MoleculeViewer::handleToggleBond(Root::Action & a)
{
	ACTION_ENABLED_IF( a, d_rw && isOccupied( d_current ) && isOccupied( d_start ) );
	handleBond();
}

void MoleculeViewer::handleMoveAllVerti(Root::Action & a)
{
	ACTION_ENABLED_IF( a, d_rw );

	bool ok;
	int res = QInputDialog::getInteger( "Move All Vertical", 
		"Please enter a valid offset:", 
		0, -255, +255, 1, &ok, this );
	if( !ok )
		return;
	ResidueType::AtomMap::const_iterator i;
	for( i = d_rt->getAtoms().begin(); i != d_rt->getAtoms().end(); ++i )
	{
		if( ( int( (*i).second->getY() ) + res ) < 0 || 
			( int( (*i).second->getY() ) + res ) > 255 )
			ok = false;
	}
	if( !ok )
	{
		QMessageBox::critical( this, "Move All Vertical",
				"At least one atom would leave valid area!", "&Cancel" );
	}
	for( i = d_rt->getAtoms().begin(); i != d_rt->getAtoms().end(); ++i )
	{
		d_rt->setPos( (*i).second, (*i).second->getX(),
			Root::UInt8( int( (*i).second->getY() ) + res ) );
	}
	loadAll();
}

void MoleculeViewer::handleMoveAllHori(Root::Action & a)
{
	ACTION_ENABLED_IF( a, d_rw );

	bool ok;
	int res = QInputDialog::getInteger( "Move All Horizontal", 
		"Please enter a valid offset:", 
		0, -255, +255, 1, &ok, this );
	if( !ok )
		return;
	ResidueType::AtomMap::const_iterator i;
	for( i = d_rt->getAtoms().begin(); i != d_rt->getAtoms().end(); ++i )
	{
		if( ( int( (*i).second->getX() ) + res ) < 0 || 
			( int( (*i).second->getX() ) + res ) > 255 )
			ok = false;
	}
	if( !ok )
	{
		QMessageBox::critical( this, "Move All Horizontal",
				"At least one atom would leave valid area!", "&Cancel" );
	}
	for( i = d_rt->getAtoms().begin(); i != d_rt->getAtoms().end(); ++i )
	{
		d_rt->setPos( (*i).second, 
			Root::UInt8( int( (*i).second->getX() ) + res ), (*i).second->getY() );
	}
	loadAll();
}

void MoleculeViewer::handleMoveTo(Root::Action & a)
{
	// d_current: Selektierter Ort vor Rechtsklick
	// d_start: Ort von Rechtsklick
	ACTION_ENABLED_IF( a, d_rw && d_selected && isOccupied( d_current ) && !isOccupied( d_start ) );

	Atom* atom = d_atoms[ d_current ];
	d_rt->setPos( atom, d_start.second, d_start.first );
	d_atoms[ d_start ] = atom;
	d_atoms.erase( d_current );
	update();
}

void MoleculeViewer::handleSelect(Root::Action & a)
{
	ACTION_ENABLED_IF( a, isOccupied( d_current ) );

	QDialog dlg( this, "", true );
	dlg.setCaption( "Select Spins" );

	Q3VBoxLayout* top = new Q3VBoxLayout(&dlg, 10);
	Q3HBoxLayout* buttons = new Q3HBoxLayout();
	Q3GridLayout* contents = new Q3GridLayout( 4, 2 );

	contents->addWidget( new QLabel( "Hops:", &dlg ), 1, 0 ); 
	QLineEdit* _hops = new QLineEdit( &dlg );
	_hops->setText( "1" );
	contents->addWidget( _hops, 1, 1 );

	contents->addWidget( new QLabel( "Repeat:", &dlg ), 2, 0 ); 
	QCheckBox* _rep = new QCheckBox( &dlg );
	contents->addWidget( _rep, 2, 1 );

	contents->addWidget( new QLabel( "To:", &dlg ), 3, 0 ); 
	QComboBox* _to = new QComboBox( &dlg );
	for( int i = AtomType::None; i <= AtomType::Lr; i++ )
		_to->insertItem( AtomType::s_labels[ i ] );
	contents->addWidget( _to, 3, 1 );
	_to->setCurrentItem( 0 );

	QPushButton* ok = new QPushButton( "&OK", &dlg );
	QObject::connect( ok, SIGNAL( clicked() ), &dlg, SLOT( accept() ) );
	buttons->addWidget( ok );
	ok->setDefault( true );

	QPushButton* cancel = new QPushButton( "&Cancel", &dlg );
	QObject::connect( cancel, SIGNAL( clicked() ), &dlg, SLOT( reject() ) );
	buttons->addWidget( cancel );

	top->addLayout( contents );
	top->addLayout( buttons );

	bool doit = false;
	int hops;
	bool rep;
	AtomType to;
	if( dlg.exec() == QDialog::Accepted )
	{
		hops = _hops->text().toUShort( &doit );
		rep = _rep->isChecked();
		to = AtomType::Isotope( _to->currentItem() );
		if( !doit )
		{
			QMessageBox::critical( this, "Select Spin",
					"Invalid hops count!", "&Cancel" );
			return;
		}
		d_sel.clear();
		d_main = d_atoms[ d_current ];
		ResidueType::Selection sel = d_rt->find( d_main->getName(),
			to, hops, rep );
		ResidueType::Selection::iterator i;
		for( i = sel.begin(); i != sel.end(); ++i )
			d_sel.insert( d_rt->getAtom( (*i).getTag() ) );
		update();
	}

}

void MoleculeViewer::handleClearSel(Root::Action & a)
{
	ACTION_ENABLED_IF( a, !d_sel.empty() );

	d_sel.clear();
	d_main = 0;
	update();
}

void MoleculeViewer::handleSetGroup(Root::Action & a)
{
	ACTION_ENABLED_IF( a, d_rw && isOccupied( d_current ) );

	Atom* atom = d_atoms[ d_current ];
	bool ok	= FALSE;
	QString res = "-";
	if( atom->getGroup() )
		res = atom->getGroup()->getName().data();
	res = QInputDialog::getText( "Set Group", "Enter group label:", QLineEdit::Normal, 
			res, &ok, this );
	if( !ok )
		return;
	if( res.isEmpty() || res == "-" )
		d_rt->setGroup( atom, 0 );
	else
	{
		AtomGroup* g = d_rt->getGroup( res.toLatin1() );
		if( g == 0 )
			g = d_rt->addGroup( res.toLatin1() );
		d_rt->setGroup( atom, g );
	}
	loadAll();
}



