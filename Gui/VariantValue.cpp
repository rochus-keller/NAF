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

#include "VariantValue.h"
#include <Text2/ValueReader.h>
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <Root/SymbolString.h>
#include <QMimeData>
#include <QApplication>
#include <QClipboard>
#include <QBuffer>
#include <QPixmap>
using namespace Gui;
using namespace Root;

QMimeData* VariantValue::createOidMime( const Root::Value::Uuid& u, Root::Value::OID o )
{
	Root::Value v;
	Root::ValueWriter vw;
	v.setUuid( u );
	vw.writeSlot( v );
	v.setOid( o );
	vw.writeSlot( v );

	QMimeData *md = new QMimeData();
    md->setData("application/oid", QByteArray( (const char*)vw.getData(), vw.getDataLen() ) );
	return md;
}

void VariantValue::copyOid( const Root::Value::Uuid& u, Root::Value::OID o )
{
	QApplication::clipboard()->setMimeData( createOidMime( u, o ) );
}

bool VariantValue::canPasteOid(const Root::Value::Uuid& soll)
{
	const QMimeData *md = QApplication::clipboard()->mimeData();
	if( md == 0 || !md->hasFormat("application/oid") )
		return false;
	QByteArray data = md->data("application/oid");
	Root::Value uuid;
	Root::ValueReader vr( (Root::UInt8*)data.data(), data.size() );
	Root::Value::Type tt = vr.nextToken();
	if( tt != Root::Value::TypeUuid )
		return false;
	vr.readValue( uuid );
	return ( uuid.getUuid() == soll );
}

bool VariantValue::readOidMime( const QMimeData * md, Root::Value::Uuid& u, Root::Value::OID& o )
{
	QByteArray data = md->data("application/oid");
	Root::Value uuid;
	Root::Value oid;
	Root::ValueReader vr( (Root::UInt8*)data.data(), data.size() );
	Root::Value::Type tt = vr.nextToken();
	if( tt != Root::Value::TypeUuid )
		return false;
	vr.readValue( uuid );
	tt = vr.nextToken();
	if( tt != Root::Value::TypeOid )
		return false;
	vr.readValue( oid );
	u = uuid.getUuid();
	o = oid.getOid();
	return true;
}

bool VariantValue::pasteOid( Root::Value::Uuid& u, Root::Value::OID& o )
{
	const QMimeData *md = QApplication::clipboard()->mimeData();
	return readOidMime( md, u, o );
}

QVariant VariantValue::toVariant( const Root::Value& v, Root::AtomManager* atoms, bool copy )
{
	switch( v.getType() )
	{
	case Value::TypeNull:
	case Value::TypeInvalid:
		return QVariant();
	case Value::TypeTrue:
		return true;
	case Value::TypeFalse:
		return false;
	case Value::TypeLong:
		return v.getLong();
	case Value::TypeDouble:
		return v.getDouble();
	case Value::TypeAnsi:
		return v.getStr();	
	case Value::TypeDate:
		return QDate::fromJulianDay( v.getDate().getValue() );
	case Value::TypeTime:
		return QTime( v.getTime().getHour(), v.getTime().getMinute(), 
			v.getTime().getSecond(), v.getTime().getMilli() );
	case Value::TypeDateTime:
		{
			Root::DateTime dt = v.getDateTime();
			return QDateTime( QDate::fromJulianDay( dt.getDate().getValue() ), 
				QTime( dt.getTime().getHour(), dt.getTime().getMinute(), 
				dt.getTime().getSecond(), dt.getTime().getMilli() ) );
		}
	case Value::TypePng:
		{
			QPixmap pix;
			pix.loadFromData( v.getData(), v.getDataLen(), "png" );
			return pix;
		}
	default:
		// TypeAtom, TypeOid, TypeByte, TypeHtml, TypeUtf8, TypeLob, TypeLbin, TypeLsrc,
		// TypeFrame, TypeTag, TypeURL, TypeGetter, TypeAppCode, TypeGetSetter, TypeType
		return QVariant::fromValue( ValueBox( v, atoms, copy ) );
	}
	return QVariant();
}

bool VariantValue::toValue( const QVariant& v, Root::Value& val )
{
	if( v.canConvert<ValueBox>() )
	{
		val.assign( v.value<ValueBox>().d_val, true, false );
		return true;
	}

	switch( v.type() )
	{
	case QVariant::Invalid:
		val.setNull();
		break;
	case QVariant::Bool:
		val.setBool( v.toBool() );
		break;
	case QVariant::Int:
	case QVariant::LongLong:
	case QVariant::ULongLong:
	case QVariant::UInt:
		val.setLong( v.toInt() );
		break;
	case QVariant::Double:
		val.setDouble( v.toDouble() );
		break;
	case QVariant::String:
		{
			QString str = v.toString();
			val.setAnsi( str.toLatin1(), true ); 
			// TODO: str auf Unicode durchsuchen und dann dynamisch
			// entscheiden ob Ansi oder Utf8 gespeichert werden soll
		}
		break;
	case QVariant::Date:
		{
			QDate f = v.value<QDate>();
			val.setDate( f.toJulianDay() );
			break;
		}
	case QVariant::DateTime:
		{
			QDateTime f = v.value<QDateTime>();
			QDate d = f.date();
			QTime t = f.time();
			val.setDateTime( Root::DateTime( Root::Date( d.toJulianDay() ),
				Root::Time( t.hour(), t.minute(), t.second(), t.msec() ) ) );
			break;
		}
	case QVariant::Time:
		{
			QTime f = v.value<QTime>();
			val.setTime( Root::Time( f.hour(), f.minute(), f.second(), f.msec() ) );
			break;
		}
	case QVariant::Pixmap:
		{
			QPixmap pix = v.value<QPixmap>();
			QBuffer b;
			pix.save( &b, "png" );
			val.setPng( (const Root::UInt8*)b.data().data(), b.data().size(), true );
			break;
		}
	case QVariant::Image:
		{
			QImage img = v.value<QImage>();
			QBuffer b;
			img.save( &b, "png" );
			val.setPng( (const Root::UInt8*)b.data().data(), b.data().size(), true );
			break;
		}
	default:
		return false;
	}
	return true;
}
