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

#if !defined(_QTL_HEADERVIEW)
#define _QTL_HEADERVIEW

typedef struct lua_State lua_State;




namespace Qtl
{
	class HeaderView
	{
	public:		
		static int count(lua_State * L); // const : int
		static int hiddenSectionCount(lua_State * L); // const : int
		static int hideSection(lua_State * L); // ( int logicalIndex )
		static int isClickable(lua_State * L); // const : bool
		static int isMovable(lua_State * L); // const : bool
		static int isSectionHidden(lua_State * L); // ( int logicalIndex ) const : bool
		static int length(lua_State * L); // const : int
		static int logicalIndex(lua_State * L); // ( int visualIndex ) const : int
		static int logicalIndexAt(lua_State * L); // ( int position ) const : int
		// ( const QPoint & pos ) const : int
		// ( int x, int y ) const : int
		static int moveSection(lua_State * L); // ( int from, int to )
		static int offset(lua_State * L); // const : int
		static int orientation(lua_State * L); // const : Qt::Orientation 
		static int resizeMode(lua_State * L); // ( int logicalIndex ) const : ResizeMode 
		static int resizeSection(lua_State * L); // ( int logicalIndex, int size )
		static int resizeSections(lua_State * L); // ( QHeaderView::ResizeMode mode )
		static int sectionPosition(lua_State * L); // ( int logicalIndex ) const : int
		static int sectionSize(lua_State * L); // ( int logicalIndex ) const : int
		static int sectionSizeHint(lua_State * L); // ( int logicalIndex ) const : int
		static int sectionViewportPosition(lua_State * L); // ( int logicalIndex ) const : int
		static int sectionsHidden(lua_State * L); // const : bool 
		static int sectionsMoved(lua_State * L); // const : bool
		static int setClickable(lua_State * L); // ( bool clickable )
		static int setMovable(lua_State * L); // ( bool movable )
		static int setResizeMode(lua_State * L); // ( ResizeMode mode )
		static int setSectionHidden(lua_State * L); // ( int logicalIndex, bool hide )
		static int setSortIndicator(lua_State * L); // ( int logicalIndex, Qt::SortOrder order )
		static int showSection(lua_State * L); // ( int logicalIndex )
		static int sizeHint(lua_State * L); // const : QSize 
		static int sortIndicatorOrder(lua_State * L); // const : Qt::SortOrder 
		static int sortIndicatorSection(lua_State * L); // const : int
		static int stretchSectionCount(lua_State * L); // const : int
		static int swapSections(lua_State * L); // ( int first, int second )
		static int visualIndex(lua_State * L); // ( int logicalIndex ) const : int
		static int visualIndexAt(lua_State * L); // ( int position ) const : int
        static int init(lua_State * L);
		static void install(lua_State * L);
	};

}

#endif // !defined(_QTL_HEADERVIEW)
