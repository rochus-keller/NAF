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

#if !defined(_QTL_URL)
#define _QTL_URL

typedef struct lua_State lua_State;




namespace Qtl
{
	class Url
	{
	public:
		
		static int init(lua_State * L);
		static int addQueryItem ( lua_State * L );// ( const QString & key, const QString & value )
		static int allQueryItemValues ( lua_State * L );// ( const QString & key ) const : QStringList
		static int authority ( lua_State * L );// const : QString
		static int clear ( lua_State * L );
		static int encodedQuery ( lua_State * L );// const : QByteArray
		static int errorString ( lua_State * L );// const : QString
		static int fragment ( lua_State * L );// const : QString
		static int hasFragment ( lua_State * L );// const : bool
		static int hasQuery ( lua_State * L );// const : bool
		static int hasQueryItem ( lua_State * L );// ( const QString & key ) const : bool
		static int host ( lua_State * L );// const : QString
		static int isEmpty ( lua_State * L );// const : bool
		static int isParentOf ( lua_State * L );// ( const QUrl & childUrl ) const : bool
		static int isRelative ( lua_State * L );// const : bool
		static int isValid ( lua_State * L );// const : bool
		static int password ( lua_State * L );// const : QString
		static int path ( lua_State * L );// const : QString
		static int port ( lua_State * L );// const : int
		static int queryItemValue ( lua_State * L );// ( const QString & key ) const : QString
		static int queryPairDelimiter ( lua_State * L );// const : char
		static int queryValueDelimiter ( lua_State * L );// const : char
		static int removeAllQueryItems ( lua_State * L );// ( const QString & key ) 
		static int removeQueryItem ( lua_State * L );// ( const QString & key )
		static int resolved ( lua_State * L );// ( const QUrl & relative ) const : QUrl
		static int scheme ( lua_State * L );// const : QString
		static int setAuthority ( lua_State * L );// ( const QString & authority )
		static int setEncodedQuery ( lua_State * L );// ( const QByteArray & query )
		static int setEncodedUrl ( lua_State * L );// ( const QByteArray & encodedUrl )
		static int setFragment ( lua_State * L );// ( const QString & fragment )
		static int setHost ( lua_State * L );// ( const QString & host )
		static int setPassword ( lua_State * L );// ( const QString & password )
		static int setPath ( lua_State * L );// ( const QString & path )
		static int setPort ( lua_State * L );// ( int port )
		//static int setQueryDelimiters ( lua_State * L );// ( char valueDelimiter, char pairDelimiter )
		static int setScheme ( lua_State * L );// ( const QString & scheme )
		static int setUrl ( lua_State * L );// ( const QString & url )
		static int setUserInfo ( lua_State * L );// ( const QString & userInfo )
		static int setUserName ( lua_State * L );// ( const QString & userName )
		static int toEncoded ( lua_State * L );// ( FormattingOptions options = None ) const : QByteArray
		static int toLocalFile ( lua_State * L );// const : QString
		static int toString ( lua_State * L );// ( FormattingOptions options = None ) const : QString
		static int userInfo ( lua_State * L );// const : QString
		static int userName ( lua_State * L );// const : QString
		static int notEquals ( lua_State * L );// ( const QUrl & url ) const : bool
		static int operatorEq ( lua_State * L );// ( const QUrl & url ) const : QUrl
		static void install(lua_State * L);


	};

}

#endif // !defined(_QTL_URL)
