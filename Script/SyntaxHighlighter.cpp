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

#include "SyntaxHighlighter.h"
#include "Lua.h"
#include <QtDebug>
using namespace Lua;

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    // Die Regeln werden in der hier gegebenen Reihenfolge abgearbeitet
    d_commentFormat.setProperty( TokenProp, Comment );
    d_commentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("--+[^\n]*");
    rule.format = d_commentFormat;
    rule.name = "Single Line Comment";
    d_rules.append(rule);

    d_literalFormat.setProperty( TokenProp, LiteralString );
    d_literalFormat.setForeground(Qt::darkRed);
    // Quelle: http://stackoverflow.com/questions/481282/how-can-i-match-double-quoted-strings-with-escaped-double-quote-characters
    rule.pattern = QRegExp( "\"(?:[^\\\\\"]|\\\\.)*\"" );
    rule.pattern.setMinimal(true);
    rule.name = "Double Quote String";
    rule.format = d_literalFormat;
    d_rules.append(rule);
    rule.pattern = QRegExp(  "'(?:[^\\\\']|\\\\.)*'" );
    rule.pattern.setMinimal(true);
    rule.name = "Single Quote String";
    d_rules.append(rule);

    QTextCharFormat keywordFormat;
    keywordFormat.setProperty( TokenProp, Keyword );
    keywordFormat.setForeground(QColor(0x00,0x00,0x7f));
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\band\\b" << "\\bbreak\\b" << "\\bdo\\b"
                    << "\\belse\\b" << "\\belseif\\b" << "\\bend\\b"
                    << "\\bfalse\\b" << "\\bfor\\b" << "\\bfunction\\b"
                    << "\\bif\\b" << "\\bin\\b" << "\\blocal\\b"
                    << "\\bnil\\b" << "\\bnot\\b" << "\\bor\\b"
                    << "\\brepeat\\b" << "\\breturn\\b" << "\\bthen\\b"
                    << "\\btrue\\b" << "\\buntil\\b" << "\\bwhile\\b";
    foreach (const QString &pattern, keywordPatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        rule.name = "Keyword " + pattern.toAscii();
        d_rules.append(rule);
    }

    QTextCharFormat numbers;
    numbers.setProperty( TokenProp, Number );
    numbers.setForeground(Qt::red);
    // 0xff   0x56
    rule.pattern = QRegExp("(^[a-zA-Z_]|\\b)0x[0-9a-fA-F]+" );
    // (^[a-zA-Z_]|\\b) heisst, dass 0x nicht Teil eines Idents sein darf
    rule.format = numbers;
    rule.name = "Number";
    d_rules.append(rule);

    QTextCharFormat idents; // Ist nötig, damit Idents der Form "abc123" nicht als Zahl interpretiert werden
    idents.setProperty( TokenProp, Ident );
    idents.setForeground(Qt::black);
    rule.pattern = QRegExp("([^0-9]|\\b)[a-zA-Z_][a-zA-Z0-9_]*");
    // ([^0-9]|\\b) heisst, entweder ist der Ident am Anfang der Zeile oder nicht unmittelbar nach Zahl.
    // Damit verhindern wir, dass 314.16e-2 das "e" als Ident verwendet wird.
    rule.format = idents;
    rule.name = "Ident";
    d_rules.append(rule);

    // 3   3.0    3.1416   314.16e-2   0.31416E1   12E2   .123 .16e-2 .31416E1
    rule.pattern = QRegExp("[0-9]*[\\.]?[0-9]+([eE][-+]?[0-9]+)?" );
    rule.format = numbers;
    rule.name = "Number";
    d_rules.append(rule);

    keywordFormat.setProperty( TokenProp, Other );
    QStringList otherTokens; // Zuerst die langen, dann die kurzen
    otherTokens <<  "\\.\\.\\." << "\\.\\." << "==" << "~=" << "<=" << ">=" <<
                    "\\*" << "/" << "%" << "\\^" << "#" << "<" << ">" << "=" <<
                    "\\(" << "\\)" << "{" << "}" << "\\[" << "\\]" <<  ";" << ":" << "," <<
                    "\\+" << "-" << "\\.";
    foreach (const QString &pattern, otherTokens)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        rule.name = "Token " + pattern.toAscii();
        d_rules.append(rule);
    }
}

static inline void empty( QString& str, int start, int len )
{
    // Kostet fast nichts, da kein dynamischer Speicher angelegt oder freigegeben wird
    for( int i = start; i < ( start + len ); i++ )
        str[i] = QChar(' ');
}

static const int s_cmntSymLen = 3;

// static QRegExp s_pattern = QRegExp("( --\\[ | \\[[=]*\\[ | \\]-- | \\][=]*\\] )"); // funktioniert nicht zuverlässig

struct Mark
{
    int d_pos; // Position in text
    qint16 d_num; // Anzahl Gleichheitszeichen + 1, positiv..start, negagiv..end
    bool d_cmnt; // true..es ist ein Comment, false..es ist ein String
    int len() const { return (d_cmnt)?s_cmntSymLen:qAbs(d_num) + 1; }
    Mark():d_pos(-1){}
};
typedef QList<Mark> Marks;

static Mark _nextMark2( const QString& text, int from = 0 )
{
    // Suche nach "--[", "]--", "]=]", "[=["
    Mark res;
    for( int i = from; i < text.size(); i++ )
    {
        const QChar c = text[i];
        if( c == QChar('-') && i + 2 < text.size() ) // "--["
        {
            if( text[i+1] == QChar('-') && text[i+2] == QChar('[') )
            {
                res.d_cmnt = true;
                res.d_pos = i;
                res.d_num = 1;
                return res;
            }
        }else if( c == QChar(']') && i + 1 < text.size() ) // "]--" oder "]=]"
        {
            if( text[i+1] == QChar('-') && i + 2 < text.size() && text[i+2] == QChar('-') ) // "]--"
            {
                res.d_cmnt = true;
                res.d_pos = i;
                res.d_num = -1;
                return res;
            }else if( text[i+1] == QChar('=') ) // "]=]"
            {
                int j = i+2;
                while( j < text.size() && text[j] == QChar('=') )
                    j++;
                if( j < text.size() && text[j] == QChar(']') )
                {
                    res.d_cmnt = false;
                    res.d_pos = i;
                    res.d_num = -(j-i);
                    return res;
                }
            }else if( text[i+1] == QChar(']') ) // "]]"
            {
                res.d_cmnt = false;
                res.d_pos = i;
                res.d_num = -1;
                return res;
            }
        }else if( c == QChar('[') && i + 1 < text.size() ) // "[=["
        {
            if( text[i+1] == QChar('=') )
            {
                int j = i+2;
                while( j < text.size() && text[j] == QChar('=') )
                    j++;
                if( j < text.size() && text[j] == QChar('[') )
                {
                    res.d_cmnt = false;
                    res.d_pos = i;
                    res.d_num = j-i;
                    return res;
                }
            }else if( text[i+1] == QChar('[') ) // "[["
            {
                res.d_cmnt = false;
                res.d_pos = i;
                res.d_num = 1;
                return res;
            }
        }
    }
    return res;
}

//static Mark _nextMark( const QString& text, int from = 0 )
//{
//    Mark res;
//    res.d_pos = s_pattern.indexIn( text, from );
//    if( res.d_pos != -1 )
//    {
//        res.d_num = text.mid( res.d_pos, s_pattern.matchedLength() ).count(QChar('=') );
//        if( res.d_num == 0 )
//            res.d_cmnt = text[res.d_pos + 1] == QChar('-'); // ]-- und --[ haben beide an zweiter Stelle ein -
//        // else eindeutig ein String
//        res.d_num++;

//        if( text[res.d_pos] == QChar(']') ) // ]-- und ]==] starten beide mit ]
//            res.d_num *= -1;
//    }
//    return res;
//}

static Marks _findMarks( const QString& text )
{
    Marks marks; // alle Marks der Zeile
    Mark pos = _nextMark2( text );
    while( pos.d_pos != -1 )
    {
        marks.append(pos);
        pos = _nextMark2( text, pos.d_pos + pos.len() );
    }
    return marks;
}

union BlockState
{
    int d_int; // Initialwert ist -1, was 0xffffffff entspricht
    struct Data
    {
        unsigned int startOfComment:1; // Auf der Zeile beginnt ein Kommentar, der dort nicht endet
        unsigned int endOfComment:1;   // Auf der Zeile endet ein Kommentar, der dort nicht beginnt
        unsigned int allLineComment:1; // Die ganze Zeile gehört zu einem Kommentar, der darüber beginnt und darunter endet
        unsigned int startOfString:1;
        unsigned int endOfString:1;
        unsigned int allLineString:1;
        unsigned int stringLevel:8;    // Anz. "=" plus 1
        unsigned int dummy:17;
        unsigned int unitialized:1;
    } d_state;
};

void SyntaxHighlighter::highlightBlock(const QString & block)
{
    QString text = block; // wir machen Kopie, damit wir die geparsten Stellen rauslöschen können

    BlockState prev;
    prev.d_int = previousBlockState();
    if( prev.d_state.unitialized )
        prev.d_int = 0;
    BlockState newCur;
    newCur.d_int = 0;

    Marks marks = _findMarks( text );
    int marksDone = 0;
    if( prev.d_state.startOfComment || prev.d_state.allLineComment )
    {   // wir sind in einem Kommentar drin
        // prüfe, ob er hier endet; suche das erste End; alle davor liegenden Starts und anderen Marks werden ignoriert
        for( int i = 0; i < marks.size(); i++ )
        {
            if( marks[i].d_cmnt && marks[i].d_num < 0 )
            {   // wir sind auf ein End gestossen
                marksDone = i + 1;
                newCur.d_state.endOfComment = true;
                stamp( text, 0, marks[i].d_pos + marks[i].len(), d_commentFormat );
                break;
            }
        }
        if( marksDone == 0 )
        {   // keine wirksamen Comment Marks gefunden; die ganze Zeile ist auch ein Kommentar
            newCur.d_state.allLineComment = true;
            stamp( text, 0, text.size(), d_commentFormat );
            marksDone = marks.size();
        }
    }else if( prev.d_state.startOfString || prev.d_state.allLineString )
    {
        // wir sind in einem String drin
        // prüfe, ob er hier endet; suche das erste End; alle davor liegenden Starts und anderen Marks werden ignoriert
        for( int i = 0; i < marks.size(); i++ )
        {
            if( !marks[i].d_cmnt && marks[i].d_num < 0 && qAbs(marks[i].d_num) == prev.d_state.stringLevel )
            {   // wir sind auf ein End gestossen
                marksDone = i + 1;
                newCur.d_state.endOfString = true;
                newCur.d_state.stringLevel = prev.d_state.stringLevel;
                stamp( text, 0, marks[i].d_pos + marks[i].len(), d_literalFormat );
                break;
            }
        }
        if( marksDone == 0 )
        {   // keine wirksamen String Marks gefunden; die ganze Zeile ist auch ein String
            newCur.d_state.allLineString = true;
            newCur.d_state.stringLevel = prev.d_state.stringLevel;
            stamp( text, 0, text.size(), d_literalFormat );
            marksDone = marks.size();
        }
    }
    // Suche ganze Paare
    for( int i = marksDone; i < marks.size(); i++ )
    {
        if( marks[i].d_num > 0 )
        {
            for(int j = i + 1; j < marks.size(); j++ )
            {
                if( marks[j].d_num == -marks[i].d_num && marks[j].d_cmnt == marks[i].d_cmnt )
                {
                    stamp( text, marks[i].d_pos, marks[j].d_pos - marks[i].d_pos + marks[j].len(),
                           (marks[i].d_cmnt)?d_commentFormat:d_literalFormat );
                    marks[i].d_num = 0;
                    marks[j].d_num = 0; // als gesehen markieren
                }
            }
        }
    }
    // Suche offene Enden
    for( int i = marksDone; i < marks.size(); i++ )
    {
        if( marks[i].d_num > 0 )
        {
            if( marks[i].d_cmnt )
                newCur.d_state.startOfComment = true;
            else
            {
                newCur.d_state.startOfString = true;
                newCur.d_state.stringLevel = marks[i].d_num;
            }
            stamp( text, marks[i].d_pos, text.size() - marks[i].d_pos, (marks[i].d_cmnt)?d_commentFormat:d_literalFormat );
        }
    }
    setCurrentBlockState( newCur.d_int );

    foreach( const HighlightingRule &rule, d_rules )
    {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while( index >= 0 )
        {
            const int length = expression.matchedLength();
            // qDebug() << "hit" << rule.name << ":" << str.mid( index, length );
            stamp( text, index, length, rule.format );
            index = expression.indexIn(text, index + length);
        }
    }
}

void SyntaxHighlighter::stamp(QString &text, int start, int len, const QTextCharFormat &f)
{
    setFormat(start, len, f );
    // Vermeide, dass mehrere Regeln auf denselben Text angewendet werden
    empty( text, start, len );
    //qDebug() << "empty:" << str;
}

QString SyntaxHighlighter::format(int tokenType)
{
    switch( tokenType )
    {
    case Ident:
        return tr("Ident");
    case Keyword:
        return tr("Keyword");
    case Number:
        return tr("Number");
    case LiteralString:
        return tr("String");
    case Comment:
        return tr("Comment");
    case Other:
        return tr("Other");
    default:
        return QString();
    }
}
