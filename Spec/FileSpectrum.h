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

#if !defined(AFX_FILESPECTRUM_H__0E256961_49CE_4E3A_8313_508EA347E4C1__INCLUDED_)
#define AFX_FILESPECTRUM_H__0E256961_49CE_4E3A_8313_508EA347E4C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Spec/Spectrum.h>
#include <Spec/Scale.h>

namespace Spec
{
	namespace Private
	{
		class Stream;
	}

	class FileSpectrum : public Spectrum
	{
	public:
		//* Overrides
		Dimension getDimCount() const { return d_scales.size(); }
		const char* getFilePath() const { return d_specFile.data(); }
		const Scale& getScale(Dimension i ) const { return d_scales[i]; }
		void adjustScale(Dimension d, PPM off ) { d_scales[ d ].adjust( off ); }
		void setFolding( Dimension, Scale::Folding );
		MHz getRfFreq( Dimension d ) const { return d_rf[ d ]; }
		int close() const;
		int open() const;
		void fillBuffer( Buffer &, const Rotation&, const Root::Cube& cube, 
			const Root::Point& ns = Root::Point() ) const;
		Amplitude getAt( const Point& ) const;
		Amplitude getRaw( Root::Int64 i ) const;
		Levels getLevels(bool force = false) const;
		bool canDownsize() const;
		void setThreshold( Amplitude a ) { d_thres = a; }
		Amplitude getThreshold() const;
		//-
		static void setMapThreshold( Root::UInt32 t );
		static Root::UInt32 getMapThreshold() { return s_mapThreshold; }

		Root::Index getWord() const { return d_word; }
	protected:
		FileSpectrum();
		virtual ~FileSpectrum();

		//. Folgende Routine hat die Idee, einen Cube oder ein Rect durch zwei Eckpunkte zu
		//. beschreiben. start ist in Ursprungsrichtung und end diametral dazu.
		//. P wandert zwischen start und end. Dabei wird immer entlang D0 gegangen. Bei 
		//. jedem Durchlauf durch ganze D0 wird D1..Dn um eins inkrementiert.
		static inline bool increment( Root::Point& p, const Root::Point& start, 
			const Root::Point& end, int off = 0 )
		{
			for( Dimension d = off; d < start.size(); d++ )
			{
				if( p[ d ] == end[ d ] )
				{
					p[ d ] = start[ d ];
					if( d == start.size() - 1 )
						return false;
				}else
				{
					p[ d ]++;
					return true;
				}
			}
			return false;
		}
		virtual Amplitude convert( const Root::Byte* ) const;

		static Root::Byte* buf() { return s_tile; }
		static void allocateTile( Root::UInt32 count );
		Private::Stream* stream() const { return d_stream; }
		void init();
		static void allocateMap( Dimension, Sample );
	protected:
		static Root::Byte* s_tile;
		static Root::UInt32 s_count;
		QByteArray  d_specFile;
		Root::FixVector<MHz> d_rf;
		ScaleVector d_scales;
		Root::Index d_word;			// Anzahl Bytes per Sample-Wert
		Root::UInt32 d_off;			// Beginn des Datablocks ab File-Anfang (default 0)
		// Redundant
		Root::Extension d_ext;

		typedef Root::FixVector< std::vector<Sample> > DownsizeMap;
		static DownsizeMap s_map;
	private:
		Amplitude d_thres;	// Damit auch MonoScope ohne Projekt funktioniert
		mutable int d_useCount;
		mutable Root::Own<Private::Stream> d_stream;
		static Root::UInt32 s_mapThreshold;
	};

	class TileSpectrum : public FileSpectrum
	{
	public:
		//* Overrides
		void fillBuffer( Buffer &, const Rotation&, const Root::Cube& cube, 
			const Root::Point& ns = Root::Point() ) const;
		Amplitude getAt( const Point& ) const;
		bool canDownsize() const;
		//-
	protected:
		TileSpectrum();
		~TileSpectrum();

		static inline Root::Int64 getIndex(const Root::Extension& tileSize,
			const Root::Extension& tileCount, Root::Index cellCount, 
			int word, const Root::Point & point)
		{
			Root::Point tile = point;
			Root::Point off;
			off.assign( tileSize.size(), 0 );
			for( Dimension i = 0; i < tileSize.size(); i++ )
			{
				tile[ i ] /= tileSize[ i ];
				off[ i ] =  point[ i ] - tileSize[ i ] * tile[ i ];
			}
			return ( tile.getIndex( tileCount ) * cellCount + 
				off.getIndex( tileSize ) ) * word;
		}
		void adjustNumOfSamples();
		void init();
	protected:
		Root::Extension d_tileSize;
		// Redundant
		Root::Index d_tileCellCount;	// Anzahl Zellen im Tile
		Root::Extension d_tileCount;
	};
}

#endif // !defined(AFX_FILESPECTRUM_H__0E256961_49CE_4E3A_8313_508EA347E4C1__INCLUDED_)
