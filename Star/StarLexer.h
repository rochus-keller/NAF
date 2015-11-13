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


#ifndef STARLEXER_H
#define STARLEXER_H

#include <QString>

class QTextStream;
class QIODevice;

namespace Star
{
	class StarLexer
	{
	public:
		struct Token
		{
			enum Type { Null,
						Comment,
						Global, Data, Save, // Cells
						Loop,  Stop,  // Loops
						Tag,
						Quoted, NonQuoted, Multiline, // Values
						ListStart, ListEnd,
						TableStart, TableEnd,
						RefStart, RefEnd,
						ElemSep,
						KeyValueSep,
						EndOfStream,
						InvalidChar, SyntaxError // Issue
					  } d_type;
			int d_line, d_col;
			QString d_text;
			Token(Type t = Null, int line = 0, int col = 0, const QString& txt = QString() ):
				d_type(t),d_line(line),d_col(col),d_text(txt){}
			const char* typeName() const;
			bool isDataValue() const;
			bool isNull() const { return d_type == Null; }
		};

		StarLexer(bool newSyntax=true);
		StarLexer( const StarLexer& );
		~StarLexer();
		bool setStream( QIODevice* in, const char* codec = "UTF-8" );
		void reset();
		Token nextToken();
		Token nextTokenNoComments();
		bool isNewSyntax() const { return d_newSyntax; }
		void setNewSyntax( bool on ) { d_newSyntax = on; }
		void dump();
		StarLexer& operator=( const StarLexer& rhs );
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
		QTextStream* d_in;
		int d_lineNr; // current line, starting with 1
		int d_colNr;  // current column (left of char), starting with 0
		QString d_line;
		bool d_newSyntax; // Follow syntax as described in Spadaccini, N. and Hall, S. R., J. Chem. Inf. Model., 52 (8), 1901-1906 (2012)
	};
}

#endif // STARLEXER_H
