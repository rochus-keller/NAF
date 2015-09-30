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

#include "OverlayManager.h"
#include "PlaneGrid.h"
#include "ContourView3.h"
#include "ContourParamDlg.h"
#include "SpecSelectDlg.h"
#include <Spec/Repository.h>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QPushButton>
using namespace Spec;

enum Columns
{
    _Spec = 0,
    _Visi = 1,
    _Pos = 2,
    _Neg = 3
};

OverlayManager::OverlayManager(PlaneGrid * pg, Project * pro, QWidget *parent) :
    QTreeWidget(parent), d_plane( pg ), d_pro(pro)
{
    Q_ASSERT( pg != 0 );

//    QPalette p = palette();
//    p.setColor( QPalette::Highlight, Qt::transparent );
//    setPalette(p);
    setSelectionMode(QAbstractItemView::NoSelection); // damit Pos und Neg bei Selektion nicht übermalt werden
    setRootIsDecorated(false);
    setAllColumnsShowFocus(false);
    setAlternatingRowColors(true);
    setHeaderLabels( QStringList() << tr("Spectrum") << tr("Visi") << tr("Pos") << tr("Neg") << tr("") );

    refill();

    connect( this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(onDblClick(QTreeWidgetItem*,int)) );
    connect( this, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onClick(QTreeWidgetItem*,int)));
    setMinimumSize( 400, 200 );
}

void OverlayManager::refill()
{
    clear();
    for( int i = d_plane->getMinOverlay(); i <= d_plane->getMaxOverlay(); i++ )
    {
        QTreeWidgetItem* item = new QTreeWidgetItem( this, i );
        setItem( item );
    }
    resizeColumnToContents(0);
    setColumnWidth( 1, 30 );
    setColumnWidth( 2, 30 );
    setColumnWidth( 3, 30 );
}

void OverlayManager::setItem(QTreeWidgetItem * item )
{
    const int overlay = item->type();
    ContourView3::Params p = d_plane->getContourParams(overlay);
    Spectrum* spec = d_plane->getSpectrum(overlay);
    const bool visi = d_plane->isContourVisible(overlay);
    if( spec )
        item->setText(_Spec, tr("%1 - %2").arg(overlay).arg( spec->getName() ) );
    else
        item->setText(_Spec, tr("%1 - <empty>").arg(overlay) );
    if( p.d_pos.isValid() )
    {
        item->setBackground( _Pos, p.d_pos );
        item->setBackground( _Neg, p.d_neg );
    }else
    {
        item->setBackground( _Pos, Qt::transparent );
        item->setBackground( _Neg, Qt::transparent );
    }
    item->setCheckState( _Visi, (visi)?Qt::Checked:Qt::Unchecked);
}

bool OverlayManager::installOverlay(quint8 id)
{
    const bool created = d_plane->installOverlay( id, true, false );
    if( created )
    {
        ContourView3::Params p = d_plane->getContourParams(); // falls neu erzeugt verwende Params des Hauptcontour
        if( !d_pro.isNull() )
        {
            const Repository::SlotColors& sc = d_pro->getRepository()->getScreenClr();
            if( id < sc.size() )
            {
                p.d_pos = sc[ id ].d_pos;
                p.d_neg = sc[ id ].d_neg;
            }
        }
        d_plane->setContourParams(p,id);
    }
    return created;
}

void OverlayManager::selectSpec()
{
    QTreeWidgetItem* item = currentItem();
    if( item == 0 )
        return;
    const int overlay = item->type();
    installOverlay( overlay );
    if( d_pro.isNull() )
    {
        qWarning() << "OverlayManager::selectSpec: d_pro not set";
        return;
    }
    SpecSelectDlg dlg(this);
    Root::Ref<Spectrum> spec = dlg.select( d_pro, d_plane->getSpectrum() );
    if( !spec.isNull() )
    {
        d_plane->setSpectrum( spec, overlay );
        setItem( item );
        resizeColumnToContents(0);
    }
}

void OverlayManager::contourParams()
{
    QTreeWidgetItem* item = currentItem();
    if( item == 0 )
        return;
    const int overlay = item->type();
    installOverlay( overlay );
    ContourView3::Params p = d_plane->getContourParams( overlay );

    if( ContourParamDlg::setParams( this, p ) )
    {
        d_plane->setContourParams( p, overlay );
        setItem( item );
    }
}

void OverlayManager::onDblClick(QTreeWidgetItem *item, int column)
{
    Q_ASSERT( item != 0 );

    switch( column )
    {
    case _Spec:
        selectSpec();
        break;
    case _Pos:
    case _Neg:
        contourParams();
        break;
    default:
        break;
    }
}

void OverlayManager::onClick(QTreeWidgetItem *item, int column)
{
    Q_ASSERT( item != 0 );
    switch( column )
    {
    case _Visi:
        installOverlay( item->type() );
        d_plane->setContourVisible( item->checkState( _Visi ) == Qt::Checked, item->type() );
        setItem( item );
        break;
    default:
        break;
    }
}

OverlayManagerDlg::OverlayManagerDlg(PlaneGrid *pg, Spec::Project * pro):QDialog(pg)
{
    setWindowTitle( tr("Configure Overlay - CARA") );

    QVBoxLayout* vbox = new QVBoxLayout(this);

    d_om = new OverlayManager( pg, pro, this );
    vbox->addWidget(d_om);

    QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this );
    vbox->addWidget( bb );
    connect(bb, SIGNAL(accepted()), this, SLOT(accept()));
    connect(bb, SIGNAL(rejected()), this, SLOT(reject()));

    QPushButton* spectrum = bb->addButton( tr("&Spectrum"), QDialogButtonBox::ActionRole );
    spectrum->setEnabled(false);
    QPushButton* params = bb->addButton( tr("&Parameter"), QDialogButtonBox::ActionRole );
    params->setEnabled(false);
    connect( spectrum, SIGNAL(clicked()), d_om, SLOT(selectSpec()) );
    connect( params, SIGNAL(clicked()), d_om, SLOT(contourParams()) );
    connect( d_om, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(onSelection()) );
    connect( this, SIGNAL(oneSelected(bool)), spectrum,SLOT(setEnabled(bool)) );
    connect( this, SIGNAL(oneSelected(bool)), params,SLOT(setEnabled(bool)) );
}

void OverlayManagerDlg::onSelection()
{
    emit oneSelected( d_om->currentItem() != 0 );
}
