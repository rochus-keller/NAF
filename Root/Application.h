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

#if !defined(AFX_APPLICATION_H__2D0B9A80_5436_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_APPLICATION_H__2D0B9A80_5436_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Root/Agent.h>
#include <Root/Sequence.h>
#include <QStringList>
#include <QApplication>

namespace Root
{
	//* AppAgent
	//. Dies ist der Top-Level-Agent einer Anwendung.

	class AboutToQuit : public ApplicationEvent	
	{
	public:
        // Agent* operator[]( UInt32 i ) const { return d_vetos[ i ]; }
		UInt32 getCount() const { return d_vetos.size(); }
		void veto( Agent* a ) { d_vetos.push_back( a ); }
		void consume() {}
		~AboutToQuit() {}
	private:
		Root::Sequence<Agent*> d_vetos;
	};

	class QuitNow : public ApplicationEvent	
	{
	public:
		void consume() {}
		QuitNow() {}
		~QuitNow() {}
	};

	class AppAgent : public Agent
	{
	public:
		AppAgent();
		static AppAgent* inst();

		static int run();
		static QStringList arguments();
		static const char* getCurrentDir() { return d_currentDir.data(); }
		static void setCurrentDir( const char* s ) { d_currentDir = s; }

		//* Folgende für langdauernde Operationen
		static bool begin( int id = 0 );	// true..continue
		static bool yield( int id = 0, float done = 0.0 ); // true..continue
		static void escape();
		static void commit( int id = 0 );
	protected:
		virtual ~AppAgent();
		void handle( Message& );
	private:
		static QByteArray  d_currentDir;
		static bool d_esc;
	};

	class Application : public QApplication
	{
	public:
		Application( int& argc, char **argv, bool gui = true );
	protected:
		bool notify( QObject * o, QEvent * e );
	};
}

#endif // !defined(AFX_APPLICATION_H__2D0B9A80_5436_11D5_8DB0_00D00918E99C__INCLUDED_)
