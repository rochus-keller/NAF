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

#if !defined(_QTL_TIME)
#define _QTL_TIME


typedef struct lua_State lua_State;


namespace Qtl
{

	class Time
	{
	public:
		// static int fromString ( const QString &, Qt::DateFormat ) : QTime 
		static int fromString(lua_State * L); // ( const QString &, const QString & ) : QTime 
		static int currentTime(lua_State * L); // () : QTime 
		// static int isValid ( int, int, int, int ) : bool 

		static int addMSecs(lua_State * L); // ( int ) const : QTime 
		static int addSecs(lua_State * L); // ( int ) const : QTime 
		static int elapsed(lua_State * L); // () const : int 
		static int hour(lua_State * L); // () const : int 
		static int isNull(lua_State * L); // () const : bool 
		static int isValid(lua_State * L); // () const : bool 
		static int minute(lua_State * L); // () const : int 
		static int msec(lua_State * L); // () const : int  
		static int msecsTo(lua_State * L); // ( const QTime & ) const : int 
		static int restart(lua_State * L); // () : int 
		static int second(lua_State * L); // () const : int 
		static int secsTo(lua_State * L); // ( const QTime & ) const : int 
		static int setHMS(lua_State * L); // ( int, int, int, int ) : bool 
		static int start(lua_State * L); // () 
		// static int toString ( const QString & ) const : QString 
		static int toString(lua_State * L); // ( Qt::DateFormat ) const : QString 
    	static int init(lua_State * L); 
		static void install(lua_State * L);
	};
}

#endif // !defined(_QTL_TIME)
