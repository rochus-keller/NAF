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

#include "Terminal.h"
#include "ExpressionParser.h"
#include <QStatusBar>
#include <QKeyEvent>
#include <QApplication>
#include <QClipboard>
#include <QPrinter>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <Root/Application.h>
#include <Gui/Menu.h>
using namespace Lua;

static QTextCharFormat s_pf;
static QTextCharFormat s_luaf;
static QTextCharFormat s_errf;
static QTextCharFormat s_outf;
static const char* s_prompt = "Lua> ";

ACTION_SLOTS_BEGIN( Terminal )
	{ Root::Action::EditCopy, &Terminal::handleCopy }, 
	{ Root::Action::EditPaste, &Terminal::handlePaste }, 
	{ Root::Action::EditSelectAll, &Terminal::handleSelectAll }, 
	{ Root::Action::EditDelete, &Terminal::handleDelete }, 
	{ Root::Action::FileExportPdf, &Terminal::handleExportPdf }, 
	{ Root::Action::FileSaveAs, &Terminal::handleSaveAs }, 
ACTION_SLOTS_END( Terminal );


Terminal::Terminal(QWidget* parent, Lua::Engine* l):
	QTextEdit( parent ), d_lua( l )
{
	assert( l );

	s_pf.setFontFamily( "Arial" );
	s_pf.setFontWeight( QFont::Bold );
	s_luaf.setFontFamily( "Courier" );
	s_luaf.setFontWeight( QFont::Normal );
	s_errf.setFontFamily( "Courier" );
	s_errf.setFontWeight( QFont::Normal );
	s_errf.setForeground( Qt::red );
	s_outf.setFontFamily( "Courier" );
	s_outf.setFontWeight( QFont::Normal );
	s_outf.setForeground( Qt::blue );

	QTextCursor cur = textCursor();
    cur.insertText( prompt(), s_pf );
	d_out = QTextCursor( document() );
	
	setFocusPolicy( Qt::StrongFocus );
	setReadOnly( true );
    setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);

	Gui::Menu* pop = new Gui::Menu( this, true );
	Gui::Menu::item( pop, this, "Copy", Root::Action::EditCopy );
	Gui::Menu::item( pop, this, "Paste", Root::Action::EditPaste );
	pop->addSeparator();
	Gui::Menu::item( pop, this, "Select All", Root::Action::EditSelectAll );
	Gui::Menu::item( pop, this, "Clear", Root::Action::EditDelete );
	pop->addSeparator();
	Gui::Menu::item( pop, this, "Export PDF...", Root::Action::FileExportPdf );
	Gui::Menu::item( pop, this, "Save Log...", Root::Action::FileSaveAs );

	d_lua->addObserver( this );
}

Terminal::~Terminal()
{
	d_lua->removeObserver( this );
}

void Terminal::handle(Root::Message & msg)
{
	BEGIN_HANDLER();
	MESSAGE( Engine::Update, a, msg )
	{
		switch( a->getType() )
		{
		case Engine::Update::Print:
            d_out.insertText( a->d_val1, s_outf );
			d_out.insertText( QString( QChar::ParagraphSeparator ), s_pf );
			break;
		case Engine::Update::Error:
            d_out.insertText( a->d_val1, s_errf );
			d_out.insertText( QString( QChar::ParagraphSeparator ), s_pf );
			break;
		case Engine::Update::Start:
		case Engine::Update::Finish:
		case Engine::Update::Abort:
            {
				moveCursor( QTextCursor::StartOfLine, QTextCursor::KeepAnchor );
				QTextCursor cur = textCursor();
				cur.insertText( prompt(), s_pf );
				cur.movePosition(QTextCursor::StartOfLine);
				d_out.setPosition( cur.position() );
				d_line.clear();
            }
            break;
        default:
            break;
		}
		//msg.consume()
		QTextCursor cur = textCursor();
		cur.setPosition( d_out.position() );
		ensureCursorVisible();
	}
	MESSAGE( Root::Action, a, msg )
	{
		EXECUTE_ACTION( Terminal, *a );
	}
	END_HANDLER();
}

void Terminal::keyPressEvent(QKeyEvent *e)
{
	switch( e->key() )
	{
	case Qt::Key_Return:
	case Qt::Key_Enter:
		{
            moveCursor( QTextCursor::End );
			QTextCursor cur = textCursor();
			cur.insertText( QString( QChar::ParagraphSeparator ), s_pf );
			d_out.setPosition( cur.position() );
			const int old = cur.position();
			cur.insertText( prompt(), s_pf );
			d_out.setPosition( old );
			if( !d_line.isEmpty() )
				d_histo.append( d_line );
			d_next.clear();
			if( d_lua->isExecuting() )
            {
                ExpressionParser p;
                if( p.parseAndPrint( d_line.toLatin1(), d_lua, false ) )
                    p.executeAndPrint( d_lua );
            }else
				d_lua->executeCmd( d_line.toLatin1(), "#Terminal" );
			d_line.clear();
		}
		return;
	case Qt::Key_Backspace:
		if( !d_line.isEmpty() )
		{
			moveCursor( QTextCursor::End );
			QTextCursor cur = textCursor();
			cur.deletePreviousChar();
			d_line.truncate( d_line.length() - 1 );
		}
		return;
	case Qt::Key_Up:
		if( !d_histo.isEmpty() )
		{
			moveCursor( QTextCursor::End );
			QTextCursor cur = textCursor();
			cur.setPosition( cur.position() - d_line.size(), QTextCursor::KeepAnchor );
			d_next.append( d_line );
			d_line = d_histo.back();
			d_histo.pop_back();
			cur.insertText( d_line, s_luaf );
		}
		break;
	case Qt::Key_Down:
		if( !d_next.isEmpty() )
		{
			moveCursor( QTextCursor::End );
			QTextCursor cur = textCursor();
			cur.setPosition( cur.position() - d_line.size(), QTextCursor::KeepAnchor );
			d_histo.append( d_line );
			d_line = d_next.back();
			d_next.pop_back();
			cur.insertText( d_line, s_luaf );
		}
		break;
	}
	bool ctrl = ( e->modifiers() == Qt::ControlModifier );
	if( ctrl )
	{
		switch( e->key() )
		{
		case Qt::Key_V:
			paste();
			return;
		case Qt::Key_C:
			copy();
			return;
		}
	}else if( !e->text().isEmpty() )
	{
		moveCursor( QTextCursor::End );
		QTextCursor cur = textCursor();
		cur.insertText( e->text(), s_luaf );
		d_line += e->text();
	}else
        e->ignore();
}

void Terminal::inputMethodEvent(QInputMethodEvent * e)
{
    // Auf Linux ist diese Methode nötig für Ü, Ö, etc.
    // Das ist eine sehr vereinfachte Methode. Für eine vollständige Behandlung siehe
    // QTextControlPrivate::inputMethodEvent in Qt/src/qui/text/qtextcontrol.cpp
    QString text = e->commitString();

    if (!text.isEmpty() &&
        (text.at(0).isPrint() || text.at(0) == QLatin1Char('\t')))
    {
        moveCursor( QTextCursor::End );
        QTextCursor cur = textCursor();
        cur.insertText( text, s_luaf );
        d_line += text;
        e->accept();
    }else
        e->ignore();
}

QString Terminal::prompt() const
{
    if( d_lua->isExecuting() )
        return "Exp>";
    else
        return s_prompt;
}

void Terminal::clear()
{
	setPlainText( "" );
	QTextCursor cur = textCursor();
    cur.insertText( prompt(), s_pf );
}

void Terminal::paste()
{
	QString s = QApplication::clipboard()->text();
	moveCursor( QTextCursor::End );
	QTextCursor cur = textCursor();
	cur.insertText( s, s_luaf );
	d_line += s;
}

void Terminal::handlePaste( Root::Action& t )
{
	QClipboard* cb = QApplication::clipboard();
	ACTION_ENABLED_IF( t, !cb->text().isNull() );
	paste();
}
void Terminal::handleCopy( Root::Action& t ) 
{ 
	ACTION_ENABLED_IF( t, textCursor().hasSelection() ); 
	copy(); 
}
void Terminal::handleSelectAll( Root::Action& t ) 
{
	ACTION_ENABLED_IF( t, true );
	selectAll();
}
void Terminal::handleDelete( Root::Action& t ) 
{
	ACTION_ENABLED_IF( t, true );
	clear();
}

void Terminal::handleExportPdf( Root::Action& t ) 
{
	ACTION_ENABLED_IF( t, true );
	QPrinter p( QPrinter::HighResolution );
	p.setOutputFormat( QPrinter::PdfFormat );
	p.setCreator( tr("CARA") );
	p.setPrintProgram( tr("CARA") );
	p.setDocName( tr("CARA Lua Terminal Log") );
	
	QString path = QFileDialog::getSaveFileName( this, tr("Export to PDF" ), 
		Root::AppAgent::getCurrentDir(), "*.pdf" );
	if( path.isEmpty() )
		return;
	p.setOutputFileName( path );
	print( &p );
}

void Terminal::handleSaveAs( Root::Action& t ) 
{
	ACTION_ENABLED_IF( t, true );

	QString path = QFileDialog::getSaveFileName( this, tr("Export to Text" ), 
		Root::AppAgent::getCurrentDir(), "*.txt" );
	if( path.isEmpty() )
		return;
	QFile out(path);
	if( !out.open( QIODevice::WriteOnly ) )
	{
		QMessageBox::critical( this, tr("Export to Text"), tr("Cannot open file for writing") );
		return;
	}
	out.write( toPlainText().toLatin1() );
}
