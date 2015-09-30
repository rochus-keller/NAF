/*
 * Copyright 2000-2015 Rochus Keller <mailto:rkeller@nmr.ch>
 * Copyright (C) 1992-2006 Trolltech ASA. All rights reserved.
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

#include "HtmlParser.h"
using namespace Gui;

#include <qbytearray.h>
#include <qtextcodec.h>
#include <qapplication.h>
#include <qstack.h>
#include <qdebug.h>


#define MAX_ENTITY 258
static const struct QTextHtmlEntity { const char *name; quint16 code; } entities[MAX_ENTITY]= {
    { "AElig", 0x00c6 },
    { "Aacute", 0x00c1 },
    { "Acirc", 0x00c2 },
    { "Agrave", 0x00c0 },
    { "Alpha", 0x0391 },
    { "AMP", 38 },
    { "Aring", 0x00c5 },
    { "Atilde", 0x00c3 },
    { "Auml", 0x00c4 },
    { "Beta", 0x0392 },
    { "Ccedil", 0x00c7 },
    { "Chi", 0x03a7 },
    { "Dagger", 0x2021 },
    { "Delta", 0x0394 },
    { "ETH", 0x00d0 },
    { "Eacute", 0x00c9 },
    { "Ecirc", 0x00ca },
    { "Egrave", 0x00c8 },
    { "Epsilon", 0x0395 },
    { "Eta", 0x0397 },
    { "Euml", 0x00cb },
    { "Gamma", 0x0393 },
    { "GT", 62 },
    { "Iacute", 0x00cd },
    { "Icirc", 0x00ce },
    { "Igrave", 0x00cc },
    { "Iota", 0x0399 },
    { "Iuml", 0x00cf },
    { "Kappa", 0x039a },
    { "Lambda", 0x039b },
    { "LT", 60 },
    { "Mu", 0x039c },
    { "Ntilde", 0x00d1 },
    { "Nu", 0x039d },
    { "OElig", 0x0152 },
    { "Oacute", 0x00d3 },
    { "Ocirc", 0x00d4 },
    { "Ograve", 0x00d2 },
    { "Omega", 0x03a9 },
    { "Omicron", 0x039f },
    { "Oslash", 0x00d8 },
    { "Otilde", 0x00d5 },
    { "Ouml", 0x00d6 },
    { "Phi", 0x03a6 },
    { "Pi", 0x03a0 },
    { "Prime", 0x2033 },
    { "Psi", 0x03a8 },
    { "QUOT", 34 },
    { "Rho", 0x03a1 },
    { "Scaron", 0x0160 },
    { "Sigma", 0x03a3 },
    { "THORN", 0x00de },
    { "Tau", 0x03a4 },
    { "Theta", 0x0398 },
    { "Uacute", 0x00da },
    { "Ucirc", 0x00db },
    { "Ugrave", 0x00d9 },
    { "Upsilon", 0x03a5 },
    { "Uuml", 0x00dc },
    { "Xi", 0x039e },
    { "Yacute", 0x00dd },
    { "Yuml", 0x0178 },
    { "Zeta", 0x0396 },
    { "aacute", 0x00e1 },
    { "acirc", 0x00e2 },
    { "acute", 0x00b4 },
    { "aelig", 0x00e6 },
    { "agrave", 0x00e0 },
    { "alefsym", 0x2135 },
    { "alpha", 0x03b1 },
    { "amp", 38 },
    { "and", 0x22a5 },
    { "ang", 0x2220 },
    { "apos", 0x0027 },
    { "aring", 0x00e5 },
    { "asymp", 0x2248 },
    { "atilde", 0x00e3 },
    { "auml", 0x00e4 },
    { "bdquo", 0x201e },
    { "beta", 0x03b2 },
    { "brvbar", 0x00a6 },
    { "bull", 0x2022 },
    { "cap", 0x2229 },
    { "ccedil", 0x00e7 },
    { "cedil", 0x00b8 },
    { "cent", 0x00a2 },
    { "chi", 0x03c7 },
    { "circ", 0x02c6 },
    { "clubs", 0x2663 },
    { "cong", 0x2245 },
    { "copy", 0x00a9 },
    { "crarr", 0x21b5 },
    { "cup", 0x222a },
    { "curren", 0x00a4 },
    { "dArr", 0x21d3 },
    { "dagger", 0x2020 },
    { "darr", 0x2193 },
    { "deg", 0x00b0 },
    { "delta", 0x03b4 },
    { "diams", 0x2666 },
    { "divide", 0x00f7 },
    { "eacute", 0x00e9 },
    { "ecirc", 0x00ea },
    { "egrave", 0x00e8 },
    { "empty", 0x2205 },
    { "emsp", 0x2003 },
    { "ensp", 0x2002 },
    { "epsilon", 0x03b5 },
    { "equiv", 0x2261 },
    { "eta", 0x03b7 },
    { "eth", 0x00f0 },
    { "euml", 0x00eb },
    { "euro", 0x20ac },
    { "exist", 0x2203 },
    { "fnof", 0x0192 },
    { "forall", 0x2200 },
    { "frac12", 0x00bd },
    { "frac14", 0x00bc },
    { "frac34", 0x00be },
    { "frasl", 0x2044 },
    { "gamma", 0x03b3 },
    { "ge", 0x2265 },
    { "gt", 62 },
    { "hArr", 0x21d4 },
    { "harr", 0x2194 },
    { "hearts", 0x2665 },
    { "hellip", 0x2026 },
    { "iacute", 0x00ed },
    { "icirc", 0x00ee },
    { "iexcl", 0x00a1 },
    { "igrave", 0x00ec },
    { "image", 0x2111 },
    { "infin", 0x221e },
    { "int", 0x222b },
    { "iota", 0x03b9 },
    { "iquest", 0x00bf },
    { "isin", 0x2208 },
    { "iuml", 0x00ef },
    { "kappa", 0x03ba },
    { "lArr", 0x21d0 },
    { "lambda", 0x03bb },
    { "lang", 0x2329 },
    { "laquo", 0x00ab },
    { "larr", 0x2190 },
    { "lceil", 0x2308 },
    { "ldquo", 0x201c },
    { "le", 0x2264 },
    { "lfloor", 0x230a },
    { "lowast", 0x2217 },
    { "loz", 0x25ca },
    { "lrm", 0x200e },
    { "lsaquo", 0x2039 },
    { "lsquo", 0x2018 },
    { "lt", 60 },
    { "macr", 0x00af },
    { "mdash", 0x2014 },
    { "micro", 0x00b5 },
    { "middot", 0x00b7 },
    { "minus", 0x2212 },
    { "mu", 0x03bc },
    { "nabla", 0x2207 },
    { "nbsp", 0x00a0 },
    { "ndash", 0x2013 },
    { "ne", 0x2260 },
    { "ni", 0x220b },
    { "not", 0x00ac },
    { "notin", 0x2209 },
    { "nsub", 0x2284 },
    { "ntilde", 0x00f1 },
    { "nu", 0x03bd },
    { "oacute", 0x00f3 },
    { "ocirc", 0x00f4 },
    { "oelig", 0x0153 },
    { "ograve", 0x00f2 },
    { "oline", 0x203e },
    { "omega", 0x03c9 },
    { "omicron", 0x03bf },
    { "oplus", 0x2295 },
    { "or", 0x22a6 },
    { "ordf", 0x00aa },
    { "ordm", 0x00ba },
    { "oslash", 0x00f8 },
    { "otilde", 0x00f5 },
    { "otimes", 0x2297 },
    { "ouml", 0x00f6 },
    { "para", 0x00b6 },
    { "part", 0x2202 },
    { "percnt", 0x0025 },
    { "permil", 0x2030 },
    { "perp", 0x22a5 },
    { "phi", 0x03c6 },
    { "pi", 0x03c0 },
    { "piv", 0x03d6 },
    { "plusmn", 0x00b1 },
    { "pound", 0x00a3 },
    { "prime", 0x2032 },
    { "prod", 0x220f },
    { "prop", 0x221d },
    { "psi", 0x03c8 },
    { "quot", 34 },
    { "rArr", 0x21d2 },
    { "radic", 0x221a },
    { "rang", 0x232a },
    { "raquo", 0x00bb },
    { "rarr", 0x2192 },
    { "rceil", 0x2309 },
    { "rdquo", 0x201d },
    { "real", 0x211c },
    { "reg", 0x00ae },
    { "rfloor", 0x230b },
    { "rho", 0x03c1 },
    { "rlm", 0x200f },
    { "rsaquo", 0x203a },
    { "rsquo", 0x2019 },
    { "sbquo", 0x201a },
    { "scaron", 0x0161 },
    { "sdot", 0x22c5 },
    { "sect", 0x00a7 },
    { "shy", 0x00ad },
    { "sigma", 0x03c3 },
    { "sigmaf", 0x03c2 },
    { "sim", 0x223c },
    { "spades", 0x2660 },
    { "sub", 0x2282 },
    { "sube", 0x2286 },
    { "sum", 0x2211 },
    { "sup1", 0x00b9 },
    { "sup2", 0x00b2 },
    { "sup3", 0x00b3 },
    { "sup", 0x2283 },
    { "supe", 0x2287 },
    { "szlig", 0x00df },
    { "tau", 0x03c4 },
    { "there4", 0x2234 },
    { "theta", 0x03b8 },
    { "thetasym", 0x03d1 },
    { "thinsp", 0x2009 },
    { "thorn", 0x00fe },
    { "tilde", 0x02dc },
    { "times", 0x00d7 },
    { "trade", 0x2122 },
    { "uArr", 0x21d1 },
    { "uacute", 0x00fa },
    { "uarr", 0x2191 },
    { "ucirc", 0x00fb },
    { "ugrave", 0x00f9 },
    { "uml", 0x00a8 },
    { "upsih", 0x03d2 },
    { "upsilon", 0x03c5 },
    { "uuml", 0x00fc },
    { "weierp", 0x2118 },
    { "xi", 0x03be },
    { "yacute", 0x00fd },
    { "yen", 0x00a5 },
    { "yuml", 0x00ff },
    { "zeta", 0x03b6 },
    { "zwj", 0x200d },
    { "zwnj", 0x200c }
};

static bool operator<(const QString &entityStr, const QTextHtmlEntity &entity)
{
    return entityStr < QLatin1String(entity.name);
}

static bool operator<(const QTextHtmlEntity &entity, const QString &entityStr)
{
    return QLatin1String(entity.name) < entityStr;
}

static QChar resolveEntity(const QString &entity)
{
    const QTextHtmlEntity *start = &entities[0];
    const QTextHtmlEntity *end = &entities[MAX_ENTITY];
    const QTextHtmlEntity *e = qBinaryFind(start, end, entity);
    if (e == end)
        return QChar();
    return e->code;
}

struct _HtmlElement
{
	const char *name;
	int id;
	HtmlParserNode::DisplayMode displayMode;
};

// the displayMode value is according to the what are blocks in the piecetable, not
// what the w3c defines.
static const _HtmlElement elements[Html_NumElements]= {
    { "a",          Html_a,          HtmlParserNode::DisplayInline },
    { "address",    Html_address,    HtmlParserNode::DisplayInline },
    { "b",          Html_b,          HtmlParserNode::DisplayInline },
    { "big",        Html_big,        HtmlParserNode::DisplayInline },
    { "blockquote", Html_blockquote, HtmlParserNode::DisplayBlock },
    { "body",       Html_body,       HtmlParserNode::DisplayBlock },
    { "br",         Html_br,         HtmlParserNode::DisplayInline },
    { "center",     Html_center,     HtmlParserNode::DisplayBlock },
    { "cite",       Html_cite,       HtmlParserNode::DisplayInline },
    { "code",       Html_code,       HtmlParserNode::DisplayInline },
    { "dd",         Html_dd,         HtmlParserNode::DisplayBlock },
    { "dfn",        Html_dfn,        HtmlParserNode::DisplayInline },
    { "div",        Html_div,        HtmlParserNode::DisplayBlock },
    { "dl",         Html_dl,         HtmlParserNode::DisplayBlock },
    { "dt",         Html_dt,         HtmlParserNode::DisplayBlock },
    { "em",         Html_em,         HtmlParserNode::DisplayInline },
    { "font",       Html_font,       HtmlParserNode::DisplayInline },
    { "h1",         Html_h1,         HtmlParserNode::DisplayBlock },
    { "h2",         Html_h2,         HtmlParserNode::DisplayBlock },
    { "h3",         Html_h3,         HtmlParserNode::DisplayBlock },
    { "h4",         Html_h4,         HtmlParserNode::DisplayBlock },
    { "h5",         Html_h5,         HtmlParserNode::DisplayBlock },
    { "h6",         Html_h6,         HtmlParserNode::DisplayBlock },
    { "head",       Html_head,       HtmlParserNode::DisplayNone },
    { "hr",         Html_hr,         HtmlParserNode::DisplayBlock },
    { "html",       Html_html,       HtmlParserNode::DisplayInline },
    { "i",          Html_i,          HtmlParserNode::DisplayInline },
    { "img",        Html_img,        HtmlParserNode::DisplayInline },
    { "kbd",        Html_kbd,        HtmlParserNode::DisplayInline },
    { "li",         Html_li,         HtmlParserNode::DisplayBlock },
    { "link",       Html_link,       HtmlParserNode::DisplayNone },
    { "meta",       Html_meta,       HtmlParserNode::DisplayNone },
    { "nobr",       Html_nobr,       HtmlParserNode::DisplayInline },
    { "object",     Html_object,     HtmlParserNode::DisplayBlock },
    { "ol",         Html_ol,         HtmlParserNode::DisplayBlock },
    { "p",          Html_p,          HtmlParserNode::DisplayBlock },
    { "pre",        Html_pre,        HtmlParserNode::DisplayBlock },
    { "qt",         Html_body /*deliberate mapping*/, HtmlParserNode::DisplayBlock },
    { "s",          Html_s,          HtmlParserNode::DisplayInline },
    { "samp",       Html_samp,       HtmlParserNode::DisplayInline },
    { "small",      Html_small,      HtmlParserNode::DisplayInline },
    { "span",       Html_span,       HtmlParserNode::DisplayInline },
    { "strong",     Html_strong,     HtmlParserNode::DisplayInline },
    { "style",      Html_style,      HtmlParserNode::DisplayNone },
    { "sub",        Html_sub,        HtmlParserNode::DisplayInline },
    { "sup",        Html_sup,        HtmlParserNode::DisplayInline },
    { "table",      Html_table,      HtmlParserNode::DisplayTable },
    { "tbody",      Html_tbody,      HtmlParserNode::DisplayTable },
    { "td",         Html_td,         HtmlParserNode::DisplayBlock },
    { "tfoot",      Html_tfoot,      HtmlParserNode::DisplayTable },
    { "th",         Html_th,         HtmlParserNode::DisplayBlock },
    { "thead",      Html_thead,      HtmlParserNode::DisplayTable },
    { "title",      Html_title,      HtmlParserNode::DisplayNone },
    { "tr",         Html_tr,         HtmlParserNode::DisplayTable },
    { "tt",         Html_tt,         HtmlParserNode::DisplayInline },
    { "u",          Html_u,          HtmlParserNode::DisplayInline },
    { "ul",         Html_ul,         HtmlParserNode::DisplayBlock },
    { "var",        Html_var,        HtmlParserNode::DisplayInline },
};


static bool operator<(const QString &str, const _HtmlElement &e)
{
    return str < QLatin1String(e.name);
}

static bool operator<(const _HtmlElement &e, const QString &str)
{
    return QLatin1String(e.name) < str;
}

static const _HtmlElement *lookupElement(const QString &element)
{
    const _HtmlElement *start = &elements[0];
    const _HtmlElement *end = &elements[Html_NumElements];
    const _HtmlElement *e = qBinaryFind(start, end, element);
    if (e == end)
        return 0;
    return e;
}

int HtmlParser::lookupElement(const QString &element)
{
    const _HtmlElement *e = ::lookupElement(element);
    if (!e)
        return -1;
    return e->id;
}

// quotes newlines as "\\n"
static QString quoteNewline(const QString &s)
{
    QString n = s;
    n.replace(QLatin1Char('\n'), QLatin1String("\\n"));
    return n;
}

HtmlParserNode::HtmlParserNode()
    : parent(0), pp(0), id(-1), isBlock(false), isListItem(false), isListStart(false), isTableCell(false), isAnchor(false),
      
      hasOwnListStyle(false), 
      displayMode(HtmlParserNode::DisplayInline), 
      listStyle(ListStyleUndefined), tableBorder(0),
      tableCellRowSpan(1), tableCellColSpan(1), 
      wsm(WhiteSpaceModeUndefined)
{
}

void HtmlParser::dumpHtml()
{
    for (int i = 0; i < count(); ++i) {
        qDebug().nospace() << qPrintable(QString(depth(i)*4, QLatin1Char(' ')))
                           << qPrintable(at(i).tag) << ":"
                           << quoteNewline(at(i).text);
            ;
    }
}

HtmlParserNode *HtmlParser::createNode(int parent)
{
    HtmlParserNode *lastNode = &nodes.last();
    HtmlParserNode *newNode = 0;

    bool reuseLastNode = true;

    if (nodes.count() == 1) {
        reuseLastNode = false;
    } else if (lastNode->tag.isEmpty()) {

        if (lastNode->text.isEmpty()) {
            reuseLastNode = true;
        } else { // last node is a text node (empty tag) with some text

            if (lastNode->text == QLatin1String(" ")) {

                int lastSibling = count() - 2;
                while (lastSibling
                       && at(lastSibling).parent != lastNode->parent
                       && at(lastSibling).displayMode == HtmlParserNode::DisplayInline) {
                    lastSibling = at(lastSibling).parent;
                }

                if (at(lastSibling).displayMode == HtmlParserNode::DisplayInline) 
				{
                    reuseLastNode = false;
                } else 
				{
                    reuseLastNode = true;
                }
            } else {
                // text node with real (non-whitespace) text -> nothing to re-use
                reuseLastNode = false;
            }

        }

    } else {
        // last node had a proper tag -> nothing to re-use
        reuseLastNode = false;
    }

    if (reuseLastNode) {
        newNode = lastNode;
        newNode->tag.clear();
        newNode->text.clear();
        newNode->id = -1;
    } else {
        nodes.resize(nodes.size() + 1);
        newNode = &nodes.last();
    }

    newNode->parent = parent;
    return newNode;
}

void HtmlParser::parse(const QString &text )
{
    nodes.clear();
    nodes.resize(1);
    txt = text;
    pos = 0;
    len = txt.length();
    parse();
    //dumpHtml();
}

int HtmlParser::depth(int i) const
{
    int depth = 0;
    while (i) {
        i = at(i).parent;
        ++depth;
    }
    return depth;
}


void HtmlParser::eatSpace()
{
    while (pos < len && txt.at(pos).isSpace() && txt.at(pos) != QChar::ParagraphSeparator)
        pos++;
}

void HtmlParser::parse() {
    HtmlParserNode::WhiteSpaceMode wsm = HtmlParserNode::WhiteSpaceNormal;
    while (pos < len) {
        QChar c = txt.at(pos++);
        if (c == QLatin1Char('<')) 
		{
            parseTag();
            wsm = nodes.last().wsm;
        } else if (c == QLatin1Char('&')) 
		{
            nodes.last().text += parseEntity();
        } else 
		{
            if (c.isSpace() && c != QChar(QChar::Nbsp) && c != QChar::ParagraphSeparator) 
			{
                if (wsm == HtmlParserNode::WhiteSpacePre ) 
				{
                    if (c == QLatin1Char('\n')) 
					{
						;
                    } else if (c == QLatin1Char('\r')) 
					{
                        continue;
                    }
                } else if (wsm != HtmlParserNode::WhiteSpacePreWrap) 
				{ // non-pre mode: collapse whitespace except nbsp
                    while (pos < len && txt.at(pos).isSpace()
                           && txt.at(pos) != QChar::Nbsp)
                        pos++;
                    if (wsm == HtmlParserNode::WhiteSpaceNoWrap)
                        c = QChar::Nbsp;
                    else
                        c = QLatin1Char(' ');
                }
            }
            nodes.last().text += c;
        }
    }
	fillChildren();
}

// parses a tag after "<"
void HtmlParser::parseTag()
{
    eatSpace();

    // handle comments and other exclamation mark declarations
    if (hasPrefix(QLatin1Char('!'))) {
        parseExclamationTag();
        if (nodes.last().wsm != HtmlParserNode::WhiteSpacePre
            && nodes.last().wsm != HtmlParserNode::WhiteSpacePreWrap)
            eatSpace();
        return;
    }

    // if close tag just close
    if (hasPrefix(QLatin1Char('/'))) {
        parseCloseTag();	
        return;
    }

    int p = last();
    while (p && at(p).tag.size() == 0)
        p = at(p).parent;

    HtmlParserNode *node = createNode(p);
    const int nodeIndex = int(QVector<HtmlParserNode>::const_iterator(node) - nodes.constBegin());

    // parse tag name
    node->tag = parseWord().toLower();

    const _HtmlElement *elem = ::lookupElement(node->tag);
    if (elem) {
        node->id = elem->id;
        node->isBlock = (elem->displayMode == HtmlParserNode::DisplayBlock);
        node->displayMode = elem->displayMode;
    } else {
        node->id = -1;
    }

    node->isListItem = (node->id == Html_li);
    node->isListStart = (node->id == Html_ol || node->id == Html_ul);
    node->isTableCell = (node->id == Html_td || node->id == Html_th);

    node->attributes.clear();
    // _need_ at least one space after the tag name, otherwise there can't be attributes
    if (pos < len && txt.at(pos).isSpace())
        node->attributes = parseAttributes();

    resolveParent();
    resolveNode();

    applyAttributes(node->attributes);

    // finish tag
    bool tagClosed = false;
    while (pos < len && txt.at(pos) != QLatin1Char('>')) {
        if (txt.at(pos) == QLatin1Char('/'))
            tagClosed = true;


        pos++;
    }
    pos++;

    if (node->wsm != HtmlParserNode::WhiteSpacePre
        && node->wsm != HtmlParserNode::WhiteSpacePreWrap )
        eatSpace();

    if (node->mayNotHaveChildren() || tagClosed) {
        createNode(node->parent);
		// resolveParent(); // RISK
        resolveNode();
    }
}

// parses a tag beginning with "/"
void HtmlParser::parseCloseTag()
{
    ++pos;
    QString tag = parseWord().toLower().trimmed();
    while (pos < len) {
        QChar c = txt.at(pos++);
        if (c == QLatin1Char('>'))
            break;
    }

    // find corresponding open node
    int opener = last();
    if (opener > 0
        && at(opener - 1).tag == tag
        && at(opener - 1).mayNotHaveChildren())
        opener--;

    while (opener && at(opener).tag != tag)
        opener = at(opener).parent;

    // simply ignore the tag if we can't find
    // a corresponding open node, for broken
    // html such as <font>blah</font></font>
    if (!opener)
        return;

    createNode(at(opener).parent);
    resolveNode();
}

// parses a tag beginning with "!"
void HtmlParser::parseExclamationTag()
{
    ++pos;
    if (hasPrefix(QLatin1Char('-'),1) && hasPrefix(QLatin1Char('-'),2)) {
        pos += 3;
        // eat comments
        int end = txt.indexOf(QLatin1String("-->"), pos);
        pos = (end >= 0 ? end + 3 : len);
    } else {
        // eat internal tags
        while (pos < len) {
            QChar c = txt.at(pos++);
            if (c == QLatin1Char('>'))
                break;
        }
    }
}

// parses an entity after "&", and returns it
QString HtmlParser::parseEntity()
{
    int recover = pos;
    QString entity;
    while (pos < len) {
        QChar c = txt.at(pos++);
        if (c.isSpace() || pos - recover > 8) {
            goto error;
        }
        if (c == QLatin1Char(';'))
            break;
        entity += c;
    }
    {
        QChar resolved = resolveEntity(entity);
        if (!resolved.isNull())
            return QString(resolved);
    }
    if (entity.length() > 1 && entity.at(0) == QLatin1Char('#')) {
        entity.remove(0, 1); // removing leading #

        int base = 10;
        bool ok = false;

        if (entity.at(0).toLower() == QLatin1Char('x')) { // hex entity?
            entity.remove(0, 1);
            base = 16;
        }

        int uc = entity.toInt(&ok, base);
        if (ok) {
            if (uc == 151) // ### hack for designer manual
                uc = '-';
            QString str;
            if (uc > 0xffff) {
                // surrogate pair
                uc -= 0x10000;
                ushort high = uc/0x400 + 0xd800;
                ushort low = uc%0x400 + 0xdc00;
                str.append(QChar(high));
                str.append(QChar(low));
            } else {
                str.append(QChar(uc));
            }
            return str;
        }
    }
error:
    pos = recover;
    return QLatin1String("&");
}

// parses one word, possibly quoted, and returns it
QString HtmlParser::parseWord()
{
    QString word;
    if (hasPrefix(QLatin1Char('\"'))) { // double quotes
        ++pos;
        while (pos < len) {
            QChar c = txt.at(pos++);
            if (c == QLatin1Char('\"'))
                break;
            else if (c == QLatin1Char('&'))
                word += parseEntity();
            else
                word += c;
        }
    } else if (hasPrefix(QLatin1Char('\''))) { // single quotes
        ++pos;
        while (pos < len) {
            QChar c = txt.at(pos++);
            if (c == QLatin1Char('\''))
                break;
            else
                word += c;
        }
    } else { // normal text
        while (pos < len) {
            QChar c = txt.at(pos++);
            if (c == QLatin1Char('>')
                || (c == QLatin1Char('/') && hasPrefix(QLatin1Char('>'), 1))
                || c == QLatin1Char('<')
                || c == QLatin1Char('=')
                || c.isSpace()) {
                --pos;
                break;
            }
            if (c == QLatin1Char('&'))
                word += parseEntity();
            else
                word += c;
        }
    }
    return word;
}

// gives the new node the right parent
void HtmlParser::resolveParent()
{
    HtmlParserNode *node = &nodes.last();
    int p = node->parent;

    // permit invalid html by letting block elements be children
    // of inline elements with the exception of paragraphs:
    //
    // a new paragraph closes parent inline elements (while loop),
    // unless they themselves are children of a non-paragraph block
    // element (if statement)
    //
    // For example:
    //
    // <body><p><b>Foo<p>Bar <-- second <p> implicitly closes <b> that
    //                           belongs to the first <p>. The self-nesting
    //                           check further down prevents the second <p>
    //                           from nesting into the first one then.
    //                           so Bar is not bold.
    //
    // <body><b><p>Foo <-- Foo should be bold.
    //
    // <body><b><p>Foo<p>Bar <-- Foo and Bar should be bold.
    //
    if (node->id == Html_p) {
        while (p && !at(p).isBlock)
            p = at(p).parent;

        if (!p || at(p).id != Html_p)
            p = node->parent;
    }

    // some elements are not self nesting
    if (node->id == at(p).id
        && node->isNotSelfNesting())
        p = at(p).parent;

    // some elements are not allowed in certain contexts
    while (p && !node->allowedInContext(at(p).id)
           // ### make new styles aware of empty tags
           || at(p).mayNotHaveChildren()
       ) {
        p = at(p).parent;
    }

	if( node->isListStart && at(node->parent).isListItem )
		p = at(node->parent).parent;	// Parser hängt nested Lists irrtümlich an LI

    node->parent = p;

    // makes it easier to traverse the tree, later
	// Das funktioniert überhaupt nicht
    // nodes[p].children.append(nodes.count() - 1);
}

// sets all properties on the new node
void HtmlParser::resolveNode()
{
    HtmlParserNode *node = &nodes.last();
    const HtmlParserNode *parent = &nodes.at(node->parent);
    node->initializeProperties(parent, this);
}

bool HtmlParserNode::isNestedList(const HtmlParser *parser) const
{
    if (!isListStart)
        return false;

    int p = parent;
    while (p) {
        if (parser->at(p).isListStart)
            return true;
        p = parser->at(p).parent;
    }
    return false;
}

void HtmlParserNode::initializeProperties(const HtmlParserNode *parent, const HtmlParser *parser)
{
    // inherit properties from parent element
    isAnchor = parent->isAnchor;

    if (parent->displayMode == HtmlParserNode::DisplayNone)
        displayMode = HtmlParserNode::DisplayNone;

    // we don't paint per-row background colors, yet. so as an
    // exception inherit the background color here

    listStyle = parent->listStyle;
    anchorHref = parent->anchorHref;
    // makes no sense to inherit that property, a named anchor is a single point
    // in the document, which is set by the DocumentFragment
    //anchorName = parent->anchorName;
    wsm = parent->wsm;

    // initialize remaining properties


    // set element specific attributes
    switch (id) {
        case Html_a:
            isAnchor = true;
            for (int i = 0; i < attributes.count(); i += 2) {
                const QString key = attributes.at(i);
                if (key.compare(QLatin1String("href")) == 0 // TODO , Qt::CaseInsensitive
                    && !attributes.at(i + 1).isEmpty()) {
                }
            }

            break;
        case Html_em:
        case Html_i:
        case Html_cite:
        case Html_address:
        case Html_var:
        case Html_dfn:
            break;
        case Html_big:
            break;
        case Html_small:
            break;
        case Html_strong:
        case Html_b:
            break;
        case Html_h1:
            break;
        case Html_h2:
            break;
        case Html_h3:
            break;
        case Html_h4:
            break;
        case Html_h5:
            break;
        case Html_p:
            break;
        case Html_center:
            break;
        case Html_ul:
            listStyle = ListDisc;
            // nested lists don't have margins, except for the toplevel one
            // no left margin as we use indenting instead
            break;
        case Html_ol:
            listStyle = ListDecimal;
            // nested lists don't have margins, except for the toplevel one
            // no left margin as we use indenting instead
            break;
        case Html_code:
        case Html_tt:
        case Html_kbd:
        case Html_samp:
            break;
        case Html_br:
            text = QChar(QChar::LineSeparator);
            break;
        // ##### sub / sup
        case Html_pre:
            wsm = WhiteSpacePre;
            // <pre> uses a fixed font
            break;
        case Html_blockquote:
            break;
        case Html_dl:
            break;
        case Html_dd:
            break;
        case Html_u:
            break;
        case Html_s:
            break;
        case Html_nobr:
            wsm = WhiteSpaceNoWrap;
            break;
        case Html_th:
            break;
        case Html_td:
            break;
        case Html_sub:
            break;
        case Html_sup:
            break;
        default: break;
    }

}

static bool setIntAttribute(int *destination, const QString &value)
{
    bool ok = false;
    int val = value.toInt(&ok);
    if (ok)
        *destination = val;

    return ok;
}

static bool setFloatAttribute(qreal *destination, const QString &value)
{
    bool ok = false;
    qreal val = value.toDouble(&ok);
    if (ok)
        *destination = val;

    return ok;
}


QStringList HtmlParser::parseAttributes()
{
    QStringList attrs;

    while (pos < len) {
        eatSpace();
        if (hasPrefix(QLatin1Char('>')) || hasPrefix(QLatin1Char('/')))
            break;
        QString key = parseWord().toLower();
        QString value = QLatin1String("1");
        if (key.size() == 0)
            break;
        eatSpace();
        if (hasPrefix(QLatin1Char('='))){
            pos++;
            eatSpace();
            value = parseWord();
        }
        if (value.size() == 0)
            continue;
        attrs << key << value;
    }

    return attrs;
}

void HtmlParser::applyAttributes(const QStringList &attributes)
{
    // local state variable for qt3 textedit mode
    QString linkHref;
    QString linkType;

    if (attributes.count() % 2 == 1)
        return;

    HtmlParserNode *node = &nodes.last();

    for (int i = 0; i < attributes.count(); i += 2) {
        QString key = attributes.at(i);
        QString value = attributes.at(i + 1);

        switch (node->id) {
            case Html_font:
                break;
            case Html_ol:
            case Html_ul:
                if (key == QLatin1String("type")) {
                    node->hasOwnListStyle = true;
                    if (value == QLatin1String("1")) {
						node->listStyle = HtmlParserNode::ListDecimal;
                    } else if (value == QLatin1String("a")) {
                        node->listStyle = HtmlParserNode::ListLowerAlpha;
                    } else if (value == QLatin1String("A")) {
                        node->listStyle = HtmlParserNode::ListUpperAlpha;
                    } else {
                        value = value.toLower();
                        if (value == QLatin1String("square"))
                            node->listStyle = HtmlParserNode::ListSquare;
                        else if (value == QLatin1String("disc"))
                            node->listStyle = HtmlParserNode::ListDisc;
                        else if (value == QLatin1String("circle"))
                            node->listStyle = HtmlParserNode::ListCircle;
                    }
                }
                break;
            case Html_a:
                if (key == QLatin1String("href"))
                    node->anchorHref = value;
                else if (key == QLatin1String("name"))
                    node->anchorName = value;
                break;
            case Html_img:
                if (key == QLatin1String("src") || key == QLatin1String("source")) {
                    node->imageName = value;
                } 
                break;
            case Html_tr:
            case Html_body:
			case Html_object:
                break;
            case Html_th:
            case Html_td:
                if (key == QLatin1String("rowspan")) {
                    setIntAttribute(&node->tableCellRowSpan, value);
                } else if (key == QLatin1String("colspan")) {
                    setIntAttribute(&node->tableCellColSpan, value);
                }
                break;
            case Html_table:
                if (key == QLatin1String("border")) {
                    setFloatAttribute(&node->tableBorder, value);
                }
                break;
            case Html_meta:
                break;
            case Html_hr:
                break;
            case Html_link:
                if (key == QLatin1String("href"))
                    linkHref = value;
                else if (key == QLatin1String("type"))
                    linkType = value;
                break;
            default:
                break;
        }

    }
}

void HtmlParser::fillChildren()
{
	QStack<HtmlParserNode*> path;
	path.push( &nodes[0] );

	for( int i = 1; i < nodes.size(); i++ )
	{
		HtmlParserNode* cur = &nodes[i];
		HtmlParserNode* par = &nodes[ cur->parent ];
		if( par == path.top() )
			; // NOP, parent bereits auf stack
		else if( path.contains( par ) )
		{
			// Parent bereits im Pfad, aber nicht zuunterst. Wir müssen rauf
			while( !path.isEmpty() && path.top() != par )
				path.pop();
		}else
			path.push( par );
		path.top()->children.append( cur );
		cur->pp = path.top();
	}
}
