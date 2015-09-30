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

#include "ActionCommand.h"
using namespace Root;

void ActionCommand::setEnabled(bool on)
{
	d_enabled = on;
	if( d_updating )
		consume();
}

Action::CmdSym ActionCommand::getCommand() const
{
	return d_command;
}

bool ActionCommand::isUpdate() const
{
	return d_updating;
}

const char* ActionCommand::getPrettyName() const
{
	if( !d_prettyName.isEmpty() )
		return d_prettyName.data();
	else if( !d_mnemonic.isEmpty() )
		return d_mnemonic.data();
	else
		return d_command;
}

void ActionCommand::execute()
{
    unconsume();
	d_updating = false;
}

void ActionCommand::update()
{
	d_enabled = false;
    unconsume();
    d_updating = true;
}

bool ActionCommand::operator ==(CmdStr cmd) const
{
	return d_command == cmd;
}

bool ActionCommand::isEnabled() const
{
	return d_enabled;
}

const char* ActionCommand::getSymbol() const
{
	if( !d_mnemonic.isEmpty() )
		return d_mnemonic.data();
	else
		return d_command;
}

void ActionCommand::registerParameter(Any::Type t, bool mandatory, const char* name )
{
	if( d_immediate )
		throw Root::Exception( "Params invalid for immediate commands" );

	switch( t )
	{
	case Any::Null:
	case Any::UChar:
		throw Root::Exception( "Invalid param type" );
	default:
		d_types.push_back( t );
	}		
	if( mandatory )
		d_mandatory = d_types.size();

	d_params.push_back( Any() );
	d_names.push_back( name );
}

Index ActionCommand::getParamCount() const
{
	return d_params.size();
}

const Any& ActionCommand::getParam(Index i) const
{
	if( i < d_params.size() )
		return d_params[ i ];
	else
		return Any::null;
}

void ActionCommand::setParam( Index i, const Any & v)
{
	if( i < d_params.size() )
		d_params[ i ] = v;
}

Any::Type ActionCommand::getType(Index i) const
{
	return d_types[ i ];
}

ActionCommand::ActionCommand(CmdSym cmd, const char * mnemonic, const char * name, 
							 bool immediate): d_command( cmd ), 
	d_mnemonic( mnemonic ), d_prettyName( name ), d_immediate( immediate ),
	d_mandatory( 0 )
{

}

ActionCommand::ActionCommand(CmdSym cmd, const Params& p, bool updating): d_command( cmd ), 
	d_immediate( false ), d_mandatory( 0 ), d_params( p ), d_updating( updating )
{

}

void ActionCommand::clear()
{
	if( d_types.size() )
		d_params.fill( Any(), d_types.size() );
}

