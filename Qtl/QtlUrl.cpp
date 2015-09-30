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

#include "QtlUrl.h"
#include <QUrl>
#include <QPaintEngine>
#include <QString>
#include "Enums.h"
#include <Script/ValueInstaller.h>
#include "Variant.h"
using namespace std;
using namespace Qtl;
using namespace Lua;

GCC_IGNORE(-Wunused-variable);
GCC_IGNORE(-Wunused-but-set-variable);

int Url::init(lua_State * L)
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	Enums enums(L);
	
	if ( Util::isNil( L, 2 ) )
	{
		*lhs = QUrl();
	}
	else if ( QUrl* url = ValueInstaller2<QUrl>::cast( L, 2 ) )
	{
		*lhs = QUrl( *url );
	}
	else if ( Util::isStr( L, 2 ) ) // QString* url = ValueInstaller2<QString>::cast( L, 2 ) )
	{
		*lhs = QUrl( Util::toString( L, 2 ) );
	}
	/*else if ( QUrl::ParsingMode f=(QUrl::ParsingMode)enums.ParsingMode( 3 ) )
	{
		*lhs = QUrl( f );
	}*/
	else
	{
		//lhs->~QUrl();
	}
	return 0;
}
int Url::addQueryItem ( lua_State * L )// ( const QString & key, const QString & value )
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	//QString* key = ValueInstaller2<QString>::check( L, 2 );
	//QString* value = ValueInstaller2<QString>::check( L, 3 );
	lhs->addQueryItem( Util::toString( L, 2 ), Util::toString( L, 3 ) );
	return 0;
}
int Url::allQueryItemValues ( lua_State * L )// ( const QString & key ) const : QStringList
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	//QString* key = ValueInstaller2<QString>::check( L, 2 );
	QStringList* res = ValueInstaller2<QStringList>::create( L );
	*res = lhs->allQueryItemValues( Util::toString( L, 2 ) );
	return 1;
}
int Url::authority ( lua_State * L )// const : QString
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	lua_pushstring(L, lhs->authority().toLatin1() );
	return 1;
}
int Url::clear ( lua_State * L )
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	lhs->clear();
	return 0;
}
int Url::encodedQuery ( lua_State * L )// const : QByteArray
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	//QByteArray* res = ValueInstaller2<QByteArray>::create( L );
	Util::push( L, lhs->encodedQuery().data() );
	return 1;
}
int Url::errorString ( lua_State * L )// const : QString
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	lua_pushstring(L, lhs->errorString().toLatin1() );
	return 1;
}
int Url::fragment ( lua_State * L )// const : QString
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	lua_pushstring(L, lhs->fragment().toLatin1() );
	return 1;
}
int Url::hasFragment ( lua_State * L )// const : bool
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	Util::push( L, lhs->hasFragment() );
	return 1;
}
int Url::hasQuery ( lua_State * L )// const : bool
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	Util::push( L, lhs->hasQuery() );
	return 1;
}
int Url::hasQueryItem ( lua_State * L )// ( const QString & key ) const : bool
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	//QString* key = ValueInstaller2<QString>::check( L, 2 );
	Util::push( L, lhs->hasQueryItem( Util::toString( L, 2 ) ) );
	return 1;
}
int Url::host ( lua_State * L )// const : QString
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	lua_pushstring(L, lhs->host().toLatin1() );
	return 1;
}
int Url::isEmpty ( lua_State * L )// const : bool
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	Util::push( L, lhs->isEmpty() );
	return 1;
}
int Url::isParentOf ( lua_State * L )// ( const QUrl & childUrl ) const : bool
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	QUrl* childUrl = ValueInstaller2<QUrl>::check( L, 2 );
	Util::push( L, lhs->isParentOf( *childUrl ) );
	return 1;
}
int Url::isRelative ( lua_State * L )// const : bool
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	Util::push( L, lhs->isRelative() );
	return 1;
}
int Url::isValid ( lua_State * L )// const : bool
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	Util::push( L, lhs->isValid() );
	return 1;
}
int Url::password ( lua_State * L )// const : QString
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	lua_pushstring(L, lhs->password().toLatin1() );
	return 1;
}
int Url::path ( lua_State * L )// const : QString
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	lua_pushstring(L, lhs->path().toLatin1() );
	return 1;
}
int Url::port ( lua_State * L )// const : int
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	if(Util::isNum( L, 2 ))
	{
		Util::push( L, lhs->port(Util::toInt( L, 2 )) );
	}
	else
	{
		Util::push( L, lhs->port() );
	}
	return 1;
}
int Url::queryItemValue ( lua_State * L )// ( const QString & key ) const : QString
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	//QString* key = ValueInstaller2<QString>::check( L, 2 );
	Util::push(L, lhs->queryItemValue( Util::toString( L, 2 ) ) );
	return 1;
}
int Url::queryPairDelimiter ( lua_State * L )// const : char
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	Util::push( L, lhs->queryPairDelimiter() );
	return 1;
}
int Url::queryValueDelimiter ( lua_State * L )// const : char
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	Util::push( L, lhs->queryValueDelimiter() );
	return 1;
} 
int Url::removeAllQueryItems ( lua_State * L )// ( const QString & key ) 
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	//QString* key = ValueInstaller2<QString>::check( L, 2 );
	lhs->removeAllQueryItems( Util::toString( L, 2 ) );
	return 0;
}
int Url::removeQueryItem ( lua_State * L )// ( const QString & key ) 
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	//QString* key = ValueInstaller2<QString>::check( L, 2 );
	lhs->removeQueryItem( Util::toString( L, 2 ) );
	return 0;
}
int Url::resolved ( lua_State * L )// ( const QUrl & relative ) const : QUrl
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	QUrl* relative = ValueInstaller2<QUrl>::check( L, 2 );
	QUrl* res = ValueInstaller2<QUrl>::create( L );
	*res = lhs->resolved( *relative );
	return 1;
}
int Url::scheme ( lua_State * L )// const : QString
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	lua_pushstring(L, lhs->scheme().toLatin1() );
	return 1;
}
int Url::setAuthority ( lua_State * L )// ( const QString & authority )
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	//QString* authority = ValueInstaller2<QString>::check( L, 2 );
	lhs->setAuthority( Util::toString( L, 2 ) );
	return 0;
}
int Url::setEncodedQuery ( lua_State * L )// ( const QByteArray & query )
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	//QByteArray* query = ValueInstaller2<QByteArray>::check( L, 2 );
	lhs->setEncodedQuery( Util::toStr( L, 2 ) );
	return 0;
}
int Url::setEncodedUrl ( lua_State * L )// ( const QByteArray & encodedUrl )
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	//QByteArray* encodedUrl = ValueInstaller2<QByteArray>::check( L, 2 );
	if( Util::isNil( L, 3 ))
	{
		lhs->setEncodedUrl( Util::toStr( L, 2 ) );
	}
	else
	{
		Enums enums(L);
		QUrl::ParsingMode f=(QUrl::ParsingMode)enums.ParsingMode( 3 );
		lhs->setEncodedUrl( Util::toStr( L, 2 ), f );
	}
	return 0;
}
int Url::setFragment ( lua_State * L )// ( const QString & fragment )
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	//QString* fragment = ValueInstaller2<QString>::check( L, 2 );
	lhs->setFragment( Util::toString( L, 2 ) );
	return 0;
}
int Url::setHost ( lua_State * L )// ( const QString & host )
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	//QString* host = ValueInstaller2<QString>::check( L, 2 );
	lhs->setHost( Util::toString( L, 2 ) );
	return 0;
}
int Url::setPassword ( lua_State * L )// ( const QString & password )
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	//QString* password = ValueInstaller2<QString>::check( L, 2 );
	lhs->setPassword( Util::toString( L, 2 ) );
	return 0;
}
int Url::setPath ( lua_State * L )// ( const QString & path )
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	//QString* path = ValueInstaller2<QString>::check( L, 2 );
	lhs->setPath( Util::toString( L, 2 ) );
	return 0;
}
int Url::setPort ( lua_State * L )// ( int port )
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	lhs->setPort( Util::toInt( L, 2 ) );
	return 0;
}/*
int Url::setQueryDelimiters ( lua_State * L )// ( char valueDelimiter, char pairDelimiter )
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	lhs->setQueryDelimiters( Util::toStr( L, 2 ), Util::toStr( L, 3 ) );
	return 0;
}*/
int Url::setScheme ( lua_State * L )// ( const QString & scheme )
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	//QString* scheme = ValueInstaller2<QString>::check( L, 2 );
	lhs->setScheme( Util::toString( L, 2 ) );
	return 0;
}
int Url::setUrl ( lua_State * L )// ( const QString & url )
//( const QString & url, ParsingMode parsingMode )
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	//QString* url = ValueInstaller2<QString>::check( L, 2 );
	if( Util::isNil( L, 3 ) )
	{
		lhs->setUrl( Util::toString( L, 2 ) );
	}
	else
	{
		Enums enums(L);
		QUrl::ParsingMode f=(QUrl::ParsingMode)enums.ParsingMode( 3 );
		lhs->setUrl( Util::toString( L, 2 ), f );
	}
	return 0;
}
int Url::setUserInfo ( lua_State * L )// ( const QString & userInfo )
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	//QString* userInfo = ValueInstaller2<QString>::check( L, 2 );
	lhs->setUserInfo( Util::toString( L, 2 ) );
	return 0;
}
int Url::setUserName ( lua_State * L )// ( const QString & userName )
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	//QString* userName = ValueInstaller2<QString>::check( L, 2 );
	lhs->setUserName( Util::toString( L, 2 ) );
	return 0;
}
int Url::toEncoded ( lua_State * L )// ( FormattingOptions options = None ) const : QByteArray
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	Enums enums(L);
	QUrl::FormattingOptions f=(QUrl::FormattingOptions)enums.FormattingOption( 2 );
	//QByteArray* res = ValueInstaller2<QByteArray>::create( L );
	Util::push( L, lhs->toEncoded( f ).data() );
	return 1;
}
int Url::toLocalFile ( lua_State * L )// const : QString
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	lua_pushstring(L, lhs->toLocalFile().toLatin1() );
	return 1;
}
int Url::toString ( lua_State * L )// ( FormattingOptions options = None ) const : QString
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	Enums enums(L);
	QUrl::FormattingOptions f=(QUrl::FormattingOptions)enums.FormattingOption( 2 );
	lua_pushstring(L, lhs->toString( f ) );
	return 1;
}
int Url::userInfo ( lua_State * L )// const : QString
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	lua_pushstring(L, lhs->userInfo().toLatin1() );
	return 1;
}
int Url::userName ( lua_State * L )// const : QString
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	lua_pushstring(L, lhs->userName().toLatin1() );
	return 1;
}
int Url::notEquals ( lua_State * L )// ( const QUrl & url ) const : bool
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	QUrl* url = ValueInstaller2<QUrl>::check( L, 2 );
	Util::push( L, !(lhs==url) );
	return 1;
}
int Url::operatorEq ( lua_State * L )// ( const QUrl & url ) const : QUrl
//( const QString & url ) const : QUrl
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	if(QUrl* url = ValueInstaller2<QUrl>::check( L, 2 ))
	{
		QUrl* res = ValueInstaller2<QUrl>::create( L );
		res =  url ;
	}
	else
	{
		QUrl* res = ValueInstaller2<QUrl>::create( L );
		res =  url ;
	}
	return 1;
}
int Url::__eq ( lua_State * L )// ( const QUrl & url ) const : bool
{
	QUrl* lhs = ValueInstaller2<QUrl>::check( L, 1 );
	QUrl* url = ValueInstaller2<QUrl>::check( L, 2 );
	Util::push( L, (lhs==url) );
	return 1;
}
static const luaL_reg _Url[] = 
{
	{ Util::s_init, Url::init },
	{ "addQueryItem", Url::addQueryItem },
	{ "allQueryItemValues", Url::allQueryItemValues },
	{ "authority", Url::authority },
	{ "clear", Url::clear },
	{ "encodedQuery", Url::encodedQuery },
	{ "errorString", Url::errorString },
	{ "fragment", Url::fragment },
	{ "hasFragment", Url::hasFragment },
	{ "hasQuery", Url::hasQuery },
	{ "hasQueryItem", Url::hasQueryItem },
	{ "host", Url::host },
	{ "isEmpty", Url::isEmpty },
	{ "isParentOf", Url::isParentOf },
	{ "isRelative", Url::isRelative },
	{ "isValid", Url::isValid },
	{ "password", Url::password },
	{ "path", Url::path },
	{ "port", Url::port },
	{ "queryItemValue", Url::queryItemValue },
	//QList<QPair<QString, QString> > queryItems () const
	{ "queryPairDelimiter", Url::queryPairDelimiter },
	{ "queryValueDelimiter", Url::queryValueDelimiter },
	{ "removeAllQueryItems", Url::removeAllQueryItems },
	{ "removeQueryItem", Url::removeQueryItem },
	{ "resolved", Url::resolved },
	{ "scheme", Url::scheme },
	{ "setAuthority", Url::setAuthority },
	{ "setEncodedQuery", Url::setEncodedQuery },
	{ "setEncodedUrl", Url::setEncodedUrl },
	{ "setFragment", Url::setFragment },
	{ "setHost", Url::setHost },
	{ "setPassword", Url::setPassword },
	{ "setPath", Url::setPath },
	{ "setPort", Url::setPort },
	//{ "setQueryDelimiters", Url::setQueryDelimiters },
	//void setQueryItems ( const QList<QPair<QString, QString> > & query )
	{ "setScheme", Url::setScheme },
	{ "setUrl", Url::setUrl },
	{ "setUserInfo", Url::setUserInfo },
	{ "setUserName", Url::setUserName },
	{ "toEncoded", Url::toEncoded },
	{ "toLocalFile", Url::toLocalFile },
	{ "toString", Url::toString },
	{ "userInfo", Url::userInfo },
	{ "userName", Url::userName },
	{ "notEquals", Url::notEquals },
	{ "operatorEq", Url::operatorEq },
	{ "__eq", Url::__eq },
	{ 0, 0 }
};
void Url::install(lua_State * L){
	ValueInstaller2<QUrl>::install( L, _Url, "QUrl" );
}
