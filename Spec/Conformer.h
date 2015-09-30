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

#if !defined(AFX_CONFORMER_H__AF921740_2091_434C_8B84_1B07E435D7AD__INCLUDED_)
#define AFX_CONFORMER_H__AF921740_2091_434C_8B84_1B07E435D7AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Object.h>
#include <Spec/Units.h>
#include <Spec/SpinSystem.h>
#include <map>

namespace Spec
{
	class Structure;

	class Conformer : public Root::Object
	{
	public:
        typedef std::map<Root::Index,Location::Position> Coords;
		// Index wird im allgemeinen als Spin-ID aufgefasst.

		Root::Index getId() const { return d_id; }
		Root::Index getNr() const { return d_nr; }
        const Location::Position& getCoord(Root::Index) const;
		const Coords& getCoords() const { return d_coords; }
        void setCoord( Root::Index, const Location::Position& );
		void setNr( Root::Index );
		Structure* getOwner() const { return d_owner; }
	
		//* Overrides von Object
		QByteArray  getInstanceName(bool = false) const;
		//-
        enum Hint { Position, Nr };
		UPDATE_MESSAGE_DEF2( Changed, Conformer, Hint );
		UPDATE_MESSAGE_DEF( Added, Conformer );
		UPDATE_MESSAGE_DEF( Removed, Conformer );
	protected:
		Conformer():d_owner(0) {}
	private:
		Root::Index d_id;
		Coords d_coords;
		Root::Index d_nr; // Frei zuweisbare Nummer. Initial gleich id.
		Structure* d_owner;

		friend class _XmlParser;
		friend class Structure;
	};

}
#endif // !defined(AFX_CONFORMER_H__AF921740_2091_434C_8B84_1B07E435D7AD__INCLUDED_)
