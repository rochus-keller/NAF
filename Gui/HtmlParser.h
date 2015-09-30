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

#ifndef __TEXT_HTMLPARSER
#define __TEXT_HTMLPARSER


#include <QVector>
#include <QString>
#include <QStringList>

namespace Gui
{
	enum HTMLElements {
		Html_qt,
		Html_body,
		Html_object,

		Html_a,
		Html_em,
		Html_i,
		Html_big,
		Html_small,
		Html_strong,
		Html_b,
		Html_cite,
		Html_address,
		Html_var,
		Html_dfn,

		Html_h1,
		Html_h2,
		Html_h3,
		Html_h4,
		Html_h5,
		Html_h6,
		Html_p,
		Html_center,

		Html_font,

		Html_ul,
		Html_ol,
		Html_li,

		Html_code,
		Html_tt,
		Html_kbd,
		Html_samp,

		Html_img,
		Html_br,
		Html_hr,

		Html_sub,
		Html_sup,

		Html_pre,
		Html_blockquote,
		Html_head,
		Html_div,
		Html_span,
		Html_dl,
		Html_dt,
		Html_dd,
		Html_u,
		Html_s,
		Html_nobr,

		// tables
		Html_table,
		Html_tr,
		Html_td,
		Html_th,
		Html_thead,
		Html_tbody,
		Html_tfoot,

		// misc...
		Html_style,
		Html_title,
		Html_meta,
		Html_link,
		Html_html,

		Html_NumElements
	};

	class HtmlParser;

	struct HtmlParserNode 
	{
		enum DisplayMode 
		{ 
			DisplayBlock, DisplayInline, DisplayTable, DisplayNone 
		};

		enum WhiteSpaceMode 
		{
			WhiteSpaceNormal,
			WhiteSpacePre,
			WhiteSpaceNoWrap,
			WhiteSpacePreWrap,
			WhiteSpaceModeUndefined = -1
		} wsm;

		HtmlParserNode();
		QString tag;
		QString text;
		QStringList attributes;
		int parent;
		QVector<HtmlParserNode*> children;
		int id;

		uint isBlock : 1;
		uint isListItem : 1;
		uint isListStart : 1;
		uint isTableCell : 1;
		uint isAnchor : 1;
		uint hasOwnListStyle : 1;
		uint displayMode : 3; // HtmlElement::DisplayMode
		enum ListStyle 
		{
			ListDisc = -1,
			ListCircle = -2,
			ListSquare = -3,
			ListDecimal = -4,
			ListLowerAlpha = -5,
			ListUpperAlpha = -6,
			ListStyleUndefined = 0
		} listStyle; // OK
		QString anchorHref;
		QString anchorName;
		QString imageName;
		qreal tableBorder; // OK
		int tableCellRowSpan;
		int tableCellColSpan;
		HtmlParserNode* pp;

		inline bool isNotSelfNesting() const
		{ return id == Html_p || id == Html_li; }

		inline bool allowedInContext(int parentId) const
		{
			switch (id) {
				case Html_dd: return (parentId == Html_dt || parentId == Html_dl);
				case Html_dt: return (parentId == Html_dl);
				case Html_tr: return (parentId == Html_table
									  || parentId == Html_thead
									  || parentId == Html_tbody
									  || parentId == Html_tfoot
									 );
				case Html_th:
				case Html_td: return (parentId == Html_tr);
				case Html_thead:
				case Html_tbody:
				case Html_tfoot: return (parentId == Html_table);
				default: break;
			}
			return true;
		}

		inline bool mayNotHaveChildren() const
		{ return id == Html_img || id == Html_hr || id == Html_br; }

		void initializeProperties(const HtmlParserNode *parent, const HtmlParser *parser);

		bool isNestedList(const HtmlParser *parser) const;

		friend class HtmlParser;
	};

	class HtmlParser
	{
	public:
		inline const HtmlParserNode &at(int i) const { return nodes.at(i); }
		inline HtmlParserNode &operator[](int i) { return nodes[i]; }
		inline int count() const { return nodes.count(); }
		inline int last() const { return nodes.count()-1; }
		int depth(int i) const;

		void dumpHtml();

		void parse(const QString &text );
		const HtmlParserNode* getRoot() const { return (nodes.size()>0)?&nodes[0]:0; }

		static int lookupElement(const QString &element);
	protected:
		HtmlParserNode *createNode(int parent);
		QVector<HtmlParserNode> nodes;
		QString txt;
		int pos, len;

		void parse();
		void parseTag();
		void parseCloseTag();
		void parseExclamationTag();
		QString parseEntity();
		QString parseWord();
		void resolveParent();
		void resolveNode();
		QStringList parseAttributes();
		void applyAttributes(const QStringList &attributes);
		void eatSpace();
		inline bool hasPrefix(QChar c, int lookahead = 0) const
			{return pos + lookahead < len && txt.at(pos) == c; }
		void fillChildren();
	};
}
#endif // __TEXT_HTMLPARSER
