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

#if !defined(AFX_SPINLABEL_H__3F9E4DE2_0CE7_4B98_AAFA_9098042BA657__INCLUDED_)
#define AFX_SPINLABEL_H__3F9E4DE2_0CE7_4B98_AAFA_9098042BA657__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/SymbolString.h>
#include <Root/Resource.h>
#include <Root/Vector.h>
#include <Spec/Units.h>
#include <set>

namespace Spec
{
	//* SpinLabel
	//. Value-Object.

	class SpinLabel  
	{
	public:
		enum State {
			Draft = 0, Assigned = 1, Finalized = 2, // Normales/Manuelles Matching/Assignment
			Wildcard = -1 // Falls das Label aus Noesy-Step entstanden ist
				   };

		SpinLabel();
		SpinLabel(Root::SymbolString t, State f = Assigned ):
			d_tag( t ), d_offset( 0 ), d_state( f ) {}
		SpinLabel(Root::SymbolString t, Root::Int16 o, State f = Assigned):
			d_tag( t ), d_offset( o ), d_state( f ) {}
		virtual ~SpinLabel();

		Root::SymbolString getTag() const { return d_tag; }
		void setTag( Root::SymbolString t ) { d_tag = t; }
		void setTag( const char* t ) { d_tag = t; }

		Root::Int16 getOff() const { return d_offset; }
		Root::Int16 getOffset() const { return d_offset; }
		void setOffset( Root::Int16 o ) { d_offset = o; }

		void setState( State s ) { d_state = s; }
		State getState() const { return (State)d_state; }

        bool isDraft() const { return ( d_state == Draft ); }
        bool isFinal() const { return ( d_state > Draft ); }
		bool isWildcard() const { return d_state == Wildcard; }
		bool isAssigned() const { return d_state == Assigned; }
		bool isFinalized() const { return d_state == Finalized; }
		void setFinal( bool f = true ) { (f)?d_state = Assigned:d_state = Draft; }
		void setDraft() { d_state = Draft; }
		void setFinalized() { d_state = Finalized; }
		void setWildcard() { d_state = Wildcard; }
		void setAssigned() { d_state = Assigned; }
		bool isNull() const { return d_tag.empty(); }

		void format( char* buffer, int size ) const;
        QByteArray format() const;
        static bool parse( const char*, SpinLabel& );
		static SpinLabel parse( const char* );
		const char* data() const;
		static const char* s_syntax;

		bool operator<(const SpinLabel & rhs) const 
			{ return d_tag < rhs.d_tag || 
				( d_tag == rhs.d_tag && d_offset < rhs.d_offset ); }	// RISK
		bool operator==( const SpinLabel& rhs ) const
			{ return d_tag == rhs.d_tag && d_offset == rhs.d_offset; }
		bool operator!=( const SpinLabel& rhs ) const 
			{ return d_tag != rhs.d_tag || d_offset != rhs.d_offset; }
		bool equals( const SpinLabel & rhs) const 
			{ return d_tag == rhs.d_tag && d_offset == rhs.d_offset && 
			  d_state == rhs.d_state; }
	private:
		Root::SymbolString d_tag;
		//. Der Zeichencode des Labels. Generalisierbar, wobei die erste
		//. Stelle links normalerweise der Atomfarbe entspricht.
		Root::Int16 d_offset;
		//. Legt fest, zu welchem System in der Kette der Spin gehört. Mit
		//. dieser Information ist eine automatische Kettenbildung möglich.
		Root::Int8 d_state; // State
		//. Die Zuweisung ist definitiv. Das Label muss daher innerhalb des
		//. zugehörigen SpinSystems eindeutig sein 
		//. (gilt nur für eigene, d.h. offset=0).

	};

	// Diese Klasse dient dazu, ein SpinLabel z.B. als Param eines Menüs zu führen (boxing)
	class SpinLabelHolder : public Root::Resource
	{
	public:
		SpinLabelHolder( const SpinLabel& l ):d_label(l) {}
		SpinLabelHolder( const char* str ) 
		{
			SpinLabel::parse( str, d_label );
		}
		SpinLabel d_label;
	protected:
		~SpinLabelHolder() {}
	};

	typedef std::set<SpinLabel> SpinLabelSet;
	typedef Root::Vector<SpinLabel> KeyLabels;
    typedef Root::FixPoint<SpinLabel> SpinLabelPoint;
    typedef QList<SpinLabelPoint> SpinLabelPoints;

	bool checkUnique( const KeyLabels&, bool ignoreNull = false );
}

#endif // !defined(AFX_SPINLABEL_H__3F9E4DE2_0CE7_4B98_AAFA_9098042BA657__INCLUDED_)
