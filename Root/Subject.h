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

#if !defined(AFX_SUBJECT_H__2D0B9A85_5436_11D5_8DB0_00D00918E99C__INCLUDED_)
#define AFX_SUBJECT_H__2D0B9A85_5436_11D5_8DB0_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Root/Object.h>

namespace Root
{
	//* Subject
	//. Dies ist die Elternklasse aller Modelle bzw. Zustandsobjekte,
	//. die eine Zustandsänderung an irgendwelche Observer senden müssen.
	//. Observer erhalten eine UpdateMessage oder eine davon abgeleitete.

	class Subject : public Object
	{
	public:
		Subject();
        ~Subject();

		void notifyObservers( UpdateMessage&, bool all = true );
        void removeObserver( Messenger* );
        void addObserver( Messenger* );

		//* Override
		virtual void setFieldValue( Field, const Any& );
	private:
        QList<Messenger*> d_observers; // QList hat im Grundzustand nur die Grösse eines Pointers!
	};
}

#endif // !defined(AFX_SUBJECT_H__2D0B9A85_5436_11D5_8DB0_00D00918E99C__INCLUDED_)
