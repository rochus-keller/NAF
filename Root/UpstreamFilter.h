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

#if !defined(AFX_UPSTREAMFILTER_H__1553F627_5004_4E77_A485_DC23F4A65AF0__INCLUDED_)
#define AFX_UPSTREAMFILTER_H__1553F627_5004_4E77_A485_DC23F4A65AF0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Root/Agent.h>

namespace Root
{
	//* UpstreamFilter
	//. Dieses Filter unterbindet den Message-Traversal von unten nach oben.
	//. Dazu wird traverseUp() überschrieben.

	class UpstreamFilter : public Agent
	{
	public:
		void traverseUp( Message &);
		void filterInput( bool on ) { d_input = on; }
		void filterWindow( bool on ) { d_window = on; }
		void filterApplication( bool on ) { d_application = on; }
		void filterSystem( bool on ) { d_system = on; }
		void filterInternal( bool on ) { d_internal = on; }
		void filterUser( bool on ) { d_user = on; }
		void filterUpdate( bool on ) { d_update = on; }
		void filterAction( bool on ) { d_action = on; }

		UpstreamFilter(Agent* parent, bool action = false );
	protected:
		virtual ~UpstreamFilter();
	private:
		bool d_input;
		bool d_window;
		bool d_application;
		bool d_system;
		bool d_internal;
		bool d_user;
		bool d_update;
		bool d_action;

	};
}

#endif // !defined(AFX_UPSTREAMFILTER_H__1553F627_5004_4E77_A485_DC23F4A65AF0__INCLUDED_)
