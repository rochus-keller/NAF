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

#include "Menu.h"
#include <cassert>
#include <QActionEvent>
#include <QApplication>
#include <QMenuBar>
#include <QShortcut>
using namespace Gui;

// Wir verwenden nicht direkt das MenuItem selber als Shortcut, da dieses mit setEnabled(false) gleich
// auch den Shortcut deaktiviert und wir somit bei Betätigung des Shortcut nicht mehr prüfen können,
// ob der Befehl verfügbar ist, sobald das Item einmal als deaktiviert erkannt wurde.
class MenuItemShortcut : public QShortcut
{
public:
    // Eigentlich ist QShortcut nicht darauf angewiesen, dass parent ein QWidget ist.
    // parentWidget wird im Code nur einmal verwendet und nur um auf Null zu testen.
    MenuItemShortcut( QWidget* w, MenuItem* item, const QKeySequence& key ):
        QShortcut( w ),d_item(item)
    {
        setContext( Qt::WidgetWithChildrenShortcut  );
        setKey( key );
    }
    bool event(QEvent *e)
    {
        // QShortcut wird vom System über diesen Event benachtrichtigt und ruft dann
        // triggered auf.
        if( e->type() == QEvent::Shortcut )
        {
            d_item->autoEnable();
            d_item->execute();
            return true;
        }else
            return QShortcut::event( e );
    }
    MenuItem* d_item;
};


static QWidget* _climbToNonMenu( QObject* o )
{
    if( dynamic_cast<QMenu*>( o ) || dynamic_cast<QMenuBar*>( o ) )
        return _climbToNonMenu( o->parent() );
    else if( QWidget* w = dynamic_cast<QWidget*>( o ) )
        return w;
    else if( o == 0 )
        return 0;
    else
        return _climbToNonMenu( o->parent() );
}

MenuItem::MenuItem(Menu *owner, Root::Messenger *handler,
			const QString& text, Root::Action::CmdSym cmd, 
			bool checkable, const QKeySequence& key,
			const Root::Action::Params& p ):
    QAction( text, owner ), d_cmd( handler, text.toLatin1(), cmd, checkable ),
    d_shortcut(0)

{
    init();
    setCheckable( checkable );
    setShortcut( key ); // damit in Menü angezeigt, nicht um auszuführen
    if( !key.isEmpty() )
    {
        QWidget* w = _climbToNonMenu( owner );
        if( w )
            d_shortcut = new MenuItemShortcut( w, this, key );
        else
            qWarning() << "MenuItem '" << text << "' cannot create shortcut ";
    }
}

MenuItem::MenuItem(Menu *owner, const QString& text, Root::Action::CmdSym cmd,
			bool checkable, const QKeySequence& key,
			const Root::Action::Params& p ):
    QAction( text, owner ), d_cmd( 0, text.toLatin1(), cmd, checkable ),
    d_shortcut(0)

{
    init();
    setCheckable( checkable );
    setShortcut( key ); // damit in Menü angezeigt, nicht um auszuführen
    if( !key.isEmpty() )
    {
        QWidget* w = _climbToNonMenu( owner );
        if( w )
            d_shortcut = new MenuItemShortcut( w, this, key );
        else
            qWarning() << "MenuItem '" << text << "' cannot create shortcut ";
    }
}

void MenuItem::init()
{
    setShortcutContext( Qt::WidgetShortcut  );   // damit keine Mehrdeutigkeit mit MenuItemShortcut
	d_autoEnabling = true;
    connect( this, SIGNAL( triggered( bool ) ), this, SLOT( handleTrigger( bool ) ) );
    setEnabled( false );
	setChecked( false );
	setMenuRole(QAction::NoRole); // damit Qt on OS X nicht wild Menüs umplatziert und umbenennt
}

MenuItem::~MenuItem()
{
    if( d_shortcut )
        delete d_shortcut;
}

void MenuItem::setAutoEnabling(bool on)
{
	d_autoEnabling = on;
}

void MenuItem::handleTrigger( bool on )
{
    execute();
}

bool MenuItem::event(QEvent *e)
{
	// auf Event hören und Broadcast auslösen, wenn d_autoEnabling
    if( int(e->type()) == Root::Message::ID )
	{
		if( dynamic_cast<AutoEnable*>( e ) == 0 )
			return true;
		autoEnable();
		return true;
    }else if( e->type() == QEvent::Shortcut )
    {
        // wir verschlucken hier den Event, da das Handling von MenuItemShortcut gemacht wird
        return true;
    }else
		return QAction::event( e );
}

void MenuItem::autoEnable()
{
    if( !d_autoEnabling )
        return;
	d_cmd.update();
    if( d_cmd.isConsumed() )
	{
        setEnabled( d_cmd.isEnabled() );
		setChecked( d_cmd.isOn() );
	}else
	{
        setEnabled( false );
		setChecked( false );
    }
}

void MenuItem::execute()
{
    if( !isEnabled() )
        return;
    d_cmd.execute();
}

Menu::Menu(QWidget * parent, const QString& title, bool context ):
	QMenu( title, parent )
{
    connect( this, SIGNAL( aboutToShow() ), this, SLOT( handleShow() ) );
	if( context )
		registerPopup();
}

Menu::Menu(QWidget * parent, const char* title, bool context ):
	QMenu( title, parent )
{
    connect( this, SIGNAL( aboutToShow() ), this, SLOT( handleShow() ) );
	if( context )
		registerPopup();
}

Menu::Menu(QWidget * parent, bool context ):
	QMenu( parent )
{
    connect( this, SIGNAL( aboutToShow() ), this, SLOT( handleShow() ) );
	if( context )
		registerPopup();
}

Menu::~Menu()
{

}

void Menu::purge()
{
	QList<QAction *> a = actions();
	for( int i = 0; i < a.size(); i++ )
		delete a[ i ];
}

void Menu::handleShow()
{
	// Gehe durch alle Actions und sende ihnen irgend ein Message-Derivat.
	// Das löst dann einen Update-Cycle aus
	MenuItem::AutoEnable msg;
	QList<QAction*> l = actions();
	foreach( QObject* a, l )
		a->event( &msg );
}

void Menu::contextRequest( const QPoint & pos )
{
	// The position pos is the position of the request in widget coordinates.
	QWidget* w = dynamic_cast<QWidget*>( sender() );
	if( w )
		popup( w->mapToGlobal( pos ) ); // NOTE: vorher exec.
	else
		popup( QCursor::pos() );
}

MenuItem* Menu::addCommand(Root::Messenger *handler,
						   const QString& text, Root::Action::CmdSym cmd, 
						   bool checkable, const QKeySequence& ks, const Root::Action::Params& p )
{
	MenuItem* a = new MenuItem( this, handler, text, cmd, checkable, ks, p );
	addAction( a );
	return a;
}

MenuItem* Menu::addCommand( const QString& text, Root::Action::CmdSym cmd, 
						   bool checkable, const QKeySequence& ks, const Root::Action::Params& p )
{
	MenuItem* a = new MenuItem( this, text, cmd, checkable, ks, p );
	addAction( a );
	return a;
}

void Menu::registerPopup()
{
	assert( parentWidget() != 0 );

	parentWidget()->setContextMenuPolicy( Qt::CustomContextMenu );
	connect( parentWidget(), SIGNAL( customContextMenuRequested ( const QPoint & ) ),
		this, SLOT( contextRequest( const QPoint &) ) );
}

void Menu::connectPopup(QWidget * w)
{
	assert( w != 0 );
	w->setContextMenuPolicy( Qt::CustomContextMenu );
	connect( w, SIGNAL( customContextMenuRequested ( const QPoint & ) ),
		this, SLOT( contextRequest( const QPoint &) ) );
}


MenuItem* Menu::item(Menu* m, Root::Messenger *handler,
			const QString& text, Root::Action::CmdStr cmd, 
			bool checkable, const QKeySequence& ks,
			const Root::Any& v )
{
	assert( m );
	Root::Action::Params pars;
	if( !v.isNull() )
		pars.push_back( v );
	return m->addCommand( handler, text, cmd, checkable, ks, pars );
}

MenuItem* Menu::item(Menu* m, Root::Messenger *handler,
			Root::Action::CmdStr cmd, const QKeySequence& ks)
{
	assert( m );
	return m->addCommand( handler, Root::Action::prettyName( cmd ), cmd, false, ks );
}

MenuItem* Menu::item(Menu* m,  
			const QString& text, Root::Action::CmdStr cmd, 
			bool checkable, const QKeySequence& ks,
			const Root::Any& v )
{
	assert( m );
	Root::Action::Params pars;
	if( !v.isNull() )
		pars.push_back( v );
	return m->addCommand( text, cmd, checkable, ks, pars );
}

MenuItem* Menu::item(Menu* m,  
			Root::Action::CmdStr cmd, const QKeySequence& ks)
{
	assert( m );
	return m->addCommand( Root::Action::prettyName( cmd ), cmd, false, ks );
}
