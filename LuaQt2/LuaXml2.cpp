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

#include "LuaXml2.h"
#include <Xml/DomDocument.h>
#include <strstream>
using namespace Lua;
using namespace Xml;

TODO( strstream ersetzen durch aktuelle variante )

const luaL_reg LuaDomDocument2::methods[] =
{
	{"getXml",  &LuaDomDocument2::getXml },
	{"saveToFile",  &LuaDomDocument2::saveToFile },
	{"getDocumentElement",  &LuaDomDocument2::getDocumentElement },
	{0,0}
};

const luaL_reg LuaDomElement2::methods[] =
{
	{"normalize",  &LuaDomElement2::normalize },
	{"isText",  &LuaDomElement2::isText },
	{"isElement",  &LuaDomElement2::isElement },
	{"createElement",  &LuaDomElement2::createElement },
	{"createText",  &LuaDomElement2::createText },
	{"removeThis",  &LuaDomElement2::removeThis },
	{"getNextSibling",  &LuaDomElement2::getNextSibling },
	{"getPrevSibling",  &LuaDomElement2::getPrevSibling },
	{"getLastChild",  &LuaDomElement2::getLastChild },
	{"getFirstChild",  &LuaDomElement2::getFirstChild },
	{"getChildren",  &LuaDomElement2::getChildren },
	{"getAttributes",  &LuaDomElement2::getAttributes },
	{"hasAttribute",  &LuaDomElement2::hasAttribute },
	{"removeAttribute",  &LuaDomElement2::removeAttribute },
	{"setAttribute",  &LuaDomElement2::setAttribute },
	{"getAttribute",  &LuaDomElement2::getAttribute },
	{"getName",  &LuaDomElement2::getName },
	{"setName",  &LuaDomElement2::setName },
	{0,0}
};

const luaL_reg LuaDomText2::methods[] =
{
	{"setText",  &LuaDomText2::setText },
	{"getText",  &LuaDomText2::getText },
	{"isText",  &LuaDomText2::isText },
	{"isElement",  &LuaDomText2::isElement },
	{"removeThis",  &LuaDomText2::removeThis },
	{"getNextSibling",  &LuaDomText2::getNextSibling },
	{"getPrevSibling",  &LuaDomText2::getPrevSibling },
	{0,0}
};

//////////////////////////////////////////////////////////////////////

typedef ValueInstaller< Root::Ref<DomDocument> > DomDocumentValue;
typedef ValueInstaller< Root::Ref<DomElement> > DomElementValue;
typedef ValueInstaller< Root::Ref<DomText> > DomTextValue;

static int openDocument( lua_State* L )
{
	Root::Ref<DomDocument> doc = new DomDocument();
	DomDocumentValue::create( L, doc );
	if( !doc->loadFromFile( luaL_checkstring( L, 1 ) ) )
	{
		luaL_error( L, doc->getError().toLatin1().data() );
	}
	return 1;
}

static int createDocument( lua_State* L )
{
	const char* name = "untitled";
	if( lua_gettop(L) > 0 )
		name = luaL_checkstring( L, 1 );
	Root::Ref<DomDocument> doc = new DomDocument();
	DomDocumentValue::create( L, doc );
	doc->getRoot()->setName( name );
	return 1;
}

static int parseDocument( lua_State* L )
{
	Root::Ref<DomDocument> doc = new DomDocument();
	DomDocumentValue::create( L, doc );
	if( !doc->loadFromString( luaL_checkstring( L, 1 ) ) )
	{
		luaL_error( L, doc->getError().toLatin1().data() );
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
void LuaDomDocument2::install(lua_State *L)
{
	luaL_openlib( L, "xml", xmllib, 0 );
	DomDocumentValue::install( L, methods, "DomDocument", false );
	DomElementValue::install( L, LuaDomElement2::methods, "DomElement", false );
	DomTextValue::install( L, LuaDomText2::methods, "DomText", false );
}

int LuaDomDocument2::getDocumentElement(lua_State *L)
{
	DomDocument* obj = DomDocumentValue::check2( L, 1 );
	DomElementValue::create( L, obj->getRoot() );
	return 1;
}

int LuaDomDocument2::saveToFile(lua_State *L)
{
	DomDocument* obj = DomDocumentValue::check2( L, 1 );
	if( !obj->writeToFile( luaL_checkstring( L, 2 ) ) )
		luaL_error( L, "Error opening file for writing" );
	return 0;
}

int LuaDomDocument2::getXml(lua_State *L)
{
	DomDocument* obj = DomDocumentValue::check2( L, 1 );
    std::ostrstream out;
	obj->write( out );
	lua_pushlstring(L, out.str(), out.pcount() ); 
	out.rdbuf()->freeze(false);
	return 1;
}

int LuaDomElement2::getName(lua_State *L)
{
	DomElement* obj = DomElementValue::check2( L, 1 );
	lua_pushstring(L, obj->getName().data() ); 
	return 1;
}

int LuaDomElement2::setName(lua_State *L)
{
	DomElement* obj = DomElementValue::check2( L, 1 );
	obj->setName( luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaDomElement2::getAttribute(lua_State *L)
{
	DomElement* obj = DomElementValue::check2( L, 1 );
	QByteArray str = obj->getAttribute( luaL_checkstring( L, 2 ) );
	if( str.isNull() )
		lua_pushnil( L ); 
	else
		lua_pushstring(L, str ); 
	return 1;
}

int LuaDomElement2::setAttribute(lua_State *L)
{
	DomElement* obj = DomElementValue::check2( L, 1 );
	obj->setAttribute( luaL_checkstring( L, 2 ), luaL_checkstring( L, 3 ) );
	return 0;
}

int LuaDomElement2::removeAttribute(lua_State *L)
{
	DomElement* obj = DomElementValue::check2( L, 1 );
	obj->removeAttribute( luaL_checkstring( L, 2 ) );
	return 0;
}

int LuaDomElement2::hasAttribute(lua_State *L)
{
	DomElement* obj = DomElementValue::check2( L, 1 );
	lua_pushboolean(L, obj->hasAttribute( luaL_checkstring( L, 2 ) ) ); 
	return 1;
}

int LuaDomElement2::getAttributes(lua_State *L)
{
	DomElement* obj = DomElementValue::check2( L, 1 );
	const DomElement::Atts& n = obj->getAttributes();
	lua_newtable( L );
	int t = lua_gettop( L );
	DomElement::Atts::const_iterator i;
	for( i = n.begin(); i != n.end(); ++i )
	{
		lua_pushstring( L, i.key() );
		lua_pushstring( L, i.value().data() );
		lua_rawset( L, t );
	}
	return 1;
}

static bool pushNode( lua_State *L, DomNode* n )
{
	if( n == 0 )
	{
		lua_pushnil( L ); 
		return true;
	}else if( n->isElement() )
	{
		DomElementValue::create( L, static_cast<DomElement*>( n ) );
		return true;
	}else if( n->isText() )
	{
		DomTextValue::create( L, static_cast<DomText*>( n ) );
		return true;
	}else
		return false;
}

int LuaDomElement2::getChildren(lua_State *L)
{
	DomElement* obj = DomElementValue::check2( L, 1 );
	DomNode* n = obj->getFirst();
	lua_newtable( L );
	int t = lua_gettop( L );
	int j = 1;
	while( n )
	{
		if( pushNode( L, n ) )	// Wir wollen nur Text und Element
		{
			lua_rawseti( L, t, j );
			j++;
		}
		n = n->getNext();
	}
	return 1;
}

int LuaDomElement2::getFirstChild(lua_State *L)
{
	DomElement* obj = DomElementValue::check2( L, 1 );
	pushNode( L, obj->getFirst() );
	return 1;
}

int LuaDomElement2::getLastChild(lua_State *L)
{
	DomElement* obj = DomElementValue::check2( L, 1 );
	pushNode( L, obj->getLast() );
	return 1;
}

int LuaDomElement2::getPrevSibling(lua_State *L)
{
	DomElement* obj = DomElementValue::check2( L, 1 );
	pushNode( L, obj->getPrev() );
	return 1;
}

int LuaDomElement2::getNextSibling(lua_State *L)
{
	DomElement* obj = DomElementValue::check2( L, 1 );
	pushNode( L, obj->getNext() );
	return 1;
}

int LuaDomElement2::removeThis(lua_State *L)
{
	DomElement* obj = DomElementValue::check2( L, 1 );
	DomElement* p = obj->getParent(); 
	if( p == 0 || !p->remove( obj ) )
		luaL_error( L, "Error removing this node" );
	return 0;
}

int LuaDomElement2::createText(lua_State *L)
{
	DomElement* obj = DomElementValue::check2( L, 1 );
	// Param: Text (optional)
	QByteArray txt;
	if( lua_gettop(L) > 1 )
		txt = luaL_checkstring( L, 2 );
	Root::Ref<DomText> e = new DomText();
	e->setText( txt );
	obj->append( e );
	DomTextValue::create( L, e );
	return 1;
}

int LuaDomElement2::createElement(lua_State *L)
{
	DomElement* obj = DomElementValue::check2( L, 1 );
	// Param: Name
	// Erzeugt ein neues Element und fügt es ans Ende der Liste
	Root::Ref<DomElement> e = new DomElement();
	e->setName( luaL_checkstring( L, 2 ) );
	DomElementValue::create( L, e );
	obj->append( e );
	return 1;
}

int LuaDomElement2::isElement(lua_State *L)
{
	DomElement* obj = DomElementValue::check2( L, 1 );
	lua_pushboolean(L, obj->isElement() ); 
	return 1;
}

int LuaDomElement2::isText(lua_State *L)
{
	DomElement* obj = DomElementValue::check2( L, 1 );
	lua_pushboolean(L, obj->isText() ); 
	return 1;
}

int LuaDomElement2::normalize(lua_State *L)
{
	DomElement* obj = DomElementValue::check2( L, 1 );
	obj->normalize();
	return 0;
}

int LuaDomText2::getPrevSibling(lua_State *L)
{
	DomText* obj = DomTextValue::check2( L, 1 );
	pushNode( L, obj->getPrev() );
	return 1;
}

int LuaDomText2::getNextSibling(lua_State *L)
{
	DomText* obj = DomTextValue::check2( L, 1 );
	pushNode( L, obj->getNext() );
	return 1;
}

int LuaDomText2::removeThis(lua_State *L)
{
	DomText* obj = DomTextValue::check2( L, 1 );
	DomElement* p = obj->getParent(); 
	if( p == 0 || !p->remove( obj ) )
		luaL_error( L, "Error removing this node" );
	return 0;
}

int LuaDomText2::isElement(lua_State *L)
{
	DomText* obj = DomTextValue::check2( L, 1 );
	lua_pushboolean(L, obj->isElement() ); 
	return 1;
}

int LuaDomText2::isText(lua_State *L)
{
	DomText* obj = DomTextValue::check2( L, 1 );
	lua_pushboolean(L, obj->isText() ); 
	return 1;
}

int LuaDomText2::getText(lua_State *L)
{
	DomText* obj = DomTextValue::check2( L, 1 );
	lua_pushstring(L, obj->getText() ); 
	return 1;
}

int LuaDomText2::setText(lua_State *L)
{
	DomText* obj = DomTextValue::check2( L, 1 );
	obj->setText( luaL_checkstring( L, 1 ) );
	return 0;
}
