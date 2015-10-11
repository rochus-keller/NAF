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

#ifndef LUACODEEDITOR_H
#define LUACODEEDITOR_H

#include <QPlainTextEdit>
#include <QSet>

// Adaptiert aus Qt CodeEditor Example (BSD License)

namespace Lua
{
    class CodeEditor : public QPlainTextEdit
    {
        Q_OBJECT
    public:
        explicit CodeEditor(QWidget *parent = 0);
		static QFont defaultFont();

        void paintHandleArea(QPaintEvent *event);
        int handleAreaWidth();
        int lineAt( const QPoint& ) const;

        void getCursorPosition(int *textLine,int *index = 0);
        void setCursorPosition(int textLine,int index);
        int getTokenTypeAtCursor() const;
        QString textLine( int i ) const;
        void setText( const QString& str ) { setPlainText( str ); }
        QString text() const { return toPlainText(); }
		QString getText() const { return toPlainText(); }
		void setName( const QString& str );
		QString getName() const { return d_name; }
        int lineCount() const;
        void ensureLineVisible( int line );
        void setPositionMarker( int line ); // -1..unsichtbar
        void setSelection(int lineFrom,int indexFrom, int lineTo,int indexTo);
        void selectLines( int lineFrom, int lineTo );
        bool hasSelection() const;
        QString selectedText() const;
        bool isUndoAvailable() const { return d_undoAvail; }
        bool isRedoAvailable() const { return d_redoAvail; }
        bool isCopyAvailable() const { return d_copyAvail; }
        void selectToMatchingBrace();
        void indent();
        void setIndentation(int);
        void unindent();
        void setShowNumbers( bool on );
        bool showNumbers() const { return d_showNumbers; }
        bool loadFromFile( const QString& filename, bool meetLineNumbers = true );
		bool loadFromString( const QByteArray& source, bool meetLineNumbers = true );
        void addBreakPoint( int );
        void removeBreakPoint( int );
        void clearBreakPoints();
        const QSet<int>& getBreakPoints() const { return d_breakPoints; }
	public slots:
		void handleEditUndo();
		void handleEditRedo();
		void handleEditCut();
		void handleEditCopy();
		void handleEditPaste();
		void handleEditSelectAll();
		void handleFind();
		void handleFindAgain();
		void handleReplace();
		void handleGoto();
		void handleIndent();
		void handleUnindent();
		void handleSetIndent();
		void handlePrint();
		void handleExportPdf();
    protected:
        friend class _HandleArea;
        void resizeEvent(QResizeEvent *event);
        void paintEvent(QPaintEvent *e);
        bool viewportEvent( QEvent * event );
        void keyPressEvent ( QKeyEvent * e );
        void paintIndents( QPaintEvent *e );
        void updateTabWidth();
		void find(bool fromTop);
        // To override
        virtual void numberAreaDoubleClicked( int line ) {}
    private slots:
        void updateLineNumberAreaWidth();
        void highlightCurrentLine();
        void updateLineNumberArea(const QRect &, int);
        void onUndoAvail(bool on) { d_undoAvail = on; }
        void onRedoAvail(bool on) { d_redoAvail = on; }
        void onCopyAvail(bool on) { d_copyAvail = on; }
	private:
        QWidget* d_numberArea;
        QSet<int> d_breakPoints;
        int d_curPos; // Zeiger für die aktuelle Ausführungsposition oder -1
		QString d_find;
		QString d_name;
		bool d_undoAvail;
        bool d_redoAvail;
        bool d_copyAvail;
        bool d_showNumbers;
    };
}

#endif // LUACODEEDITOR_H
