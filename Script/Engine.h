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

#if !defined(AFX_LUAENGINE_H__2C694B7E_FD38_49C2_A0EF_AD5953597FE4__INCLUDED_)
#define AFX_LUAENGINE_H__2C694B7E_FD38_49C2_A0EF_AD5953597FE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Subject.h>
#include <Root/Vector.h>
#include <Script/Engine2.h>
#include <QSet>
#include <QList>
#include <QUuid>

typedef struct lua_State lua_State;
typedef struct lua_Debug lua_Debug;

namespace Lua
{
	class Engine : public Engine2, public Root::Subject
	{
	public:
        Engine();
        virtual ~Engine();

		static Engine* inst();
		static void kill();

		class Update : public Root::UpdateMessage
		{
		public:
			Update( Engine* m, MessageType t, const QByteArray& val1 = QByteArray(), int val2 = 0 ):
                Root::UpdateMessage( m ), d_type( t ), d_val1( val1 ), d_val2(val2), d_cancel(false) {}

			MessageType getType() const { return d_type; }
            QByteArray d_val1; // Text, Script Name
            int d_val2;
            bool d_cancel;
		private:
			MessageType d_type;
		};
	protected:
		// Override
		void notify( MessageType messageType, const QByteArray &val1 = "", int val2 = 0 );
	};
}

#endif // !defined(AFX_LUAENGINE_H__2C694B7E_FD38_49C2_A0EF_AD5953597FE4__INCLUDED_)
