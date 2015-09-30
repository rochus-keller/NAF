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
#include "ObjectRedir.h"
using namespace Gui;

ObjectRedir::ObjectRedir(QObject* owner, QObject* target, bool filterOwner ):
	QObject( owner ), d_target( target )
{
	if( filterOwner )
		owner->installEventFilter( this );
}

ObjectRedir::~ObjectRedir()
{

}

bool ObjectRedir::eventFilter(QObject *watched, QEvent *event)
{
	if( event->type() == Root::Message::ID )
	{
		if( d_target )
			return d_target->eventFilter( watched, event );
		else
			return false;
	}else
		return QObject::eventFilter( watched, event );
}

void ObjectRedir::observe(QObject * o)
{
	o->installEventFilter( this );
}

void ObjectRedir::unobserve(QObject *o)
{
	o->removeEventFilter( this );
}
