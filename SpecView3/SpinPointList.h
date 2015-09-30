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
#ifndef SPINPOINTLIST_H
#define SPINPOINTLIST_H

#include <Gui/ListView.h>
#include <Spec/SpinSpace.h>
#include <SpecView3/SpinPointView3.h>
#include <QMap>

class _SpinPointListItem;

namespace Spec
{
    class SpinPointList : public Gui::ListView, public Root::Messenger
    {
        Q_OBJECT
    public:
		explicit SpinPointList(QWidget *parent = 0, bool createMenu = true);
        ~SpinPointList();
        void setSpace(SpinSpace*);
        SpinSpace* getSpace() const { return d_space; }
        SpinSpace::Element getCurrent() const;
        void refill();
		SpinPointView3::LabelType getLabelType() const { return d_labelType; }
	signals:
        void doubleClicked( Spec::SpinSpace::Element );
        void singleClicked( Spec::SpinSpace::Element );
		void refilled( int count );
    protected:
        void handle( Root::Message & msg );
    protected slots:
        void onDblClck( Gui::ListViewItem * );
        void onCurrentChanged();
		void handleLabelType();
		void handleGoto();
		void handleShowGhosts();
		void handleShowPathSim();
    private:
        Root::Ref<SpinSpace> d_space;
        QMap<SpinPoint,_SpinPointListItem*> d_cache;
		SpinPointView3::LabelType d_labelType;
		bool d_showGhosts;
    };
}

#endif // SPINPOINTLIST_H
