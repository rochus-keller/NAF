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
	class StarParser : public QObject
	{
	public:
		struct Loop
		{
			// RISK: laut Spec sollte d_names mehrere Levels haben und d_values einen hierarchischen Aufbau.
			// aber sehr schlecht spezifiziert und keine praktische Anwendung bekannt; darum hier nur ein Level unterstützt.
			QStringList d_names;
			QVector<QVariantList> d_values;
		};
		struct Block
		{
			QMap<QString,Block> d_blocks;
			QMap<QString,QVariant> d_items;
			QList<Loop> d_loops;
		};
		struct Reference
		{
			QVariantList d_source;
			QVariantList d_block;
			QVariantList d_frame;
			QVariantList d_item;
			QVariantList d_key;
		};

		StarParser();
		bool parse( QIODevice* );
		const QString& getError() const { return d_error; }
		void clear();
		void dump();
	protected:
		bool error( const QString& msg );
		bool error( const QString &msg, const StarLexer::Token& );
		bool unexpectedToken( const StarLexer::Token& );
		bool parseBlock( Block&, bool frameIn, bool frameOut );
		bool parseLoopHeader( Loop&, int level );
		bool parseDataValue( QVariant& );
		bool parseRefValue( QVariant& );
		bool parseList( QVariantList&, bool first = true );
		bool parseTable( QVariantMap&, bool first = true );
		bool parseReference( Reference& );
		StarLexer::Token nextToken();
		StarLexer::Token peekToken();
		void dumpBlock( const Block&, const QString &title, int level );
	private:
		StarLexer d_lex;
		QString d_error;
		Block d_global;
		QMap<QString,Block> d_blocks;
		QQueue<StarLexer::Token> d_peeks;
	};
}

Q_DECLARE_METATYPE( Star::StarParser::Reference )

#endif // STARPARSER_H
