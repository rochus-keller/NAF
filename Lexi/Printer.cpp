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

#include "Printer.h"
#include <qpainter.h>
#include <qcursor.h>
#include <q3paintdevicemetrics.h> 
#include <QApplication>
#include <Lexi/GlyphTraversal.h>
#include <Lexi/Event.h>
#include <Lexi/MonoGlyph.h>
#include <Lexi/Viewport.h>
#include <Lexi/Extension.h>
#include <Root/Action.h>
using namespace Lexi;
using namespace Root;


const Printer::PageSize Printer::s_sizes[] = 
{
	{210, 297}, // A4  
	{182, 257}, // B5  
	{216, 279 }, // Letter  
	{216, 356 }, // Legal  
	{191, 254 }, // Executive  
	{841 , 1189 }, // A0  
	{594 , 841 }, // A1  
	{420 , 594 }, // A2  
	{297 , 420 }, // A3  
	{148 , 210 }, // A5  
	{105 , 148 }, // A6  
	{74 , 105 }, // A7  
	{52 , 74 }, // A8  
	{37 , 52 }, // A9  
	{1030 , 1456 }, // B0  
	{728 , 1030 }, // B1  
	{32 , 45 }, // B10  
	{515 , 728 }, // B2  
	{364 , 515 }, // B3  
	{257 , 364 }, // B4  
	{128 , 182 }, // B6  
	{91 , 128 }, // B7  
	{64 , 91 }, // B8  
	{45 , 64 }, // B9  
	{163 , 229 }, // C5E  
	{105 , 241}, // Comm10E  
	{110 , 220 }, // DLE  
	{210 , 330 }, // Folio  
	{432 , 279 }, // Ledger  
	{279 , 432 } // Tabloid 
};

const char* Printer::s_sizeNames[] = 
{
	"A4 (210x297 mm, 8.26x11.7 inches)",
	"B5 (182x257 mm, 7.17x10.13 inches)",
	"Letter (8.5x11 inches, 216x279 mm)",
	"Legal (8.5x14 inches, 216x356 mm)",
	"Executive (7.5x10 inches, 191x254 mm)",
	"A0 (841 x 1189 mm)",
	"A1 (594 x 841 mm)",
	"A2 (420 x 594 mm)",
	"A3 (297 x 420 mm)",
	"A5 (148 x 210 mm)",
	"A6 (105 x 148 mm)",
	"A7 (74 x 105 mm)",
	"A8 (52 x 74 mm)",
	"A9 (37 x 52 mm)",
	"B0 (1030 x 1456 mm)",
	"B1 (728 x 1030 mm)",
	"B10 (32 x 45 mm)",
	"B2 (515 x 728 mm)",
	"B3 (364 x 515 mm)",
	"B4 (257 x 364 mm)",
	"B6 (128 x 182 mm)",
	"B7 (91 x 128 mm)",
	"B8 (64 x 91 mm)",
	"B9 (45 x 64 mm)",
	"C5E (163 x 229 mm)",
	"Comm10E (105 x 241 mm, US Common #10 Envelope)",
	"DLE (110 x 220 mm)",
	"Folio (210 x 330 mm)",
	"Ledger (432 x 279 mm)",
	"Tabloid (279 x 432 mm)",
	0
};

static const QPrinter::PageSize s_sizeMap[] = 
{
	QPrinter::A4, // A4  
	QPrinter::B5, // B5  
	QPrinter::Letter, // Letter  
	QPrinter::Legal, // Legal  
	QPrinter::Executive, // Executive  
	QPrinter::A0, // A0  
	QPrinter::A1, // A1  
	QPrinter::A2, // A2  
	QPrinter::A3, // A3  
	QPrinter::A5, // A5  
	QPrinter::A6, // A6  
	QPrinter::A7, // A7  
	QPrinter::A8, // A8  
	QPrinter::A9, // A9  
	QPrinter::B0, // B0  
	QPrinter::B1, // B1  
	QPrinter::B10, // B10  
	QPrinter::B2, // B2  
	QPrinter::B3, // B3  
	QPrinter::B4, // B4  
	QPrinter::B6, // B6  
	QPrinter::B7, // B7  
	QPrinter::B8, // B8  
	QPrinter::B9, // B9  
	QPrinter::C5E, // C5E  
	QPrinter::Comm10E, // Comm10E  
	QPrinter::DLE, // DLE  
	QPrinter::Folio, // Folio  
	QPrinter::Ledger, // Ledger  
	QPrinter::Tabloid // Tabloid 
};

struct _PrinterImp : public Viewport
{
	_PrinterImp():
		Viewport( nil ), d_body( 0 ), d_pageSize( 0 ) 
		{
		}
	~_PrinterImp()
	{
	}

	Ref<Glyph> d_body;
	Allocation d_allocation;
	Requisition d_requisition;
	QRegion d_damage;
	int d_pageSize;
public:
	void paint(QPainter*);
	void request();
	bool isDamaged( Twips left, Twips top, Twips width, Twips height,
		Twips offX, Twips offY ) const;
};

Printer::Printer(Glyph* body ):
	QPrinter( QPrinter::ScreenResolution ) // QPrinter::HighResolution ) // TODO
{
	setFontEmbeddingEnabled( false );
	setFullPage( true );

	d_this = new _PrinterImp( );

	if( body == nil )
		body = new Glyph();
	setBody( body );
}

Printer::~Printer()
{
	delete d_this;
}

Viewport* Printer::getViewport() const
{
	return d_this;
}

void Printer::setBody(Glyph *body)
{
	assert( body );
	d_this->d_body = body; 
	d_this->request();
}

Glyph* Printer::getBody() const
{
	return d_this->d_body;
}

void _PrinterImp::request()
{
	d_body->changed();
	d_body->request( d_requisition );
}

void _PrinterImp::paint( QPainter* p )
{
	Canvas c;
	c.begin( this, p, d_damage, true );
	d_body->draw( c, d_allocation );
	c.commit();
}

void Printer::print()
{
	if( orientation() == QPrinter::Portrait )
		d_this->d_allocation = Allocation( 0, 0, 
			mmToTwip( s_sizes[ d_this->d_pageSize ].w ), 
			mmToTwip( s_sizes[ d_this->d_pageSize ].h ) );
	else
		d_this->d_allocation = Allocation( 0, 0, 
			mmToTwip( s_sizes[ d_this->d_pageSize ].h ), 
			mmToTwip( s_sizes[ d_this->d_pageSize ].w ) );

	d_this->d_damage = QRegion( 0, 0, d_this->d_allocation.getWidth(), 
		d_this->d_allocation.getHeight() ); // Soll: Points: sicher zu gross

	d_this->d_body->allocate( d_this->d_allocation );
	QPainter p( this );
    //qreal res = 72;
	/* RISK: Widget meldet 96 dpi, was mit meiner Annahme betr. TWIPS kollidiert. 72 funktioniert.
	if( QApplication::activeWindow() )
		res = QApplication::activeWindow()->logicalDpiX();
		*/
    //qreal factor = resolution() / res; // Auf Empfehlung von Troll
	// TODO p.scale( factor, factor );
	d_this->paint( &p );
}

bool _PrinterImp::isDamaged(Twips left, Twips top, Twips width, Twips height,
	Twips offX, Twips offY ) const
{
	return true;	
	// Da wir ja alles drucken wollen, wird diese Frage immer mit true beantwortet
}


void Lexi::Printer::setPageSize(int size)
{
	QPrinter::setPageSize( s_sizeMap[ size ] );
	d_this->d_pageSize = size;
}

Picture::Picture(Glyph* body )
{
	d_this = new _PrinterImp();
	d_landscape = false;
	if( body == nil )
		body = new Glyph();
	setBody( body );
}

Picture::~Picture()
{
	delete d_this;
}

Viewport* Picture::getViewport() const
{
	return d_this;
}

void Picture::setBody(Glyph *body)
{
	assert( body );
	d_this->d_body = body; 
	d_this->request();
}

Glyph* Picture::getBody() const
{
	return d_this->d_body;
}

void Picture::print()
{
	if( !d_landscape )
		d_this->d_allocation = Allocation( 0, 0, 
			Printer::mmToTwip( Printer::s_sizes[ d_this->d_pageSize ].w ), 
			Printer::mmToTwip( Printer::s_sizes[ d_this->d_pageSize ].h ) );
	else
		d_this->d_allocation = Allocation( 0, 0, 
			Printer::mmToTwip( Printer::s_sizes[ d_this->d_pageSize ].h ), 
			Printer::mmToTwip( Printer::s_sizes[ d_this->d_pageSize ].w ) );

	d_this->d_damage = QRegion( 0, 0, d_this->d_allocation.getWidth(), 
		d_this->d_allocation.getHeight() ); // Soll: Points: sicher zu gross

	d_this->d_body->allocate( d_this->d_allocation );
	QPainter p( this );
	d_this->paint( &p );
}

void Picture::setPageSize(int size)
{
	d_this->d_pageSize = size;
}
