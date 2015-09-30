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

#include <QBitArray>
#include <QSize>
#include <QLine>
#include <QRect>
#include <QRegion>
#include <QVector>
#include <QSizePolicy>
#include <QPixmap>
#include <QBitmap>
#include <QImage>
#include <QIcon>
#include <QMatrix>
#include <QBrush>
#include <QPen>
#include <QEvent>
#include <QVariant>
#include <QTime>
#include <QDateTime>
#include <QWidget>
#include <QPalette>


#include "Variant.h"
#include "QtlFont.h"
#include "QtlColor.h"
#include "QtlDate.h"
#include "QtlPoint.h"
#include "QtlBitArray.h"
#include "QtlSize.h"
#include "QtlRect.h"
#include "QtlLine.h"
#include "QtlRegion.h"
#include "QtlSizePolicy.h"
#include "QtlPixmap.h"
#include "QtlIcon.h"
#include "QtlImage.h"
#include "QtlBrush.h"
#include "QtlPen.h"
#include "QtlTime.h"
#include "QtlDateTime.h"
#include "QtlBitmap.h"
#include "QtlPolygon.h"
#include "QtlPalette.h"
#include "QtlKeySequence.h"
#include "QtlFontInfo.h"
#include "QtlFontMetrics.h"
#include "QtlMatrix.h"
#include "QtlPainter.h"
#include "QtlPainterPath.h"
#include "QtlPaintDevice.h"
#include "QtlPaintEngine.h"
#include "QtlTextOption.h"
#include "QtlTableWidgetItem.h"
#include "QtlTableWidgetSelectionRange.h"
#include "QtlTextBlock.h"
#include "QtlPrinter.h"

#include "QtlTreeWidgetItem.h"
#include "QtlTemporaryFile.h"
#include "QtlUrl.h"
#include "QtlTextCharFormat.h"
#include "QtlTextFrameFormat.h"
#include "QtlTextListFormat.h"
#include "QtlString.h"
#include "QtlResource.h"

#include <Script/ValueInstaller.h>
#include <Script/ObjectHelper.h>
using namespace Qtl;
using namespace Lua;

const char* Variant::multiply = "_multiplyBy";
const char* Variant::divide = "_divideBy";
const char* Variant::add = "_add";
const char* Variant::subtract = "_subtract";
const char* Variant::intersectedAssign = "_intersectWith";
const char* Variant::unitedAssign = "_unitedWith";
const char* Variant::notEquals = "_notEqualTo";
const char* Variant::subtractedAssign = "_subtractedBy";
const char* Variant::xoredAssign = "_xoredWith";
const char* Variant::lessThan = "_lessThan";
const char* Variant::lessThanEqualTo = "_lessThanEqualTo";
const char* Variant::greaterThan = "_greaterThan";
const char* Variant::greaterThanEqualTo = "_greaterThanEqualTo";

void Variant::push(lua_State *L, const QVariant &v)
{
	switch( v.type() )
	{
	case QVariant::Invalid:
		lua_pushnil( L );
		break;
	case QVariant::Bool:
		lua_pushboolean( L, v.toBool() );
		break;
	case QVariant::Int:
		lua_pushnumber( L, v.toInt() );
		break;
	case QVariant::Double:
		lua_pushnumber( L, v.toDouble() );
		break;
	case QVariant::LongLong:
		lua_pushnumber( L, v.toLongLong() );
		break;
	case QVariant::ULongLong:
		lua_pushnumber( L, (double)(Root::UInt32)v.toULongLong() );
		break;
	case QVariant::UInt:
		lua_pushnumber( L, v.toUInt() );
		break;
	case QVariant::Char:
		lua_pushnumber( L, v.toChar().unicode() );
		break;
	case QVariant::ByteArray:
		lua_pushstring( L, v.toByteArray() );
		break;
	case QVariant::String:
		{
			//QString* str = ValueInstaller2<QString>::create( L );
			//*str = v.toString();
			lua_pushstring( L, v.toString().toLatin1() );
			break;
		}
	case QVariant::Color:
		{
			QColor* c = ValueInstaller2<QColor>::create( L );
			*c = qVariantValue<QColor>( v );
			break;
		}
	case QVariant::Font:
		{
			QFont* f = ValueInstaller2<QFont>::create( L );
			*f = qVariantValue<QFont>( v );
			break;
		}
	case QVariant::Date:
		{
			QDate* f = ValueInstaller2<QDate>::create( L );
			*f = qVariantValue<QDate>( v );
			break;
		}
	case QVariant::DateTime:
		{
			QDateTime* f = ValueInstaller2<QDateTime>::create( L );
			*f = qVariantValue<QDateTime>( v );
			break;
		}
	case QVariant::LineF:
	case QVariant::Line:
		{
			QLineF* f = ValueInstaller2<QLineF>::create( L );
			*f = qVariantValue<QLineF>( v );
			break;
		}
	case QVariant::Point:
	case QVariant::PointF:
		{
			QPointF* f = ValueInstaller2<QPointF>::create( L );
			*f = qVariantValue<QPointF>( v );
			break;
		}
	case QVariant::Rect:
	case QVariant::RectF:
		{
			QRectF* f = ValueInstaller2<QRectF>::create( L );
			*f = qVariantValue<QRectF>( v );
			break;
		}
	case QVariant::BitArray:
		{
			QBitArray* f = ValueInstaller2<QBitArray>::create( L );
			*f = qVariantValue<QBitArray>( v );
			break;
		}
	case QVariant::Bitmap:
		{
			QBitmap* f = ValueInstaller2<QBitmap>::create( L );
			*f = qVariantValue<QBitmap>( v );
			break;
		}
	case QVariant::Brush:
		{
			QBrush* f = ValueInstaller2<QBrush>::create( L );
			*f = qVariantValue<QBrush>( v );
			break;
		}
	case QVariant::Icon:
		{
			QIcon* f = ValueInstaller2<QIcon>::create( L );
			*f = qVariantValue<QIcon>( v );
			break;
		}
	case QVariant::Image:
		{
			QImage* f = ValueInstaller2<QImage>::create( L );
			*f = qVariantValue<QImage>( v );
			break;
		}
	case QVariant::Pen:
		{
			QPen* f = ValueInstaller2<QPen>::create( L );
			*f = qVariantValue<QPen>( v );
			break;
		}
	case QVariant::Pixmap:
		{
			QPixmap* f = ValueInstaller2<QPixmap>::create( L );
			*f = qVariantValue<QPixmap>( v );
			break;
		}
	case QVariant::Polygon:
		{
			QPolygonF* f = ValueInstaller2<QPolygonF>::create( L );
			*f = qVariantValue<QPolygon>( v );
			break;
		}
	case QVariant::Region:
		{
			QRegion* f = ValueInstaller2<QRegion>::create( L );
			*f = qVariantValue<QRegion>( v );
			break;
		}
	case QVariant::Size:
		{
			QSizeF* f = ValueInstaller2<QSizeF>::create( L );
			*f = qVariantValue<QSize>( v );
			break;
		}
	case QVariant::SizePolicy:
		{
			QSizePolicy* f = ValueInstaller2<QSizePolicy>::create( L );
			*f = qVariantValue<QSizePolicy>( v );
			break;
		}
	case QVariant::Time:
		{
			QTime* f = ValueInstaller2<QTime>::create( L );
			*f = qVariantValue<QTime>( v );
			break;
		}
	case QVariant::KeySequence:
		{
			QKeySequence* f = ValueInstaller2<QKeySequence>::create( L );
			*f = qVariantValue<QKeySequence>( v );
			break;
		}
	case QVariant::Palette:
		{
			QPalette* f = ValueInstaller2<QPalette>::create( L );
			*f = qVariantValue<QPalette>( v );
			break;
		}
	case QVariant::TextLength:
	case QVariant::TextFormat:
	case QVariant::Locale:
	case QVariant::Url:
	case QVariant::Cursor:
	case QVariant::List:
	case QVariant::Map:
	case QVariant::StringList:
		// TODO
		Util::push( L );
		break;
    default:
        Util::push( L );
        break;
	}
}

QVariant Variant::toVariant(lua_State *L, int n)
{
	int t = lua_type( L, n );

	if( t == LUA_TNIL )
		return QVariant();
	else if( t == LUA_TBOOLEAN )
        return (lua_toboolean( L, n ))?true:false;
    else if( t == LUA_TNUMBER )
	{
		double d = lua_tonumber( L, n );
		if( d - (double)(int)d == 0 )
			return (qint32)d;
		else
			return d;
	}else if( t == LUA_TSTRING )
        return QString::fromLatin1( lua_tostring( L, n ) );
	else if( lua_isuserdata( L, n ) )
	{
		if( QFont* f = ValueInstaller2<QFont>::cast( L, n ) )
			return *f;
		else if( QColor* x = ValueInstaller2<QColor>::cast( L, n ) )
			return *x;
		else if( QDateTime* x = ValueInstaller2<QDateTime>::cast( L, n ) )
			return *x;
		else if( QTime* x = ValueInstaller2<QTime>::cast( L, n ) )
			return *x;
		else if( QDate* x = ValueInstaller2<QDate>::cast( L, n ) )
			return *x;
		else if( QPen* x = ValueInstaller2<QPen>::cast( L, n ) )
			return *x;
		else if( QBrush* x = ValueInstaller2<QBrush>::cast( L, n ) )
			return *x;
		else if( QBitArray* x = ValueInstaller2<QBitArray>::cast( L, n ) )
			return *x;
		else if( QPointF* x = ValueInstaller2<QPointF>::cast( L, n ) )
			return *x;
		/* TODO
		else if( QSizeF* x = ValueInstaller2<QSizeF>::cast( L, n ) )
			return *x;*/
		else if( QRectF* x = ValueInstaller2<QRectF>::cast( L, n ) )
			return *x;
		else if( QLineF* x = ValueInstaller2<QLineF>::cast( L, n ) )
			return *x;
		else if( QPolygonF* x = ValueInstaller2<QPolygonF>::cast( L, n ) )
			return x->toPolygon();
		else if( QRegion* x = ValueInstaller2<QRegion>::cast( L, n ) )
			return *x;
		else if( QSizePolicy* x = ValueInstaller2<QSizePolicy>::cast( L, n ) )
			return *x;
		else if( QPixmap* x = ValueInstaller2<QPixmap>::cast( L, n ) )
			return *x;
		else if( QBitmap* x = ValueInstaller2<QBitmap>::cast( L, n ) )
			return *x;
		else if( QIcon* x = ValueInstaller2<QIcon>::cast( L, n ) )
			return *x;
		else if( QImage* x = ValueInstaller2<QImage>::cast( L, n ) )
			return *x;
		else if( QString* x = ValueInstaller2<QString>::cast( L, n ) )
			return *x;
		else
			return QVariant();
	}else
		return QVariant();
}

void Variant::install(lua_State * L)
{
	Lua::ObjectPeer::toVariant = toVariant;
	Lua::ObjectPeer::pushVariant = push;

	Font::install(L);
	Color::install(L);
	DateTime::install(L);
	Time::install(L);
	Date::install(L);
	Pen::install(L);
	Brush::install(L);
	BitArray::install(L);
	Point::install(L);
	Size::install(L);
	Rect::install(L);
	Line::install(L);
	Polygon::install(L);
	MyRegion::install(L);
	SizePolicy::install(L);
	Pixmap::install(L);
	Bitmap::install(L);
	Icon::install(L);
	Image::install(L);
	Palette::install(L);
	KeySequence::install(L);
	FontInfo::install(L);
	FontMetrics::install(L);
	Matrix::install(L);
	Painter::install(L);
	PainterPath::install(L);
	PaintDevice::install(L);
	PaintEngine::install(L);
	TextOption::install(L);
	TableWidgetItem::install(L);
	TableWidgetSelectionRange::install(L);
	TextBlock::install(L);
	
	Printer::install(L);
	TreeWidgetItem::install(L);
	TemporaryFile::install(L);
	Url::install(L);
	TextCharFormat::install(L);
	TextFrameFormat::install( L );
	TextListFormat::install( L );
	Resource::install( L );
	
	// TODO String::install( L );
}
