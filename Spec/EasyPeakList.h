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

#if !defined(AFX_EASYPEAKLIST_H__F9AD069E_117C_4D68_BE71_40CC07FB7F9D__INCLUDED_)
#define AFX_EASYPEAKLIST_H__F9AD069E_117C_4D68_BE71_40CC07FB7F9D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Spec/PointSet.h>
#include <Root/Vector.h>
#include <Spec/AtomType.h>
#include <Root/Vector.h>

#define _EasyPeak_amplitude_
#define _EasyPeak_atoms_

namespace Spec
{
	using Root::Index;
	namespace Private
	{
		class EasyPeak 
		{
		public:
			EasyPeak();

			void init(Dimension);
			const char* getComment() const { return d_comment.data(); }
			PPM getFreq( Dimension d ) const { return d_point[ d ]; }
			Index getId() const { return d_id; }
			const PeakPos& getPoint() const { return d_point; }
			void rotate( const Rotation& );
		private:
			friend class EasyPeakList;
			// Intensity d_level;
			Index d_id;
			QByteArray  d_comment;
			PeakPos d_point;
			Amplitude d_volume;
			Root::UInt8 d_color;
	#ifdef _EasyPeak_amplitude_
			Amplitude d_level;
	#endif
	#ifdef _EasyPeak_atoms_
			PointSet::Assig d_atoms;
	#endif
	#ifdef _EasyPeak_options_
			//* Attribute der Peak-List ohne Verwendung
			Amplitude d_volError;
			char d_integrationMethod;
			QByteArray  d_specType;
	#endif
		};

		class EasyPeakList : public PointSet
		{
		public:
			typedef std::map<Index,EasyPeak> PeakList;
			typedef Root::Vector<Index> Selection;

			const EasyPeak& getPeak( Index ) const;
			void clear();

			//* Overrides von PointSet
			void addDimension( const AtomType&, PPM = 0 );
			AtomType getColor(Dimension) const;
			Dimension getDimCount() const;
			void saveToFile( const char* path = "" );
			const char* getFilePath() const { return d_filePath.data(); }
			bool isDirty() const { return d_dirty; }
			void deletePoint( Index id );
			Selection find( const PpmCube& ) const;
			Selection findAll() const;
			void rotate( const Rotation& );
			Root::ULong getPointCount() const { return d_peaks.size(); }
			const PeakPos& getPoint( Index id ) const;
			const char* getComment( Index id ) const;
			Index getNextId();
			bool isDefined( Index id ) const;
			void setPoint( Index id, const PeakPos& );
			void setComment( Index id, const char* );
			Index getCount() const { return d_peaks.size(); }
			Amplitude getVolume( Index ) const;
			void setVolume( Index, Amplitude );
			const Assig& getAssig( Index ) const;
			void setAssig( Index, const Assig& );
			void setCode( Index, Root::UInt8 );
			Root::UInt8 getCode(Index) const;
	#ifdef _EasyPeak_amplitude_
			void setAmplitude( Index, Amplitude );
			Amplitude getAmplitude( Index ) const;
	#endif
			//-

			EasyPeakList(const char* filePath);
			//. Lade Liste aus Datei. Gibt Exception bei Lese- oder Syntaxfehler.
			EasyPeakList();
			//. Erstelle eine leere Peak-Liste.
			virtual ~EasyPeakList();

		private:
			void writePeakList();
			void readPeakList();
			QByteArray  d_filePath;
			PeakList d_peaks;
			Root::Vector<QByteArray > d_labels;
			Index d_nextId;
			bool d_dirty;
			ColorMap d_colors;
		};
	}
}

#endif // !defined(AFX_EASYPEAKLIST_H__F9AD069E_117C_4D68_BE71_40CC07FB7F9D__INCLUDED_)
