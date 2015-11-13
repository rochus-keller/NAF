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

#include "StarParser.h"
#include <QDebug>
using namespace Star;


StarParser::StarParser(bool newSyntax):d_lex(newSyntax)
{
}

bool StarParser::parse(QIODevice * in, const char *codec)
{
	clear();
	if( !d_lex.setStream( in, codec ) )
		return error( QString("cannot open input device" ) );

	StarLexer::Token t = nextToken();
	while( t.d_type != StarLexer::Token::EndOfStream )
	{
		switch( t.d_type )
		{
		case StarLexer::Token::Global:
			if( !t.d_text.isEmpty() )
				return error( QString("global_ with block name"), t );
			else if( !parseBlock( d_global, false, false ) )
				return unarm(false);
			break;
		case StarLexer::Token::Data:
			if( t.d_text.isEmpty() )
				return error( QString("data_ with empty block name"), t );
			if( d_blocks.contains( t.d_text ) )
				return error( QString("data_ block name not unique"), t );
			else if( !parseBlock( d_blocks[t.d_text], true, false ) )
				return unarm(false);
			break;
		case StarLexer::Token::EndOfStream:
			break;
		default:
			return unexpectedToken( t );
		}
		t = nextToken();
	}

	if( d_lex.isNewSyntax() && d_blocks.isEmpty() )
		return error( QString("file contains no data_ block" ) );

	return unarm(true);
}

void StarParser::clear()
{
	d_error.clear();
	d_blocks.clear();
	d_lookAhead = StarLexer::Token();
	d_global = Block();
}

bool StarParser::error(const QString & msg)
{
	d_error = msg;
	return unarm(false);
}

bool StarParser::error(const QString & msg, const StarLexer::Token & t)
{
	d_error = QString("%1:%2: %3 : %4").arg( t.d_line ).arg( t.d_col ).arg( msg ).arg( t.d_text.simplified() );
	return unarm(false);
}

bool StarParser::unarm(bool res)
{
	d_lex.setStream( 0 );
	return res;
}

bool StarParser::unexpectedToken(const StarLexer::Token & t)
{
	return error( QString("unexpected token '%1'").arg( t.typeName() ), t );
}

bool StarParser::parseBlock(StarParser::Block & db, bool frameIn, bool frameOut)
{
	bool firstTime = true;
	while( true )
	{
		StarLexer::Token t = lookAhead();
		if( t.d_type == StarLexer::Token::Tag )
		{
			nextToken(); // Eat token
			if( db.d_items.contains( t.d_text ) )
				return error( QString("data name not unique"), t );
			else if( !parseDataValue( db.d_items[t.d_text] ) )
				return false;
		}else if( t.d_type == StarLexer::Token::Loop )
		{
			nextToken(); // Eat token
			Loop l;
			if( !parseLoopHeader( l, 0 ) )
				return false;
			if( !parseLoopTable( l.d_headers, l.d_table, 0 ) )
				return false;
			db.d_loops.append( l );
		}else if( t.d_type == StarLexer::Token::Save )
		{
			nextToken(); // Eat token
			if( t.d_text.isEmpty() )
			{
				if( frameOut )
					return true; // save_ closes the frame
				else if( !frameIn )
					return unexpectedToken( t );
				else
					return error( QString("save_ with empty block name"), t );
			}else if( !frameIn )
				return unexpectedToken( t );

			if( db.d_blocks.contains( t.d_text ) )
				return error( QString("save_ block name not unique"), t );
			else if( !parseBlock( db.d_blocks[t.d_text], d_lex.isNewSyntax(), true ) )
				return false;
		}else if( !frameOut && !firstTime && ( t.d_type == StarLexer::Token::Data || t.d_type == StarLexer::Token::Global ||
								 t.d_type == StarLexer::Token::EndOfStream ) )
			// don't eat token
			return true;
		else
			return unexpectedToken( t );
		firstTime = false;
	}
	Q_ASSERT( false );
	return false;
}

bool StarParser::parseLoopHeader(StarParser::Loop & l, int level)
{
	bool firstTime = true;
	Loop::Header header;
	l.d_headers.resize( level + 1 );
	while( true )
	{
		StarLexer::Token t = lookAhead();
		if( t.d_type == StarLexer::Token::Tag )
		{
			nextToken(); // eat token
			header.append( t.d_text );
		}else if( t.d_type == StarLexer::Token::Loop )
		{
			nextToken(); // eat token
			if( !parseLoopHeader( l, level + 1 ) )
				return false;
		}else if( !firstTime && t.isDataValue() )
		{
			l.d_headers[level] = header;
			return true; // gehe zurück ohne eat token
		}else if( !firstTime && t.d_type == StarLexer::Token::Stop )
		{
			// Im BNF 1993 ist stop_ optional; im BNF 2012 ist es obligatorisch, aber im Beispiel im Paper fehlt es auch!
			nextToken(); // eat token
			l.d_headers[level] = header;
			return true;
		}else
			return unexpectedToken( t );
		firstTime = false;
	}
	Q_ASSERT( false );
	return false;
}

bool StarParser::parseLoopTable(const Loop::Headers & headers, Loop::Table & table, int level)
{
	Q_ASSERT( level < headers.size() );
	bool firstTime = true;
	while( true )
	{
		StarLexer::Token t = lookAhead();
		if( t.isDataValue() )
		{
			if( table.isEmpty() )
				table.append( Loop::Row() );
			else if( table.last().size() == headers[level].size() )
			{
				// Hierhin kommen wir, wenn eine Zeile von table vollständig gelesen wurde und entschieden werden muss,
				// ob die nächsten Daten zu table oder einer Untertabelle gehören
				if( headers.size() > level + 1 )
				{
					Loop::Table t2;
					if( !parseLoopTable( headers, t2, level + 1 ) )
						return false;
					table.last().append( QVariant::fromValue( t2 ) );
				}else
					table.append( Loop::Row() );
			}else if( table.last().size() > headers[level].size() )
				table.append( Loop::Row() ); // Hierher kommen wir nach Rückkehr von der Untertabelle
			if( table.last().size() < headers[level].size() )
			{
				nextToken(); // eat token
				if( d_lex.isNewSyntax() )
					table.last().append( t.d_text );
				else
					table.last().append( t.d_text.toAscii() );
			}
		}else if( t.d_type == StarLexer::Token::Stop )
		{
			nextToken(); // eat token
			if( table.last().size() < headers[level].size() )
				return error( QString("invalid number of loop values"), t );
			// Fall: wenn zwar verschachtelte Loops definiert, aber auch Records nur mit Werten für Toplevel in Tabelle sind.
			// Siehe Beispiel: http://www.iucr.org/__data/iucr/cif/software/oostar/oostar_ddl1/bin/data/ex.1
			// Fall: array_usage.str in starlib2 examples; überzählige loop_ am Schluss
			// Mit folgendem Hack werden diese Fälle abgefangen:
			t = lookAhead();
			if( level == 0 )
			{
				if( t.isDataValue() )
				{
					table.append( Loop::Row() );
					continue;
				}else if( t.d_type == StarLexer::Token::Stop && headers.size() > 1 )
				{
					// Überzählige stop_ am Schluss werden toleriert, wenn der loop_ verschachtelt war
					nextToken(); // eat token
					continue;
				}
			}
			// Ende Hack
			return true;
		}else if( !firstTime && ( t.d_type == StarLexer::Token::Tag || t.d_type == StarLexer::Token::Loop ||
								  t.d_type == StarLexer::Token::Save ||
								  t.d_type == StarLexer::Token::Data || t.d_type == StarLexer::Token::Global ||
								  t.d_type == StarLexer::Token::EndOfStream ) )
		{
			if( table.last().size() < headers[level].size() )
				return error( QString("invalid number of loop values"), t );
			return true; // zurück ohne eat token
		}else
			return unexpectedToken( t );
		firstTime = false;
	}
	Q_ASSERT( false );
	return false;
}

bool StarParser::parseDataValue(QVariant & v)
{
	StarLexer::Token t = nextToken();
	switch( t.d_type )
	{
	case StarLexer::Token::Quoted:
	case StarLexer::Token::NonQuoted:
	case StarLexer::Token::Multiline:
		if( d_lex.isNewSyntax() )
			v = t.d_text;
		else
			v = t.d_text.toAscii();
		return true;
	case StarLexer::Token::ListStart:
		{
			QVariantList l;
			if( !parseList( l ) )
				return false;
			v.setValue( l );
		}
		break;
	case StarLexer::Token::TableStart:
		{
			QVariantMap m;
			if( !parseTable( m ) )
				return false;
			v.setValue( m );
		}
		break;
	case StarLexer::Token::RefStart:
		{
			Reference r;
			if( !parseReference( r ) )
				return false;
			v.setValue( r );
		}
		break;
	default:
		return unexpectedToken( t );
	}
	return true;
}

bool StarParser::parseRefValue(QVariant & v)
{
	StarLexer::Token t = nextToken();
	switch( t.d_type )
	{
	case StarLexer::Token::Quoted:
	case StarLexer::Token::NonQuoted:
		v = t.d_text;
		return true;
	case StarLexer::Token::ListStart:
		{
			QVariantList l;
			if( !parseList( l ) )
				return false;
			v.setValue( l );
		}
		break;
	default:
		return unexpectedToken( t );
	}
	return true;
}

bool StarParser::parseList(QVariantList & l, bool first)
{
	QVariant v;
	StarLexer::Token t;
	if( first )
	{
		t = lookAhead();
		if( t.d_type == StarLexer::Token::ListEnd )
		{
			nextToken(); // eat token
			return true;
		}
	}
	if( !parseDataValue( v ) )
		return false;
	l.append( v );
	t = nextToken();
	switch( t.d_type )
	{
	case StarLexer::Token::ElemSep:
		if( !parseList( l, false ) )
			return false;
		break;
	case StarLexer::Token::ListEnd:
		break;
	default:
		return unexpectedToken( t );
	}
	return true;
}

bool StarParser::parseTable(QVariantMap & m, bool first)
{
	QString key;
	StarLexer::Token t = nextToken();
	if( t.d_type == StarLexer::Token::Quoted )
		key = t.d_text;
	else if( first && t.d_type == StarLexer::Token::TableEnd )
		return true;
	else
		return unexpectedToken( t );
	t = nextToken();
	if( t.d_type != StarLexer::Token::KeyValueSep )
		return unexpectedToken( t );

	QVariant v;
	if( !parseDataValue( v ) )
		return false;
	m[key] = v;

	t = nextToken();
	switch( t.d_type )
	{
	case StarLexer::Token::ElemSep:
		if( !parseTable( m, false ) )
			return false;
		break;
	case StarLexer::Token::TableEnd:
		break;
	default:
		return unexpectedToken( t );
	}
	return true;
}

bool StarParser::parseReference(StarParser::Reference & r)
{
	QString key;
	StarLexer::Token t1 = nextToken();
	if( t1.d_type == StarLexer::Token::Quoted )
		key = t1.d_text;
	else
		return unexpectedToken( t1 );

	StarLexer::Token t2 = nextToken();
	if( t2.d_type != StarLexer::Token::KeyValueSep )
		return unexpectedToken( t2 );

	QVariant v;
	if( !parseRefValue( v ) )
		return false;

	if( key == "block" && r.d_block.isNull() )
		r.d_block = v;
	else if( key == "source" && r.d_source.isNull() )
		r.d_source = v;
	else if( key == "frame" && r.d_frame.isNull() )
		r.d_frame = v;
	else if( key == "item" && r.d_item.isNull() )
		r.d_item = v;
	else if( key == "key" && r.d_key.isNull() )
		r.d_key = v;
	else
		return error( QString("invalid ref-label '%1'").arg( key ), t1 );

	t1 = nextToken();
	switch( t1.d_type )
	{
	case StarLexer::Token::ElemSep:
		if( !parseReference( r ) )
			return false;
		break;
	case StarLexer::Token::RefEnd:
		break;
	default:
		return unexpectedToken( t1 );
	}
	return true;
}

StarLexer::Token StarParser::nextToken()
{
	if( !d_lookAhead.isNull() )
	{
		StarLexer::Token t = d_lookAhead;
		d_lookAhead = StarLexer::Token(); // remove
		return t;
	}else
		return d_lex.nextTokenNoComments();
}

StarLexer::Token StarParser::lookAhead()
{
	if( d_lookAhead.isNull() )
		d_lookAhead = d_lex.nextTokenNoComments();
	return d_lookAhead;
}

static QString _toString( const QVariant& v )
{
	return v.toString(); // TODO
}

void StarParser::dump(QTextStream & out)
{
	out << "******** Dumping Star File *******" << endl;

	dumpBlock( out, d_global, "### Global Block", 0 );
	QMap<QString,Block>::const_iterator i;
	for( i = d_blocks.begin(); i != d_blocks.end(); ++i )
	{
		dumpBlock( out, i.value(), QString("### Block '%1'").arg(i.key()), 0 );
	}

	out << "*********** End of Dump **********" << endl;
}

void StarParser::dump()
{
	QString str;
	QTextStream out( &str, QIODevice::WriteOnly );
	dump(out);
	qDebug() << str;
}

void StarParser::dumpBlock(QTextStream& out, const StarParser::Block & b, const QString& title, int level)
{
	const QString indent( level * 4, QChar(' ') );
	out << indent << title << endl;
	QMap<QString,QVariant>::const_iterator j;
	for( j = b.d_items.begin(); j != b.d_items.end(); ++j )
		out << indent << "    " << j.key() << " = " << _toString(j.value()) << endl;
	for( int n = 0; n < b.d_loops.size(); n++ )
	{
		for( int h = 0; h < b.d_loops[n].d_headers.size(); h++ )
		{
			const QString dbindent( h * 4, QChar(' ') );
			out << indent << dbindent << "    ### Loop: ";
			for( int k = 0; k < b.d_loops[n].d_headers[h].size(); k++ )
				out << b.d_loops[n].d_headers[h][k] << " ";
			out << endl;
		}
		dumpTable( out, b.d_loops[n].d_table, level + 1 );
	}
	QMap<QString,Block>::const_iterator i;
	for( i = b.d_blocks.begin(); i != b.d_blocks.end(); ++i )
	{
		dumpBlock( out, i.value(), QString("### Block '%1'").arg(i.key()), level + 1 );
	}
}

void StarParser::dumpTable(QTextStream & out, const StarParser::Loop::Table & table, int level)
{
	const QString indent( level * 4, QChar(' ') );
	for( int m = 0; m < table.size(); m++ )
	{
		out << indent;
		bool doEndl = true;
		for( int k = 0; k < table[m].size(); k++ )
		{
			QVariant v = table[m][k];
			if( v.canConvert<Loop::Table>() )
			{
				Q_ASSERT( k == table[m].size() - 1 );
				out << endl;
				Loop::Table t2 = v.value<Loop::Table>();
				dumpTable( out, t2, level + 1 );
				doEndl = false;
			}else
				out << _toString( v ) << " ";
		}
		if( doEndl )
			out << endl;
	}
}
