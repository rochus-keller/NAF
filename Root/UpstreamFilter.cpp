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

#include "UpstreamFilter.h"
#include <Root/Message.h>
#include <Root/Subject.h>
#include <Root/Action.h>
using namespace Root;


UpstreamFilter::UpstreamFilter(Agent* parent, bool action ):
	Agent( parent ), d_action( action )
{
	d_input = false;
	d_window = false;
	d_application = false;
	d_system = false;
	d_internal = false;
	d_user = false;
	d_update = false;
}

UpstreamFilter::~UpstreamFilter()
{

}

void UpstreamFilter::traverseUp(Message& msg )
{
	if( d_action && dynamic_cast<Action*>( &msg ) )
		return;
	else if( d_input && dynamic_cast<InputEvent*>( &msg ) )
		return;
	else if( d_window && dynamic_cast<WindowEvent*>( &msg ) )
		return;
	else if( d_application && dynamic_cast<ApplicationEvent*>( &msg ) )
		return;
	else if( d_system && dynamic_cast<SystemEvent*>( &msg ) )
		return;
	else if( d_internal && dynamic_cast<InternalMessage*>( &msg ) )
		return;
	else if( d_user && dynamic_cast<UserMessage*>( &msg ) )
		return;
	else if( d_update && dynamic_cast<UpdateMessage*>( &msg ) )
		return;
	else
		Agent::traverseUp( msg );
}
