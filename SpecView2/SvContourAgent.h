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

#if !defined(AFX_CONTOURAGENT_H__1620B8AD_FD06_4179_9A34_B4E025061C97__INCLUDED_)
#define AFX_CONTOURAGENT_H__1620B8AD_FD06_4179_9A34_B4E025061C97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Gui/Reactor.h>
#include <SpecView2/ContourView.h>
#include <QList>

class QWidget;

namespace Spec2
{
	class ContourAgent : public Gui::Reactor
	{
	public:
		struct ContourParams
		{
			Root::Float d_factor;
			ContourView::Option d_option;
			Amplitude d_threshold;
		};

		void addView( ContourView* );
		ContourAgent(QWidget*, bool filterOwner = true);
		virtual ~ContourAgent();
		QWidget* wnd() const;

		bool setParams(QWidget *parent, ContourParams & p);

		static const char* DoOption;
		static const char* DoFactor;
		static const char* DoThreshold;
		static const char* DoNegColor;
		static const char* DoPosColor;
		static const char* DoAutoGain;
		static const char* DoParams;
		static const char* DoVisible;
		static const char* DoAuto;
	protected:
		QList< Root::ExRef<ContourView> > d_views;
	private:
		void handleOption( Root::Action& );
		void handleFactor( Root::Action& );
		void handleThreshold( Root::Action& );
		void handleNegColor( Root::Action& );
		void handlePosColor( Root::Action& );
		void handleAutoGain( Root::Action&);
		void handleParams( Root::Action& );
		void handleVisible( Root::Action& );
		void handleAuto( Root::Action& );
		IMPLEMENT_REACTOR( ContourAgent );
	};
}

#endif // !defined(AFX_CONTOURAGENT_H__1620B8AD_FD06_4179_9A34_B4E025061C97__INCLUDED_)
