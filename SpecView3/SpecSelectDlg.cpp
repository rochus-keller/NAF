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

#include "SpecSelectDlg.h"
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <Spec/SpecRotator.h>
#include <SpecView/RotateDlg.h>
using namespace Spec;

static const int s_firstDim = 3;

SpecSelectDlg::SpecSelectDlg(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle( tr("Select Spectrum - CARA") );

    QVBoxLayout* vbox = new QVBoxLayout( this );

    d_list = new QTreeWidget(this);
    d_list->setRootIsDecorated(false);
    d_list->setAllColumnsShowFocus(true);
    d_list->setAlternatingRowColors(true);
    d_list->setSortingEnabled(true);
    vbox->addWidget(d_list);

    QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Ok
        | QDialogButtonBox::Cancel, Qt::Horizontal, this );
    vbox->addWidget( bb );
    QPushButton* rotate = bb->addButton( tr("Rotate"), QDialogButtonBox::ActionRole );
    rotate->setEnabled(false);
    connect( rotate, SIGNAL(clicked()), this, SLOT(onRotate()) );
    connect( d_list, SIGNAL(itemSelectionChanged()), this, SLOT(onSelection()) );
    connect( this, SIGNAL(oneSelected(bool)), rotate,SLOT(setEnabled(bool)) );
    connect(bb, SIGNAL(accepted()), this, SLOT(accept()));
    connect(bb, SIGNAL(rejected()), this, SLOT(reject()));

    connect( d_list, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this, SLOT(accept()) );
    setMinimumSize( 640, 480 );
}

Root::Ref<Spectrum> SpecSelectDlg::select(Project * pro, Spectrum *likeThis)
{
    Q_ASSERT( pro != 0 );
    d_pro = pro;
    d_likeThis = likeThis;

    QStringList headers;
    headers << tr("Spectrum") << tr("Type") << tr("ID");
    if( likeThis == 0 )
    {
        headers << tr("Dims");
        d_list->setHeaderLabels( headers );
        for( Project::SpectrumMap::const_iterator i = pro->getSpectra().begin(); i != pro->getSpectra().end(); ++i )
        {
            Spectrum* spec = (*i).second;
            QTreeWidgetItem* item = new QTreeWidgetItem( d_list, spec->getId() );
            item->setText(0, spec->getName() );
            item->setText(1, spec->getType()->getName() );
            item->setText(2, QString::number( spec->getId() ) );
            item->setText(3, QString::number( spec->getDimCount() ) );
        }
        d_list->sortByColumn(0);
    }else
    {
        for( Dimension d = 0; d < likeThis->getDimCount(); d++ )
            headers << QString::fromLatin1( Spec::getDimSymbol( d ) );
        d_list->setHeaderLabels( headers );

        ColorMap cm;
        likeThis->getColors(cm);
        for( Project::SpectrumMap::const_iterator i = pro->getSpectra().begin(); i != pro->getSpectra().end(); ++i )
        {
            Spectrum* spec = (*i).second;
            if( spec->getDimCount() == likeThis->getDimCount() )
            {
                Rotation rot( likeThis->getDimCount() ); // Falls autoRotate nichts machen kann, vorinitialisiert
                SpectrumType::autoRotate( cm, spec, rot, false );
                QTreeWidgetItem* item = new QTreeWidgetItem( d_list );
                item->setText(0, spec->getName() );
                item->setText(1, spec->getType()->getName() );
                item->setText(2, QString::number( spec->getId() ) );
                if( !rot.isIdentity() )
                    item->setData( s_firstDim, Qt::UserRole, QVariant::fromValue(rot) );
                for( Dimension d = 0; d < likeThis->getDimCount(); d++ )
                    item->setText( s_firstDim + d, Spec::getDimSymbol( rot[d] ) );
                item->setData( 0, Qt::UserRole, QVariant::fromValue( spec ) );
            }
        }
        d_list->sortByColumn(0);
    }
    for( int i = 0; i < d_list->columnCount(); i++ )
        d_list->resizeColumnToContents( i );
    if( exec() == QDialog::Accepted )
    {
        QTreeWidgetItem* item = d_list->currentItem();
        if( item == 0 )
            return 0;
        if( item->data( s_firstDim, Qt::UserRole ).canConvert<Spec::Rotation>() )
        {
            Rotation rot = item->data( s_firstDim, Qt::UserRole ).value<Spec::Rotation>();
            return new SpecRotator( item->data( 0, Qt::UserRole ).value<Spec::Spectrum*>(), rot );
        }else
            return item->data( 0, Qt::UserRole ).value<Spec::Spectrum*>();
    }else
        return 0;
}

void SpecSelectDlg::onRotate()
{
    QTreeWidgetItem* item = d_list->currentItem();
    Q_ASSERT( item != 0 );
    Spectrum* spec = item->data( 0, Qt::UserRole ).value<Spec::Spectrum*>();
    Q_ASSERT( spec != 0 );
    Rotation rot( spec->getDimCount() );
    if( item->data( s_firstDim, Qt::UserRole ).canConvert<Spec::Rotation>() )
        rot = item->data( s_firstDim, Qt::UserRole ).value<Spec::Rotation>();
    RotateDlg dlg( this, "To", "From" );
    dlg.setShowDimLetter(false);
    dlg.setWindowTitle( tr("Rotate '%1'").arg( spec->getName() ) );
    for( Dimension d = 0; d < spec->getDimCount(); d++ )
    {
        dlg.addDimension( (!d_likeThis.isNull())?tr("%1 (%2)").arg( d_likeThis->getScale( d ).getColor().getIsoLabel() ).
                                                arg( d_likeThis->getLabel( d ) ):QString(),
                          tr("%1 (%2)").arg( spec->getScale( d ).getColor().getIsoLabel() ).
                          arg( spec->getLabel( d ) ) );
    }
    if( dlg.rotate( rot ) )
    {
        if( !rot.isIdentity() )
            item->setData( s_firstDim, Qt::UserRole, QVariant::fromValue(rot) );
        else
            item->setData( s_firstDim, Qt::UserRole, QVariant() );
        if( !d_likeThis.isNull() )
        {
            for( Dimension d = 0; d < spec->getDimCount(); d++ )
                item->setText( s_firstDim + d, Spec::getDimSymbol( rot[d] ) );
        }
    }
}

void SpecSelectDlg::onSelection()
{
    emit oneSelected( d_list->currentItem() != 0 );
}
