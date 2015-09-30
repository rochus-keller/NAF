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

#include "DomDocument.h"
#include "XmlHandler.h"
#include <fstream>
#include <deque>
using namespace Xml;

class _DomParser : public XmlHandler
{
	typedef std::deque<QByteArray> Stack;
	Root::Ref<DomElement> d_root;
	Root::Ref<DomElement> d_cur;
	Stack d_stack;
public:
	DomElement* getRoot() const { return d_root; }
	bool characters( const QByteArray& txt )
	{
		assert( !d_stack.empty() );
		d_stack.back() += txt;
		return true;
	}
	bool endElement( Root::SymbolString name )
	{
		checkText();
		assert( !d_cur.isNull() );
		if( d_cur->getName() != name )
			return false;
		d_cur = d_cur->getParent();
		d_stack.pop_back();
		return true;
	}
	bool startElement( Root::SymbolString name, const Atts& atts )
	{
		Root::Ref<DomElement> e = new DomElement();
		if( d_cur.isNull() )
		{
			if( !d_root.isNull() )
			{
				d_error = "More than one top level element detected";
				return false;
			}
			d_root = e;
		}else
			checkText();
		e->setName( name );
		Atts::const_iterator i;
		for( i = atts.begin(); i != atts.end(); ++i )
		{
			e->setAttribute( i.key(), i.value() );
		}
		if( d_cur )
			d_cur->append( e );
		d_cur = e;
		d_stack.push_back("");
		return true;
	}
	void checkText()
	{
		assert( !d_stack.empty() );
		if( !d_stack.back().isEmpty() )
		{
			Root::Ref<DomText> t = new DomText();
			t->setText( d_stack.back() );
			assert( !d_cur.isNull() );
			d_cur->append( t );
			d_stack.back() = "";
		}
	}
};

DomDocument::DomDocument()
{
	d_root = new DomElement();
}

DomDocument::~DomDocument()
{
	// Um Stacküberlauf bei grossen Docs zu verhindern, iteriere hier explizit den
	// den Baum runter mit einem auf dem Heap alloziierten Stack.
	std::deque<DomNode*> stack;
	stack.push_back( d_root );
	DomNode* node;
	while( !stack.empty() )
	{
		node = stack.back();
		if( node->getFirst() )
			stack.push_back( node->getFirst() );
		else if( node->getNext() )
			stack.push_back( node->getNext() );
		else
		{
			stack.pop_back();
			if( !stack.empty() )
			{
				node = stack.back();
				if( node->getFirst() )
					node->clearChildren();
				else
				{
					if( node->d_next )
					{
						node->d_next->d_prev = 0;
						node->d_next->d_parent = 0;
					}
					node->d_next = 0;
				}
			}
		}
	}
}

bool DomElement::clearChildren()
{
	DomNode* node = d_first;
	while( node )
	{
		node->d_parent = 0;
		node = node->d_next;
	}
	d_first = 0;
	d_last = 0;
	return true;
}

bool DomDocument::loadFromFile( const char* path )
{
	d_root = new DomElement();
	_DomParser h;
	if( !h.parse( path ) )
	{
		d_error = h.getError();
		return false;
	}
	DomElement* r = h.getRoot();
	assert( r );
	d_root = r;
	return true; 
}

bool DomDocument::loadFromString( const char* xml )
{
	d_root = new DomElement();
	_DomParser h;
	if( !h.parseString( xml ) )
	{
		d_error = h.getError();
		return false;
	}
	DomElement* r = h.getRoot();
	assert( r );
	d_root = r;
	return true;
}

bool DomDocument::writeToFile( const char* path ) const
{
	std::ofstream out( path );
	if( !out )
		return false;
	return write( out ); 
}

bool DomDocument::write( std::ostream& out ) const
{
	out << "<?xml version='1.0' encoding='ISO-8859-1'?>" << std::endl;
	if( d_root )
		return d_root->write( out );
	else
		return false; 
}

const char* DomElement::getAttribute( Root::SymbolString name ) const
{
	Atts::const_iterator i = d_atts.find( name );
	if( i == d_atts.end() )
		return "";
	else
		return i.value().data();
}

void DomElement::setAttribute( Root::SymbolString n, const char* v )
{
	d_atts[ n ] = v;
}

void DomElement::removeAttribute( Root::SymbolString n )
{
	d_atts.remove( n );
}

bool DomElement::remove( DomNode* child )
{
	if( child == 0 || child->d_parent != this )
		return false;
	if( child == d_first )
		d_first = child->d_next;
	if( child == d_last )
		d_last = child->d_prev;
	if( child->d_prev )
		child->d_prev->d_next = child->d_next;
	if( child->d_next )
		child->d_next->d_prev = child->d_prev;
	child->d_prev = 0;
	child->d_next = 0;
	child->d_parent = 0;
	return true; 
}

bool DomElement::append( DomNode* child )
{
	if( child == 0 || child->d_parent != 0 )
		return false;
	if( d_first == 0 )
		d_first = child;
	else
		d_last->d_next = child;
	child->d_prev = d_last;
	d_last = child;
	child->d_parent = this;
	return true; 
}

bool DomElement::normalize()
{
	return false; // TODO
}

bool DomText::write( std::ostream& out ) const
{
	// Gleicher Code wie in Serializer für Procedure-Output
	const char* str = d_text.data();
	int j = 0;
	int i = 0;
	while( str[ j ] != 0 )
	{
		if( str[j] == '>' )
		{
			out.write( str + i, j - i ); // Schreibe alles bis vor Zeichen
			out << "&gt;";
			i = j + 1;
		}else if( str[j] == '<' )
		{
			out.write( str + i, j - i ); // Schreibe alles bis vor Zeichen
			out << "&lt;";
			i = j + 1;
		}else if( str[j] == '&' )
		{
			out.write( str + i, j - i ); // Schreibe alles bis vor Zeichen
			out << "&amp;";
			i = j + 1;
		}
		++j;
	}
	if( j > i )
		out.write( str + i, j - i );
	return true;
}

static QByteArray encode( const char* str )
{
	// Gleich wie in Serializer
	if( str == 0 )
		str = "";
	QByteArray tmp( str );
    uint len = tmp.length();
    uint i = 0;
    while ( i < len ) 
	{
		if ( tmp[(int)i] == '<' ) 
		{
			tmp.replace( i, 1, "&lt;" );
			len += 3;
			i += 4;
		} else if ( tmp[(int)i] == '"' ) 
		{
			tmp.replace( i, 1, "&quot;" );
			len += 5;
			i += 6;
		} else if ( tmp[(int)i] == '\'' ) 
		{
			tmp.replace( i, 1, "&apos;" );
			len += 5;
			i += 6;
		} else if ( tmp[(int)i] == '&' ) 
		{
			tmp.replace( i, 1, "&amp;" );
			len += 4;
			i += 5;
		} else if ( tmp[(int)i] == '>' && i>=2 && tmp[(int)i-1]==']' && tmp[(int)i-2]==']' ) 
		{
			tmp.replace( i, 1, "&gt;" );
			len += 3;
			i += 4;
		} else 
		{
			++i;
		}
    }
	
    return tmp;
}

bool DomElement::write( std::ostream& out ) const
{
	out << "<" << d_name.data();
	if( !d_atts.empty() )
	{
		out << " ";
		Atts::const_iterator i;
		for( i = d_atts.begin(); i != d_atts.end(); ++i )
			out << i.key().data() << "='" << encode( i.value().data() ).data() << "' ";
	}
	if( d_first )
	{
		out << ">";
		DomNode* n = d_first;
		while( n )
		{
			if( !n->write( out ) )
				return false;
			n = n->d_next;
		}
		out << "</" << d_name.data() << ">";
	}else
		out << "/>";
	return true;
}
