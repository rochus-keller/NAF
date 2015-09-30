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
 
#include "StarFile.h"
#include <Root/Any.h>
#include <ctype.h>
using namespace Spec;

#define _Atom_shift_assign_ID "_Atom_shift_assign_ID"
#define _Residue_seq_code "_Residue_seq_code"
#define _Residue_label "_Residue_label"
#define _Residue_author_seq_code "_Residue_author_seq_code"
#define _Atom_name "_Atom_name"
#define _Atom_type "_Atom_type"
#define _Chem_shift_value "_Chem_shift_value"
#define _Chem_shift_value_error "_Chem_shift_value_error"
#define _Chem_shift_ambiguity_code "_Chem_shift_ambiguity_code"

static const char* s_tok[] =
{
	"GLOBAL",
	"DATA",
	"SAVE",
	"LOOP",
	"STOP",
	"NAME",
	"VALUE",
	"EOF"
};


StarFile::StarFile(const char* path):
	d_in( path ), d_first( 0 ), d_last( 0 ), d_count( 0 ), d_autorSeq( false ), d_line( 0 )
{
	qDebug( "**** Parsing BMRB File %s", path );
}

StarFile::~StarFile()
{
	if( d_first )
		delete d_first;
	qDebug( "**** Finished Parsing BMRB File" );
}

bool StarFile::isWhitespace( int ch )
{
	if( ch == '\n' )
		d_line++;
	return isspace( ch );
}

void StarFile::next(Token & t)
{
	int ch = d_in.get();
	while( !d_in.eof() )
	{
		while( isWhitespace( ch ) && !d_in.eof() )
			ch = d_in.get();
comment:
		if( ch == '#' ) // überlese Kommentare bis Zeilenende
		{
			while( ch != 10 && ch != 13 && !d_in.eof() )
				ch = d_in.get();
		}
		while( isWhitespace( ch ) && !d_in.eof() )
			ch = d_in.get();
		if( ch == '#' )
			goto comment;

		if( ch == '_' && !d_in.eof() )
		{
			// Parse DATA_NAME
			t.d_type = Token::NAME;
			t.d_val ="_";
			ch = d_in.get();
			while( !isWhitespace( ch ) && !d_in.eof() )
			{
				t.d_val += ch;
				ch = d_in.get();
			}
			return;
		}else if( ch == '\"' && !d_in.eof() )
		{
			t.d_type = Token::VALUE;
			t.d_val ="";
			ch = d_in.get();
			while( ch != '\"' && !d_in.eof() )
			{
				t.d_val += ch;
				ch = d_in.get();
			}
			return;
		}else if( ch == '\'' && !d_in.eof() )
		{
			t.d_type = Token::VALUE;
			t.d_val = "";
			ch = d_in.get();
			while( ch != '\'' && !d_in.eof() )
			{
				t.d_val += ch;
				ch = d_in.get();
			}
			return;
		}else if( ch == ';' && !d_in.eof() )
		{
			t.d_type = Token::VALUE;
			t.d_val = "";
			bool first = false;
			while( !d_in.eof() )
			{
				ch = d_in.get();
				if( first && ch == ';' )
					return;
				if( ch == '\n' )
					d_line++;
				first = ch == 10 || ch == 13;
				t.d_val += ch;
			}
			return;
		}else if( !d_in.eof() )
		{
			t.d_type = Token::VALUE;
			t.d_val = "";
continueValue:
			while( !isWhitespace( ch ) && ch != '_' && !d_in.eof() )
			{
				t.d_val += ch;
				ch = d_in.get();
			}
			if( ch == '_' )
			{
				if( t.d_val == "global" || t.d_val == "GLOBAL" )
					t.d_type = Token::GLOBAL;
				else if( t.d_val == "data" || t.d_val == "DATA" )
					t.d_type = Token::DATA;
				else if( t.d_val == "save" || t.d_val == "SAVE" )
					t.d_type = Token::SAVE;
				else if( t.d_val == "loop" || t.d_val == "LOOP" )
					t.d_type = Token::LOOP;
				else if( t.d_val == "stop" || t.d_val == "STOP" )
					t.d_type = Token::STOP;
				else
				{
					t.d_val += ch;
					ch = d_in.get();
					goto continueValue;
				}
			}
			return;
		}
	}
	t.d_type = Token::_EOF;
}

void StarFile::parse()
{
	d_err = "";
	starFile();
}

void StarFile::add(const AtomSym & a)
{
	if( d_last )
	{
		d_last->d_next = new AtomSym( a );
		d_last = d_last->d_next;
	}else
	{
		d_last = new AtomSym( a );
		d_first = d_last;
	}
	d_count++;
}

void StarFile::starFile()
{
	Token t;
	next( t );
	while( t.d_type != Token::_EOF )
	{
		switch( t.d_type )
		{
		case Token::_EOF:
			return;
		case Token::DATA:
			qDebug( "** Uninterpreted Data: %s", t.d_val.data() );
			next( t );
			if( t.d_type != Token::VALUE )
			{
				d_err = "Expecting VALUE token after DATA token.";
				return;
			}
			break;
		case Token::SAVE:
			if( !parseSave() )
				return;
			break;
		default:
			d_err = "Unexpected token '";
			d_err += s_tok[ t.d_type ];
			d_err += "'";
			break;
		}
		next( t );
	}
}

bool StarFile::parseSave()
{
	QByteArray  name = "save_";
	Token t, t2;
	next( t );

	d_stat = IDLE;

	// überlese alle 
	while( t.d_type == Token::VALUE )
	{
		name += t.d_val;
		next( t );
	}

	while( t.d_type != Token::_EOF )
	{
		switch( t.d_type )
		{
		case Token::_EOF:
			return false;
		case Token::NAME:
			next( t2 );
			if( t2.d_type != Token::VALUE )
			{
				d_err = "Expecting VALUE token after NAME token.";
				return false;
			}
			if( d_stat == INFO && t.d_val == "_Entry_title" )
				d_Entry_title = t2.d_val;
			else if( d_stat == INFO && t.d_val == "_BMRB_accession_number" )
				d_BMRB_accession_number = t2.d_val;
			else if( d_stat == INFO && t.d_val == "_BMRB_flat_file_name" )
				d_BMRB_flat_file_name = t2.d_val;
			else if( d_stat == INFO && t.d_val == "_Submission_date" )
				d_Submission_date = t2.d_val;
			else if( d_stat == INFO && t.d_val == "_Accession_date" )
				d_Accession_date = t2.d_val;
			else if( d_stat == CITATION && t.d_val == "_Citation_full" )
				d_Citation_full = t2.d_val;
			else if( t.d_val == "_Saveframe_category" )
			{
				if( t2.d_val == "entry_citation" )
					d_stat = CITATION;
				else if( t2.d_val == "entry_information" )
					d_stat = INFO;
				else if( t2.d_val == "assigned_chemical_shifts" )
					d_stat = ASSIG;
				else if( t2.d_val == "monomeric_polymer" )
					d_stat = POLY;
			}else
				qDebug( "** Uninterpreted Name: %s", t.d_val.data() );
			break;
		case Token::LOOP:
			if( !parseLoop() )
				return false;
			break;
		case Token::SAVE:
			return true;
		default:
			d_err = "Unexpected token '";
			d_err += s_tok[ t.d_type ];
			d_err += "' in ";
			d_err += name;
			return false;
		}
		next( t );
	}
	return true;
}

bool StarFile::parseLoop()
{
	Token t;
	next( t );
	if( t.d_type != Token::NAME )
	{
		d_err = "Expecting NAME token in loop_";
		return false;
	}
	if( d_stat == POLY && t.d_val == _Residue_seq_code )
	{
        /* 
		_Residue_seq_code
		[ _Residue_author_seq_code ]
        _Residue_label
		*/
		return parseSequence();
	}else if( d_stat == ASSIG && t.d_val == _Atom_shift_assign_ID )
	{
		/*
		_Atom_shift_assign_ID
		_Residue_seq_code
		_Residue_label
		_Atom_name
		_Atom_type
		_Chem_shift_value
		_Chem_shift_value_error
		_Chem_shift_ambiguity_code
		*/
		return parseAtoms();
	}else
	{
		qDebug( "** Uninterpreted Loop:" );
		while( t.d_type == Token::NAME )
		{
			qDebug( "    %s", t.d_val.data() );
			next( t );
		}
		while( t.d_type == Token::VALUE )
			next( t );
		if( t.d_type != Token::STOP )
		{
			d_err = "Expecting stop_ token closing the loop_";
			return false;
		}
		/*
		d_err = "Unexpected first column NAME '";
		d_err += t.d_val;
		d_err += "'. Expecting '_Residue_seq_code' or '_Atom_shift_assign_ID'";
		return false;
		*/
	}
	return true;
}

bool StarFile::parseSequence()
{
	if( !d_seq1.empty() )
	{
		d_err = "Sequence already specified";
		return false;
	}

	bool author_seq = false;
	Token t;
	next( t );
	if( t.d_type != Token::NAME || 
		( t.d_val != _Residue_label && t.d_val != _Residue_author_seq_code ) )
	{
		d_err = "Expecting column name '_Residue_label' or '_Residue_author_seq_code' in sequence loop_";
		return false;
	}
	author_seq = ( t.d_val == _Residue_author_seq_code );
	next( t );
	if( author_seq )
	{
		if( t.d_type != Token::NAME || t.d_val != _Residue_label )
		{
			d_err = "Expecting column name '_Residue_label' in sequence loop_";
			return false;
		}
		next( t );
	}

	int col = 0;
	Root::Any id1;
	Root::Any id2;
	Root::Any name;
	while( t.d_type == Token::VALUE )
	{
		if( col == 0 )
		{
			if( !id1.parse( t.d_val.data(), Root::Any::ULong ) )
			{
				d_err = "Invalid value '";
				d_err += t.d_val;
				d_err += "' in column '_Residue_seq_code' in sequence loop_";
				return false;
			}
			if( d_seq1.find( id1.getLong() ) != d_seq1.end() )
			{
				d_err = "Residue id '";
				d_err += t.d_val;
				d_err += "' already used";
				return false;
			}
			col++;
		}else if( col == 1 && author_seq )
		{
			if( !id2.parse( t.d_val.data(), Root::Any::ULong ) )
			{
				d_err = "Invalid value '";
				d_err += t.d_val;
				d_err += "' in column '_Residue_author_seq_code' in sequence loop_";
				return false;
			}
			if( d_seq2.find( id2.getLong() ) != d_seq2.end() )
			{
				d_err = "Residue id '";
				d_err += t.d_val;
				d_err += "' already used";
				return false;
			}
			col++;
        }else if( ( col == 1 && !author_seq ) || ( col == 2 && author_seq ) )
		{
			if( !name.parse( t.d_val.data(), Root::Any::CStr ) )
			{
				d_err = "Invalid value '";
				d_err += t.d_val;
				d_err += "' in column '_Residue_label'";
				return false;
			}
			d_seq1[ id1.getLong() ] = name.getCStr();
			d_seq2[ id2.getLong() ] = name.getCStr();
			col = 0;
		}else
			col++;
		next( t );
	}
	if( t.d_type != Token::STOP )
	{
		d_err = "Expecting stop_ token closing sequence loop_";
		return false;
	}
	return true;
}

bool StarFile::parseAtoms()
{
	if( d_first != 0 )
	{
		d_err = "AtomSym list already specified";
		return false;
	}
	enum { Atom_shift_assign_ID, Residue_author_seq_code, Residue_seq_code, 
		Residue_label, Atom_name, Atom_type, Chem_shift_value,
		Chem_shift_value_error, Chem_shift_ambiguity_code };
	std::vector<int> route;
	route.push_back( Atom_shift_assign_ID );
	Token t;
	next( t );
	if( t.d_type != Token::NAME || 
		( t.d_val != _Residue_seq_code && t.d_val != _Residue_author_seq_code ) )
	{
		d_err = "Expecting column name '_Residue_seq_code' or '_Residue_author_seq_code' in atom list loop_";
		return false;
	}
	d_autorSeq = ( t.d_val == _Residue_author_seq_code );
	next( t );
	if( d_autorSeq )
	{
		route.push_back( Residue_author_seq_code );
		if( t.d_type != Token::NAME || t.d_val != _Residue_seq_code )
		{
			d_err = "Expecting column name '_Residue_seq_code' in atom list loop_";
			return false;
		}else
			route.push_back( Residue_seq_code );
		next( t );
	}else
		route.push_back( Residue_seq_code );
	if( t.d_type != Token::NAME || t.d_val != _Residue_label )
	{
		d_err = "Expecting column name '_Residue_label' in atom list loop_";
		return false;
	}else
		route.push_back( Residue_label );
	next( t );
	if( t.d_type != Token::NAME || t.d_val != _Atom_name )
	{
		d_err = "Expecting column name '_Atom_name' in atom list loop_";
		return false;
	}else
		route.push_back( Atom_name );
	next( t );
	if( t.d_type != Token::NAME || t.d_val != _Atom_type )
	{
		d_err = "Expecting column name '_Atom_type' in atom list loop_";
		return false;
	}else
		route.push_back( Atom_type );
	next( t );
	if( t.d_type != Token::NAME || t.d_val != _Chem_shift_value )
	{
		d_err = "Expecting column name '_Chem_shift_value' in atom list loop_";
		return false;
	}else
		route.push_back( Chem_shift_value );
	next( t );
	if( t.d_type != Token::NAME || t.d_val != _Chem_shift_value_error )
	{
		d_err = "Expecting column name '_Chem_shift_value_error' in atom list loop_";
		return false;
	}else
		route.push_back( Chem_shift_value_error );
	next( t );
	if( t.d_type != Token::NAME || t.d_val != _Chem_shift_ambiguity_code )
	{
		d_err = "Expecting column name '_Chem_shift_ambiguity_code' in atom list loop_";
		return false;
	}else
		route.push_back( Chem_shift_ambiguity_code );
	next( t );

	int col = 0;
	Root::Any resi;
	Root::Any rl;
	Root::Any spin;
	Root::Any sl;
	Root::Any shift;
	Resi::const_iterator i;
	AtomSym a;
	while( t.d_type == Token::VALUE )
	{
		switch( route[col] )
		{
		case Atom_shift_assign_ID:
			if( !spin.parse( t.d_val.data(), Root::Any::ULong ) )
			{
				d_err = "Invalid value '";
				d_err += t.d_val;
				d_err += "' in column '_Atom_shift_assign_ID'";
				return false;
			}
			a.d_spin = spin.getLong();
			col++;
			break;
		case Residue_author_seq_code:
		case Residue_seq_code:
            if( ( d_autorSeq && route[col] == Residue_author_seq_code ) ||
                ( !d_autorSeq && route[col] == Residue_seq_code ) )
			{
				if( !resi.parse( t.d_val.data(), Root::Any::ULong ) )
				{
					d_err = "Invalid value '";
					d_err += t.d_val;
					d_err += "' in column '_Residue_seq_code'";
					return false;
				}
				a.d_resi = resi.getLong();
			}
			col++;
			break;
		case Residue_label:
			if( !rl.parse( t.d_val.data(), Root::Any::CStr ) )
			{
				d_err = "Invalid value '";
				d_err += t.d_val;
				d_err += "' in column '_Residue_label'";
				return false;
			}
			col++;
			break;
		case Atom_name:
			if( !sl.parse( t.d_val.data(), Root::Any::CStr ) )
			{
				d_err = "Invalid value '";
				d_err += t.d_val;
				d_err += "' in column '_Atom_name'";
				return false;
			}
			a.d_label = sl.getCStr();
			col++;
			break;
		case Atom_type:
			col++; // _Atom_type
			break;

		case Chem_shift_value:
			if( !shift.parse( t.d_val.data(), Root::Any::Float ) )
			{
				d_err = "Invalid value '";
				d_err += t.d_val;
				d_err += "' in column '_Chem_shift_value'";
				return false;
			}
			a.d_shift = shift.getFloat();
			col++;
			break;
		case Chem_shift_value_error:
			col++; // _Chem_shift_value_error
			break;
		case Chem_shift_ambiguity_code: 
			{
				Resi& seq = (d_autorSeq)?d_seq2:d_seq1;
				i = seq.find( resi.getLong() );
				if( i != seq.end() // Residue vorhanden
					&& (*i).second != Root::SymbolString( rl.getCStr() ) )
				{
					d_err = "Residue number '";
					d_err += resi.getCStr();
					d_err += "' in column '_Residue_seq_code' has inconsistent residue type '";
					d_err += rl.getCStr();
					d_err += "'";
					return false;
				}
				seq[ resi.getLong() ] = rl.getCStr();
				add( a );
				col = 0;
			}
			break;
		}
		next( t );
	}
	if( t.d_type != Token::STOP )
	{
		d_err = "Expecting stop_ token closing atom list loop_";
		return false;
	}
	return true;
}

