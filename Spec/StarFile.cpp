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
#include <QFile>
using namespace Spec;

StarFile::StarFile(const char* path):
	d_first( 0 ), d_last( 0 ), d_count( 0 ),
	d_fileName(path),
	d_star(false) // still old syntax, even for BMRB 3.1
{
}

StarFile::~StarFile()
{
	if( d_first )
		delete d_first;
}

bool StarFile::error(const QByteArray & msg)
{
	d_err = msg;
	return false;
}

bool StarFile::errorRowCol(int row, const QByteArray &col)
{
	d_err = "Invalid '" + col + "' on row " + QByteArray::number(row+1);
	return false;
}

Star::StarParser::Blocks::const_iterator StarFile::findBlock(const Star::StarParser::Block& block, const QString &key, const QString &value) const
{
	Star::StarParser::Blocks::const_iterator i;
	for( i = block.d_blocks.begin(); i != block.d_blocks.end(); ++i )
	{
		if( i.value().d_items.value( key ) == value )
			return i;
	}
	return block.d_blocks.end();
}

int StarFile::findLoop(const Star::StarParser::Block & block, const QString &header) const
{
	for( int i = 0; i < block.d_loops.size(); i++ )
	{
		if( block.d_loops[i].d_headers.isEmpty() )
			continue; // Loop ohne Header; eigentlich nicht möglich
		for( int j = 0; j < block.d_loops[i].d_headers.first().size(); j++ )
		{
			if( block.d_loops[i].d_headers.first()[j] == header )
				return i;
		}
	}
	return -1;
}

bool StarFile::parse()
{
	d_err = "";
	if( d_first )
		delete d_first;
	d_first = 0;
	d_seq.clear();
	QFile file( d_fileName );
	if( !d_star.parse( &file, "ASCII" ) )
	{
		d_err = d_star.getError().toLatin1();
		return false;
	}
	if( d_star.getBlocks().size() != 1 )
		return error("BMRB format only supports one data_ cell");

	const Star::StarParser::Block& data = d_star.getBlocks().begin().value();
	if( !data.d_blocks.contains("entry_information") )
		return error("Invalid BMRB format: missing save_entry_information");

	const Star::StarParser::Block& einfo = data.d_blocks.value( "entry_information" );
	if( einfo.d_items.contains("NMR_STAR_version") )
		d_newVersion = false;
	else if( einfo.d_items.contains("Entry.NMR_STAR_version") )
		d_newVersion = true;
	else
		return error("Invalid BMRB format: missing NMR_STAR_version item");


	if( d_newVersion )
		d_Entry_title = einfo.d_items.value( "Entry.Title" ).toByteArray().trimmed();
	else
		d_Entry_title = einfo.d_items.value( "Entry_title" ).toByteArray().trimmed();
	if( d_newVersion )
		d_BMRB_accession_number = einfo.d_items.value( "Entry.ID" ).toByteArray();
	else
		d_BMRB_accession_number = einfo.d_items.value( "BMRB_accession_number" ).toByteArray();
	if( !d_newVersion )
		d_BMRB_flat_file_name = einfo.d_items.value( "BMRB_flat_file_name" ).toByteArray();
	if( d_newVersion )
		d_Submission_date = einfo.d_items.value( "Entry.Submission_date" ).toByteArray();
	else
		d_Submission_date = einfo.d_items.value( "Submission_date" ).toByteArray();
	if( d_newVersion )
		d_Accession_date = einfo.d_items.value( "Entry.Accession_date" ).toByteArray();
	else
		d_Accession_date = einfo.d_items.value( "Accession_date" ).toByteArray();


	if( data.d_blocks.contains("entry_citation") )
	{
		if( d_newVersion )
			d_Citation_full = data.d_blocks.value("entry_citation").d_items.value( "Citation.Full_citation" ).toByteArray();
		else
			d_Citation_full = data.d_blocks.value("entry_citation").d_items.value( "Citation_full" ).toByteArray();
	}

	Star::StarParser::Blocks::const_iterator entity;
	if( d_newVersion )
		entity = findBlock( data, "Entity.Sf_category", "entity" );
	else
		entity = findBlock( data, "Saveframe_category", "monomeric_polymer" );
	if( entity == data.d_blocks.end() )
		return error("Invalid BMRB format: no sequence found");
	if( !parseSequence( entity.value() ) )
		return false;

	Star::StarParser::Blocks::const_iterator shiftset;
	if( d_newVersion )
		shiftset = findBlock( data, "Assigned_chem_shift_list.Sf_category", "assigned_chemical_shifts" );
	else
		shiftset = findBlock( data, "Saveframe_category", "assigned_chemical_shifts" );
	if( shiftset != data.d_blocks.end() && !parseAtoms( shiftset.value() ) )
		return false;

	return true;
}

bool StarFile::parseSequence(const Star::StarParser::Block& block )
{
	if( !d_seq.empty() )
		return error( "Sequence already specified" );

	int loop;
	if( d_newVersion )
		loop = findLoop( block, "Entity_comp_index.ID" );
	else
		loop = findLoop( block, "Residue_seq_code" );
	if( loop == -1 )
		return error("Invalid BMRB format: missing sequence loop");

	Q_ASSERT( !block.d_loops[loop].d_headers.isEmpty() );
	enum { Residue_seq_code, Residue_label, Residue_author_seq_code };
	int map[3] = { -1, -1, -1 };
	for( int i = 0; i < block.d_loops[loop].d_headers.first().size(); i++ )
	{
		const QString header = block.d_loops[loop].d_headers.first()[i];
		if( !d_newVersion )
		{
			if( header == "Residue_seq_code" )
				map[Residue_seq_code] = i;
			else if( header == "Residue_label" )
				map[Residue_label] = i;
			else if( header == "Residue_author_seq_code" )
				map[Residue_author_seq_code] = i;
		}else
		{
			if( header == "Entity_comp_index.ID" )
				map[Residue_seq_code] = i;
			else if( header == "Entity_comp_index.Comp_ID" )
				map[Residue_label] = i;
			else if( header == "Entity_comp_index.Auth_seq_ID" )
				map[Residue_author_seq_code] = i;
		}
	}
	if( map[Residue_seq_code] == -1 || map[Residue_label] == -1 )
		return error( "Invalid BMRB format: missing mandatory sequence loop headers" );

	for( int row = 0; row < block.d_loops[loop].d_table.size(); row++ )
	{
		bool ok;
		const Star::StarParser::Loop::Row& curRow = block.d_loops[loop].d_table[row];

		const quint32 id = curRow[map[Residue_seq_code]].toByteArray().toULong( &ok );
		if( !ok )
			return error( "Invalid sequence ID in row " + QByteArray::number( row+1 ) );
		if( d_seq.find( id ) != d_seq.end() )
			return error( "Sequence ID in row " + QByteArray::number( row+1 ) + " not unique" );
		const QByteArray name = curRow[map[Residue_label]].toByteArray();
		if( name.isEmpty() || name == "." || name == "@" )
			return error( "Invalid residue code in row " + QByteArray::number( row+1 ) );
		d_seq[ id ] = name;
	}
	return true;
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

bool StarFile::parseAtoms(const Star::StarParser::Block & block )
{
	if( d_first != 0 )
	{
		d_err = "AtomSym list already specified";
		return false;
	}

	int loop;
	if( d_newVersion )
		loop = findLoop( block, "Atom_chem_shift.ID" );
	else
		loop = findLoop( block, "Atom_shift_assign_ID" );
	if( loop == -1 )
		return true; // Tabelle nicht vorhanden

	Q_ASSERT( !block.d_loops[loop].d_headers.isEmpty() );

	enum { Atom_shift_assign_ID, Residue_author_seq_code, Residue_seq_code,
		Atom_name, Atom_type, Chem_shift_value,
		Chem_shift_value_error, Chem_shift_ambiguity_code, MAXCOL };

	int map[MAXCOL];
	for( int i = 0; i < MAXCOL; i++ )
		map[i] = -1;

	for( int i = 0; i < block.d_loops[loop].d_headers.first().size(); i++ )
	{
		const QString header = block.d_loops[loop].d_headers.first()[i];
		if( !d_newVersion )
		{
			if( header == "Atom_shift_assign_ID" )
				map[Atom_shift_assign_ID] = i;
			else if( header == "Residue_author_seq_code" )
				map[Residue_author_seq_code] = i;
			else if( header == "Residue_seq_code" )
				map[Residue_seq_code] = i;
			else if( header == "Atom_name" )
				map[Atom_name] = i;
			else if( header == "Atom_type" )
				map[Atom_type] = i;
			else if( header == "Chem_shift_value" )
				map[Chem_shift_value] = i;
			else if( header == "Chem_shift_value_error" )
				map[Chem_shift_value_error] = i;
			else if( header == "Chem_shift_ambiguity_code" )
				map[Chem_shift_ambiguity_code] = i;
		}else
		{
			if( header == "Atom_chem_shift.ID" )
				map[Atom_shift_assign_ID] = i;
			else if( header == "Atom_chem_shift.Auth_seq_ID" )
				map[Residue_author_seq_code] = i;
			else if( header == "Atom_chem_shift.Seq_ID" )
				map[Residue_seq_code] = i;
			else if( header == "Atom_chem_shift.Atom_ID" )
				map[Atom_name] = i;
			else if( header == "Atom_chem_shift.Atom_type" )
				map[Atom_type] = i;
			else if( header == "Atom_chem_shift.Val" )
				map[Chem_shift_value] = i;
			else if( header == "Atom_chem_shift.Val_err" )
				map[Chem_shift_value_error] = i;
			else if( header == "Atom_chem_shift.Ambiguity_code" )
				map[Chem_shift_ambiguity_code] = i;
		}
	}

	if( map[Atom_shift_assign_ID] == -1 || map[Atom_name] == -1 || map[Atom_type] == -1
		|| map[Chem_shift_value] == -1 || map[Chem_shift_ambiguity_code] == -1)
		return error( "Invalid BMRB format: missing mandatory atom shift loop headers" );
	if( map[Residue_seq_code] == -1  )
		return error( "CARA needs the Residue_seq_code to import the atom shift table" );

	for( int row = 0; row < block.d_loops[loop].d_table.size(); row++ )
	{
		AtomSym a;
		bool ok;

		const Star::StarParser::Loop::Row& curRow = block.d_loops[loop].d_table[row];

		a.d_spin = curRow[map[Atom_shift_assign_ID]].toByteArray().toULong( &ok );
		if( !ok )
			return errorRowCol( row, "Atom_shift_assign_ID" );

		a.d_resi = curRow[map[Residue_seq_code]].toByteArray().toULong( &ok );
		if( !ok )
			return errorRowCol( row, "Residue_seq_code" );

		a.d_label = curRow[map[Atom_name]].toByteArray();
		if( a.d_label.isNull() )
			return errorRowCol( row, "Atom_name" );

		a.d_shift = curRow[map[Chem_shift_value]].toByteArray().toFloat( &ok );
		if( !ok )
			return errorRowCol( row, "Chem_shift_value" );

		add( a );
	}
	return true;
}

