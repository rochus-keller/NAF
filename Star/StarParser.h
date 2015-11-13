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

#ifndef STARPARSER_H
#define STARPARSER_H

#include <QObject>
#include <Star/StarLexer.h>
#include <QMap>
#include <QQueue>
#include <QVariant>
#include <QStringList>
#include <QVector>

namespace Star
{
	/*
	  This is a Star file format parser only depending on Qt. It can handle the 1991 as well as the 2012 format versions
	  and passes all test sets available from the internet.
	  See:
			Hall, S. R. and Spadaccini, N., "The STAR File: Detailed Specifications,"
			J. Chem. Inf. Comput. Sci. 34, 505-508 (1994)

			Spadaccini, N. and Hall, S. R., "Extensions to the STAR File Syntax"
			J. Chem. Inf. Model., 52 (8), 1901-1906 (2012) DOI: 10.1021/ci300074v
	*/

	class StarParser
	{
	public:
		struct Loop
		{
			typedef QVariantList Row; // Wenn die Row eine Sub-Table hat, wird diese einfach als weiterer Variant angefügt
			typedef QVector<Row> Table;

			typedef QStringList Header;
			typedef QVector<Header> Headers;
			Headers d_headers; // Top-down ein Header für jeden Loop-Level
			Table d_table;
		};
		struct Block
		{
			QMap<QString,Block> d_blocks;
			QMap<QString,QVariant> d_items;
			QList<Loop> d_loops;
		};
		typedef QMap<QString,Block> Blocks;
		struct Reference
		{
			QVariant d_source;
			QVariant d_block;
			QVariant d_frame;
			QVariant d_item;
			QVariant d_key;
		};

		StarParser(bool newSyntax = true); // true..2012 syntax; false..1991 syntax
		bool parse( QIODevice*, const char* codec = "UTF-8" );
		const QString& getError() const { return d_error; }
		const Block& getGlobal() const { return d_global; }
		const Blocks& getBlocks() const { return d_blocks; }
		void clear();
		bool isNewSyntax() const { return d_lex.isNewSyntax(); }
		void setNewSyntax(bool on) { d_lex.setNewSyntax(on); }
		void dump(QTextStream&);
		void dump();
	protected:
		bool error( const QString& msg );
		bool error( const QString &msg, const StarLexer::Token& );
		bool unarm( bool );
		bool unexpectedToken( const StarLexer::Token& );
		bool parseBlock( Block&, bool frameIn, bool frameOut );
		bool parseLoopHeader( Loop&, int level );
		bool parseLoopTable( const Loop::Headers&, Loop::Table &, int level );
		bool parseDataValue( QVariant& );
		bool parseRefValue( QVariant& );
		bool parseList( QVariantList&, bool first = true );
		bool parseTable( QVariantMap&, bool first = true );
		bool parseReference( Reference& );
		StarLexer::Token nextToken();
		StarLexer::Token lookAhead();
		void dumpBlock( QTextStream&, const Block&, const QString &title, int level );
		void dumpTable( QTextStream&, const Loop::Table&, int level );
	private:
		StarLexer d_lex;
		QString d_error;
		Block d_global;
		Blocks d_blocks;
		StarLexer::Token d_lookAhead;
	};
}

Q_DECLARE_METATYPE( Star::StarParser::Reference )
Q_DECLARE_METATYPE( Star::StarParser::Loop::Table )

#endif // STARPARSER_H
