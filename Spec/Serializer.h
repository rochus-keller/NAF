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

#if !defined(AFX_SERIALIZER_H__447E11EA_9923_4794_8C9E_C1C673F186A6__INCLUDED_)
#define AFX_SERIALIZER_H__447E11EA_9923_4794_8C9E_C1C673F186A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Object.h>
#include <Spec/Units.h>
#include <QTextStream>

namespace Spec
{
	class Repository;
	class Project;
	class ResidueType;
	class SpectrumType;
	class Residue;
	class SpectrumPeer;
	class Spin;
	class SpinSystem;
	class PeakList;
	class SystemType;
	class SpinLink;
	class SpinBase;
	class ObjectDef;
	class Structure;
	class LabelingScheme;
	class BioSample;

	//* Serializer
	//. Dies ist eine Value-Klasse, welche das ganze Know-How versteckt,
	//. wie ein Repository samt enthaltener Klassen serialisiert bzw. 
	//. deserialisiert werden soll.
	class Serializer 
	{
	public:
		void deserialize( const char* path, bool asTemplate );
		void serialize( const char* path, const char* creator = "" );

		Serializer(Repository*);
		virtual ~Serializer();

		static const int s_version;
		static const int s_subversion;
	private:
		void writeObjectDef( QTextStream&, ObjectDef* );
		void writeObject( QTextStream&, Root::Object* );
        void writeObject( QTextStream&, Root::SmallObject* );
		void writePeakList( QTextStream&, PeakList* );
		void writeLink( QTextStream&, SpinSystem* );
		void writePair( QTextStream&, SpinBase*, SpinLink* );
		void writeSpin( QTextStream&, Spin* );
		void writeSpinSystem( QTextStream&, SpinSystem* );
		void writeSample( QTextStream&, BioSample* );
		void writeSpectrum( QTextStream&, SpectrumPeer* );
		void writeStruct( QTextStream&, Structure* );
		void writeResidue( QTextStream&, Residue* );
		void writeProject( QTextStream&, Project* );
		void writeResiType( QTextStream&, ResidueType* );
		void writeLabelingScheme( QTextStream&, LabelingScheme* );
		void writeSpecType( QTextStream&, SpectrumType* );
		void writeSpinSystemType( QTextStream&, SystemType* );
		Repository* d_rep;
	};
}

#endif // !defined(AFX_SERIALIZER_H__447E11EA_9923_4794_8C9E_C1C673F186A6__INCLUDED_)
