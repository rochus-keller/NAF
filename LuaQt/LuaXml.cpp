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

#include "LuaXml.h"
#include <qfile.h>
using namespace Spec;

const char LuaDomDocument::s_className[] = "DomDocument";

PeerHelper<LuaDomDocument>::MethodSlot LuaDomDocument::s_methods[] = 
{
	{"getXml",  &LuaDomDocument::getXml },
	{"saveToFile",  &LuaDomDocument::saveToFile },
	{"getDocumentElement",  &LuaDomDocument::getDocumentElement },
	{0,0}
};

const char LuaDomElement::s_className[] = "DomElement";

PeerHelper<LuaDomElement>::MethodSlot LuaDomElement::s_methods[] = 
{
	{"normalize",  &LuaDomElement::normalize },
	{"isText",  &LuaDomElement::isText },
	{"isElement",  &LuaDomElement::isElement },
	{"createElement",  &LuaDomElement::createElement },
	{"createText",  &LuaDomElement::createText },
	{"removeThis",  &LuaDomElement::removeThis },
	{"getNextSibling",  &LuaDomElement::getNextSibling },
	{"getPrevSibling",  &LuaDomElement::getPrevSibling },
	{"getLastChild",  &LuaDomElement::getLastChild },
	{"getFirstChild",  &LuaDomElement::getFirstChild },
	{"getChildren",  &LuaDomElement::getChildren },
	{"getAttributes",  &LuaDomElement::getAttributes },
	{"hasAttribute",  &LuaDomElement::hasAttribute },
	{"removeAttribute",  &LuaDomElement::removeAttribute },
	{"setAttribute",  &LuaDomElement::setAttribute },
	{"getAttribute",  &LuaDomElement::getAttribute },
	{"getName",  &LuaDomElement::getName },
	{"setName",  &LuaDomElement::setName },
	{0,0}
};

const char LuaDomText::s_className[] = "DomText";

PeerHelper<LuaDomText>::MethodSlot LuaDomText::s_methods[] = 
{
	{"setText",  &LuaDomText::setText },
	{"getText",  &LuaDomText::getText },
	{"isText",  &LuaDomText::isText },
	{"isElement",  &LuaDomText::isElement },
	{"removeThis",  &LuaDomText::removeThis },
	{"getNextSibling",  &LuaDomText::getNextSibling },
	{"getPrevSibling",  &LuaDomText::getPrevSibling },
	{0,0}
};

//////////////////////////////////////////////////////////////////////

static int openDocument( lua_State* L )
{
	QFile f( luaL_checkstring( L, 1 ) );
	if( !f.open( IO_ReadOnly ) )
		luaL_error( L, "Error opening file" );
	LuaDomDocument* doc = PeerHelper<LuaDomDocument>::instance( L );
	if( !doc->d_obj.setContent( &f ) )
	{
		f.close();
		luaL_error( L, "Error parsing XML document: syntax errors" );
		// TODO: vernünftige Fehlermeldung
	}
	f.close();
	return 1;
}

static int createDocument( lua_State* L )
{
	const char* xml = "<?xml version='1.0' encoding='ISO-8859-1'?><%s/>";
	QString str;
	if( lua_gettop(L) > 0 )
		str.sprintf( xml, luaL_checkstring( L, 1 ) );
	else
		str.sprintf( xml, "untitled" );
	LuaDomDocument* doc = PeerHelper<LuaDomDocument>::instance( L );
	doc->d_obj.setContent( str ); // RISK: dirty trick.
	return 1;
}

static int parseDocument( lua_State* L )
{
	LuaDomDocument* doc = PeerHelper<LuaDomDocument>::instance( L );
	if( !doc->d_obj.setContent( QByteArray( luaL_checkstring( L, 1 ) ) ) )
	{
		luaL_error( L, "Error parsing XML document: syntax errors" );
		// TODO: vernünftige Fehlermeldung
	}
	return 1;
}

static const luaL_reg xmllib[] = 
{
	{ "parseDocument", parseDocument },
	{ "createDocument", createDocument },
	{ "openDocument", openDocument },
	{ 0, 0 }
};

//////////////////////////////////////////////////////////////////////

void LuaDomDocument::install(lua_State *L)
{
	luaL_openlib( L, "xml", xmllib, 0 );
	PeerHelper<LuaDomDocument>::install( L );
	PeerHelper<LuaDomElement>::install( L );
}

LuaDomDocument::LuaDomDocument()
{

}

LuaDomDocument::~LuaDomDocument()
{

}

int LuaDomDocument::getDocumentElement(lua_State *L)
{
	LuaDomElement* e = PeerHelper<LuaDomElement>::instance( L );
	e->d_obj = d_obj.documentElement();
	return 1;
}

int LuaDomDocument::saveToFile(lua_State *L)
{
	QFile f( luaL_checkstring( L, 1 ) );
	if( !f.open( IO_WriteOnly ) )
		luaL_error( L, "Error opening file for writing" );
	QByteArray str = d_obj.toByteArray();
	f.writeBlock( str, str.length() );
	f.close();
	return 0;
}

int LuaDomDocument::getXml(lua_State *L)
{
	QByteArray str = d_obj.toByteArray();
	lua_pushstring(L, str ); 
	return 1;
}

int LuaDomElement::getName(lua_State *L)
{
	lua_pushstring(L, d_obj.tagName() ); 
	return 1;
}

int LuaDomElement::setName(lua_State *L)
{
	d_obj.setTagName( luaL_checkstring( L, 1 ) );
	return 0;
}

int LuaDomElement::getAttribute(lua_State *L)
{
	QString str = d_obj.attribute( luaL_checkstring( L, 1 ) );
	if( str.isNull() )
		lua_pushnil( L ); 
	else
		lua_pushstring(L, str ); 
	return 1;
}

int LuaDomElement::setAttribute(lua_State *L)
{
	d_obj.setAttribute( luaL_checkstring( L, 1 ), luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaDomElement::removeAttribute(lua_State *L)
{
	d_obj.removeAttribute( luaL_checkstring( L, 1 ) );
	return 0;
}

int LuaDomElement::hasAttribute(lua_State *L)
{
	lua_pushboolean(L, d_obj.hasAttribute( luaL_checkstring( L, 1 ) ) ); 
	return 1;
}

int LuaDomElement::getAttributes(lua_State *L)
{
	QDomNamedNodeMap n = d_obj.attributes();
	lua_newtable( L );
	int t = lua_gettop( L );
	for( int i = 0; i < n.length(); ++i )
	{
		QDomAttr a = n.item( i ).toAttr();
		lua_pushstring( L, a.name() );
		lua_pushstring( L, a.value() );
		lua_rawset( L, t );
	}
	return 1;
}

static bool pushNode( lua_State *L, QDomNode n )
{
	if( n.isNull() )
	{
		lua_pushnil( L ); 
		return true;
	}else if( n.isElement() )
	{
		LuaDomElement* e = PeerHelper<LuaDomElement>::instance( L );
		e->d_obj = n.toElement();
		return true;
	}else if( n.isText() )
	{
		LuaDomText* e = PeerHelper<LuaDomText>::instance( L );
		e->d_obj = n.toText();
		return true;
	}else if( n.isCDATASection() )
	{
		QDomNode p = n.parentNode(); 
		QDomText txt = n.ownerDocument().createTextNode( n.nodeValue() );
		p.replaceChild( txt, n );
		LuaDomText* e = PeerHelper<LuaDomText>::instance( L );
		e->d_obj = txt;
		return true;
	}else
		return false;
}

int LuaDomElement::getChildren(lua_State *L)
{
	QDomNodeList n = d_obj.childNodes();
	lua_newtable( L );
	int t = lua_gettop( L );
	int j = 1;
	for( int i = 0; i < n.length(); ++i )
	{
		lua_pushnumber( L, j );
		if( pushNode( L, n.item( i ) ) )	// Wir wollen nur Text und Element
		{
			lua_rawset( L, t );
			j++;
		}else
			lua_pop( L, 2 );
	}
	return 1;
}

int LuaDomElement::getFirstChild(lua_State *L)
{
	pushNode( L, d_obj.firstChild() );
	return 1;
}

int LuaDomElement::getLastChild(lua_State *L)
{
	pushNode( L, d_obj.lastChild() );
	return 1;
}

int LuaDomElement::getPrevSibling(lua_State *L)
{
	pushNode( L, d_obj.previousSibling() );
	return 1;
}

int LuaDomElement::getNextSibling(lua_State *L)
{
	pushNode( L, d_obj.nextSibling() );
	return 1;
}

int LuaDomElement::removeThis(lua_State *L)
{
	QDomNode p = d_obj.parentNode(); 
	d_obj = p.removeChild( d_obj ).toElement();
	if( d_obj.isNull() )
		luaL_error( L, "Error removing this node" );
	return 0;
}

int LuaDomElement::createText(lua_State *L)
{
	// Param: Text (optional)
	QDomDocument doc = d_obj.ownerDocument();
	QString txt;
	if( lua_gettop(L) > 0 )
		txt = luaL_checkstring( L, 1 );
	LuaDomText* e = PeerHelper<LuaDomText>::instance( L );
	e->d_obj = doc.createTextNode( txt );
	d_obj.appendChild( e->d_obj );
	return 1;
}

int LuaDomElement::createElement(lua_State *L)
{
	// Param: Name
	// Erzeugt ein neues Element und fügt es ans Ende der Liste
	QDomDocument doc = d_obj.ownerDocument();
	LuaDomElement* e = PeerHelper<LuaDomElement>::instance( L );
	e->d_obj = doc.createElement( luaL_checkstring( L, 1 ) );
	d_obj.appendChild( e->d_obj );
	return 1;
}

int LuaDomElement::isElement(lua_State *L)
{
	lua_pushboolean(L, d_obj.isElement() ); 
	return 1;
}

int LuaDomElement::isText(lua_State *L)
{
	lua_pushboolean(L, d_obj.isText() ); 
	return 1;
}

int LuaDomElement::normalize(lua_State *L)
{
	d_obj.normalize();
	return 0;
}

int LuaDomText::getPrevSibling(lua_State *L)
{
	pushNode( L, d_obj.previousSibling() );
	return 1;
}

int LuaDomText::getNextSibling(lua_State *L)
{
	pushNode( L, d_obj.nextSibling() );
	return 1;
}

int LuaDomText::removeThis(lua_State *L)
{
	QDomNode p = d_obj.parentNode(); 
	d_obj = p.removeChild( d_obj ).toText();
	if( d_obj.isNull() )
		luaL_error( L, "Error removing this node" );
	return 0;
}

int LuaDomText::isElement(lua_State *L)
{
	lua_pushboolean(L, d_obj.isElement() ); 
	return 1;
}

int LuaDomText::isText(lua_State *L)
{
	lua_pushboolean(L, d_obj.isText() ); 
	return 1;
}

int LuaDomText::getText(lua_State *L)
{
	lua_pushstring(L, d_obj.nodeValue() ); 
	return 1;
}

int LuaDomText::setText(lua_State *L)
{
	d_obj.setNodeValue( luaL_checkstring( L, 1 ) );
	return 0;
}
