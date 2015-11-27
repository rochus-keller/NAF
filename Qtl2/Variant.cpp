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
#include <QModelIndex>
#include <QUrl>
#include <QFontInfo>
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
#include "QtlPainterPath.h"
#include "QtlUrl.h"
#include "QtlString.h"
#include "QtlStringList.h"
#include "QtlChar.h"
#include "QtlTextOption.h"
#include <Script2/QtObject.h>
#include <Script2/QtValue.h>
#include <Script/Util.h>
using namespace Qtl;
using namespace Lua;

const char* Variant::multiply = "_multiplyBy";
const char* Variant::divide = "_divideBy";
const char* Variant::add = "_add";
const char* Variant::subtract = "_subtract";
const char* Variant::intersectedAssign = "_intersectWith";
const char* Variant::unitedAssign = "_unitedWith";
const char* Variant::subtractedAssign = "_subtractedBy";
const char* Variant::xoredAssign = "_xoredWith";

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
        *QtValue<QChar>::create( L ) = v.toChar();
		break;
	case QVariant::ByteArray:
		lua_pushstring( L, v.toByteArray() );
		break;
    case QVariant::Font:
        {
            QFont* f = QtValue<QFont>::create( L );
            *f = qVariantValue<QFont>( v );
            break;
        }
    case QVariant::Color:
		{
            QColor* c = QtValue<QColor>::create( L );
			*c = qVariantValue<QColor>( v );
			break;
		}
    case QVariant::DateTime:
        {
            QDateTime* f = QtValue<QDateTime>::create( L );
            *f = qVariantValue<QDateTime>( v );
            break;
        }
    case QVariant::Time:
        {
            QTime* f = QtValue<QTime>::create( L );
            *f = qVariantValue<QTime>( v );
            break;
        }
    case QVariant::Date:
		{
            QDate* f = QtValue<QDate>::create( L );
			*f = qVariantValue<QDate>( v );
			break;
		}
    case QVariant::Pen:
        {
            QPen* f = QtValue<QPen>::create( L );
            *f = qVariantValue<QPen>( v );
            break;
        }
    case QVariant::Brush:
        {
            QBrush* f = QtValue<QBrush>::create( L );
            *f = qVariantValue<QBrush>( v );
            break;
        }
    case QVariant::BitArray:
        {
            QBitArray* f = QtValue<QBitArray>::create( L );
            *f = qVariantValue<QBitArray>( v );
            break;
        }
	case QVariant::Point:
	case QVariant::PointF:
		{
            QPointF* f = QtValue<QPointF>::create( L );
			*f = qVariantValue<QPointF>( v );
			break;
		}
    case QVariant::Size:
	case QVariant::SizeF:
        {
            QSizeF* f = QtValue<QSizeF>::create( L );
			*f = qVariantValue<QSizeF>( v );
            break;
        }
    case QVariant::Rect:
	case QVariant::RectF:
		{
            QRectF* f = QtValue<QRectF>::create( L );
			*f = qVariantValue<QRectF>( v );
			break;
		}
    case QVariant::LineF:
    case QVariant::Line:
        {
            QLineF* f = QtValue<QLineF>::create( L );
            *f = qVariantValue<QLineF>( v );
            break;
        }
    case QVariant::Polygon:
        {
            QPolygonF* f = QtValue<QPolygonF>::create( L );
            *f = qVariantValue<QPolygon>( v );
            break;
        }
    case QVariant::Region:
        {
            QRegion* f = QtValue<QRegion>::create( L );
            *f = qVariantValue<QRegion>( v );
            break;
        }
    case QVariant::SizePolicy:
        {
            QSizePolicy* f = QtValue<QSizePolicy>::create( L );
            *f = qVariantValue<QSizePolicy>( v );
            break;
        }
    case QVariant::Pixmap:
        {
            QPixmap* f = QtValue<QPixmap>::create( L );
            *f = qVariantValue<QPixmap>( v );
            break;
        }
    case QVariant::Bitmap:
		{
            QBitmap* f = QtValue<QBitmap>::create( L );
			*f = qVariantValue<QBitmap>( v );
			break;
		}
	case QVariant::Icon:
		{
            QIcon* f = QtValue<QIcon>::create( L );
			*f = qVariantValue<QIcon>( v );
			break;
		}
	case QVariant::Image:
		{
            QImage* f = QtValue<QImage>::create( L );
			*f = qVariantValue<QImage>( v );
			break;
		}
	case QVariant::Palette:
		{
            QPalette* f = QtValue<QPalette>::create( L );
			*f = qVariantValue<QPalette>( v );
			break;
		}
    case QVariant::KeySequence:
        {
            QKeySequence* f = QtValue<QKeySequence>::create( L );
            *f = qVariantValue<QKeySequence>( v );
            break;
        }
    case QVariant::Url:
        {
            QUrl* f = QtValue<QUrl>::create( L );
            *f = qVariantValue<QUrl>( v );
            break;
        }
    case QVariant::String:
        {
            QString* str = QtValue<QString>::create( L );
            *str = v.toString();
            break;
        }
    case QVariant::StringList:
        {
            QStringList* str = QtValue<QStringList>::create( L );
            *str = v.toStringList();
            break;
        }
	case QVariant::Matrix:
		{
			QMatrix* m = QtValue<QMatrix>::create( L );
			*m = qVariantValue<QMatrix>( v );
			break;
		}
		// Not supported:
	case QVariant::Map:
	case QVariant::List:
	case QVariant::Locale:
	case QVariant::UserType:
	case QVariant::LastType:
#ifdef QT3_SUPPORT
	case QVariant::ColorGroup:
#endif
		lua_pushnil( L );
		break;
		// TODO:
	case QVariant::RegExp:
	case QVariant::Cursor:
	case QVariant::TextLength:
	case QVariant::TextFormat:
	case QVariant::Transform:
		qWarning() << "Variant::push pending conversion for" << v.typeName();
		lua_pushnil( L );
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
        // Hier sind alle Values vertreten, die in Qtl*.cpp definiert sind; bei den auskommentierten
        // gibt es keine gültige Konversion
        if( QFont* f = QtValue<QFont>::cast( L, n ) )
			return *f;
        else if( QColor* x = QtValue<QColor>::cast( L, n ) )
			return *x;
        else if( QDateTime* x = QtValue<QDateTime>::cast( L, n ) )
			return *x;
        else if( QTime* x = QtValue<QTime>::cast( L, n ) )
			return *x;
        else if( QDate* x = QtValue<QDate>::cast( L, n ) )
			return *x;
        else if( QPen* x = QtValue<QPen>::cast( L, n ) )
			return *x;
        else if( QBrush* x = QtValue<QBrush>::cast( L, n ) )
			return *x;
        else if( QBitArray* x = QtValue<QBitArray>::cast( L, n ) )
			return *x;
        else if( QPointF* x = QtValue<QPointF>::cast( L, n ) )
			return *x;
        else if( QSizeF* x = QtValue<QSizeF>::cast( L, n ) )
            return *x;
        else if( QRectF* x = QtValue<QRectF>::cast( L, n ) )
			return *x;
        else if( QLineF* x = QtValue<QLineF>::cast( L, n ) )
			return *x;
        else if( QPolygonF* x = QtValue<QPolygonF>::cast( L, n ) )
			return x->toPolygon();
        else if( QRegion* x = QtValue<QRegion>::cast( L, n ) )
			return *x;
        else if( QSizePolicy* x = QtValue<QSizePolicy>::cast( L, n ) )
			return *x;
        else if( QPixmap* x = QtValue<QPixmap>::cast( L, n ) )
			return *x;
        else if( QBitmap* x = QtValue<QBitmap>::cast( L, n ) )
			return *x;
        else if( QIcon* x = QtValue<QIcon>::cast( L, n ) )
			return *x;
        else if( QImage* x = QtValue<QImage>::cast( L, n ) )
			return *x;
        else if( QPalette* x = QtValue<QPalette>::cast( L, n ) )
            return *x;
        else if( QKeySequence* x = QtValue<QKeySequence>::cast( L, n ) )
            return *x;
		else if( QMatrix* x = QtValue<QMatrix>::cast( L, n ) )
			return *x;
//        else if( QFontInfo* x = QtValue<QFontInfo>::cast( L, n ) )
//            return QVariant::fromValue(*x);
//        else if( QFontMetrics* x = QtValue<QFontMetrics>::cast( L, n ) )
//            return QVariant::fromValue(*x);
//        else if( QPainterPath* x = QtValue<QPainterPath>::cast( L, n ) )
//            return QVariant::fromValue(*x);
        else if( QUrl* x = QtValue<QUrl>::cast( L, n ) )
            return *x;
        else if( QString* x = QtValue<QString>::cast( L, n ) )
            return *x;
        else if( QChar* x = QtValue<QChar>::cast( L, n ) )
            return *x;
        else if( QStringList* x = QtValue<QStringList>::cast( L, n ) )
            return *x;
        else
			return QVariant();
	}else
		return QVariant();
}

void Variant::install(lua_State * L)
{
    QtValueBase::toVariant = toVariant;
    QtValueBase::pushVariant = push;

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
	PainterPath::install(L);
    Url::install(L);
    String::install( L );
    StringList::install( L );
    Char::install( L );
	TextOption::install( L );
}
