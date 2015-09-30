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

#include "PathSimDlg.h"
#include <QDialogButtonBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QTextStream>
#include <QLabel>
#include <QTabWidget>
#include <QtDebug>
#include <QShortcut>
#include <strstream>
#include <Spec/TypeBase.h>
#include <Spec/Repository.h>
using namespace Spec;

PathSimDlg::PathSimDlg(SpectrumType * st, QWidget *parent) :
    QDialog(parent), d_type(st)
{
    Q_ASSERT( st != 0 );

    setWindowTitle( tr("Pathway Simulation of '%1' - CARA" ).arg( st->getName().data() ) );
    d_rot = Rotation( st->getDimCount() );
    d_rot.setIdentity();
    init();
}

PathSimDlg::PathSimDlg(Spectrum * s, QWidget *parent):
    QDialog(parent)
{
    Q_ASSERT( s != 0 && s->getType() != 0 );
    d_type = s->getType();
    setWindowTitle( tr("Pathway Simulation of '%1' (%2) - CARA" ).arg( s->getName() ).
            arg( d_type->getName().data() ) );
    d_rot = s->getTypeMapping();
    init();
}

void PathSimDlg::setResiType(ResidueType * rt)
{
    if( rt )
		d_resiTypes->setCurrentIndex( d_resiTypes->findText( QString::fromAscii(rt->getShort().data() ) ) );
	else
		d_resiTypes->setCurrentIndex( d_resiTypes->findText( QString() ) );
}

void PathSimDlg::setSystem(SpinSystem * sys)
{
	if( sys == 0 )
		return;
	if( sys->getAssig() )
		setResiType( sys->getAssig()->getType() );
	else if( sys->getSysType() && sys->getSysType()->getRt() )
		setResiType( sys->getSysType()->getRt() );
}

static int _printCol( QTextStream& out, NmrExperiment::PathNode* node, int row, int col )
{
    out << "<td>" << node->d_id.data() << "</td>";
    // qDebug() << row << col << node->d_id.data();
    if( !node->d_subs.isEmpty() )
    {
        col++;
        row = _printCol( out, node->d_subs.first(), row, col );
        for( int i = 1; i < node->d_subs.size(); i++ )
        {
            out << "<tr>";
            if( col > 0 )
                out << "<td>" << row + 1 << "</td>";
            for( int c = 1; c <= col; c++ )
                out << "<td>\"</td>";
            row = _printCol( out, node->d_subs[i], row, col );
        }
    }else // node->d_subs.isEmpty()
    {
        out << "</tr>" << endl;
        row++;
    }
    return row;
}

void PathSimDlg::calcPath()
{
    d_paths->clear();
    d_peaks->clear();

    ResidueType* rt = d_type->getOwner()->getOwner()->findResidueType( d_resiTypes->currentText().toAscii() );
    if( rt == 0 )
        d_mol->setResidueType( 0 );
	else
        d_mol->setResidueType( rt );

	Root::Ref<NmrExperiment> nmr = d_type->getOwner()->inferExperiment4( d_type, rt );

//    std::ostrstream stream;
//	nmr->printTable( stream );
//    qDebug() << stream.str();

    QString html;
    {
        QTextStream out( &html, QIODevice::WriteOnly );
        out << "<html><body>";
        out << "<table border=1 cellspacing=0 cellpadding=2>";
        out << "<tr><th>Nr.</th>";
        for( int i = 0; i < d_type->getProc().size(); i++ )
        {
            out << "<th>Step " << i+1 << "</th>";
        }
        out << "</tr>" << endl;
        out << "<tr><th>Name:</th>";
        for( int i = 0; i < d_type->getProc().size(); i++ )
        {
            out << "<th>" << d_type->getProc()[i].d_text << "</th>";
        }
        out << "</tr>" << endl;
        out << "<tr><th>Dimension:</th>";
        for( int i = 0; i < d_type->getProc().size(); i++ )
        {
            out << "<th>" << Spec::getDimSymbol(
                       d_rot.getInverted(d_type->getProc()[i].d_dim) ) << "</th>";
        }
        out << "</tr>" << endl;

        int row = 0;
        for( int i = 0; i < nmr->getRoot().d_subs.size(); i++ )
        {
            out << "<tr>";
            out << "<td>" << row + 1 << "</td>";
            row = _printCol( out, nmr->getRoot().d_subs[i], row, 0 );
        }

        out << "</table>";
        out << "</body></html>";
    }
    d_paths->setHtml( html );
    html.clear();
    {
        QTextStream out( &html, QIODevice::WriteOnly );
        out << "<html><body>";
        out << "<table border=1 cellspacing=0 cellpadding=4>";
        out << "<tr><th>Nr.</th>";
        const int dim = nmr->getTable().getDimCount();
        for( int i = 0; i < dim; i++ )
        {
            out << "<th>D" << i+1 << "</th>";
        }
        out << "</tr>" << endl;
        out << "<tr><th>Name:</th>";
        for( int d = 0; d < dim; d++ )
        {
            if( nmr->getTable().d_step[d] < d_type->getProc().size() )
                out << "<th>" << d_type->getProc()[nmr->getTable().d_step[d]].d_text << "</th>";
            else
                out << "<th>" << d_type->getName(d).data() << "</th>";
        }
        out << "</tr>" << endl;
        int row = 0;
        PathTable::Table::const_iterator it;
        for( it = nmr->getTable().begin(); it != nmr->getTable().end(); ++it )
        {
            row++;
            out << "<tr><td>" << row << "</td>";
            for( int d = 0; d < dim; d++ )
                out << "<td>" << (*it)[d_rot[d]].data() << "</td>";
            out << "</tr>" << endl;
        }
        out << "</table>";
        out << "</body></html>";
    }
    d_peaks->setHtml( html );
}

void PathSimDlg::nextTab()
{
    d_tab->setCurrentIndex( ( d_tab->currentIndex() + 1 ) % d_tab->count() );
}

void PathSimDlg::prevTab()
{
    d_tab->setCurrentIndex( ( d_tab->count() + d_tab->currentIndex() - 1 ) % d_tab->count() );
}

void PathSimDlg::init()
{
    QVBoxLayout* vbox = new QVBoxLayout( this );

    d_resiTypes = new QComboBox( this );
    QHBoxLayout* hbox = new QHBoxLayout();
    vbox->addLayout( hbox );
    hbox->addWidget( new QLabel( tr("Select residue type for pathway simulation:"), this ) );
    hbox->addWidget( d_resiTypes );
    hbox->addStretch();
    // d_resiTypes->setInsertPolicy( QComboBox::InsertAlphabetically ); // funktioniert nicht
    d_resiTypes->setSizeAdjustPolicy( QComboBox::AdjustToContents );
    const Repository::ResidueTypeMap& rm = d_type->getOwner()->getOwner()->getResidueTypes();
	Repository::ResidueTypeMap::const_iterator p;
	for( p = rm.begin(); p != rm.end(); ++p )
        d_resiTypes->addItem( QString::fromAscii((*p).second->getShort().data()) );
	d_resiTypes->addItem( QString() );
    d_resiTypes->model()->sort(0);

    d_tab = new QTabWidget( this );
    vbox->addWidget( d_tab );

    d_paths = new QTextEdit( this );
    d_paths->setReadOnly(true);
    d_tab->addTab( d_paths, tr("P&aths") );

    d_peaks = new QTextEdit( this );
    d_peaks->setReadOnly(true);
    d_tab->addTab( d_peaks, tr("P&eaks") );

    d_mol = new MoleculeViewer( this, 0, false );
    d_tab->addTab( d_mol, tr("&Molecule") );

    QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this );
    vbox->addWidget( bb );
    connect(bb, SIGNAL(accepted()), this, SLOT(accept()));
    connect(bb, SIGNAL(rejected()), this, SLOT(reject()));

    new QShortcut( tr("CTRL+TAB"), this, SLOT(nextTab()) );
    new QShortcut( tr("CTRL+SHIFT+TAB"), this, SLOT(prevTab()) );

    connect( d_resiTypes, SIGNAL(currentIndexChanged(QString)), this, SLOT(calcPath()) );
    d_resiTypes->setCurrentIndex(0);

    resize( 640, 480 );
}
