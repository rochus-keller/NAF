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

#if !defined(AFX_STRUCTURE_H__C5C22E6B_E2D4_4FDB_83F9_185BEB7FF373__INCLUDED_)
#define AFX_STRUCTURE_H__C5C22E6B_E2D4_4FDB_83F9_185BEB7FF373__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Object.h>
#include <Spec/Conformer.h>

namespace Spec
{
	class Structure : public Root::Object
	{
	public:
		typedef std::map<Root::Index,Root::Ref<Conformer> > Conformers; 
		typedef std::map<Root::Index,Location> Locations; 
		// Index wird im allgemeinen als Spin-ID aufgefasst.

		void setName( const char* );
		const char* getName() const { return d_name.data(); }
		void setLoc( Root::Index, const Location& );
		const Location& getLoc( Root::Index ) const;
		const Locations& getLocs() const { return d_locs; }
		void removeConformer( Conformer* );
		Conformer* addConformer();
		const Conformers& getConfs() const { return d_confs; }
		Root::Index getId() const { return d_id; }
		const Location::Position& getOrig() const { return d_orig; }
		void setOrig(const Location::Position&);
		Project* getOwner() const { return d_owner; }

		//* Overrides von Object
		QByteArray  getInstanceName(bool = false) const;
		//-
		enum Hint { Orig, Loc };
		UPDATE_MESSAGE_DEF2( Changed, Structure, Hint );
		UPDATE_MESSAGE_DEF( Added, Structure );
		UPDATE_MESSAGE_DEF( Removed, Structure );
	protected:
		Structure():d_owner(0) {}
	private:
		Conformers d_confs;
		Location::Position d_orig; // Ursprung im Raum, auf den sich die Locations beziehen
		QByteArray  d_name;
		Root::Index d_id;
		Locations d_locs;
		Project* d_owner;

		friend class _XmlParser;
		friend class Project;
	};

}

#endif // !defined(AFX_STRUCTURE_H__C5C22E6B_E2D4_4FDB_83F9_185BEB7FF373__INCLUDED_)
