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

#if !defined(_QTL_TABLEWIDGETITEM)
#define _QTL_TABLEWIDGETITEM

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

//#include <Root/Units.h>

typedef struct lua_State lua_State;




namespace Qtl
{
	class TableWidgetItem
	{
	public:
		static int background(lua_State * L);
		static int clone (lua_State * L); // virtual const
		static int column (lua_State * L); // int
		static int checkState (lua_State * L);// const Qt::CheckState 
		static int data (lua_State * L);// const( int role ) virtual QVariantconst
		static int font (lua_State * L);// const QFont


		static int foreground (lua_State * L); // const QBrush 
		static int icon (lua_State * L);// const QIcon
		static int isSelected (lua_State * L);// const bool
		static int read (lua_State * L); // ( QDataStream & in ) virtual void
		static int row (lua_State * L);// const int
		static int setBackground (lua_State * L);// ( const QBrush & brush )void
		static int setData (lua_State * L);// ( int role, const QVariant & value ) virtual void 
		static int setFont (lua_State * L);// ( const QFont & font )void
		static int setForeground (lua_State * L);// ( const QBrush & brush )void
		static int setIcon (lua_State * L);// ( const QIcon & icon )void
		static int setSelected (lua_State * L);// ( bool select )void
		static int setSizeHint(lua_State * L);// ( const QSize & size )void
		static int setStatusTip (lua_State * L);// ( const QString & statusTip )void
		static int setText (lua_State * L);// ( const QString & text )void
		static int setTextAlignment (lua_State * L);// ( int alignment )void
		static int setToolTip (lua_State * L);// ( const QString & toolTip ) void
		static int setWhatsThis(lua_State * L);// ( const QString & whatsThis ) void
		static int sizeHint(lua_State * L);// const QSize
		static int statusTip(lua_State * L);// const QString
		static int tableWidget(lua_State * L);// const * QTableWidget 
		static int text(lua_State * L);// const QString
		static int textAlignment (lua_State * L); // const int
		static int type (lua_State * L); // const int
		static int whatsThis (lua_State * L);//  const QString
		static int write (lua_State * L);// ( QDataStream & out ) void
		static int lessThan(lua_State * L);
		static int __eq(lua_State * L);
		static int setFlags(lua_State * L);//( Qt::ItemFlags flags )void  
		static  int setCheckState(lua_State * L);//( Qt::CheckState state )void  

		static int toolTip (lua_State * L);//  const QString
		static int flags (lua_State * L); //   const Qt::ItemFlags 
		static int init(lua_State * L);
	    static void install(lua_State * L);
	};

}

#endif // !defined(_QTL_TABLEWIDGETITEM)
