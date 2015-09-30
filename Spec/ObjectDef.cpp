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

#include "ObjectDef.h"
#include "Repository.h"
using namespace Spec;

ObjectDef::ObjectDef(Root::SymbolString cls):
	d_class( cls ), d_owner( 0 )
{

}

ObjectDef::ObjectDef(Repository* o, Root::SymbolString cls):
	d_class( cls ), d_owner( o )
{

}

ObjectDef::~ObjectDef()
{
}

void ObjectDef::setField(Root::Object::Field f, Root::Any::Type t, const QByteArray  & s)
{
	Attribute& a = d_atts[ f ];
	a.d_type = t;
	a.d_description = s;
	Changed m( this, f );
	if( d_owner )
		d_owner->notifyObservers( m );
}

void ObjectDef::removeField(Root::Object::Field f)
{
	d_atts.erase( f );
	Changed m( this, f );
	if( d_owner )
		d_owner->notifyObservers( m );
}
