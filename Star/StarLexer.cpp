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

#include "StarLexer.h"
#include <QtDebug>
#include <QTextStream>
using namespace Star;

enum { BEL = 7,
	   USC = 0x5F, // Underscore
	   LCB = 0x7B, // Left Curly Bracket
	   RCB = 0x7D, // Right Curly Bracket
	   LSB = 0x5B, // Left Square Bracket
	   RSB = 0x5D, // Right Square Bracket
	   CMA = 0x2C, // Comma
	   APO = 0x27, // Apostrophe
	   QOT = 0x22, // Quote
	   SEM = 0x3B, // Semicolon
	   COL = 0x3A, // Colon
	   PND = 0x23, // Pound
	   DLR = 0x24  // Dollar
	 };

StarLexer::StarLexer(bool newSyntax) :
	d_lineNr(0),d_colNr(0),d_newSyntax(newSyntax),d_in(0)
{
}

StarLexer::StarLexer(const StarLexer & rhs):
	d_lineNr(0),d_colNr(0),d_newSyntax(true),d_in(0)
{
	*this = rhs;
}

StarLexer::~StarLexer()
{
	if( d_in )
		delete d_in;
}

bool StarLexer::setStream(QIODevice *in, const char *codec)
{
	if( in == 0 )
	{
		if( d_in )
			delete d_in;
		// das muss so sein; wenn erst im Destruktor gelöscht oder Stream sogar ein Value Object, dann stürzt
		// Prozess beim Löschen von Lexer in QObject bzw. dessen Mutex.
		d_in = 0;
		return false;
	}
	if( !in->isOpen() )
	{
		if( !in->open( QIODevice::ReadOnly ) )
			return false;
	}
	if( d_in == 0 )
		d_in = new QTextStream();
	d_in->setAutoDetectUnicode(false);
	d_in->setDevice( in );
	d_in->setCodec( codec );
	d_in->setAutoDetectUnicode(true);
	d_lineNr = 0;
	d_colNr = 0;
	d_line.clear();
	return true;
}

void StarLexer::reset()
{
	d_in->seek(0);
	d_in->reset();
	d_in->resetStatus();
	d_lineNr = 0;
	d_colNr = 0;
	d_line.clear();
}

StarLexer::Token StarLexer::nextToken()
{
	skipWhiteSpace();
	while( d_colNr >= d_line.size() )
	{
		if( d_in->atEnd() )
			return Token( Token::EndOfStream, d_lineNr, d_colNr );
		nextLine();
		if( !checkLineChars() )
			return Token( Token::InvalidChar, d_lineNr, d_colNr, d_line );
		skipWhiteSpace();
	}
	Q_ASSERT( d_colNr < d_line.size() );
	const int uc = d_line[d_colNr].unicode();
	switch( uc )
	{
	case USC:
		return readTag();
	case QOT:
		if( d_newSyntax && matchTuple( d_colNr+1, QOT, 2 ) )
			return readTrippleQuotString(QOT);
		else
			return readSingleQuotString(QOT);
	case APO:
		if( d_newSyntax && matchTuple( d_colNr+1, APO, 2 ) )
			return readTrippleQuotString(APO);
		else
			return readSingleQuotString(APO);
	case SEM:
		if( d_colNr == 0 )
			return readSemicolonString();
		else
			return Token( Token::SyntaxError, d_lineNr, d_colNr, QChar(';') );
	case PND:
		{
			const int col = d_colNr;
			const QString cmt = d_line.mid(d_colNr);
			d_colNr = d_line.size();
			return Token( Token::Comment, d_lineNr, col, cmt );
		}
	default:
		break;
	}
	if( d_newSyntax )
	{
		switch( uc )
		{
		case LCB:
			return Token( Token::TableStart, d_lineNr, d_colNr++ );
		case RCB:
			{
				if( (d_colNr + 1) < d_line.size() && d_line[d_colNr+1].unicode() == DLR )
				{
					d_colNr++;
					return Token( Token::RefEnd, d_lineNr, d_colNr++ );
				}else
					return Token( Token::TableEnd, d_lineNr, d_colNr++ );
			}
		case LSB:
			return Token( Token::ListStart, d_lineNr, d_colNr++ );
		case RSB:
			return Token( Token::ListEnd, d_lineNr, d_colNr++ );
		case CMA:
			return Token( Token::ElemSep, d_lineNr, d_colNr++ );
		case COL:
			return Token( Token::KeyValueSep, d_lineNr, d_colNr++ );
		default:
			break;
		}
	}
	switch( uc )
	{
	case DLR:
		if( d_newSyntax && (d_colNr + 1) < d_line.size() && d_line[d_colNr+1].unicode() == LCB )
		{
			d_colNr++;
			return Token( Token::RefStart, d_lineNr, d_colNr++ );
		}// else fall through
	default:
		{
			const int col = d_colNr;
			const QString val = readNonWhiteSpace();
			if( val.startsWith( QLatin1String("loop_"), Qt::CaseInsensitive ) )
				return Token( Token::Loop, d_lineNr, col, val.mid(5) );
			else if( val.startsWith( QLatin1String("global_"), Qt::CaseInsensitive ) )
				return Token( Token::Global, d_lineNr, col, val.mid(7) );
			else if( val.startsWith( QLatin1String("save_"), Qt::CaseInsensitive ) )
				return Token( Token::Save, d_lineNr, col, val.mid(5) );
			else if( val.startsWith( QLatin1String("stop_"), Qt::CaseInsensitive ) )
				return Token( Token::Stop, d_lineNr, col, val.mid(5) );
			else if( val.startsWith( QLatin1String("data_"), Qt::CaseInsensitive ) )
				return Token( Token::Data, d_lineNr, col, val.mid(5) );
			else
				return Token( Token::NonQuoted, d_lineNr, col, val );
		}

	}
	Q_ASSERT( false );
	return Token( Token::SyntaxError, d_lineNr, d_colNr, QChar(';') );
}

StarLexer::Token StarLexer::nextTokenNoComments()
{
	Token t = nextToken();
	while( t.d_type == Token::Comment )
		t = nextToken();
	return t;
}

void StarLexer::dump()
{
	Token t = nextToken();
	while( t.d_type < Token::EndOfStream )
	{
		qDebug() << "****" << t.typeName() << t.d_line << t.d_col << t.d_text;
		t = nextToken();
	}
	qDebug() << "****" << t.typeName() << t.d_line << t.d_col << t.d_text;
}

StarLexer &StarLexer::operator =(const StarLexer &rhs)
{
	d_newSyntax = rhs.d_newSyntax;
	return *this;
}

void StarLexer::nextLine()
{
	d_colNr = 0;
	d_lineNr++;
	d_line = d_in->readLine();
}

void StarLexer::skipWhiteSpace()
{
	while( d_colNr < d_line.size() && d_line[d_colNr].isSpace() )
		d_colNr++;
}

bool StarLexer::isValid(const QChar &ch) const
{
	const ulong uc = ch.unicode();
	if( uc <= 127 )
		return ch.isSpace() || ch.isPunct() || ch.isPrint() || ( d_newSyntax && uc == BEL );
	else if( d_newSyntax && uc >= 128 && uc <= 0xD7FF )
		return true;
	else if( d_newSyntax && uc >= 0xE000 && uc <= 0xFFFD )
		return true;
	else if( d_newSyntax && uc >= 0x10000 && uc <= 0x10FFF )
		return true;
	return false;
}

bool StarLexer::checkLineChars()
{
	for( int i = 0; i < d_line.size(); i++ )
	{
		if( !isValid( d_line[i] ) )
		{
			d_colNr = i;
			return false;
		}
	}
	return true;
}

StarLexer::Token StarLexer::readTag()
{
	const int col = d_colNr;
	QString res;
	res.reserve( d_line.size() - d_colNr );
	while( d_colNr < d_line.size() && !d_line[d_colNr].isSpace() )
	{
		res += d_line[d_colNr];
		d_colNr++;
	}
	return Token( Token::Tag, d_lineNr, col, res.mid(1) ); // mid..ohne USC
}

StarLexer::Token StarLexer::readSingleQuotString(int sym)
{
	QString res;
	res.reserve( d_line.size() - d_colNr );
	Q_ASSERT( d_colNr < d_line.size() && d_line[d_colNr].unicode() == sym );
	d_colNr++;
	const int col = d_colNr;
	while( d_colNr < d_line.size() )
	{
		if( d_newSyntax && d_line[d_colNr].unicode() == BEL )
		{
			if( ( d_colNr + 1 ) < d_line.size() && d_line[d_colNr+1].unicode() == sym )
			{
				res += QChar(sym);
				d_colNr++;
			}else
				return Token( Token::SyntaxError, d_lineNr, d_colNr, res );
		}else if( d_line[d_colNr].unicode() == sym )
		{
			if( !d_newSyntax && ( d_colNr + 1 ) < d_line.size() && !d_line[d_colNr+1].isSpace() )
			{
				res += d_line[d_colNr];
			}else
			{
				d_colNr++;
				return Token( Token::Quoted, d_lineNr, col, res );
			}
		}else
			res += d_line[d_colNr];
		d_colNr++;
	}
	return Token( Token::SyntaxError, d_lineNr, d_colNr, res ); // Zeilenende ohne abschliessendes sym
}

StarLexer::Token StarLexer::readTrippleQuotString(int sym)
{
	Q_ASSERT( d_newSyntax );
	const QString pattern( 3, QChar(sym) );
	Q_ASSERT( ( d_colNr + 2 ) < d_line.size() && d_line.mid(d_colNr,3) == pattern );
	d_colNr += 3;
	const int col = d_colNr;
	const int line = d_lineNr;
	const int lhsPos = findTripple( pattern, d_colNr );
	bool ok;
	if( lhsPos >= 0 )
	{
		// Ende auf gleicher Zeile
		const QString res = escapeString( d_line.mid( d_colNr, lhsPos - d_colNr ), sym, &ok );
		if( ok )
		{
			d_colNr = lhsPos + 3;
			return Token( Token::Multiline, line, col, res );
		}else
			return Token( Token::SyntaxError, d_lineNr, d_colNr, res );
	}else if( lhsPos == -1 )
	{
		// Ende auf künftiger Zeile
		QString res = escapeString( d_line.mid( d_colNr ), sym, &ok ) + QChar('\n');
		if( !ok )
			return Token( Token::SyntaxError, d_lineNr, d_colNr, res );
		while( true )
		{
			if( d_in->atEnd() )
				return Token( Token::SyntaxError, d_lineNr, d_colNr, res ); // TrippleQuote without end
			nextLine();
			if( !checkLineChars() )
				return Token( Token::InvalidChar, d_lineNr, d_colNr, QString(d_line[d_colNr]) );
			const int rhsPos = findTripple( pattern, d_colNr );
			if( rhsPos >= 0 )
			{
				// Ende auf dieser Zeile
				res += escapeString( d_line.mid( d_colNr, rhsPos - d_colNr ), sym, &ok );
				if( ok )
				{
					d_colNr = rhsPos + 3;
					return Token( Token::Multiline, line, col, res );
				}else
					return Token( Token::SyntaxError, d_lineNr, d_colNr, res );
			}else if( rhsPos == -1 )
			{
				// Ende noch nicht auf dieser Zeile
				res += escapeString(d_line, sym, &ok ) + QChar('\n');
				if( !ok )
					return Token( Token::SyntaxError, d_lineNr, d_colNr, res );
			}else
				return Token( Token::SyntaxError, d_lineNr, d_colNr, d_line.mid(d_colNr) );
		}
		Q_ASSERT( false );
		return Token( Token::SyntaxError, d_lineNr, d_colNr, res );
	}else
		return Token( Token::SyntaxError, d_lineNr, d_colNr, d_line.mid(d_colNr) );
}

StarLexer::Token StarLexer::readSemicolonString()
{
	Q_ASSERT( d_colNr == 0 && !d_line.isEmpty() && d_line[0].unicode() == SEM );
	QString res = d_line.mid(1) + QChar('\n');
	const int col = d_colNr + 1;
	const int line = d_lineNr;
	while( true )
	{
		if( d_in->atEnd() )
			return Token( Token::SyntaxError, d_lineNr, d_colNr, res ); // Semicolon String without end
		nextLine();
		if( !checkLineChars() )
			return Token( Token::InvalidChar, d_lineNr, d_colNr, QString(d_line[d_colNr]) );
		if( !d_line.isEmpty() && d_line[0].unicode() == SEM )
		{
			// Ende gefunden
			d_colNr++;
			return Token( Token::Multiline, line, col, res );
		}else
		{
			// Ende noch nicht gefunden; ganze Zeile gehört zu String
			res += d_line + QChar('\n');
		}
	}
	Q_ASSERT( false );
	return Token( Token::SyntaxError, d_lineNr, d_colNr );
}

QString StarLexer::readNonWhiteSpace()
{
	QString res;
	res.reserve( d_line.size() - d_colNr );
	const QString pattern = (d_newSyntax) ? QLatin1String("{}[],") : QLatin1String("");
	while( d_colNr < d_line.size() && !d_line[d_colNr].isSpace() && !pattern.contains(d_line[d_colNr]) )
	{
		res += d_line[d_colNr];
		d_colNr++;
	}
	return res;
}

int StarLexer::findTripple( const QString& pattern, int from) const
{
	Q_ASSERT( d_newSyntax );
	Q_ASSERT( !pattern.isEmpty() );
	int pos = d_line.indexOf( pattern, from );
	if( pos != -1 )
	{
		Q_ASSERT( pos > 0 );
		if( d_line[pos-1].unicode() == BEL )
		{
			if( (pos + 3) < d_line.size() && d_line[pos+3] == pattern[0] )
				return pos + 1; // BEL, QOT, QOT, QOT, QOT
			else
				return -2; // error, BEL, QOT, QOT, QOT
		}
	}
	return pos; // -1..not found
}

QString StarLexer::escapeString(const QString &str, int sym, bool *ok) const
{
	Q_ASSERT( d_newSyntax );
	int col = 0;
	QString res;
	res.reserve( str.size() );
	while( col < str.size() )
	{
		if( str[col].unicode() == BEL )
		{
			if( ( col + 1 ) < str.size() && str[col+1].unicode() == sym )
			{
				res += QChar(sym);
				col++;
			}else
			{
				if( ok )
					*ok = false;
				return res;
			}
		}else
			res += str[col];
		col++;
	}
	if( ok )
		*ok = true;
	return res;
}

bool StarLexer::matchTuple(int cur, int sym, int count) const
{
	Q_ASSERT( d_newSyntax );
	for( int i = cur; i < ( cur + count ); i++ )
	{
		if( i >= d_line.size() )
			return false;
		if( d_line[i].unicode() != sym )
			return false;
	}
	return true;
}

const char *StarLexer::Token::typeName() const
{
	static const char* names[] = { "Null",
								   "Comment",
								   "Global", "Data", "Save", // Cells
								   "Loop",  "Stop",  // Loops
								   "Tag",
								   "Quoted", "NonQuoted", "Multiline", // Values
								   "ListStart", "ListEnd",
								   "TableStart", "TableEnd",
								   "RefStart", "RefEnd",
								   "ElemSep",
								   "KeyValueSep",
								   "EndOfStream",
								   "InvalidChar", "SyntaxError"
								 };

	return names[d_type];
}

bool StarLexer::Token::isDataValue() const
{
	switch( d_type )
	{
	case Quoted:
	case NonQuoted:
	case Multiline:
	case ListStart:
	case TableStart:
	case RefStart:
		return true;
	default:
		return false;
	}
}
