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

#if !defined(_QTL_WIDGET)
#define _QTL_WIDGET

typedef struct lua_State lua_State;




namespace Qtl
{
	class Widget
	{
	public:
		static int actions(lua_State * L); // const : QList<QAction *>
		static int activateWindow(lua_State * L);
		static int addAction(lua_State * L); // ( QAction * action )
		static int addActions(lua_State * L); // ( const QAction action, QAction action ...)
// table {QAction action, QAction action ...}
		static int adjustSize(lua_State * L);
		static int backgroundRole(lua_State * L); // const : QPalette::ColorRole
		static int childAt(lua_State * L); //( int x, int y ) const : QWidget
		// ( const QPoint & p ) const : QWidget
		static int clearFocus(lua_State * L);
		static int clearMask(lua_State * L);
		static int contentsRect(lua_State * L); // const : QRect
		static int ensurePolished(lua_State * L);
		static int focusProxy(lua_State * L); // const : QWidget
		static int focusWidget(lua_State * L); // const : QWidget
		static int fontInfo(lua_State * L); // const : QFontInfo
		static int fontMetrics(lua_State * L); // const : QFontMetrics 
		static int foregroundRole(lua_State * L); // const : QPalette::ColorRole
		static int getContentsMargins(lua_State * L); // ( int * left, int * top, int * right, int * bottom ) 
		static int grabKeyboard(lua_State * L);
		static int grabMouse(lua_State * L);
		static int grabShortcut(lua_State * L); // ( const QKeySequence & key, Qt::ShortcutContext context = Qt::WindowShortcut ) : int
		static int heightForWidth(lua_State * L); // ( int w ) const : int
		static int inputContext(lua_State * L); // QInputContext
		static int inputMethodQuery(lua_State * L); // ( Qt::InputMethodQuery query ) const : QVariant
		static int insertAction(lua_State * L); // ( QAction * before, QAction * action )
		static int insertActions(lua_State * L); // ( QAction * before, table {QAction action, QAction action ...} )
		static int isAncestorOf(lua_State * L); // ( const QWidget * child ) const : bool
		static int isEnabledTo(lua_State * L); // ( QWidget * ancestor ) const : bool
		static int isHidden(lua_State * L); // const : bool
		static int isVisibleTo(lua_State * L); // ( QWidget * ancestor ) const : bool
		static int isWindow(lua_State * L); // const : bool
		static int layout(lua_State * L); // const : QLayout
		static int mapFrom(lua_State * L); // ( QWidget * parent, const QPoint & pos ) const : QPoint
		static int mapFromGlobal(lua_State * L); // ( const QPoint & pos ) const : QPoint
		static int mapFromParent(lua_State * L); // ( const QPoint & pos ) const : QPoint
		static int mapTo(lua_State * L); // ( QWidget * parent, const QPoint & pos ) const : QPoint
		static int mapToGlobal(lua_State * L); // ( const QPoint & pos ) const : QPoint
		static int mapToParent(lua_State * L); // ( const QPoint & pos ) const : QPoint
		static int mask(lua_State * L); // const : QRegion
		static int nextInFocusChain(lua_State * L); // const : QWidget
		static int overrideWindowFlags(lua_State * L); // ( Qt::WindowFlags flags )
		static int paintEngine(lua_State * L); // const : QPaintEngine
		static int parentWidget(lua_State * L); // const : QWidget
		static int releaseKeyboard(lua_State * L);
		static int releaseMouse(lua_State * L);
		static int releaseShortcut(lua_State * L); // ( int id )
		static int removeAction(lua_State * L); // ( QAction * action )
		static int repaint(lua_State * L); // ( int x, int y, int w, int h )
		// ( const QRect & r ) // ( const QRegion & rgn )
		static int restoreGeometry(lua_State * L); // ( const QByteArray & geometry ) : bool
		static int saveGeometry(lua_State * L); // const : QByteArray
		static int scroll(lua_State * L); // ( int dx, int dy )
		static int setAttribute(lua_State * L); //( Qt::WidgetAttribute attribute, bool on = true ) 
		static int setBackgroundRole(lua_State * L); // ( QPalette::ColorRole role )
		static int setContentsMargins(lua_State * L); // ( int left, int top, int right, int bottom )
		static int setFixedHeight(lua_State * L); // ( int h )
		static int setFixedSize(lua_State * L); // ( const QSize & s )
		// ( int w, int h )
		static int setFixedWidth(lua_State * L); // ( int w )
		static int setFocus(lua_State * L); // ( Qt::FocusReason reason )
		static int setFocusProxy(lua_State * L); // ( QWidget * w )
		static int setForegroundRole(lua_State * L); // ( QPalette::ColorRole role )
		static int setInputContext(lua_State * L); // ( QInputContext * context )
		static int setLayout(lua_State * L); // ( QLayout * layout )
		static int setMask(lua_State * L); // ( const QBitmap & bitmap )
		// ( const QRegion & region )
		static int setParent(lua_State * L); // ( QWidget * parent )
	    // ( QWidget * parent, Qt::WindowFlags f )
		static int setShortcutAutoRepeat(lua_State * L); // ( int id, bool enable = true )
		static int setShortcutEnabled(lua_State * L); // ( int id, bool enable = true )
		static int setStyle(lua_State * L); // ( QStyle * style )
		static int setWindowFlags(lua_State * L); // ( Qt::WindowFlags type )
		static int setWindowRole(lua_State * L); // ( const QString & role )
		static int setWindowState(lua_State * L); // ( Qt::WindowStates windowState )
		static int stackUnder(lua_State * L); // ( QWidget * w )
		static int style(lua_State * L); // const : QStyle
		static int testAttribute(lua_State * L); // ( Qt::WidgetAttribute attribute ) const : bool
		static int underMouse(lua_State * L); // const :bool
	    static int update(lua_State * L); // ( int x, int y, int w, int h )
		// ( const QRect & r ) // ( const QRegion & rgn )
		static int updateGeometry(lua_State * L);
		static int visibleRegion(lua_State * L); // const : QRegion
		static int window(lua_State * L); // const : QWidget
		static int windowFlags(lua_State * L); // const : Qt::WindowFlags
		static int windowRole(lua_State * L); // const : QString
		static int windowState(lua_State * L); // () const : Qt::WindowStates
		static int windowType(lua_State * L); // const : Qt::WindowType
		static int init(lua_State * L);
		static void install(lua_State * L);


	};

}

#endif // !defined(_QTL_WIDGET)
