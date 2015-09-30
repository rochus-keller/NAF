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

#if !defined(AFX_FRAGMENTER_H__F4ABEC62_AC1B_11D5_8DB1_00D00918E99C__INCLUDED_)
#define AFX_FRAGMENTER_H__F4ABEC62_AC1B_11D5_8DB1_00D00918E99C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Root/Subject.h>
#include <Spec/SpinSystem.h>
#include <Root/Vector.h>
#include <Root/Vector.h>
#include <Root/Vector.h>

namespace Spec
{
	using Root::Index;

	class Fragmenter : public Root::Subject
	{
	public:
		typedef Root::Deque< Root::Ref<SpinSystem> > Fragment;
		typedef std::map<Index,Fragment> FragmentMap;

		bool writeMapperFile( const char* path, Spectrum* = 0,
			Root::SymbolString CA = "CA", Root::SymbolString CB = "CB" ) const;
		void fillFragment( Fragment&, SpinSystem* );
		void rebuildAll();
		const FragmentMap& getFragments() const { return d_frags; }
		SpinBase* getBase() const { return d_base; }
		void setMinLen( Index );
		Index getMinLen() const { return d_minLen; }
		bool isOn() const { return d_on; }
		void setOn( bool on );

		Fragmenter(SpinBase*, Index minLen = 2);

		class Update : public Root::UpdateMessage
		{
		public:
			enum Type { Add, Remove, Change, All };

			Update( Root::Subject* m, Type t, Index frag = -1 ):
				Root::UpdateMessage( m ), d_type( t ), d_frag( frag ) {}

			Type getType() const { return d_type; }
			Index getFrag() const { return d_frag; }
		private:
			Type d_type;
			Index d_frag;
		};
	protected:
		void notifyRemove( int frag );
		void notifyChange( int frag );
		void notifyAdd( int frag );
		virtual ~Fragmenter();
		void handle(Root::Message&);
	private:
		void removeFragment( Index );
		void dump( int i, const char* title = "" );
		bool addFragment( const Fragment& );
		void handleUnlink( Index pred, Index succ );
		void handleLink( Index pred, Index succ );
		void handleDelete( Index );
		void dump();

		Root::Ref<SpinBase> d_base;
		FragmentMap d_frags;
		//. Menge aller erkannten Fragmente länger als d_minLen;
		typedef std::map<SpinSystem*,Index> StripToFrag;
		StripToFrag d_stripToFrag;
		//. Map von StripId auf Index in d_frags.
		Index d_minLen;
		//. Minimale Anzahl verketteter Strips, um als Fragment erkannt zu werden.
		bool d_on;
	};

}

#endif // !defined(AFX_FRAGMENTER_H__F4ABEC62_AC1B_11D5_8DB1_00D00918E99C__INCLUDED_)
