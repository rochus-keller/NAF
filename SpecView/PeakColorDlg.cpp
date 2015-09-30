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

#include "PeakColorDlg.h"
#include <QListWidget>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QColorDialog>
#include <QPushButton>
using namespace Spec;

PeakColorDlg::PeakColorDlg(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle( tr("Set Peak Colors - CARA") );
    QVBoxLayout* vbox = new QVBoxLayout(this);

    d_list = new QListWidget( this );
    d_list->setViewMode( QListView::IconMode );
    d_list->setResizeMode(QListView::Adjust);
    d_list->setUniformItemSizes(true);
    d_list->setWrapping(true);
    vbox->addWidget( d_list );

    QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this );
    vbox->addWidget( bb );
    connect(bb, SIGNAL(accepted()), this, SLOT(accept()));
    connect(bb, SIGNAL(rejected()), this, SLOT(reject()));

    QPushButton* b = bb->addButton( tr("Set Color"), QDialogButtonBox::ActionRole );
    b->setEnabled(false);
    connect( b, SIGNAL(clicked()), this, SLOT(onColor()) );
    connect( d_list, SIGNAL(itemSelectionChanged()), this, SLOT(onSelection()) );
    connect( this, SIGNAL(oneSelected(bool)), b,SLOT(setEnabled(bool)) );
    connect( d_list, SIGNAL(itemDoubleClicked(QListWidgetItem*)),this, SLOT(onColor()) );

    setMinimumSize(400,300);
}

void PeakColorDlg::select(PeakColorList * list)
{
    Q_ASSERT( list != 0 );
    d_colors = list;

    for( int i = 1; i <= 255; i++ )
    {
        QListWidgetItem* item = new QListWidgetItem(d_list,i);
        item->setSizeHint( QSize(30,30) );
        QColor clr = d_colors->getColor(i);
        if( !clr.isValid() )
            clr = Qt::transparent;
        item->setBackground( clr );
        item->setText( QString::number(i) );
    }
    exec();
}

void PeakColorDlg::onSelection()
{
    emit oneSelected( d_list->currentItem() != 0 );
}

void PeakColorDlg::onColor()
{
    QListWidgetItem* item = d_list->currentItem();
    Q_ASSERT( item != 0 );
    const QColor clr = QColorDialog::getColor( item->background().color(), this );
    if( !clr.isValid() )
        return;
    item->setBackground( clr );
    d_colors->setColor( item->type(), clr );
}
