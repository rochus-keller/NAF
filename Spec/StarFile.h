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

#if !defined(AFX_STARFILE_H__DD1089DA_859D_4928_9596_E3E68A42C233__INCLUDED_)
#define AFX_STARFILE_H__DD1089DA_859D_4928_9596_E3E68A42C233__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/SymbolString.h>
#include <Spec/Units.h>
#include <fstream>
#include <map>

namespace Spec
{
	class StarFile
	{
	public:
		typedef std::map<Root::Index,Root::SymbolString> Resi;
		class AtomSym 
		{
		public:
			Root::Index d_resi;
			Root::Index d_spin;
			Root::SymbolString d_label;
			PPM d_shift;
			AtomSym( Root::Index r, Root::Index j, Root::SymbolString l, PPM s )
			{
				d_resi = r;
				d_spin = j; 
				d_label = l;
				d_shift = s;
				d_next = 0;
			}
			AtomSym( const AtomSym& lhs )
			{
				d_resi = lhs.d_resi;
				d_spin = lhs.d_spin; 
				d_label = lhs.d_label;
				d_shift = lhs.d_shift;
				d_next = 0;
			}
			AtomSym()
			{
				d_resi = 0;
				d_spin = 0; 
				d_shift = 0;
				d_next = 0;
			}
			~AtomSym()
			{
				if( d_next )
					delete d_next;
			}
			AtomSym* getNext() const { return d_next; }
		private:
			AtomSym* d_next;
			friend class StarFile;
		};

		QByteArray  d_Entry_title;
		QByteArray  d_BMRB_accession_number;
		QByteArray  d_BMRB_flat_file_name;
		QByteArray  d_Submission_date;
		QByteArray  d_Accession_date;
		QByteArray  d_Citation_full;

		QByteArray  d_err;
		int d_line;
		AtomSym* getFirst() const { return d_first; }
		const Resi& getSeq() const { return (d_autorSeq)?d_seq2:d_seq1; }
		Root::Index getCount() const { return d_count; }
		void parse();
		StarFile(const char*);
		~StarFile();
	private:
		std::ifstream d_in;
        void* operator new( size_t size ) throw() { return 0; }
        void* operator new( size_t size, void *p ) throw() { return 0; }

	
		Root::Index d_count;
		Resi d_seq1;
		Resi d_seq2;
		AtomSym* d_first;
		AtomSym* d_last;
		bool d_autorSeq;

		struct Token 
		{
			enum Type { GLOBAL, DATA, SAVE, LOOP, STOP, NAME, VALUE, _EOF };
			Type d_type;
			QByteArray  d_val;
		};
		void next( Token& );
		enum State { IDLE, INFO, CITATION, ASSIG, POLY };
		State d_stat;
	protected:
		bool parseSave();
		bool parseAtoms();
		bool parseSequence();
		bool parseLoop();
		void starFile();
		void add( const AtomSym& );
		bool isWhitespace( int ch );
	};
}

#endif // !defined(AFX_STARFILE_H__DD1089DA_859D_4928_9596_E3E68A42C233__INCLUDED_)
