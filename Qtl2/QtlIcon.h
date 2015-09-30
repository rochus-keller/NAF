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

#if !defined(_QTL_ICON)
#define _QTL_ICON


typedef struct lua_State lua_State;


namespace Qtl
{

	class Icon
	{
	public:
		static int actualSize(lua_State * L); // ( const QSize &, Mode, State ) const : QSize 
		static int addFile(lua_State * L); // ( const QString &, const QSize &, Mode, State )  
		static int addPixmap(lua_State * L); // ( const QPixmap &, Mode, State ) 
		static int isNull(lua_State * L); // () const : bool 
//		static int paint(lua_State * L); // ( QPainter *, const QRect &, Qt::Alignment, Mode, State ) const 
		static int paint(lua_State * L); // ( QPainter *, int, int, int, int, Qt::Alignment, Mode, State ) const 
//		static int pixmap(lua_State * L); // ( const QSize &, Mode, State ) const : QPixmap 
//		static int pixmap(lua_State * L); // ( int, int, Mode, State ) const : QPixmap 
		static int pixmap(lua_State * L); // ( int, Mode, State ) const : QPixmap 
		static int serialNumber(lua_State * L); // () const : int 
 		static int init(lua_State * L); 
		static void install(lua_State * L);
	};
}

#endif // !defined(_QTL_ICON)
