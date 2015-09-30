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

#include "CommandLine2.h"
#include <QKeyEvent>
#include <Root/Message.h>
#include <Root/Exception.h>
#include <Gui2/NamedFunction.h>
#include <QtDebug>
using namespace Spec; 
using namespace Root;

static const char CR = 13;
static const char LF = 10;
static const char BK = 8;
static const char SP = 32;

CommandLine2::CommandLine2(QObject* owner, bool caseSens):
	QObject( owner ), d_caseSensitive( caseSens ), d_active( true ),
    d_state( Idle ), d_cur( 0 )
{
    addCommand( this, SLOT(handleHelp()), "?", "Help", false )->registerParameter( Any::CStr, false, "Mnemonic" );
}

CommandLine2::~CommandLine2()
{
}

void CommandLine2::notify(const QString &message)
{
    ReportStatus msg( message.toLatin1() ); // RISK
    msg.sendToQt();
    if( !msg.isConsumed() )
        qWarning() << "CommandLine2::notify not consumed";
}

void CommandLine2::clearParamsOfCur()
{
    Q_ASSERT( d_cur != 0 );
    for( int i = 0; i < d_cur->d_params.size(); i++ )
    {
        d_cur->d_cmd->setProperty( QByteArray::number(i), QVariant() );
    }
}

void CommandLine2::printToLog()
{
	CommandLine2::Commands::const_iterator p;
	QByteArray out;
    for( p = d_commands.begin(); p != d_commands.end(); ++p )
	{
        out += p.value().d_mnemonic;
		out += "\t";
        out += p.value().d_cmd->text().toLatin1();
		out += "\n";
	}
	out += "For detailed help enter '? <command>'";
	out += "\n";
    ReportToLog msg( ReportToLog::Information, "CmdStr Line", out );
    msg.sendToQt();
    if( !msg.isConsumed() )
        qWarning() << "CommandLine2::printToLog not consumed";

}

static const char* _TypeName(Any::Type t)
{
    if( t == Any::Memo )
		return "CmdStr String";
	else
        return Any::getTypeName( t );
}

void CommandLine2::printToLog( const Command * cmd )
{
    QByteArray str;
    str += cmd->d_mnemonic;
	str += "\t";
    str += cmd->d_cmd->text();
	str += "\t";
    for( int i = 0; i < cmd->d_params.size(); i++ )
	{
        const Command::Param& p = cmd->d_params[i];
		str += " ";
        if( i < p.d_mandatory )
		{
            str += p.d_name;
			str += ": ";
            str += _TypeName( p.d_type );
		}else
		{
			str += "[";
            str += p.d_name;
			str += ": ";
            str += _TypeName( p.d_type );
			str += "]";
		}
	}
    ReportToLog msg( ReportToLog::Information, "CmdStr Line", str );
    msg.sendToQt();
    if( !msg.isConsumed() )
        qWarning() << "CommandLine2::printToLog not consumed";
}

void CommandLine2::execute(const Command *cmd)
{
    cmd->d_cmd->prepare();

    if( !cmd->d_cmd->hasTarget() )
	{
        notify( QString( "%1: can't find a handler for this command" ).arg( cmd->d_cmd->text() ) );
		reset();
		return;
	}
    if( !cmd->d_cmd->isEnabled() )
	{
        notify( QString( "%1: not available in this state!" ).arg( cmd->d_cmd->text() ) );
		reset();
		return;
	}
    notify( QString( "%1: executed" ).arg( cmd->d_cmd->text() ) );
	
    cmd->d_cmd->execute();
	reset();
}

void CommandLine2::reset()
{
	d_state = Idle;
    d_cur = 0;
	d_line = "";
}

bool CommandLine2::parse(char ch)
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

bool CommandLine2::consume(char ch)
{
	return d_line.isEmpty() || d_line[ d_line.size() - 1 ] == ch;		
}

void CommandLine2::handleIdle(char ch)
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

void CommandLine2::handleReadSymbol(char ch)
{
    const Command* cmd = 0;
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
            notify( QString( "%1: Invalid Command!" ).arg( d_line.data() ) );
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
            d_cur = cmd;
			d_line = "";
			d_currentParam = 0;
            if( !cmd->d_params.isEmpty() )
			{
				d_state = ReadParam;
                clearParamsOfCur();
				writeCommand();
			}else
			{
                notify( QString( "%1: Command has no Parameters!" ).arg( cmd->d_cmd->text() ) );
			}
		}else
		{
            notify( QString( "%1: Invalid Command!" ).arg( cmd->d_cmd->text() ) );
		}
		break;
	default:
		// Check if line equals a Symbol
		d_line += ch;
		writeLine( d_line );
		cmd = find( d_line );
		if( cmd )
		{
            if( cmd->d_immediate )
				execute( cmd );
			else
			{
				d_line = "";
				d_currentParam = 0;
                d_cur = cmd;
				d_state = ReadParam;
                clearParamsOfCur();
				writeCommand();
			}
		}
		break;
	}
}

void CommandLine2::handleReadParam(char ch)
{
	// TODO: fertig implementieren.
	switch( ch )
	{
	case LF:
		break;
	case CR:
		if( interpretParam() )
		{
            if( d_currentParam < d_cur->d_mandatory - 1 )
			{
                notify( QString( "%1: Command needs %2 Parameter(s)!" ).arg( d_cur->d_cmd->text() ).
                    arg( d_cur->d_mandatory ) );
			}else
                execute( d_cur );
		}
		break;
	case BK:
		d_line.truncate( d_line.size() - 1 );
		if( d_line.isEmpty() )
		{
			d_currentParam--;
            if( d_currentParam >= 0 && d_currentParam < d_cur->d_params.size() )
			{
                d_line = d_cur->d_cmd->property( QString::number(d_currentParam) ).toByteArray();
                writeLine( d_cur->d_cmd->text(), d_line );
			}else
			{
				notify( "Idle" );
				reset();
			}
		}else
            writeLine( d_cur->d_cmd->text(), d_line );
		break;
	case SP:
        if( d_currentParam < d_cur->d_params.size() &&
                d_cur->d_params[ d_currentParam ].d_type == Any::CStr )
		{
			d_line += ch;
            writeLine( d_cur->d_cmd->text(), d_line );
        }else if( d_currentParam >= d_cur->d_params.size() - 1 )
		{	
			notify( QString( "%1: Command accepts only %2 Parameter(s)!" ).
                arg( d_cur->d_cmd->text() ).arg( d_cur->d_params.size() ) );
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
        writeLine( d_cur->d_cmd->text(), d_line );
		break;
	}
}

void CommandLine2::writeLine(const char * lhs, const char * rhs)
{
	QByteArray par;
    for( int i = 0; d_cur && i < d_currentParam; i++ )
	{
        par += d_cur->d_cmd->property( QString::number(i) ).toByteArray();
		par += " ";
	}
	par += rhs;

    notify( QString( "Input> %1 %2" ).arg( lhs ).arg( par.data() ) );
}

const CommandLine2::Command *CommandLine2::find(QByteArray str) const
{
	if( !d_caseSensitive )
		str = str.toUpper();
    Commands::const_iterator pos = d_commands.find( str );
	if( pos != d_commands.end() )
        return &pos.value();
	else
		return 0;

}

bool CommandLine2::interpretParam()
{
	QString str;
    QVariant val;
	bool ok = true;
	if( d_line.isEmpty() )
	{
        if( d_currentParam < d_cur->d_mandatory )
		{
			notify( QString( "%1: at least %2 Parameters expected!" ).
                arg( d_cur->d_cmd->text() ).
                arg( d_cur->d_mandatory ) );
			return false;
		}else
		{
            d_cur->d_cmd->setProperty( QByteArray::number(d_currentParam), QVariant() );
			return true;
		}
	}
	str = d_line;
    switch( d_cur->d_params[ d_currentParam ].d_type )
	{
	case Any::Null:
		qDebug( "CommandLine2: Invalid Parameter Type!" );
		return false;
	case Any::Boolean:
		str = str.toUpper();
		if( str == "TRUE" || str == "YES" || str == "1" || str == "ON" )
            val = true;
		else if( str == "FALSE" || str == "NO" || str == "0" || str == "OFF" )
            val = false;
		else
		{
			notify( QString( "%1: '%2' invalid Boolean!" ).
                arg( d_cur->d_cmd->text() ).
                arg( d_line.data() ) );
			return false;
		}
		break;
	case Any::Char:
	case Any::UChar:
		if( d_line.size() != 1 )
		{
			notify( QString( "%1: '%2' invalid Char!" ).
                arg( d_cur->d_cmd->text() ).
                arg( d_line.data() ) );
			return false;
		}else
            val = QChar( d_line[ 0 ] );
		break;
	case Any::Short:
        val = str.toShort( &ok );
		if( !ok )
		{
			notify( QString( "%1: '%2' invalid short!" ).
                arg( d_cur->d_cmd->text() ).
                arg( d_line.data() ) );
			return false;
		}
		break;
	case Any::UShort:
        val = str.toUShort( &ok );
		if( !ok )
		{
			notify( QString( "%1: '%2' invalid ushort!" ).
                arg( d_cur->d_cmd->text() ).
                arg( d_line.data() ) );
			return false;
		}
		break;
	case Any::Long:
        val = str.toInt( &ok );
		if( !ok )
		{
			notify( QString( "%1: '%2' invalid Long!" ).
                arg( d_cur->d_cmd->text() ).
                arg( d_line.data() ) );
			return false;
		}
		break;
	case Any::ULong:
        val = str.toUInt( &ok );
		if( !ok )
		{
			notify( QString( "%1: '%2' invalid ULong!" ).
                arg( d_cur->d_cmd->text() ).
                arg( d_line.data() ) );
			return false;
		}
		break;
	case Any::Float:
        val = str.toFloat( &ok );
		if( !ok )
		{
            notify( QString( "%1: '%2' invalid Float!" ).arg( d_cur->d_cmd->text() ).arg( d_line.data() ) );
			return false;
		}
		break;
	case Any::Double:
        val = str.toDouble( &ok );
		if( !ok )
		{
            notify( QString( "%1: '%2' invalid Double!" ).arg( d_cur->d_cmd->text() ).arg( d_line.data() ) );
			return false;
		}
		break;
	default:
        val = d_line;
		break;
	};
    d_cur->d_cmd->setProperty( QByteArray::number(d_currentParam), val );
	return true;
}

bool CommandLine2::nextParam()
{
	d_currentParam++;
	d_line = "";
	return true;
}

void CommandLine2::writeLine(const QByteArray &s)
{
    notify( QString("Input> %1").arg(s.data() ) );
}

void CommandLine2::writeCommand()
{
    QByteArray str = d_cur->d_cmd->text().toLatin1();
    for( int i = 0; i < d_cur->d_params.size(); i++ )
	{
		str += " ";
        if( i < d_cur->d_mandatory )
            str += _TypeName( d_cur->d_params[i].d_type );
		else
		{
			str += "[";
            str += _TypeName( d_cur->d_params[i].d_type );
			str += "]";
		}
	}
	str += " <Enter>";
    writeLine( str );
}

bool CommandLine2::eventFilter(QObject *watched, QEvent *event)
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

void CommandLine2::handleHelp()
{
    Gui2::UiFunction* a = Gui2::UiFunction::me();
    ENABLED_IF(true);

    if( a->property("0").isNull() )
        printToLog();
    else
    {
        const Command* c = find( a->property("0").toByteArray() );
        if( c )
            printToLog( c );
        else
            printToLog();
    }
    notify( QString( "%1: executed" ).arg( a->name() ) );
    reset();
}

CommandLine2::Command* CommandLine2::addCommand(QObject* receiver, const char *member,
                                                      QByteArray mnemonic, const QString &text, bool immediate)
{
    Gui2::UiFunction* a;
    if( receiver )
        a = new Gui2::UiFunction( text, this, receiver, member );
    else
        a = new Gui2::NamedFunction( text, member, this );
    if( d_caseSensitive )
        mnemonic = mnemonic.toUpper();

    Command& cmd = d_commands[mnemonic];
    if( cmd.d_cmd )
        delete cmd.d_cmd;
    cmd.d_cmd = a;
    cmd.d_mnemonic = mnemonic;
    cmd.d_immediate = immediate;
    return &cmd;
}

void CommandLine2::Command::registerParameter(Any::Type type, bool mandatory, const QByteArray &name)
{
    if( d_immediate )
        throw Exception( "Params invalid for immediate commands" );

    Command::Param p;
    p.d_mandatory = mandatory;
    p.d_name = name;
    p.d_type = type;
    switch( type )
    {
    case Any::Null:
    case Any::UChar:
        throw Exception( "Invalid param type" );
    default:
        d_params.append( p );
    }
    if( mandatory )
        d_mandatory = d_params.size();
}

