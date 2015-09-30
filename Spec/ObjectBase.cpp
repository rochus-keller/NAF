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

#include "ObjectBase.h"
#include "Repository.h"
#include <stdio.h>
using namespace Spec;

ObjectBase::ObjectBase(Repository* r):d_owner(r)
{
	assert( r );
}

ObjectBase::~ObjectBase()
{

}

Record* ObjectBase::createObject()
{
	Root::Index id = 1;
	if( !d_base.empty() )
	{
		Base::const_iterator p = d_base.end();
		--p;
		id = (*p).first + 1;
	}
	Record* obj = new Record( id );
	d_base[ id ] = obj;
	Record::Added m( obj );
	d_owner->notifyObservers( m );
	return obj;
}

Record* ObjectBase::getObject(Root::UInt32 id) const
{
 	Base::const_iterator p = d_base.find( id );
	if( p == d_base.end() )
		return 0;
	else
		return (*p).second;
}

void ObjectBase::removeObject(Record * obj)
{
	assert( obj );
	Base::const_iterator p = d_base.find( obj->getId() );
	if( p != d_base.end() )
	{
		Root::Ref<Record> tmp = obj;
		d_base.erase( obj->getId() );
		Record::Removed m( obj );
		d_owner->notifyObservers( m );
	}
}

Record::Record( Root::UInt32 id, ObjectBase* o )
{
	d_id = id;
	d_owner = o;
}

Record::~Record()
{
}

QByteArray  Record::getInstanceName(bool sort) const
{
	if( d_id == 0 )
		return "<nil>";

	char buf[16];
	if( sort )
		::sprintf( buf, "%09d", d_id );
	else
		::sprintf( buf, "%d", d_id );
	return buf; // RISK
}

Root::UInt32 Record::getOID() const
{
	return d_id;
}

void Record::setFieldValue( Field f, const Root::Any& v )
{
	Object::setFieldValue( f, v );
	if( d_owner )
	{
		Root::UpdateDynValue msg( this, f );
		d_owner->getOwner()->notifyObservers( msg );
	}
}
