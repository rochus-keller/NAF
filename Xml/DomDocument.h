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

#ifndef _Xml_DomDocument
#define _Xml_DomDocument

#include <Root/Resource.h>
#include <Root/SymbolString.h>
#include <QMap>
#include <QString>

namespace Xml
{
	class DomElement;

	class DomNode : public Root::Resource
	{
	public:
		virtual bool isText() const { return false; }
		virtual bool isElement() const { return false; }
		virtual DomNode* getFirst() const { return 0; }
		virtual DomNode* getLast() const { return 0; }
		virtual bool write( std::ostream& ) const { return false; }
		virtual bool append( DomNode* child ) { return false; }
		virtual Root::SymbolString getName() const { return ""; }
		virtual bool clearChildren() { return false; }
		DomNode* getPrev() const { return d_prev; }
		DomNode* getNext() const { return d_next; }
		DomElement* getParent() const { return d_parent; }
	protected:
		friend class DomElement;
		friend class DomDocument;
		Root::Ref<DomNode> d_next;
		Root::Ptr<DomNode> d_prev;
		Root::Ptr<DomElement> d_parent;
	};

	class DomElement : public DomNode
	{
	public:
		typedef QMap<Root::SymbolString,QByteArray > Atts;

		bool isElement() const { return true; }
		DomNode* getFirst() const { return d_first; }
		DomNode* getLast() const { return d_last; }
		const Atts& getAttributes() const { return d_atts; }
		const char* getAttribute( Root::SymbolString ) const;
		void setAttribute( Root::SymbolString, const char* );
		void removeAttribute( Root::SymbolString );
		bool hasAttribute( Root::SymbolString n ) const { return d_atts.contains( n ); }
		Root::SymbolString getName() const { return d_name; }
		void setName( Root::SymbolString n ) { d_name = n; }
		bool remove( DomNode* child );
		bool append( DomNode* child );
		bool normalize();
		bool write( std::ostream& ) const;
		bool clearChildren();

		DomElement():d_name("untitled") {}
	protected:
		Root::Ref<DomNode> d_first;
		Root::Ptr<DomNode> d_last;
		Atts d_atts;
		Root::SymbolString d_name;
	};

	class DomText : public DomNode
	{
	public:
		bool isText() const { return true; }
		const QByteArray& getText() const { return d_text; }
		void setText( const QByteArray& t ) { d_text = t; }
		bool write( std::ostream& ) const;
	private:
		QByteArray d_text;
	};

	class DomDocument : public Root::Resource
	{
	public:
		DomDocument();
		~DomDocument();
		DomElement* getRoot() const { return d_root; }
		const QString& getError() const { return d_error; }
		bool loadFromFile( const char* path );
		bool loadFromString( const char* xml );
		bool writeToFile( const char* path ) const;
		bool write( std::ostream& ) const;
	private:
		Root::Ref<DomElement> d_root;
		QString d_error;
	};
}

#endif // _Xml_DomDocument
