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

#if !defined(_LuaQt3_LuaCanvas2)
#define _LuaQt3_LuaCanvas2

#include <Script/Engine.h>
#include <Script2/QtObject.h>
#include <QMainWindow> 
#include <QPicture>
#include <QPainter>

namespace Lua
{
	class LuaCanvas2 : public QMainWindow
	{
		Q_OBJECT
	public:
		LuaCanvas2(QWidget*);
		static void install( lua_State *L );
	private:
		class View : public QWidget
		{
		public:
			View( QWidget* p ):QWidget(p) 
			{
				setBackgroundColor( Qt::white );
			}
			QPicture d_pic;
			QPainter d_p;	// use isActive, begin( &d_pic ), end
			QPoint d_pt;
		protected:
			void paintEvent( QPaintEvent * event )
			{
				if( d_p.isActive() )
					return;
				QPainter p( this );
				p.drawPicture( 0, 0, d_pic );
			}
		};
		View* d_view;

		static const luaL_reg methods[];

		static int _setWindowSize(lua_State *L);
		static int _showNormal(lua_State *L);
		static int _showFullScreen(lua_State *L);
		static int _showMinimized(lua_State *L);
		static int _showMaximized(lua_State *L);
		static int _setZoomFactor(lua_State *L);
		static int _drawSlice(lua_State *L);
		static int _drawIcon(lua_State *L);
		static int _drawContour(lua_State *L);
		static int _drawImage(lua_State *L);
		static int _setCaption(lua_State *L);
		static int _setBrush(lua_State *L);
		static int _setPen(lua_State *L);
		static int _getBounding(lua_State *L);
		static int _setFont(lua_State *L);
		static int _drawText(lua_State *L);
		static int _fillRect(lua_State *L);
		static int _drawRect(lua_State *L);
		static int _drawLine(lua_State *L);
		static int _drawPoint(lua_State *L);
		static int _lineTo(lua_State *L);
		static int _moveTo(lua_State *L);
		static int _setBgColor(lua_State *L);
		static int _setSize(lua_State *L);
		static int _drawEllipse(lua_State *L);
		static int _commit(lua_State *L);
		static int _begin(lua_State *L);
	protected slots:
		void handlePrint();
		void handlePageSize();
		void handleClose();
		void handleLoad();
		void handleSave();
	};
}

#endif // !defined(_LuaQt3_LuaCanvas2)
