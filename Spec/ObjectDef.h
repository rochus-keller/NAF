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

#if !defined(AFX_OBJECTDEF_H__15C89DDC_828C_443B_9A39_28F222ECFFFB__INCLUDED_)
#define AFX_OBJECTDEF_H__15C89DDC_828C_443B_9A39_28F222ECFFFB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Subject.h>
#include <Root/Any.h>
#include <Root/Vector.h>

namespace Spec
{
	class Repository;

	class ObjectDef : public Root::Resource
	{
	public:
		ObjectDef(Root::SymbolString);
		ObjectDef(Repository*, Root::SymbolString);
		Repository* getOwner() const { return d_owner; }

		struct Attribute
		{
			Root::Any::Type d_type;
			QByteArray  d_description;
			Attribute():d_type( Root::Any::Null ) {}
		};
		typedef std::map<Root::Object::Field,Attribute> Attributes;

		Root::SymbolString getClass() const { return d_class; }
		const Attributes& getAttributes() const { return d_atts; }
		void setField( Root::Object::Field, Root::Any::Type, const QByteArray & );
		void removeField( Root::Object::Field );
		UPDATE_MESSAGE_DEF2( Changed, ObjectDef, Root::Object::Field );
	protected:
		virtual ~ObjectDef();
	private:
		friend class Repository;
		Attributes d_atts;
		Root::SymbolString d_class;
		Repository* d_owner;
	};
}

#endif // !defined(AFX_OBJECTDEF_H__15C89DDC_828C_443B_9A39_28F222ECFFFB__INCLUDED_)
