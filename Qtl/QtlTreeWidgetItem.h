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

#if !defined(_QTL_TREEWIDGETITEM)
#define _QTL_TREEWIDGETITEM

typedef struct lua_State lua_State;




namespace Qtl
{
	class TreeWidgetItem
	{
	private:
		static void setOptionMap();
	public:		
		static int addChild(lua_State * L); // ( QTreeWidgetItem * child )
		static int background( lua_State * L ); // ( int column ) const: QBrush
		static int checkState( lua_State * L ); // ( int column ) const: Qt::CheckState
		static int child( lua_State * L ); // ( int column ) const: QTreeWidgetItem
		static int childCount( lua_State * L ); // const: int
		static int clone( lua_State * L ); // const: QTreeWidgetItem virtual
		static int columnCount( lua_State * L );// const: int
		static int data( lua_State * L );// ( int column, int role ) const : QVariant virtual
		static int tabIcon( lua_State * L );// ( int index ) const : QIcon
		static int flags( lua_State * L );// const: Qt::ItemFlags
		static int font( lua_State * L );// ( int column ) const : QFont
		static int foreground( lua_State * L );// ( int column ) const : QBrush 
		static int icon( lua_State * L );// ( int column ) const : QIcon
		static int indexOfChild( lua_State * L );// ( QTreeWidgetItem * child ) const : int
		static int insertChild( lua_State * L );// ( int index, QTreeWidgetItem * child )
		static int isExpanded( lua_State * L );// const : bool
		static int isHidden( lua_State * L );// const : bool
		static int isSelected( lua_State * L );// const : bool
		static int parent( lua_State * L );// const: QTreeWidgetItem
		static int read( lua_State * L );// ( QDataStream & in ) : virtual
		static int setBackground( lua_State * L );// ( int column, const QBrush & brush )
		static int setCheckState( lua_State * L );// ( int column, Qt::CheckState state )
		static int setData( lua_State * L );// ( int column, int role, const QVariant & value ): virtual
		static int setExpanded( lua_State * L );// ( bool expand )
		static int setFlags( lua_State * L );// ( Qt::ItemFlags flags )
		static int setFont( lua_State * L );// ( int column, const QFont & font )
		static int setForeground( lua_State * L );// ( int column, const QBrush & brush )
		static int setHidden( lua_State * L );// ( bool hide )
		static int setIcon( lua_State * L );// ( int column, const QIcon & icon )
		static int setSelected( lua_State * L );// ( bool select )
		static int setSizeHint( lua_State * L );// ( int column, const QSize & size )
		static int setStatusTip( lua_State * L );// ( int column, const QString & statusTip )
		static int setText( lua_State * L );// ( int column, const QString & text )
		static int setTextAlignment( lua_State * L );// ( int column, int alignment )
		static int setToolTip( lua_State * L );// ( int column, const QString & toolTip )
		static int setWhatsThis( lua_State * L );// ( int column, const QString & whatsThis )
		static int sizeHint( lua_State * L );// ( int column ) const : QSize
		static int sortChildren( lua_State * L );// ( int column, Qt::SortOrder order )
		static int statusTip( lua_State * L );// ( int column ) const : QString
		static int takeChild( lua_State * L );// ( int index ) const: QTreeWidgetItem
		static int text( lua_State * L );// ( int column ) const : QString
		static int textAlignment( lua_State * L );// ( int column ) const: int
		static int toolTip( lua_State * L );// ( int column ) const : QString
		static int treeWidget( lua_State * L );// const: QTreeWidget
		static int type( lua_State * L );// const: int
		static int whatsThis( lua_State * L );// ( int column ) const : QString
		static int write( lua_State * L );// ( QDataStream & out ) const: virtual
		static int lessThan( lua_State * L );// ( const QTreeWidgetItem & other ) const: bool virtual
		static int operatorEq( lua_State * L );// //( const QTreeWidgetItem & other ) : QTreeWidgetItem
		static int init(lua_State * L);
		static void install(lua_State * L);
	};

}

#endif // !defined(_QTL_PAINTER)
