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

#include "LxCommandLine.h"
#include <Root/Exception.h>
#include <Root/Message.h>
#include <Root/MessageLog.h>
#include <QKeyEvent>
#include <QString>
using namespace Root;
using namespace Lexi2;

static const char CR = 13;
static const char LF = 10;
static const char BK = 8;
static const char SP = 32;

static Root::ActionCommand* s_help = 0;

CommandLine::CommandLine(QObject* owner, bool caseSens):
	QObject( owner ), d_caseSensitive( caseSens ), d_active( true ),
	d_state( Idle ), d_current( 0 )
{
	if( s_help == 0 )
	{
		s_help = new Root::ActionCommand( Action::HelpContext, "?", "Help", false );
		s_help->registerParameter( Any::CStr, false, "Mnemonic" );
	}
	registerCommand( s_help );
}

CommandLine::~CommandLine()
{
}

void CommandLine::notify(const char * message)
{
	ReportStatus msg( message );
	msg.sendTo( parent() );
}

void CommandLine::registerCommand(ActionCommand * cmd )
{
	QByteArray str( cmd->getSymbol() );
	if( !d_caseSensitive )
		str = str.toUpper();

	ActionCommand* old = d_commands[ str ];
	d_commands[ str ] = cmd;
}

static void _printToLog( CommandLine* cl )
{
	const CommandLine::Commands& cmds = cl->getCmds();
	CommandLine::Commands::const_iterator p;
	QByteArray out;
	for( p = cmds.begin(); p != cmds.end(); ++p )
	{
		out += p.value()->getSymbol();
		out += "\t";
		out += p.value()->getPrettyName();
		out += "\n";
	}
	out += "For detailed help enter '? <command>'";
	out += "\n";
	MessageLog::inst()->info( "CmdStr Line", out );
}

static const char* _TypeName(Root::Any::Type t)
{
	if( t == Root::Any::Memo )
		return "CmdStr String";
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
	MessageLog::inst()->info( "CmdStr Line", str.toLatin1() );
}

void CommandLine::execute(ActionCommand * cmd)
{
	if( cmd == s_help )
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
		notify( QString( "%1: executed" ).arg( cmd->getPrettyName() ).toLatin1() );
		reset();
		return;
	}

	cmd->update();
	cmd->sendTo( parent() );

	if( !cmd->isConsumed() )
	{
		notify( QString( "%1: can't find a handler for this command" ).arg( 
			cmd->getPrettyName() ).toLatin1() );
		reset();
		return;
	}
	if( !cmd->isEnabled() )
	{
		notify( QString( "%1: not available in this state!" ).
			arg( cmd->getPrettyName() ).toLatin1() );
		reset();
		return;
	}
	notify( QString( "%1: executed" ).arg( cmd->getPrettyName() ).toLatin1() );
	
	cmd->execute();
	cmd->sendTo( parent() );

	if( !cmd->isConsumed() )
	{
		notify( QString( "%1: could not execute" ).
			arg( cmd->getPrettyName() ).toLatin1() );
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
	switch( ch )
	{
	case LF:
		break;
	case CR:
		// Prepare for Execution
		cmd = find( d_line );
		if( cmd )
			execute( cmd );
		else
		{
			notify( QString( "%1: Invalid Command!" ).arg( d_line.data() ).toLatin1() );
			d_line = "";
			d_state = Idle;
		}
		break;
	case BK:
		d_line.truncate( d_line.size() - 1 );
		if( d_line.isEmpty() )
		{
			notify( "Idle" );
			d_state = Idle;
		}else
			writeLine( d_line );
		break;
	case SP:
		// Prepare for Parameter
		cmd = find( d_line );
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
				notify( QString( "%1: Command has no Parameters!" ).
					arg( cmd->getPrettyName() ).toLatin1() );
			}
		}else
		{
			notify( QString( "%1: Invalid Command!" ).
				arg( cmd->getPrettyName() ).toLatin1() );
		}
		break;
	default:
		// Check if line equals a Symbol
		d_line += ch;
		writeLine( d_line );
		cmd = find( d_line );
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
	switch( ch )
	{
	case LF:
		break;
	case CR:
		if( interpretParam() )
		{
			if( d_currentParam < d_current->getMandatory() - 1 )
			{
				notify( QString( "%1: Command needs %2 Parameter(s)!" ).
					arg( d_current->getPrettyName() ).
					arg( d_current->getMandatory() ).toLatin1() );
			}else
				execute( d_current );
		}
		break;
	case BK:
		d_line.truncate( d_line.size() - 1 );
		if( d_line.isEmpty() )
		{
			d_currentParam--;
			if( d_currentParam >= 0 && d_currentParam < d_current->getTypes().size() )
			{
				d_line = d_current->getParam( d_currentParam ).getCStr();
				writeLine( d_current->getPrettyName(), d_line );
			}else
			{
				notify( "Idle" );
				reset();
			}
		}else
			writeLine( d_current->getPrettyName(), d_line );
		break;
	case SP:
		if( d_currentParam < d_current->getTypes().size() &&
			d_current->getType( d_currentParam ) == QVariant::ByteArray )
		{
			d_line += ch;
			writeLine( d_current->getPrettyName(), d_line );
		}else if( d_currentParam >= d_current->getTypes().size() - 1 )
		{	
			notify( QString( "%1: Command accepts only %2 Parameter(s)!" ).
				arg( d_current->getPrettyName() ).
				arg( d_current->getTypes().size() ).toLatin1() );
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
		writeLine( d_current->getPrettyName(), d_line );
		break;
	}
}

void CommandLine::writeLine(const char * lhs, const char * rhs)
{
	QByteArray par;
	for( int i = 0; d_current && i < d_currentParam; i++ )
	{
		par += d_current->getParam( i ).getCStr();
		par += " ";
	}
	par += rhs;

	notify( QString( "Input> %1 %2" ).arg( lhs ).arg( par.data() ).toLatin1() );
}

ActionCommand* CommandLine::find(const char * s) const
{
	QString str( s );
	if( !d_caseSensitive )
		str = str.toUpper();
	Commands::const_iterator pos = d_commands.find( str.toLatin1() );
	if( pos != d_commands.end() )
		return pos.value();
	else
		return 0;

}

bool CommandLine::interpretParam()
{
	QString str;
	Root::Any val;
	bool ok = true;
	if( d_line.isEmpty() )
	{
		if( d_currentParam < d_current->getMandatory() )
		{
			notify( QString( "%1: at least %2 Parameters expected!" ).
				arg( d_current->getPrettyName() ).
				arg( d_current->getMandatory() ).toLatin1() );
			return false;
		}else
		{
			d_current->setParam( d_currentParam, val );
			return true;
		}
	}
	str = d_line;
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
			notify( QString( "%1: '%2' invalid Boolean!" ).
				arg( d_current->getPrettyName() ).
				arg( d_line.data() ).toLatin1() );
			return false;
		}
		break;
	case Any::Char:
	case Any::UChar:
		if( d_line.size() != 1 )
		{
			notify( QString( "%1: '%2' invalid Char!" ).
				arg( d_current->getPrettyName() ).
				arg( d_line.data() ).toLatin1() );
			return false;
		}else
			val.setChar( d_line[ 0 ] );
		break;
	case Any::Short:
		val.setShort( str.toShort( &ok ) );
		if( !ok )
		{
			notify( QString( "%1: '%2' invalid short!" ).
				arg( d_current->getPrettyName() ).
				arg( d_line.data() ).toLatin1() );
			return false;
		}
		break;
	case Any::UShort:
		val.setUShort( str.toUShort( &ok ) );
		if( !ok )
		{
			notify( QString( "%1: '%2' invalid ushort!" ).
				arg( d_current->getPrettyName() ).
				arg( d_line.data() ).toLatin1() );
			return false;
		}
		break;
	case Any::Long:
		val.setLong( str.toLong( &ok ) );
		if( !ok )
		{
			notify( QString( "%1: '%2' invalid Long!" ).
				arg( d_current->getPrettyName() ).
				arg( d_line.data() ).toLatin1() );
			return false;
		}
		break;
	case Any::ULong:
		val.setULong( str.toULong( &ok ) );
		if( !ok )
		{
			notify( QString( "%1: '%2' invalid ULong!" ).
				arg( d_current->getPrettyName() ).
				arg( d_line.data() ).toLatin1() );
			return false;
		}
		break;
	case Any::Float:
		val.setFloat( str.toFloat( &ok ) );
		if( !ok )
		{
			notify( QString( "%1: '%2' invalid Float!" ).
				arg( d_current->getPrettyName() ).
				arg( d_line.data() ).toLatin1() );
			return false;
		}
		break;
	case Any::Double:
		val.setDouble( str.toDouble( &ok ) );
		if( !ok )
		{
			notify( QString( "%1: '%2' invalid Double!" ).
				arg( d_current->getPrettyName() ).
				arg( d_line.data() ).toLatin1() );
			return false;
		}
		break;
	default:
		val.setCStr( d_line );
		break;
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
	notify( str.toLatin1() );
}

void CommandLine::writeCommand()
{
	QString str( d_current->getPrettyName() );
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
	writeLine( str.toLatin1() );
}

bool CommandLine::eventFilter(QObject *watched, QEvent *event)
{
	if( event->type() == QEvent::KeyPress ||
		event->type() == QEvent::KeyRelease )
	{
		QKeyEvent* e = (QKeyEvent*)event;
		QString txt = e->text();
		if( e->modifiers() != Qt::NoModifier && txt.isEmpty() )
			return false;

		const QChar ch = txt[0];
		const char c = ch.toAscii();
		if( !ch.isPrint() && c != CR && c != BK )
			return false;

		if( event->type() == QEvent::KeyPress )
			return parse( c );
		else if( event->type() == QEvent::KeyRelease )
			return consume( c );
		else
			return false;
	}else
		return QObject::eventFilter( watched, event );
}

bool CommandLine::handleKey(bool press, char c)
{
	if( c == 0 )
		return false;

	const QChar ch = c;
	if( !ch.isPrint() && c != CR && c != BK )
		return false;


	if( press )
		return parse( c );
	else 
		return consume( c );
}

ActionCommand* CommandLine::addCommand(Action::CmdSym cmd, const char* mnem, 
			const char* name )
{
	ActionCommand* a = new Root::ActionCommand( cmd, mnem, name );
	registerCommand( a );
	return a;
}
