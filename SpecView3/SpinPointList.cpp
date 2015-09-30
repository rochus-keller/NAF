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

#include "SpinPointList.h"
#include "SpinPointView3.h"
#include <Gui2/AutoMenu.h>
#include <SpecView/PathSimDlg.h>
using namespace Spec;

static const char*  c_higher[] = {
    "14 15 2 1",
    " 	c None",
    ".	c #000000",
    "              ",
    "    .         ",
    "    ..        ",
    "    . .       ",
    "    .  .      ",
    "    .   .     ",
    "    .    .    ",
    "    .     .   ",
    "    .         ",
    "    .         ",
    "    .         ",
    "    .         ",
    "    .         ",
    "    .         ",
    "              "};

static const char*  c_lower[] = {
    "14 15 2 1",
    " 	c None",
    ".	c #000000",
    "              ",
    "    .         ",
    "    .         ",
    "    .         ",
    "    .         ",
    "    .         ",
    "    .         ",
    "    .     .   ",
    "    .    .    ",
    "    .   .     ",
    "    .  .      ",
    "    . .       ",
    "    ..        ",
    "    .         ",
    "              "};

static const char*  c_ghost[] = {
	"14 15 2 1",
	" 	c None",
	".	c #000000",
	"    .         ",
	"    ..        ",
	"    . .       ",
	"    .  .      ",
	"    .   .     ",
	"    .    .    ",
	"    .         ",
	"    .         ",
	"    .         ",
	"    .    .    ",
	"    .   .     ",
	"    .  .      ",
	"    . .       ",
	"    ..        ",
	"    .         "};

static const char*  c_link[] = {
    "16 15 2 1",
    " 	c None",
    ".	c #000000",
    "                ",
    "                ",
    "                ",
    "                ",
    "                ",
    "  ...      ...  ",
    " .   .    .   . ",
    " .   ......   . ",
    " .	  .    .   . ",
    "  ...      ...  ",
    "                ",
    "                ",
    "                ",
    "                ",
    "                "};

struct _SpinPointListItem : public Gui::ListViewItem
{
    SpinSpace::Element d_elem;

	_SpinPointListItem(  SpinPointList * parent, const SpinSpace::Element& e ):
        Gui::ListViewItem(parent), d_elem(e) {}
    SpinPointList* getList() const { return static_cast<SpinPointList*>( listView() ); }
    QString text( int f ) const
    {
        if( d_elem.d_point[f] )
        {
			return SpinPointView3::formatLabel(d_elem.d_point,getList()->getLabelType(), f );
//            return QString( "%1 (%2)" ).arg( d_elem.d_point[f]->getId(), 4, 10, QChar('0') ).
//                    arg( d_elem.d_point[f]->getShift(), 0, 'f', 1 );
        }else
            return QChar('?');
    }
//    QString key( int f, bool ) const
//    {
//        if( d_elem.d_point[f] )
//        {
//            return QString( "%1" ).arg( d_elem.d_point[f]->getId(), 8, 10, QChar('0') );
//        }else
//            return QChar('?');
//    }
	QString tooltip( int f ) const
	{
		Q_UNUSED(f);
		return formatToolTip( d_elem );
	}
	QVariant foreground( int col ) const
    {
        Q_UNUSED(col)
        if( d_elem.isHidden() )
            return Qt::lightGray;
        else if( d_elem.isGhost() )
            return Qt::gray;
        else if( d_elem.isLink() )
            return Qt::cyan;
        else
            return Qt::black;
    }
    QVariant pixmap( int col ) const
    {
        if( col != 0 )
            return QVariant();
        if( d_elem.isHigher() )
            return QPixmap(c_higher);
        else if( d_elem.isLower() )
            return QPixmap(c_lower);
		else if( d_elem.isGhost() )
			return QPixmap(c_ghost);
        else if( d_elem.isLink() )
            return QPixmap(c_link);
        else
            return QVariant();
    }
	static QString formatToolTip(const SpinSpace::Element& e)
	{
		QString res;
		QTextStream out(&res, QIODevice::WriteOnly );
		out << "Spins: ";
		for( Dimension d = 0; d < e.d_point.maxSize(); d++ )
		{
			if( e.d_point[d] == 0 )
				break;
			if( d > 0 )
				out << " / ";
			out << e.d_point[d]->getId();
		}
		out << endl;
		out << "PPM: ";
		for( Dimension d = 0; d < e.d_point.maxSize(); d++ )
		{
			if( e.d_point[d] == 0 )
				break;
			if( d > 0 )
				out << " / ";
			out << e.d_point[d]->getShift();
		}
		out << endl;
		out << "Systems: ";
		for( Dimension d = 0; d < e.d_point.maxSize(); d++ )
		{
			if( e.d_point[d] == 0 )
				break;
			if( d > 0 )
				out << " / ";
			if( e.d_point[d]->getSystem() )
				out << e.d_point[d]->getSystem()->getId();
			else
				out << "-";
		}
		out << endl;
		if( e.isInfer() )
			out << "Pathsim ";
		else
			out << "Cross-product ";
		if( e.isLower() )
			out << "Lower ";
		else if( e.isHigher() )
			out << "Higher ";
		else if( e.isGhost() )
			out << "Ghost ";
		if( e.isHidden() )
			out << "Hidden";
		out << endl;
		if( e.isLink() )
		{
			out << "Links: ";
			for( int d1 = 0; d1 < e.d_point.maxSize(); d1++ )
			{
				for( int d2 = 0; d2 < e.d_point.maxSize(); d2++ )
				{
					if( e.isLink( d1, d2 ) )
						out << QString("%1-%2 ").arg( d1+1 ).arg( d2+1 );
				}
			}
		}
		return res;
	}
};

SpinPointList::SpinPointList(QWidget *parent, bool createMenu) :
	Gui::ListView(parent),d_showGhosts(true)
{
	d_labelType = SpinPointView3::SysOrResiTagAll;
	setRootIsDecorated( false );
    setShowSortIndicator( true );
    connect( this, SIGNAL(doubleClicked(Gui::ListViewItem*)), this, SLOT(onDblClck(Gui::ListViewItem*)) );
    connect(this,SIGNAL(currentChanged()), this, SLOT(onCurrentChanged()) );

	if( createMenu )
	{
		Gui2::AutoMenu* pop = new Gui2::AutoMenu( this, true );
		pop->addCommand( tr("Goto"), this, SLOT(handleGoto()));
		pop->addCommand( tr("Show Path Simulation..."), this, SLOT(handleShowPathSim() ) );
		Gui2::AutoMenu* sub = new Gui2::AutoMenu( tr("Spin Label Format"), this, false );
		pop->addMenu( sub );
		for( int i = SpinPointView3::None; i < SpinPointView3::End; i++ )
			sub->addCommand( SpinPointView3::menuText[ i ], this, SLOT(handleLabelType() ) )->setProperty("0", i );
		pop->addCommand( tr("Show Ghosts"), this, SLOT(handleShowGhosts()) );
	}
}

SpinPointList::~SpinPointList()
{
    if( d_space )
        d_space->removeObserver(this);
}

void SpinPointList::setSpace(SpinSpace * s)
{
    if( d_space == s )
        return;
    if( d_space )
        d_space->removeObserver(this);
    d_space = s;
    if( d_space )
        d_space->addObserver(this);
    refill();
}

SpinSpace::Element SpinPointList::getCurrent() const
{
    _SpinPointListItem* item = static_cast<_SpinPointListItem*>( currentItem() );
    if( item )
        return item->d_elem;
    else
        return SpinSpace::Element();
}

void SpinPointList::refill()
{
    d_cache.clear();
    clear();
    removeAllColumns();
    if( d_space.isNull() )
        return;
    for( Dimension d = 0; d < d_space->getDimCount(); d++ )
    {
        addColumn( Spec::getDimSymbol( d, true ) );
    }
	int count = 0;
    for( SpinSpace::Iterator pos = d_space->begin(); pos != d_space->end(); ++pos )
    {
        SpinSpace::Element e;
        d_space->fetch( pos, e );
		if( !e.isGhost() || d_showGhosts )
		{
			_SpinPointListItem* item = new _SpinPointListItem( this, e );
			d_cache[e.d_point] = item;
			count++;
		}
	}
    QMetaObject::invokeMethod( this, "resizeAllColumnsToContents", Qt::QueuedConnection );
	emit refilled( count );
}

void SpinPointList::handle( Root::Message & msg )
{
    BEGIN_HANDLER();
    MESSAGE( SpinSpace::Update, a, msg )
    {
        PpmPoint p;
        switch( a->getType() )
        {
        case SpinSpace::Update::Create:
            d_cache[a->getElem().d_point] = new _SpinPointListItem( this, a->getElem() );
            break;
        case SpinSpace::Update::Delete:
            {
                QMap<SpinPoint,_SpinPointListItem*>::iterator i = d_cache.find( a->getElem().d_point );
                if( i != d_cache.end() )
                {
                    i.value()->removeMe();
                    d_cache.erase( i );
                }
            }
            break;
        case SpinSpace::Update::Param:
            {
                QMap<SpinPoint,_SpinPointListItem*>::iterator i = d_cache.find( a->getElem().d_point );
                if( i != d_cache.end() )
                    i.value()->repaint();
            }
            break;
        case SpinSpace::Update::All:
            refill();
            break;
        default:
            break;
        }
        msg.consume();
    }
    END_HANDLER();
}

void SpinPointList::onDblClck(Gui::ListViewItem * i)
{
    _SpinPointListItem* item = static_cast<_SpinPointListItem*>( i );
    emit doubleClicked( item->d_elem );
}

void SpinPointList::onCurrentChanged()
{
    _SpinPointListItem* item = static_cast<_SpinPointListItem*>( currentItem() );
    if( item )
		emit singleClicked( item->d_elem );
}

void SpinPointList::handleLabelType()
{
	Gui2::UiFunction* a = Gui2::UiFunction::me();
	SpinPointView3::LabelType q = (SpinPointView3::LabelType) a->property("0").toInt();
	if( q < SpinPointView3::None || q >= SpinPointView3::End )
		return;

	CHECKED_IF( true, d_labelType == q );

	d_labelType = q;
	update();
}

void SpinPointList::handleGoto()
{
	_SpinPointListItem* item = static_cast<_SpinPointListItem*>( currentItem() );
	ENABLED_IF( item != 0 );

	emit doubleClicked( item->d_elem );
}

void SpinPointList::handleShowGhosts()
{
	CHECKED_IF( true, d_showGhosts );
	d_showGhosts = !d_showGhosts;
	refill();
}

void SpinPointList::handleShowPathSim()
{
	_SpinPointListItem* item = static_cast<_SpinPointListItem*>( currentItem() );
	ENABLED_IF( item != 0 );
	PathSimDlg dlg(d_space->getSpecType(), this );
	if( item->d_elem.d_point[DimX] )
		dlg.setSystem( item->d_elem.d_point[DimX]->getSystem() );
	dlg.exec();
}
