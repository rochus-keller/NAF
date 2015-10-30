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

#ifndef STARLEXER_H
#define STARLEXER_H

#include <QTextStream>

namespace Star
{
	class StarLexer
	{
	public:
		struct Token
		{
			enum Type { Tag, Quoted, NonQuoted, Multiline, Loop, Global, Save, Stop, Data, ListStart, ListEnd, ElemSep,
						TableStart, TableEnd, RefStart, RefEnd, KeyValueSep, Comment,
						EndOfStream, InvalidChar, SyntaxError } d_type;
			int d_line, d_col;
			QString d_text;
			Token(Type t = EndOfStream, int line = 0, int col = 0, const QString& txt = QString() ):
				d_type(t),d_line(line),d_col(col),d_text(txt){}
			const char* typeName() const;
			bool isDataValue() const;
		};

		StarLexer();
		bool setStream( QIODevice* in, const char* codec = "UTF-8" );
		void reset();
		Token nextToken();
		Token nextTokenNoComments();
		void dump();
	protected:
		void nextLine();
		void skipWhiteSpace();
		bool isValid( const QChar& ch ) const;
		bool checkLineChars();
		Token readTag();
		Token readSingleQuotString(int sym);
		Token readTrippleQuotString(int sym);
		Token readSemicolonString();
		QString readNonWhiteSpace();
		int findTripple(const QString &pattern, int from = 0) const;
		QString escapeString( const QString& str, int sym, bool* ok = 0 ) const;
		bool matchTuple( int cur, int sym, int count ) const;
	private:
		QTextStream d_in;
		int d_lineNr; // current line, starting with 1
		int d_colNr;  // current column (left of char), starting with 0
		QString d_line;
	};
}

#endif // STARLEXER_H
