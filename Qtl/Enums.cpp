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

#include "Enums.h"
#include <Script/ObjectInstaller.h>
#include <QPainter>
#include <QPrinter>
#include <QUrl>
#include <QStringList>
#include <QTextFormat>
#include <QTextOption>
#include <QTabWidget>
#include <QTextCursor>
#include <QTextDocument>
#include <QFileDialog>
using namespace Lua;

GCC_IGNORE(-Wmaybe-uninitialized);

Enums::Enums(lua_State * X)
{
	L=X;
}

Enums::~Enums(void)
{
}
int Enums::ClipOperation(int position)
{
    Qt::ClipOperation f = Qt::NoClip;
	switch( Util::toInt( L, position ) )
	{
	case Qt::NoClip: 
		f = Qt::NoClip;
		break;
	case Qt::ReplaceClip: 
		f = Qt::ReplaceClip;
		break;
	case Qt::IntersectClip: 
		f = Qt::IntersectClip;
		break;
	case Qt::UniteClip: 
		f = Qt::UniteClip;
		break;
	default :
		Lua::Util::error( L, "expecting a valid Qt::ClipOperation" );
	}
	return f;
}
int Enums::CompositionMode(int position)
{
	QPainter::CompositionMode f;
	switch( Util::toInt( L, position ) )
	{
	case QPainter::CompositionMode_SourceOver: 
		f = QPainter::CompositionMode_SourceOver;
		break;
	case QPainter::CompositionMode_DestinationOver: 
		f = QPainter::CompositionMode_DestinationOver;
		break;
	case QPainter::CompositionMode_Clear: 
		f = QPainter::CompositionMode_Clear;
		break;
	case QPainter::CompositionMode_Source: 
		f = QPainter::CompositionMode_Source;
		break;
	case QPainter::CompositionMode_Destination: 
		f = QPainter::CompositionMode_Destination;
		break;
	case QPainter::CompositionMode_SourceIn: 
		f = QPainter::CompositionMode_SourceIn;
		break;
	case QPainter::CompositionMode_DestinationIn: 
		f = QPainter::CompositionMode_DestinationIn;
		break;
	case QPainter::CompositionMode_SourceOut: 
		f = QPainter::CompositionMode_SourceOut;
		break;
	case QPainter::CompositionMode_DestinationOut: 
		f = QPainter::CompositionMode_DestinationOut;
		break;
	case QPainter::CompositionMode_SourceAtop: 
		f = QPainter::CompositionMode_SourceAtop;
		break;
	case QPainter::CompositionMode_DestinationAtop: 
		f = QPainter::CompositionMode_DestinationAtop;
		break;
	case QPainter::CompositionMode_Xor: 
		f = QPainter::CompositionMode_Xor;
		break;
	default :
		Lua::Util::error( L, "expecting a valid QPainter::CompositionMode" );
	}
	return f;
}
int Enums::LayoutDirection(int position)
{
	Qt::LayoutDirection f;
	switch( Util::toInt( L, position ) )
	{
	case Qt::LeftToRight: 
		f = Qt::LeftToRight;
		break;
	case Qt::RightToLeft: 
		f = Qt::RightToLeft;
		break;
	default :
		Lua::Util::error( L, "expecting a valid Qt::LayoutDirection" );
	}
	return f;
}
int Enums::PenStyle(int position)
{
	Qt::PenStyle f;
	switch( Util::toInt( L, position ) )
	{
	case Qt::NoPen: 
		f = Qt::NoPen;
		break;
	case Qt::SolidLine: 
		f = Qt::SolidLine;
		break;
	case Qt::DashLine: 
		f = Qt::DashLine;
		break;
	case Qt::DotLine: 
		f = Qt::DotLine;
		break;
	case Qt::DashDotLine: 
		f = Qt::DashDotLine;
		break;
	case Qt::DashDotDotLine: 
		f = Qt::DashDotDotLine;
		break;
	case Qt::CustomDashLine: 
		f = Qt::CustomDashLine;
		break;
	case Qt::MPenStyle: 
		f = Qt::MPenStyle;
		break;
	default :
		Lua::Util::error( L, "expecting a valid Qt::PenStyle" );
	}
	return f;
}
int Enums::RenderHint(int position)
{
	QPainter::RenderHint f;
	switch( Util::toInt( L, position ) )
	{
	case QPainter::Antialiasing: 
		f = QPainter::Antialiasing;
		break;
	case QPainter::TextAntialiasing: 
		f = QPainter::TextAntialiasing;
		break;
	case QPainter::SmoothPixmapTransform: 
		f = QPainter::SmoothPixmapTransform;
		break;
	default :
		Lua::Util::error( L, "expecting a valid Painter::RenderHint" );
	}
	return f;
}
int Enums::FormattingOption(int position)
{
	QUrl::FormattingOption f;
	switch( Util::toInt( L, position ) )
	{
	case QUrl::None: 
		f = QUrl::None;
		break;
	case QUrl::RemoveScheme: 
		f = QUrl::RemoveScheme;
		break;
	case QUrl::RemovePassword: 
		f = QUrl::RemovePassword;
		break;
	case QUrl::RemoveUserInfo: 
		f = QUrl::RemoveUserInfo;
		break;
	case QUrl::RemovePort: 
		f = QUrl::RemovePort;
		break;
	case QUrl::RemoveAuthority: 
		f = QUrl::RemoveAuthority;
		break;
	case QUrl::RemovePath: 
		f = QUrl::RemovePath;
		break;
	case QUrl::RemoveQuery: 
		f = QUrl::RemoveQuery;
		break;
	case QUrl::RemoveFragment: 
		f = QUrl::RemoveFragment;
		break;
	case QUrl::StripTrailingSlash: 
		f = QUrl::StripTrailingSlash;
		break;
	default :
		Lua::Util::error( L, "expecting a valid Painter::FormattingOption" );
	}
	return f;
}
int Enums::ColorMode(int position)
{
	QPrinter::ColorMode f;
	switch( Util::toInt( L, position ) )
	{
	case QPrinter::Color: 
		f = QPrinter::Color;
		break;
	case QPrinter::GrayScale : 
		f = QPrinter::GrayScale ;
		break;
	default :
		Lua::Util::error( L, "expecting a valid Painter::ColorMode" );
	}
	return f;
}
int Enums::ParsingMode(int position)
{
	QUrl::ParsingMode f;
	switch( Util::toInt( L, position ) )
	{
	case QUrl::TolerantMode: 
		f = QUrl::TolerantMode;
		break;
	case QUrl::StrictMode  : 
		f = QUrl::StrictMode  ;
		break;
	default :
		Lua::Util::error( L, "expecting a valid Url::ParsingMode" );
	}
	return f;
}
int Enums::VerticalAlignment(int position)
{
	QTextCharFormat::VerticalAlignment f;
	switch( Util::toInt( L, position ) )
	{
	case QTextCharFormat::AlignNormal: 
		f = QTextCharFormat::AlignNormal;
		break;
	case QTextCharFormat::AlignSuperScript: 
		f = QTextCharFormat::AlignSuperScript;
		break;
	case QTextCharFormat::AlignSubScript: 
		f = QTextCharFormat::AlignSubScript;
		break;
	default :
		Lua::Util::error( L, "expecting a valid Painter::VerticalAlignment" );
	}
	return f;
}
int Enums::Orientation(int position)
{
	QPrinter::Orientation f;
	switch( Util::toInt( L, position ) )
	{
	case QPrinter::Portrait: 
		f = QPrinter::Portrait;
		break;
	case QPrinter::Landscape  : 
		f = QPrinter::Landscape  ;
		break;
	default :
		Lua::Util::error( L, "expecting a valid Painter::Orientation" );
	}
	return f;
}
int Enums::UnderlineStyle(int position)
{
	QTextCharFormat::UnderlineStyle f;
	switch( Util::toInt( L, position ) )
	{
	case QTextCharFormat::NoUnderline: 
		f = QTextCharFormat::NoUnderline;
		break;
	case QTextCharFormat::SingleUnderline  : 
		f = QTextCharFormat::SingleUnderline  ;
		break;
	case QTextCharFormat::DashUnderline  : 
		f = QTextCharFormat::DashUnderline  ;
		break;
	case QTextCharFormat::DotLine  : 
		f = QTextCharFormat::DotLine  ;
		break;
	case QTextCharFormat::DashDotLine  : 
		f = QTextCharFormat::DashDotLine  ;
		break;
	case QTextCharFormat::DashDotDotLine  : 
		f = QTextCharFormat::DashDotDotLine  ;
		break;
	case QTextCharFormat::WaveUnderline  : 
		f = QTextCharFormat::WaveUnderline  ;
		break;
	case QTextCharFormat::SpellCheckUnderline  : 
		f = QTextCharFormat::SpellCheckUnderline  ;
		break;
	default :
		Lua::Util::error( L, "expecting a valid Painter::UnderlineStyle" );
	}
	return f;
}
int Enums::OutputFormat(int position)
{
	QPrinter::OutputFormat f;
	switch( Util::toInt( L, position ) )
	{
	case QPrinter::NativeFormat: 
		f = QPrinter::NativeFormat;
		break;
	case QPrinter::PdfFormat  : 
		f = QPrinter::PdfFormat  ;
		break;
	case QPrinter::PostScriptFormat   : 
		f = QPrinter::PostScriptFormat   ;
		break;
	default :
		Lua::Util::error( L, "expecting a valid Painter::OutputFormat" );
	}
	return f;
}
int Enums::PageOrder(int position)
{
	QPrinter::PageOrder f;
	switch( Util::toInt( L, position ) )
	{
	case QPrinter::FirstPageFirst: 
		f = QPrinter::FirstPageFirst;
		break;
	case QPrinter::LastPageFirst   : 
		f = QPrinter::LastPageFirst   ;
		break;
	default :
		Lua::Util::error( L, "expecting a valid Painter::PageOrder" );
	}
	return f;
}
int Enums::PageSize(int position)
{
	QPrinter::PageSize f;
	switch( Util::toInt( L, position ) )
	{
	case QPrinter::A0: 
		f = QPrinter::A0;
		break;
	case QPrinter::A1   : 
		f = QPrinter::A1   ;
		break;
	case QPrinter::A2   : 
		f = QPrinter::A2   ;
		break;
	case QPrinter::A3   : 
		f = QPrinter::A3   ;
		break;
	case QPrinter::A4   : 
		f = QPrinter::A4   ;
		break;
	case QPrinter::A5   : 
		f = QPrinter::A5   ;
		break;
	case QPrinter::A6   : 
		f = QPrinter::A6   ;
		break;
	case QPrinter::A7   : 
		f = QPrinter::A7   ;
		break;
	case QPrinter::A8   : 
		f = QPrinter::A8   ;
		break;
	case QPrinter::A9   : 
		f = QPrinter::A9   ;
		break;
	case QPrinter::B0   : 
		f = QPrinter::B0   ;
		break;
	case QPrinter::B1   : 
		f = QPrinter::B1   ;
		break;
	case QPrinter::B10   : 
		f = QPrinter::B10   ;
		break;
	case QPrinter::B2   : 
		f = QPrinter::B2   ;
		break;
	case QPrinter::B3   : 
		f = QPrinter::B3   ;
		break;
	case QPrinter::B4   : 
		f = QPrinter::B4   ;
		break;
	case QPrinter::B5   : 
		f = QPrinter::B5   ;
		break;
	case QPrinter::B6   : 
		f = QPrinter::B6   ;
		break;
	case QPrinter::B7   : 
		f = QPrinter::B7   ;
		break;
	case QPrinter::B8   : 
		f = QPrinter::B8   ;
		break;
	case QPrinter::B9   : 
		f = QPrinter::B9   ;
		break;
	case QPrinter::C5E   : 
		f = QPrinter::C5E   ;
		break;
	case QPrinter::Comm10E   : 
		f = QPrinter::Comm10E   ;
		break;
	case QPrinter::DLE   : 
		f = QPrinter::DLE   ;
		break;
	case QPrinter::Executive   : 
		f = QPrinter::Executive   ;
		break;
	case QPrinter::Folio   : 
		f = QPrinter::Folio   ;
		break;
	case QPrinter::Ledger   : 
		f = QPrinter::Ledger   ;
		break;
	case QPrinter::Legal   : 
		f = QPrinter::Legal   ;
		break;
	case QPrinter::Letter   : 
		f = QPrinter::Letter   ;
		break;
	case QPrinter::Tabloid   : 
		f = QPrinter::Tabloid   ;
		break;
	case QPrinter::Custom   : 
		f = QPrinter::Custom   ;
		break;

	default :
		Lua::Util::error( L, "expecting a valid Painter::PageSize" );
	}
	return f;
}
int Enums::PaperSource(int position)
{
	QPrinter::PaperSource f;
	switch( Util::toInt( L, position ) )
	{
	case QPrinter::OnlyOne: 
		f = QPrinter::OnlyOne;
		break;
	case QPrinter::Lower   : 
		f = QPrinter::Lower   ;
		break;
	case QPrinter::Middle   : 
		f = QPrinter::Middle   ;
		break;
	case QPrinter::Manual   : 
		f = QPrinter::Manual   ;
		break;
	case QPrinter::Envelope   : 
		f = QPrinter::Envelope   ;
		break;
	case QPrinter::EnvelopeManual   : 
		f = QPrinter::EnvelopeManual   ;
		break;
	case QPrinter::Auto   : 
		f = QPrinter::Auto   ;
		break;
	case QPrinter::Tractor   : 
		f = QPrinter::Tractor   ;
		break;
	case QPrinter::SmallFormat   : 
		f = QPrinter::SmallFormat   ;
		break;
	case QPrinter::LargeFormat   : 
		f = QPrinter::LargeFormat   ;
		break;
	case QPrinter::LargeCapacity   : 
		f = QPrinter::LargeCapacity   ;
		break;
	case QPrinter::Cassette   : 
		f = QPrinter::Cassette   ;
		break;
	case QPrinter::FormSource   : 
		f = QPrinter::FormSource   ;
		break;
	default :
		Lua::Util::error( L, "expecting a valid Painter::PaperSource" );
	}
	return f;
}
int Enums::PrintRange(int position)
{
	QPrinter::PrintRange f;
	switch( Util::toInt( L, position ) )
	{
	case QPrinter::AllPages: 
		f = QPrinter::AllPages;
		break;
	case QPrinter::Selection   : 
		f = QPrinter::Selection   ;
		break;
	case QPrinter::PageRange    : 
		f = QPrinter::PageRange    ;
		break;
	default :
		Lua::Util::error( L, "expecting a valid Painter::PrintRange" );
	}
	return f;
}
int Enums::Alignment(int position)
{
	Qt::Alignment f;
	switch( Util::toInt( L, position ) )
	{
	case Qt::AlignLeft: 
		f = Qt::AlignLeft;
		break;
	case Qt::AlignRight:
		f = Qt::AlignRight;
		break;
	default :
		Lua::Util::error( L, "expecting a valid Qt::Alignment" );
	}
	return f;
}

int Enums::ItemFlags( int position )
{
	Qt::ItemFlags f;
	switch( Util::toInt( L, position ) )
	{
	case Qt::ItemIsSelectable: 
		f = Qt::ItemIsSelectable;
		break;
	case Qt::ItemIsEditable:
		f = Qt::ItemIsEditable;
		break;
	case Qt::ItemIsDragEnabled: 
		f = Qt::ItemIsDragEnabled;
		break;
	case Qt::ItemIsDropEnabled:
		f = Qt::ItemIsDropEnabled;
		break;
	case Qt::ItemIsUserCheckable:
		f = Qt::ItemIsUserCheckable;
		break;
	case Qt::ItemIsEnabled: 
		f = Qt::ItemIsEnabled;
		break;
	case Qt::ItemIsTristate:
		f = Qt::ItemIsTristate;
		break;
	default :
		Lua::Util::error( L, "expecting a valid Qt::ItemFlags" );
	}
	return f;

	if ( Util::isNum( L, position ) )
	{
		f = Qt::ItemFlags();
	}
	else
	{
		Lua::Util::error( L, "expecting a valid Qt::ItemFlags" );
	}
	return f;
}
int Enums::CheckState( int position )
{
	Qt::CheckState f;
	switch( Util::toInt( L, position ) )
	{
	case Qt::Unchecked: 
		f = Qt::Unchecked;
		break;
	case Qt::PartiallyChecked:
		f = Qt::PartiallyChecked;
		break;
	case Qt::Checked:
		f = Qt::Checked;
		break;
	default :
		Lua::Util::error( L, "expecting a valid Qt::CheckState" );
	}
	return f;
}
int Enums::CaseSensitivity( int position )
{
	Qt::CaseSensitivity f;
	switch( Util::toInt( L, position ) )
	{
	case Qt::CaseInsensitive:
		f = Qt::CaseInsensitive;
		break;
	case Qt::CaseSensitive:
		f = Qt::CaseSensitive;
		break;
	default :
		Lua::Util::error( L, "expecting a valid Qt::CheckState" );
	}
	return f;
}
int Enums::Corner( int position )
{
	Qt::Corner f;
	switch( Util::toInt( L, position ) )
	{
	case Qt::TopLeftCorner:
		f = Qt::TopLeftCorner;
		break;
	case Qt::TopRightCorner:
		f = Qt::TopRightCorner;
		break;
	case Qt::BottomLeftCorner:
		f = Qt::BottomLeftCorner;
		break;
	case Qt::BottomRightCorner:
		f = Qt::TopRightCorner;
		break;
	default :
		Lua::Util::error( L, "expecting a valid Qt::Corner" );
	}
	return f;
}
int Enums::TextElideMode( int position )
{
	Qt::TextElideMode f;
	switch( Util::toInt( L, position ) )
	{
	case Qt::ElideLeft:
		f = Qt::ElideLeft;
		break;
	case Qt::ElideRight:
		f = Qt::ElideRight;
		break;
	case Qt::ElideMiddle:
		f = Qt::ElideMiddle;
		break;
	case Qt::ElideNone:
		f = Qt::ElideNone;
		break;
	default :
		Lua::Util::error( L, "expecting a valid Qt::TextElideMode" );
	}
	return f;
}

int Enums::PageBreakFlags( int position )
{
	QTextFormat::PageBreakFlags f;
	switch( Util::toInt( L, position ) )
	{
	case QTextFormat::PageBreak_Auto:
		f = QTextFormat::PageBreak_Auto;
		break;
	case QTextFormat::PageBreak_AlwaysBefore:
		f = QTextFormat::PageBreak_AlwaysBefore;
		break;
	case QTextFormat::PageBreak_AlwaysAfter:
		f = QTextFormat::PageBreak_AlwaysAfter;
		break;
	default :
		Lua::Util::error( L, "expecting a valid QTextFormat::PageBreakFlags" );
	}
	return f;
}
int Enums::Flags( int position )
{
	QTextOption::Flags f;
	switch ( Util::toInt( L, position ) )
	{
	case QTextOption::IncludeTrailingSpaces:
		f = QTextOption::IncludeTrailingSpaces ;
		break;
	default :
		Lua::Util::error( L, "expecting a valid QTextOption::Flags" );
	}
	return f;
}

int Enums::TabPosition( int position )
{
	QTabWidget::TabPosition f;
	switch ( Util::toInt( L, position ) )
	{
	case QTabWidget::North:
		f = QTabWidget::North;
		break;
	case QTabWidget::South:
		f = QTabWidget::South;
		break;
	case QTabWidget::West:
		f = QTabWidget::West;
		break;
	case QTabWidget::East:
		f = QTabWidget::East;
		break;
	default :
		Lua::Util::error( L, "expecting a valid QTabWidget::TabPosition" );
	}
	return f;
}
int Enums::TabShape( int position )
{
	QTabWidget::TabShape f;
	switch ( Util::toInt( L, position ) )
	{
	case QTabWidget::Rounded:
		f = QTabWidget::Rounded;
		break;
	case QTabWidget::Triangular:
		f = QTabWidget::Triangular;
		break;
	default :
		Lua::Util::error( L, "expecting a valid QTabWidget::TabShape" );
	}
	return f;
}

int Enums::wrapMode( int position )
{
	//QTextOption::wrapMode f;
	switch( Util::toInt( L, position ) )
	{
	case QTextOption::NoWrap:
		return  QTextOption::NoWrap;
		break;
	case QTextOption::WordWrap:
		return QTextOption::WordWrap;
		break;
	case QTextOption::ManualWrap:
		return QTextOption::ManualWrap;
		break;
	case QTextOption::WrapAnywhere:
		return QTextOption::WrapAnywhere;
		break;
	case QTextOption::WrapAtWordBoundaryOrAnywhere:
		return QTextOption::WrapAtWordBoundaryOrAnywhere;
		break;
	default :
		Lua::Util::error( L, "expecting a valid QTextOption::WrapMode for WrapMode" );
	}
	return QTextOption::NoWrap;;
}
int Enums::Style( int position )
{
	QTextListFormat::Style f;
	switch ( Util::toInt( L, position ) )
	{
	case QTextListFormat::ListDisc:
		f = QTextListFormat::ListDisc;
		break;
	case QTextListFormat::ListCircle:
		f = QTextListFormat::ListCircle;
		break;
	case QTextListFormat::ListSquare:
		f = QTextListFormat::ListSquare;
		break;
	case QTextListFormat::ListDecimal:
		f = QTextListFormat::ListDecimal;
		break;
	case QTextListFormat::ListLowerAlpha:
		f = QTextListFormat::ListLowerAlpha;
		break;
	case QTextListFormat::ListUpperAlpha:
		f = QTextListFormat::ListUpperAlpha;
		break;
	default :
		Lua::Util::error( L, "expecting a valid QTextListFormat::Style" );
	}
	return f;
}

int Enums::MoveOperation ( int position )
{
	QTextCursor::MoveOperation f;
	switch ( Util::toInt( L, position ) )
	{
	case QTextCursor::NoMove:
		f = QTextCursor::NoMove;
		break;
	case QTextCursor::Start:
		f = QTextCursor::Start;
		break;
	case QTextCursor::StartOfLine:
		f = QTextCursor::StartOfLine;
		break;
	case QTextCursor::StartOfBlock:
		f = QTextCursor::StartOfBlock;
		break;
	case QTextCursor::StartOfWord:
		f = QTextCursor::StartOfWord;
		break;
	case QTextCursor::PreviousBlock:
		f = QTextCursor::PreviousBlock;
		break;
	case QTextCursor::PreviousCharacter:
		f = QTextCursor::PreviousCharacter;
		break;
	case QTextCursor::PreviousWord:
		f = QTextCursor::PreviousWord;
		break;
	case QTextCursor::Up:
		f = QTextCursor::Up;
		break;
	case QTextCursor::Left:
		f = QTextCursor::Left;
		break;
	case QTextCursor::WordLeft:
		f = QTextCursor::WordLeft;
		break;
	case QTextCursor::End:
		f = QTextCursor::End;
		break;
	case QTextCursor::EndOfLine:
		f = QTextCursor::EndOfLine;
		break;
	case QTextCursor::EndOfWord:
		f = QTextCursor::EndOfWord;
		break;
	case QTextCursor::EndOfBlock:
		f = QTextCursor::EndOfBlock;
		break;
	case QTextCursor::NextBlock:
		f = QTextCursor::NextBlock;
		break;
	case QTextCursor::NextCharacter:
		f = QTextCursor::NextCharacter;
		break;
	case QTextCursor::NextWord:
		f = QTextCursor::NextWord;
		break;
	case QTextCursor::Down:
		f = QTextCursor::Down;
		break;
	case QTextCursor::Right:
		f = QTextCursor::Right;
		break;
	case QTextCursor::WordRight:
		f = QTextCursor::WordRight;
		break;
	default :
		Lua::Util::error( L, "expecting a valid QTextListFormat::MoveOperation" );
	}
	return f;
}

int Enums::MoveMode ( int position )
{
	QTextCursor::MoveMode f;
	switch ( Util::toInt( L, position ) )
	{
	case QTextCursor::MoveAnchor:
		f = QTextCursor::MoveAnchor;
		break;
	case QTextCursor::KeepAnchor:
		f = QTextCursor::KeepAnchor;
		break;
	default :
		Lua::Util::error( L, "expecting a valid QTextListFormat::MoveMode" );
	}
	return f;
}
int Enums::SelectionType ( int position )
{
	QTextCursor::SelectionType f;
	switch ( Util::toInt( L, position ) )
	{
	case QTextCursor::Document:
		f = QTextCursor::Document;
		break;
	case QTextCursor::BlockUnderCursor:
		f = QTextCursor::BlockUnderCursor;
		break;
	case QTextCursor::LineUnderCursor:
		f = QTextCursor::LineUnderCursor;
		break;
	case QTextCursor::WordUnderCursor:
		f = QTextCursor::WordUnderCursor;
		break;
	default :
		Lua::Util::error( L, "expecting a valid QTextListFormat::SelectionType" );
	}
	return f;
}

int Enums::Position ( int position )
{
	QTextFrameFormat::Position f;
	switch ( Util::toInt( L, position ) )
	{
	case QTextFrameFormat::InFlow:
		f = QTextFrameFormat::InFlow;
		break;
	case QTextFrameFormat::FloatLeft:
		f = QTextFrameFormat::FloatLeft;
		break;
	case QTextFrameFormat::FloatRight:
		f = QTextFrameFormat::FloatRight;
		break;
	default :
		Lua::Util::error( L, "expecting a valid QTextFrameFormat::Position" );
	}
	return f;
}
int Enums::FindFlags(int position)
{
	QTextDocument::FindFlags f;
	switch( Util::toInt( L, position ) )
	{
	case QTextDocument::FindBackward: 
		f = QTextDocument::FindBackward;
		break;
	case QTextDocument::FindCaseSensitively: 
		f = QTextDocument::FindCaseSensitively;
		break;
	case QTextDocument::FindWholeWords:
		f = QTextDocument::FindWholeWords;
		break;
	default :
		Lua::Util::error( L, "expecting a valid QTextDocument::FindFlags" );
	}
	return f;
}
int Enums::MetaInformation(int position)
{
	QTextDocument::MetaInformation f;
	switch( Util::toInt( L, position ) )
	{
	case QTextDocument::DocumentTitle: 
		f = QTextDocument::DocumentTitle;
		break;
	default :
		Lua::Util::error( L, "expecting a valid QTextDocument::MetaInformation" );
	}
	return f;
}
int Enums::DateFormat ( int position )
{
	Qt::DateFormat f;
	switch ( Util::toInt( L, position ) )
	{
	case Qt::TextDate:
		f = Qt::TextDate;
		break;
	case Qt::ISODate:
		f = Qt::ISODate;
		break;
	case Qt::LocalDate:
		f = Qt::SystemLocaleDate;
		break;
	//case Qt::SystemLocaleDate:
	//	f = Qt::SystemLocaleDate;
	//	break;
	case Qt::LocaleDate:
		f = Qt::LocaleDate;
		break;
	default :
		Lua::Util::error( L, "expecting a valid Qt::DateFormat" );
	}
	return f;
}
int Enums::DialogLabel( int position )
{
	QFileDialog::DialogLabel f;
	switch ( Util::toInt( L, position ) )
	{
	case QFileDialog::LookIn:
		f = QFileDialog::LookIn;
		break;
	case QFileDialog::FileName:
		f = QFileDialog::FileName;
		break;
	case QFileDialog::FileType:
		f = QFileDialog::FileType;
		break;
	case QFileDialog::Accept:
		f = QFileDialog::Accept;
		break;
	case QFileDialog::Reject :
		f = QFileDialog::Reject ;
		break;
	default :
		Lua::Util::error( L, "expecting a valid QFileDialog::DialogLabel" );
	}
	return f;
}
