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

#ifndef __DBGUI_VARIANT_VALUE
#define __DBGUI_VARIANT_VALUE

#include <Text2/Value.h>
#include <Text2/AtomManager.h>
#include <QVariant>
#include <QLatin1String>
#include <QMimeData>

namespace Gui
{
	class VariantValue
	{
	public:
		struct ValueBox
		{
			Root::Value d_val;
			Root::AtomManager* d_atoms;
			ValueBox():d_atoms(0) {}
			ValueBox( const Root::Value& rhs, Root::AtomManager* atoms = 0, bool copy = false )
			{
				d_val.assign( rhs, copy, false );
				d_atoms = atoms;
			}
		};

		static QVariant toVariant( const Root::Value&, Root::AtomManager* = 0, bool copy = false );
		static bool toValue( const QVariant&, Root::Value& );
		static QMimeData* createOidMime( const Root::Value::Uuid&, Root::Value::OID );
		static void copyOid( const Root::Value::Uuid&, Root::Value::OID );
		static bool canPasteOid(const Root::Value::Uuid&);
		static bool pasteOid( Root::Value::Uuid&, Root::Value::OID& );
		static bool readOidMime( const QMimeData *, Root::Value::Uuid&, Root::Value::OID& );
	private:
		VariantValue() {}
	};
}

Q_DECLARE_METATYPE( Gui::VariantValue::ValueBox )


#endif __DBGUI_VARIANT_VALUE
