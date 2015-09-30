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

#if !defined(_QTL_TABWIDGET)
#define _QTL_TABWIDGET

typedef struct lua_State lua_State;




namespace Qtl
{
	class TabWidget
	{
	public:			
		static int init(lua_State * L);
		static int addTab(lua_State * L); //( QWidget * child, const QString & label )int
		static int cornerWidget(lua_State * L); //( Qt::Corner corner = Qt::TopRightCorner ) const QWidget
		static int count(lua_State * L); // const int
		static int currentIndex(lua_State * L);// const int 
		static int currentWidget(lua_State * L);// const QWidget
		static int elideMode(lua_State * L);// const Qt::TextElideMode
		static int iconSize(lua_State * L);// const QSize
		static int indexOf(lua_State * L);// (QWidget* w)const int
		static int insertTab(lua_State * L); 
		static int isTabEnabled(lua_State * L);// const bool
		static int removeTab(lua_State * L);// const bool
		static int setCornerWidget(lua_State * L);//( QWidget * widget, Qt::Corner corner = Qt::TopRightCorner )void
		static int setElideMode(lua_State * L);//( Qt::TextElideMode )void
		static int setIconSize(lua_State * L); // ( const QSize & size )void
		static int setTabEnabled(lua_State * L); // ( int index, bool enable )void
		static int setTabIcon(lua_State * L); //( int index, const QIcon & icon )void
		static int setTabPosition(lua_State * L);//( TabPosition )void
		static int setTabShape(lua_State * L); //( TabPosition )void
		static int setTabText(lua_State * L); //( int index, const QString & label )void
		static int setTabToolTip(lua_State * L); //( int index, const QString & tip )void
		static int setTabWhatsThis(lua_State * L); //( int index, const QString & text )void
		static int setUsesScrollButtons(lua_State * L); //( bool useButtons )void
		static int tabIcon(lua_State * L); //( int index ) const QIcon
		static int tabPosition(lua_State * L); //const TabPosition
		static int tabShape(lua_State * L); //const TabShape
		static int tabText(lua_State * L); //( int index ) const QString
		static int tabToolTip(lua_State * L); //( int index ) const QString
		static int tabWhatsThis(lua_State * L); //( int index ) const QString
		static int usesScrollButtons(lua_State * L); // const bool
		static int widget(lua_State * L); //( int index ) const QWidget

		static void install(lua_State * L);
	};

}

#endif // !defined(_QTL_TABWIDGET)
