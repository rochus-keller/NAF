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

#include "MessageLog.h"
#include <QtDebug>
#include <Root/Vector.h>
#include <QTextStream>
using namespace Root;

static ExRef<MessageLog> s_inst;
static Deque<MessageLog::Entry> s_log;	// RISK: Multithreading
static bool s_remember = true;

const char* MessageLog::s_pretty[] = 
{
	"TRACE", "INFO", "WARNING", "ERROR"
};

MessageLog::MessageLog()
{
	d_level = Info;
}

MessageLog::~MessageLog()
{

}

MessageLog* MessageLog::inst()
{
	if( s_inst == 0 )
		s_inst = new MessageLog();
	return s_inst;
}

void MessageLog::log(Kind k, const char *src, const QByteArray &msg)
{
	// qDebug( "%s: %s (%s)", s_pretty[ k ], msg, src );
	if( k >= d_level )
	{
		if( s_remember )
			s_log.push_back( Entry( k, src, msg ) );
		Update msg( this, Update::Add, s_log.size() - 1 );
		notifyObservers( msg );
	}
}

void MessageLog::saveToStream( QIODevice *dev )
{
    QTextStream out(dev);
	for( int i = 0; i < s_log.size(); i++ )
	{
		out << "#" << i << "\t" << s_pretty[ s_log[ i ].d_kind ] << "\t" <<
               s_log[ i ].d_src.data() << "\n";
		out << s_log[ i ].d_msg << "\n";
	}
}

int MessageLog::getCount() const
{
	return s_log.size();
}

void MessageLog::clearLog()
{
	s_log.clear();
	Update msg( this );
	notifyObservers( msg );
}

const MessageLog::Entry& MessageLog::getEntry(int i) const
{
	return s_log[ i ];
}

void MessageLog::setRemember(bool r)
{
	s_remember = r;
	if( !r )
		clearLog();
}

void MessageLog::kill()
{
	s_inst = 0;
	s_log.clear();
}
