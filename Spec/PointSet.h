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

#if !defined(AFX_CURSORSET_H__0DF9C25D_9EED_4A6D_AC8B_01EB432A5E00__INCLUDED_)
#define AFX_CURSORSET_H__0DF9C25D_9EED_4A6D_AC8B_01EB432A5E00__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Root/Subject.h>
#include <Spec/Units.h>
#include <Spec/AtomType.h>
#include <Root/Vector.h>

namespace Spec
{
	using Root::Subject;
	using Root::Index;
	using Root::ULong;
	using Root::Tag;

	//* PointSet
	//. Dies ist die (abstrakte) Basisklasse für konkrete Implementationen von
	//. ladbaren und speicherbaren Peaklisten verschiedener Formate.

	class PointSet : public Subject
	{
	public:
		typedef Root::Vector<Index> Selection;
		typedef Root::FixPoint<Root::Index, PpmPoint::MAX_DIM > Assig;

		//* Point Interface
		virtual void setComment( Index id, const char* );
		//. Ändere nur den Kommentar eines *bestehenden* Points.
		virtual const char* getComment( Index id ) const;
		//. Gebe den mit Point id assoziierten Kommentar zurück.
		virtual void setPoint( Index id, const PeakPos& );
		//. Füge einen neuen Point ein oder ändere einen bestehenden.
		virtual const PeakPos& getPoint( Index id ) const = 0;
		virtual void deletePoint( Index id );
		//. Entferne den Punkt mit der id aus der Liste.
		virtual bool isDefined( Index id ) const;
		//. Gebe zurück, ob eine id bereits belegt ist.
		virtual Index getNextId();
		//. Ermittle die nächste freie id, unter der ein Point gespeichert werden kann.
		virtual Selection find( const PpmCube& ) const;
		//. Gebe eine Selection zurück mit allen Punkten, die in Cube liegen.
		virtual Selection findAll() const;
		//. Gebe die IDs aller enthaltenen Peaks zurück.
		virtual const Assig& getAssig( Index ) const;  
		virtual void setAssig( Index, const Assig& );
		//. Setze und lese Assignment pro Dimension.
		//-

		//* Additional Point Interface
		virtual void setAmplitude( Index, Amplitude );
		virtual Amplitude getAmplitude( Index ) const;
		virtual void setVolume( Index, Amplitude );
		virtual Amplitude getVolume( Index ) const;
		virtual Root::UInt8 getCode( Index ) const; // Peak Color
		virtual void setCode( Index, Root::UInt8 );
		//-

		//* Dimension Interface
		virtual AtomType getColor( Dimension d ) const = 0;
		virtual Dimension getDimCount() const = 0;
		//-

		//* File Interface
		virtual void saveToFile( const char* path = "" ) {}
		//. Implementiert SaveAs (path!="") oder Save (path=="").
		virtual const char* getFilePath() const { return ""; }
		//. Gibt "" zurück, wenn Liste nicht mit einer Datei verknüpft ist.
		virtual bool isDirty() const { return false; }
		//. Gibt true zurück, falls ungesicherte Änderungen vorhanden
		//- 

		PointSet();

		//* Helper
		void getColors( ColorMap& ) const;

		class Update : public Root::UpdateMessage
		{
		public:
			enum Type { Add, Delete, Change, Rotate, Clear, All, Name };

			Update( PointSet* m, Type t, Index id = 0 ):
				Root::UpdateMessage( m ), d_type( t ), d_id( id ) {}

			Type getType() const { return d_type; }
			Index getId() const { return d_id; }
		private:
			Type d_type;
			Index d_id;
		};
	protected:
		virtual ~PointSet();
	};

}

#endif // !defined(AFX_CURSORSET_H__0DF9C25D_9EED_4A6D_AC8B_01EB432A5E00__INCLUDED_)
