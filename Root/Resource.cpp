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

#include "Resource.h"
#include <Root/Mem.h>
#include <set>
#include <map>
#include "Sequence.h"
#include <qstring.h>
using namespace Root;

#ifdef _DEBUG
static std::set<Resource*>::size_type s_count = 0;
static std::set<Resource*> s_inst;
#endif

void Resource::release() const
{
	d_refCount--;
	if( d_refCount <= 0 )
	{
		if( d_refCount < 0 )
			qDebug( "WARNING: Resource::release deleting d_refCount < 0" );
		delete this;
	}
}

void Resource::addRef() const
{
	d_refCount++;
}

/*
void* Resource::operator new( size_t size ) 
{
    return Mem::alloc( size );
}
    
void Resource::operator delete( void* obj ) 
{
	Mem::free( obj );
}
*/

Resource::Resource( const Resource& )
{
	d_refCount = 0;
#ifdef _DEBUG
	s_count++;
	s_inst.insert( this );
	d_new = true;
#endif
}

Resource::Resource(bool cleanup):
	d_refCount( 0 )
{
#ifdef _DEBUG
	s_count++;
	s_inst.insert( this );
	d_new = true;
#endif
}

Resource::~Resource()
{
	Mem::notifyGuard( this );
#ifdef _DEBUG
	s_count--;
	s_inst.erase( this );
#endif
}

void Resource::requestCleanup()
{
}


void Resource::flushDeferreds()
{
}

void Resource::toVariant(QVariant &v) const
{
	qVariantSetValue<Ref<Resource> >( v, (Resource*)this );
}

Resource* Resource::fromVariant(QVariant & v)
{
	return qVariantValue<Ref<Resource> >( v ); 
}

QVariant Resource::pack() const
{
	return qVariantFromValue<Ref<Resource> >( (Resource*) this ); 
}

QVariant Resource::pack( Resource* r )
{
	return qVariantFromValue<Ref<Resource> >( r ); 
}

bool Resource::write( ValueWriter&, AtomManager* ) const 
{ 
	return false; 
}

bool Resource::read( ValueReader&, AtomManager*, ObjectCreator )
{
	return false;
}


#ifdef _DEBUG
int Resource::getInstanceCount()
{
	assert( s_count == s_inst.size() );
	return s_count;
}

bool Resource::getInstanceList(std::vector<Resource*>& v)
{
	v.assign( s_inst.size(), 0 );
	std::set<Resource*>::const_iterator i;
	int j = 0;
	for( i = s_inst.begin(); i != s_inst.end(); ++i, ++j )
		v[j] = (*i);
	return false;
}

void Resource::printLeftOvers(bool summary, bool newOnly)
{
	if( summary )
	{
		qDebug( "/********************************************************" );
		qDebug( "Instance overview: total numer %d", getInstanceCount() );
		typedef std::map<QByteArray ,int> Base;
		Base base;
		int count = 0;
		std::set<Resource*>::const_iterator i;
		for( i = s_inst.begin(); i != s_inst.end(); ++i )
		{
			if( !newOnly || (*i)->d_new )
			{
				count++;
				base[typeid( *(*i) ).name()]++;
			}
		}

		Base::const_iterator j;
		for( j = base.begin(); j != base.end(); ++j )
			qDebug( "    %4d %s ", (*j).second, (*j).first.data() );
			qDebug( "    %4d", count );
		qDebug( "********************************************************/" );
	}else
	{
		qDebug( "/********************************************************" );
		qDebug( "Instance list: total numer %d", getInstanceCount() );
		typedef std::map<QByteArray , Sequence<Resource*> > Base;
		Base base;
		int count = 0;
		std::set<Resource*>::const_iterator i;
		for( i = s_inst.begin(); i != s_inst.end(); ++i )
		{
			if( !newOnly || (*i)->d_new )
			{
				count++;
				base[typeid( *(*i) ).name()].append( (*i) );
			}
		}

		Base::const_iterator j;
		for( j = base.begin(); j != base.end(); ++j )
		{
			qDebug( "    %4d %s ", (*j).second.size(), (*j).first.data() );
			for( int k = 0; k < (*j).second.size(); k++ )
                qDebug( "          %08x %d ", (unsigned int)(*j).second[k], (*j).second[k]->getRefCount() );
		}
		qDebug( "********************************************************/" );
	}
}

void Resource::checkPoint()
{
	std::set<Resource*>::const_iterator i;
	for( i = s_inst.begin(); i != s_inst.end(); ++i )
		(*i)->d_new = false;
}

#endif
