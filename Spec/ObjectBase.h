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

#if !defined(AFX_OBJECTBASE_H__C3D0B029_8E40_47EE_8CCF_D91354503F3D__INCLUDED_)
#define AFX_OBJECTBASE_H__C3D0B029_8E40_47EE_8CCF_D91354503F3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Subject.h>
#include <Root/Any.h>
#include <Root/Vector.h>

namespace Spec
{
	class Repository;
	class ObjectBase;

	class Record : public Root::Object
	{
	public:
		Record( Root::UInt32 id = 0, ObjectBase* = 0 );
		~Record();
		Root::UInt32 getId() const { return d_id; }
		ObjectBase* getOwner() const { return d_owner; }

		//* Overrides von Object und DynObject
		virtual void setFieldValue( Field, const Root::Any& );
		QByteArray  getInstanceName(bool = false) const;
		Root::UInt32 getOID() const;
		//-
		UPDATE_MESSAGE_DEF( Added, Record );
		UPDATE_MESSAGE_DEF( Removed, Record );
	private:
		Root::UInt32 d_id;
		ObjectBase* d_owner;
	};

	class ObjectBase : public Root::Resource
	{
	public:
		typedef std::map<Root::UInt32,Root::Ref<Record> > Base;

		const Base& getBase() const { return d_base; }
		Record* getObject( Root::UInt32 ) const;
		Record* createObject();
		void removeObject( Record* );
		Repository* getOwner() const { return d_owner; }

		ObjectBase(Repository*);
	protected:
		virtual ~ObjectBase();
	private:
		friend class _XmlParser;
		Base d_base;
		Repository* d_owner;
	};
}

#endif // !defined(AFX_OBJECTBASE_H__C3D0B029_8E40_47EE_8CCF_D91354503F3D__INCLUDED_)
