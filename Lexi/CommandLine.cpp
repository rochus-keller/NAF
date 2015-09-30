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

#include "CommandLine.h"
#include <qstring.h>
#include <Lexi/Event.h>
#include <Root/MessageLog.h>
using namespace Lexi;
using namespace Root;

static const char CR = 13;
static const char LF = 10;
static const char BK = 8;
static const char SP = 32;

CommandLine::CommandLine(QObject* handler, QObject* output, bool caseSens):
	d_actionHandler( handler ), d_outputHandler( output ), 
	d_caseSensitive( caseSens ), d_active( true ),
	d_state( Idle ), d_current( 0 )
{
	if( handler == 0 )
		throw Root::Exception( "need a handler" );
	ActionCommand* help = new ActionCommand( Action::HelpContext, "?", "Help", false );
	help->registerParameter( Any::CStr, false, "Mnemonic" );
	registerCommand( help );
}

CommandLine::~CommandLine()
{
	Commands::iterator i;
	for( i = d_commands.begin(); i != d_commands.end(); ++i )
        delete i.value();
	d_commands.clear();

}

bool CommandLine::handle(const Allocation &, GlyphTraversal & t)
{
	if( !d_active )
		return false;

	if( KeyEvent* e = dynamic_cast<KeyEvent*>( t.getMessage() ) )
	{
		if( !e->isPlainKey() && !e->isChar() )
			return false;

		if( !acceptable( e->getChar() ) )
			return false;

		if( e->keyPress() )
			return parse( e->getChar() );
		else if( e->keyRelease() )
			return consume( e->getChar() );
		else
			return false;
	}else
		return false;
}



void CommandLine::notify(const QByteArray &message)
{
	CommandLineMessage msg( message );
	if( d_outputHandler != nil )
		d_outputHandler->event( &msg );
	else
		d_actionHandler->event( &msg );
		
}

void CommandLine::registerCommand(ActionCommand * cmd )
{
	QString str( cmd->getSymbol() );
	if( !d_caseSensitive )
		str = str.toUpper();

	ActionCommand* old = d_commands[ str.toLatin1().data() ];
	if( old && old )
		delete old;
	d_commands[ str.toLatin1().data() ] = cmd;
}

static void _printToLog( CommandLine* cl )
{
	const CommandLine::Commands& cmds = cl->getCmds();
	CommandLine::Commands::const_iterator p;
	QString out;
	for( p = cmds.begin(); p != cmds.end(); ++p )
	{
		out += p.value()->getSymbol();
		out += "\t";
		out += p.value()->getPrettyName();
		out += "\n";
	}
	out += "For detailed help enter '? <command>'";
	out += "\n";
	MessageLog::inst()->info( "CmdStr Line", out.toLatin1() );
}

static const char* _TypeName(Root::Any::Type t)
{
	if( t == Root::Any::Memo )
		return "Command String";
	else
		return Any::getTypeName( t );
}

static void _printToLog( ActionCommand * cmd )
{
	QString str;
	str += cmd->getSymbol();
	str += "\t";
	str += cmd->getPrettyName();
	str += "\t";
	for( int i = 0; i < cmd->getParamCount(); i++ )
	{
		str += " ";
		if( i < cmd->getMandatory() )
		{
			str += cmd->getName( i );
			str += ": ";
			str += _TypeName( cmd->getType( i ) );
		}else
		{
			str += "[";
			str += cmd->getName( i );
			str += ": ";
			str += _TypeName( cmd->getType( i ) );
			str += "]";
		}
	}
	MessageLog::inst()->info( "Command Line", str.toLatin1() );
}

void CommandLine::execute(ActionCommand * cmd)
{
	QString str;
	if( cmd->getCommand() == Action::HelpContext )
	{
		if( cmd->getParam( 0 ).isNull() )
			_printToLog( this );
		else
		{
			ActionCommand* c = find( cmd->getParam( 0 ).getCStr() );
			if( c )
				_printToLog( c );
			else
				_printToLog( this );
		}
		str.sprintf( "%s: executed", cmd->getPrettyName() );
		notify( str );
		reset();
		return;
	}
	cmd->update();
	d_actionHandler->event( cmd );
	if( !cmd->isConsumed() )
	{
		str.sprintf( "%s: can't find a handler for this command", cmd->getPrettyName() );
		notify( str );
		reset();
		return;
	}
	if( !cmd->isEnabled() )
	{
		str.sprintf( "%s: not available in this state!", cmd->getPrettyName() );
		notify( str );
		reset();
		return;
	}
	str.sprintf( "%s: executed", cmd->getPrettyName() );
	notify( str );
	cmd->execute();
	d_actionHandler->event( cmd );
	if( !cmd->isConsumed() )
	{
		str.sprintf( "%s: could not execute", cmd->getPrettyName() );
		notify( str );
	}
	reset();
}

void CommandLine::reset()
{
	d_state = Idle;
	if( d_current )
		d_current->clear();
	d_current = 0;
	d_line = "";
}

bool CommandLine::parse(char ch)
{
	switch( d_state )
	{
	case Idle:
		handleIdle( ch );
		break;
	case ReadSymbol:
		handleReadSymbol( ch );
		break;
	case ReadParam:
		handleReadParam( ch );
		break;
	}
	return true;
}

bool CommandLine::consume(char ch)
{
	return d_line.isEmpty() || d_line[ d_line.size() - 1 ] == ch;
}

void CommandLine::handleIdle(char ch)
{
	switch( ch )
	{
	case CR:
	case LF:
	case BK:
	case SP:
		break;
	default:
		d_state = ReadSymbol;
		d_line = "";
		handleReadSymbol( ch );
		break;
	}
}

void CommandLine::handleReadSymbol(char ch)
{
	ActionCommand* cmd;
	QString str;
	switch( ch )
	{
	case LF:
		break;
	case CR:
		// Prepare for Execution
		cmd = find( d_line.data() );
		if( cmd )
			execute( cmd );
		else
		{
			str.sprintf( "%s: Invalid Command!", d_line.data() );
			notify( str );
			d_line = "";
			d_state = Idle;
		}
		break;
	case BK:
		d_line.chop(1);
		if( d_line.isEmpty() )
		{
			notify( "Idle" );
			d_state = Idle;
		}else
			writeLine( d_line.data() );
		break;
	case SP:
		// Prepare for Parameter
		cmd = find( d_line.data() );
		if( cmd )
		{
			d_current = cmd;
			d_line = "";
			d_currentParam = 0;
			if( cmd->getParamCount() > 0 )
			{
				d_state = ReadParam;
				writeCommand();
			}else
			{
				str.sprintf( "%s: Command has no Parameters!", cmd->getPrettyName() );
				notify( str );
			}
		}else
		{
			str.sprintf( "%s: Invalid Command!", d_line.data() );
			notify( str );
		}
		break;
	default:
		// Check if line equals a Symbol
		d_line += ch;
		writeLine( d_line.data() );
		cmd = find( d_line.data() );
		if( cmd )
		{
			if( cmd->isImmediate() )
				execute( cmd );
			else
			{
				d_line = "";
				d_currentParam = 0;
				d_current = cmd;
				d_state = ReadParam;
				writeCommand();
			}
		}
		break;
	}
}

void CommandLine::handleReadParam(char ch)
{
	// TODO: fertig implementieren.
	QString str;
	switch( ch )
	{
	case LF:
		break;
	case CR:
		if( interpretParam() )
		{
			if( d_currentParam < d_current->getMandatory() - 1 )
			{
				str.sprintf( "%s: Command needs %d Parameter(s)!", d_current->getPrettyName(),
					d_current->getMandatory() );
				notify( str );
			}else
				execute( d_current );
		}
		break;
	case BK:
		d_line.chop( 1 );
		if( d_line.isEmpty() )
		{
			d_currentParam--;
			if( d_currentParam >= 0 && d_currentParam < d_current->getParamCount() )
			{
				d_line = d_current->getParam( d_currentParam ).getCStr();
				writeLine( d_current->getPrettyName(), d_line.data() );
			}else
			{
				notify( "Idle" );
				reset();
			}
		}else
			writeLine( d_current->getPrettyName(), d_line.data() );
		break;
	case SP:
		if( d_currentParam < d_current->getParamCount() &&
			d_current->getType( d_currentParam ) == Root::Any::Memo )
		{
			d_line += ch;
			writeLine( d_current->getPrettyName(), d_line.data() );
		}else if( d_currentParam >= d_current->getParamCount() - 1 )
		{	
			str.sprintf( "%s: Command accepts only %d Parameter(s)!", 
				d_current->getPrettyName(),	d_current->getParamCount() );
			notify( str );
		}else
		{
			// Beginne den nächsten Parameter. Validiere den vorhergehenden.
			if( interpretParam() )
				nextParam();
		}
		break;
	default:
		// Check if line equals a Symbol
		d_line += ch;
		writeLine( d_current->getPrettyName(), d_line.data() );
		break;
	}
}

void CommandLine::writeLine(const char * lhs, const char * rhs)
{
	QString par;
	for( int i = 0; d_current && i < d_currentParam; i++ )
	{
		par += d_current->getParam( i ).getCStr();
		par += " ";
	}
	par += rhs;

	QString str;
    str.sprintf( "Input> %s %s", lhs, par.toAscii().data() );
	notify( str );
}

ActionCommand* CommandLine::find(const char * s) const
{
	QByteArray str( s );
	if( !d_caseSensitive )
		str = str.toUpper();
	Commands::const_iterator pos = d_commands.find( str );
	if( pos != d_commands.end() )
        return pos.value();
	else
		return nil;

}

bool CommandLine::acceptable(char ch) const
{
	switch( ch )
	{
	case CR:
	case BK:
		return true;
	}
	QChar test( ch );
	return test.isPrint();
}

bool CommandLine::interpretParam()
{
	QString str;
	Any val;
	bool ok = true;
	if( d_line.isEmpty() )
	{
		if( d_currentParam < d_current->getMandatory() )
		{
			str.sprintf( "%s: at least %d Parameters expected!", d_current->getPrettyName(),
				d_current->getMandatory() );
			notify( str );
			return false;
		}else
		{
			d_current->setParam( d_currentParam, val );
			return true;
		}
	}
	str = d_line.data();
	switch( d_current->getType( d_currentParam ) )
	{
	case Any::Null:
		qDebug( "CommandLine: Invalid Parameter Type!" );
		return false;
	case Any::Boolean:
		str = str.toUpper();
		if( str == "TRUE" || str == "YES" || str == "1" || str == "ON" )
			val.setBoolean( true );
		else if( str == "FALSE" || str == "NO" || str == "0" || str == "OFF" )
			val.setBoolean( false );
		else
		{
			str.sprintf( "%s: '%s' invalid Boolean!", d_current->getPrettyName(), d_line.data() );
			notify( str );
			return false;
		}
		break;
	case Any::Char:
	case Any::UChar:
		if( d_line.size() != 1 )
		{
			str.sprintf( "%s: '%s' invalid Char!", d_current->getPrettyName(), d_line.data() );
			notify( str );
			return false;
		}else
			val.setChar( d_line[ 0 ] );
		break;
	case Any::Short:
		val.setShort( str.toShort( &ok ) );
		if( !ok )
		{
			str.sprintf( "%s: '%s' invalid Short!", d_current->getPrettyName(), d_line.data() );
			notify( str );
			return false;
		}
		break;
	case Any::UShort:
		val.setUShort( str.toUShort( &ok ) );
		if( !ok )
		{
			str.sprintf( "%s: '%s' invalid UShort!", d_current->getPrettyName(), d_line.data() );
			notify( str );
			return false;
		}
		break;
	case Any::Long:
		val.setLong( str.toLong( &ok ) );
		if( !ok )
		{
			str.sprintf( "%s: '%s' invalid Long!", d_current->getPrettyName(), d_line.data() );
			notify( str );
			return false;
		}
		break;
	case Any::ULong:
		val.setULong( str.toULong( &ok ) );
		if( !ok )
		{
			str.sprintf( "%s: '%s' invalid ULong!", d_current->getPrettyName(), d_line.data() );
			notify( str );
			return false;
		}
		break;
	case Any::Float:
		val.setFloat( str.toFloat( &ok ) );
		if( !ok )
		{
			str.sprintf( "%s: '%s' invalid Float!", d_current->getPrettyName(), d_line.data() );
			notify( str );
			return false;
		}
		break;
	case Any::Double:
		val.setDouble( str.toDouble( &ok ) );
		if( !ok )
		{
			str.sprintf( "%s: '%s' invalid Double!", d_current->getPrettyName(), d_line.data() );
			notify( str );
			return false;
		}
		break;
	default:
		val.setCStr( d_line.data() );
	};
	d_current->setParam( d_currentParam, val );
	return true;
}

bool CommandLine::nextParam()
{
	d_currentParam++;
	d_line = "";
	return true;
}

void CommandLine::writeLine(const char * s)
{
	QString str;
	str.sprintf( "Input> %s", s );
	notify( str );
}

void CommandLine::writeCommand()
{
	QByteArray str( d_current->getPrettyName() );
	for( int i = 0; i < d_current->getParamCount(); i++ )
	{
		str += " ";
		if( i < d_current->getMandatory() )
			str += _TypeName( d_current->getType( i ) );
		else
		{
			str += "[";
			str += _TypeName( d_current->getType( i ) );
			str += "]";
		}
	}
	str += " <Enter>";
	writeLine( str );
}
