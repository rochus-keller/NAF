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

#if !defined(AFX_CURSORSET_H__7F6D2665_418A_4849_BD93_05A9BAAF3442__INCLUDED_)
#define AFX_CURSORSET_H__7F6D2665_418A_4849_BD93_05A9BAAF3442__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Subject.h>
#include <Spec/Units.h>
#include <Root/Vector.h>

namespace Spec
{
	using Root::Index;

	//* CursorSet
	//. Dies ist das Modell zur Verwaltung von einem oder mehreren Cursors.
	//. Ein Cursor wird auf dem Bildschirm als Linie dargestellt.

	class CursorSet : public Root::Subject
	{
	public:
		typedef std::set<Index> Cursors;

		Cursors getCursors() const;
		Index createCursor();

		bool hasActiveCursor() const { return d_active != -1; }
		Index getActiveCursor() const { return d_active; }
		void setActiveCursor( Index );
		Index findCursor( Dimension, PPM v, PPM delta, bool notActive = true ) const;
	
		const PpmPoint& getCursor( Index ) const;
		void setCursor( Index, Dimension, PPM );
		//. Verwendet active oder lastActive Cursor, falls Index = -1.
		const PpmPoint& getCursor() const { assert( d_active != -1 ); return getCursor( d_active ); }
		void setCursor( Dimension, PPM );
		void setCursor( const PpmPoint& );
		void setCursor( Index, const PpmPoint& );
		void removeCursor( Index );

		void setDimension( Dimension d );
		Dimension getDimCount() const { return d_dim; }
		CursorSet(Dimension);

		class Update : public Root::UpdateMessage
		{
		public:
			enum Type { Active, Add, Remove, Upd, Rotate, Clear };

			// Für Rotate und Clear
			Update( CursorSet* m, Type t, Index id = -1 ):
				Root::UpdateMessage( m ), d_type( t ), d_id( id ) {}
			// Für Active, Add, Remove und Update
			Update( CursorSet* m, Type t, Index id, const PpmPoint& old ):
				Root::UpdateMessage( m ), d_type( t ), d_id( id ), d_old( old ) {}

			Type getType() const { return d_type; }
			Index getId() const { return d_id; }
			const PpmPoint& getOld() const { return d_old; }
		private:
			Type d_type;
			Index d_id;
			PpmPoint d_old;
		};
	protected:
		virtual ~CursorSet();
	private:
		typedef std::map<Index,PpmPoint> Base;

		Index d_active;
		Index d_lastActive;
		Base d_cursors;
		Dimension d_dim;

		Index nextCursor() const;
	};

}

#endif // !defined(AFX_CURSORSET_H__7F6D2665_418A_4849_BD93_05A9BAAF3442__INCLUDED_)
