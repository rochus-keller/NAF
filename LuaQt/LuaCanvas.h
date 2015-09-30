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

#if !defined(AFX_LUACANVAS_H__04D9AB82_5A49_436F_A240_9CF106858555__INCLUDED_)
#define AFX_LUACANVAS_H__04D9AB82_5A49_436F_A240_9CF106858555__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <q3mainwindow.h> 
#include <q3picture.h>
#include <qimage.h> 
#include <QPixmap>
#include <Script/LuaEngine.h>
#include <Script/PeerHelper.h>
#include <LuaQt/LuaPeer.h>

class Q3ScrollView;
class _LuaCanvasView;

namespace Spec
{
	class LuaCanvasPeer;

	class LuaCanvas : public Q3MainWindow
	{
		Q_OBJECT
	public:
		LuaCanvas();
		virtual ~LuaCanvas();
	private:
		_LuaCanvasView* d_view;
		LuaCanvasPeer* d_peer;
		friend class LuaCanvasPeer;
	protected slots:
		void handlePrint();
		void handlePageSize();
		void handleClose();
		void handleLoad();
		void handleSave();
	};

	class LuaCanvasPeer : public LuaPeer
	{
	private:
		LuaCanvas* d_cv;
		QPoint d_pt;
	public:
		int setWindowSize(lua_State *L);
		int showNormal(lua_State *L);
		int showFullScreen(lua_State *L);
		int showMinimized(lua_State *L);
		int showMaximized(lua_State *L);
		int setZoomFactor(lua_State *L);
		int drawSlice(lua_State *L);
		int drawIcon(lua_State *L);
		int drawContour(lua_State *L);
		int drawImage(lua_State *L);
		int setCaption(lua_State *L);
		int setBrush(lua_State *L);
		int setPen(lua_State *L);
		int getBounding(lua_State *L);
		int setFont(lua_State *L);
		int drawText(lua_State *L);
		int fillRect(lua_State *L);
		int drawRect(lua_State *L);
		int drawLine(lua_State *L);
		int drawPoint(lua_State *L);
		int lineTo(lua_State *L);
		int moveTo(lua_State *L);
		int setBgColor(lua_State *L);
		int setSize(lua_State *L);
		int drawEllipse(lua_State *L);
		int commit(lua_State *L);
		int begin(lua_State *L);
		~LuaCanvasPeer();
		LuaCanvasPeer( LuaCanvas* );
		static const char s_className[];
		static PeerHelper<LuaCanvasPeer>::MethodSlot s_methods[];
	protected:
		friend class PeerHelper<LuaCanvasPeer>;
		friend class LuaCanvas;
	};

	class LuaImage
	{
	public:
		std::string getInstanceName() const { return "Image"; }
		int getSize(lua_State *L);
		LuaImage();

		QImage d_img;
		static const char s_className[];
		static PeerHelper<LuaImage>::MethodSlot s_methods[];
	};

	class LuaIcon
	{
	public:
		int getSize(lua_State *L);
		std::string getInstanceName() const { return "Icon"; }
		QPixmap d_pix;
		static const char s_className[];
		static PeerHelper<LuaIcon>::MethodSlot s_methods[];
	};
}

#endif // !defined(AFX_LUACANVAS_H__04D9AB82_5A49_436F_A240_9CF106858555__INCLUDED_)
