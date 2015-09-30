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

#if !defined(AFX_MESSAGELOG_H__EA448B21_DD96_43C4_B5E2_1847736AA777__INCLUDED_)
#define AFX_MESSAGELOG_H__EA448B21_DD96_43C4_B5E2_1847736AA777__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Subject.h>
#include <Root/SymbolString.h>
#include <QString>

namespace Root
{
	//* MessageLog
	//. Wird systemweit zur Nachrichtenerfassung verwendet. Kann von 
	//. einem Nachrichtenfenster abonniert werden.

	class MessageLog : public Subject
	{
	public:
		enum Kind { Trace, Info, Warning, Error };
		static const char* s_pretty[]; 
		struct Entry
		{
			SymbolString d_src;
			QByteArray d_msg;
			Kind d_kind;
			Entry( Kind k , const char* src, const QByteArray& msg ):
				d_kind( k ), d_src( src ), d_msg( msg ) {}
		};

		void log( Kind, const char* src, const QByteArray& msg );
        void warning( const char* src, const char* msg ) { log( Warning, src, msg ); }
		void warning( const char* src, const QByteArray& msg ) { log( Warning, src, msg ); }
        void warning( const char* src, const QString& msg ) { log( Warning, src, msg.toLatin1() ); }
		void error( const char* src, const QByteArray& msg ) { log( Error, src, msg ); }
		void info( const char* src, const QByteArray& msg ) { log( Info, src, msg ); }
		void trace( const char* src, const QByteArray& msg ) { log( Trace, src, msg ); }
		void saveToStream( QIODevice* );

		int getCount() const;
		void clearLog();
		const Entry& getEntry( int ) const;

		static MessageLog* inst();
		Kind getLevel() const { return d_level; }
		void setLevel( Kind k ) { d_level = k; }
		void setRemember( bool = true );
		static void kill();

		class Update : public UpdateMessage
		{
		public:
			enum Type { Add, All };
			Update( Root::Object* m, Type t = All, int i = -1 ):
				UpdateMessage( m ), d_type( t ), d_nr( i ) {}
			Type getType() const { return d_type; }
			int getNr() const { return d_nr; }
		private:
			Type d_type;
			int d_nr;
		};
	protected:
		MessageLog();
		virtual ~MessageLog();
		Kind d_level;
	};
}

#endif // !defined(AFX_MESSAGELOG_H__EA448B21_DD96_43C4_B5E2_1847736AA777__INCLUDED_)
