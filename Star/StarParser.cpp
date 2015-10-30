/*
 * Copyright 2000-2015 Rochus Keller <mailto:rkeller@nmr.ch>
 *
 * This file is part of CARA (Computer Aided Resonance Assignment,
 * see <http://cara.nmr.ch/>).
 *
 * CARA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License (GPL) as
 * published by the Free Software Foundation, either version 2 of
 * the License, or (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "StarParser.h"
#include <QDebug>
using namespace Star;

StarParser::StarParser()
{
}

bool StarParser::parse(QIODevice * in)
{
	clear();
	if( !d_lex.setStream( in ) )
		return error( tr("cannot open input device" ) );

	StarLexer::Token t = nextToken();
	switch( t.d_type )
	{
	case StarLexer::Token::Global:
		if( !t.d_text.isEmpty() )
			return error( tr("global_ with block name"), t );
		else if( !parseBlock( d_global, false, false ) )
			return false;
		break;
	case StarLexer::Token::Data:
		if( t.d_text.isEmpty() )
			return error( tr("data_ with empty block name"), t );
		if( d_blocks.contains( t.d_text ) )
			return error( tr("data_ block name not unique"), t );
		else if( !parseBlock( d_blocks[t.d_text], true, false ) )
			return false;
		break;
	case StarLexer::Token::EndOfStream:
		break;
	default:
		return unexpectedToken( t );
	}

	if( d_blocks.isEmpty() )
		return error( tr("file contains no data_ block" ) );

	return true;
}

void StarParser::clear()
{
	d_error.clear();
	d_blocks.clear();
	d_peeks.clear();
	d_global = Block();
}

void StarParser::dump()
{
	qDebug() << "******** Dumping Star File *******";

	dumpBlock( d_global, "### Global Block", 0 );
	QMap<QString,Block>::const_iterator i;
	for( i = d_blocks.begin(); i != d_blocks.end(); ++i )
	{
		dumpBlock( i.value(), tr("### Block '%1'").arg(i.key()), 0 );
	}

	qDebug() << "*********** End of Dump **********";
}

bool StarParser::error(const QString & msg)
{
	d_error = msg;
	return false;
}

bool StarParser::error(const QString & msg, const StarLexer::Token & t)
{
	d_error = tr("%1:%2: %3 : %4").arg( t.d_line ).arg( t.d_col ).arg( msg ).arg( t.d_text.simplified() );
	return false;
}

bool StarParser::unexpectedToken(const StarLexer::Token & t)
{
	return error( tr("unexpected token '%1'").arg( t.typeName() ), t );
}

bool StarParser::parseBlock(StarParser::Block & db, bool frameIn, bool frameOut)
{
	bool firstTime = true;
	while( true )
	{
		StarLexer::Token t = peekToken();
		if( t.d_type == StarLexer::Token::Tag )
		{
			nextToken(); // Eat token
			if( db.d_items.contains( t.d_text ) )
				return error( tr("data name not unique"), t );
			else if( !parseDataValue( db.d_items[t.d_text] ) )
				return false;
		}else if( t.d_type == StarLexer::Token::Loop )
		{
			nextToken(); // Eat token
			Loop l;
			while( true )
			{
				// Verschachtelte loops sind irrational; die Spezifikation ist grottenschlecht; keine BMRB-Datei braucht das;
				// also weggelassen; RISK
				t = peekToken();
				if( t.d_type == StarLexer::Token::Tag )
				{
					nextToken(); // eat token
					l.d_names.append( t.d_text );
				}else if( !l.d_names.isEmpty() && t.isDataValue() )
				{
					break;
				}else
					return unexpectedToken( t );
			}
			Q_ASSERT( t.isDataValue() );
			QVariantList row;
			while( true )
			{
				t = nextToken();
				if( t.isDataValue() )
				{
					if( row.size() == l.d_names.size() )
					{
						l.d_values.append( row );
						row.clear();
					}
					row.append( t.d_text );
				}else if( t.d_type == StarLexer::Token::Stop )
				{
					if( !row.isEmpty() )
					{
						if( row.size() != l.d_names.size() )
							return error( tr("invalid number of loop values"), t );
						else
							l.d_values.append( row );
					}
					break;
				}else
					return unexpectedToken( t );
			}
			db.d_loops.append( l );
		}else if( frameIn && t.d_type == StarLexer::Token::Save )
		{
			nextToken(); // Eat token
			if( t.d_text.isEmpty() )
			{
				if( frameOut )
					return true; // save_ closes the frame
				else
					return error( tr("save_ with empty block name"), t );
			}
			if( db.d_blocks.contains( t.d_text ) )
				return error( tr("save_ block name not unique"), t );
			else if( !parseBlock( db.d_blocks[t.d_text], true, true ) )
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
	QStringList header;
	//l.d_names.resize( level + 1 );
	while( true )
	{
		StarLexer::Token t = peekToken();
		if( t.d_type == StarLexer::Token::Tag )
		{
			nextToken(); // eat token
			header.append( t.d_text );
		}else if( t.d_type == StarLexer::Token::Loop )
		{
			nextToken(); // eat token
			if( !parseLoopHeader( l, level + 1 ) )
				return false;
		}else if( !firstTime && level == 0 && t.isDataValue() )
		{
			//l.d_names[level] = header;
			return true; // gehe zurück ohne eat token
		}else if( !firstTime && level > 0 && t.d_type == StarLexer::Token::Stop )
		{
			nextToken(); // eat token
			// l.d_names[level] = header;
			return true;
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
		t = peekToken();
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

	if( key == "block" )
		r.d_block.append( v );
	else if( key == "source" )
		r.d_source.append( v );
	else if( key == "frame" )
		r.d_frame.append( v );
	else if( key == "item" )
		r.d_item.append( v );
	else if( key == "key" )
		r.d_key.append( v );
	else
		return error( tr("invalid ref-label '%1'").arg( key ), t1 );

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
	if( !d_peeks.isEmpty() )
		return d_peeks.dequeue();
	else
		return d_lex.nextTokenNoComments();
}

StarLexer::Token StarParser::peekToken()
{
	StarLexer::Token t = d_lex.nextTokenNoComments();
	d_peeks.enqueue( t );
	return t;
}

void StarParser::dumpBlock(const StarParser::Block & b, const QString& title, int level)
{
	const QString indent( level * 4, QChar(' ') );
	qDebug() << indent << title;
	QMap<QString,QVariant>::const_iterator j;
	for( j = b.d_items.begin(); j != b.d_items.end(); ++j )
		qDebug() << indent << "  " << j.key() << " = " << j.value();
	for( int n = 0; n < b.d_loops.size(); n++ )
	{
		qDebug() << indent << "  Loop________________";
		qDebug() << indent << "  " << b.d_loops[n].d_names;
		for( int m = 0; m < b.d_loops[n].d_values.size(); m++ )
			qDebug() << indent << "  " << b.d_loops[n].d_values[m];
	}
	QMap<QString,Block>::const_iterator i;
	for( i = b.d_blocks.begin(); i != b.d_blocks.end(); ++i )
	{
		dumpBlock( i.value(), tr("### Block '%1'").arg(i.key()), level + 1 );
	}
}
