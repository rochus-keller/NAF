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

#ifndef _Spec_Sample
#define _Spec_Sample

#include <Root/Subject.h>
#include <Root/UniqueList.h>

namespace Spec
{
	using Root::Index;
	class Project;

	// Defaultbereich deckt das ganze BioSample ab und hat schema=0 und d_assignable
	class BioSample : public Root::Object
	{
	public:
		struct Range
		{
			Index d_start;
			Index d_end;
			Index d_schema;
			bool d_assignable; // Ist ein Peak des Samples an diese Range zuordbar?
			Range(Index s = 0, Index e = 0, Index sch = 0, bool ass = false ):
				d_start(s),d_end(e),d_schema(sch),d_assignable(ass) {}
			bool operator<( const Range& rhs ) const { return d_start < rhs.d_start; }
		};
		typedef Root::UniqueList<Range> Ranges; // Start, Range

		const Ranges& getRanges() const { return d_regions; }
		bool addRange( Index start, Index end, Index schema, bool assignable );
		bool eraseRange( Index start );

		const char* getName() const { return d_name.data(); }
		void setName( const char* );

		bool isAssignable(Index pos) const;
		bool setAssignable( Index, bool );
		Index getSchema( Index pos ) const;
		bool setSchema( Index pos, Index schema );

		bool isAssignable() const { return d_assignable; }
		void setAssignable( bool );

		//* Overrides von Object
		QByteArray  getInstanceName(bool=false) const;
		//-
		Project* getOwner() const { return d_owner; }
		Index getId() const { return d_id; }
		BioSample();

		enum Hint { AddRange, EraseRange, Schema, Assignable, Name };
		UPDATE_MESSAGE_DEF3( Changed, BioSample, Hint, Index );
		UPDATE_MESSAGE_DEF( Added, BioSample );
		UPDATE_MESSAGE_DEF( Removed, BioSample );
	protected:
		const Range* getRange( Index pos ) const;
	private:
		friend class _XmlParser;
		friend class Project;

		Project* d_owner;
		Index d_id;
		QByteArray  d_name;
		Ranges d_regions;
		bool d_assignable; // Ist Ist ein Peak an die Umgebung (d.h. nicht mit Ranges belegte) zuordbar?
	};
}

#endif
