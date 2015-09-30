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

#if !defined(AFX_VECTOR_H__128EAD00_B502_11D5_8DB1_00D00918E99C__INCLUDED_)
#define AFX_VECTOR_H__128EAD00_B502_11D5_8DB1_00D00918E99C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Root/Exception.h>

#include <deque>
#include <set>
#include <map>
#include <QVector>
#include <QList>
#include <QPair>

namespace Root
{
	template<class T>
	class Vector : public QVector<T>
	{
	public:
		typedef QVector<T> Parent;
		typedef Vector<T> Me;
		typedef typename Parent::size_type size_type;
		typedef typename Parent::reference reference;
		typedef typename Parent::const_reference const_reference;

		Vector() {}
		Vector( const Me& v ):Parent( v ) {}
		explicit Vector(size_type n, const T& v = T() ):Parent(n,v) {}

        void assign( size_type i, const T& val ) { QVector<T>::fill( val, i ); }

	};

	template<class T >
	class Deque : public QList< T >
	{
	public:
		typedef QList<T > Parent;
		typedef typename Parent::size_type size_type;
		typedef typename Parent::reference reference;
		typedef typename Parent::const_reference const_reference;

		Deque() {}
		Deque( const Deque<T>& v ):Parent( v ) {}

	};
}

#endif // !defined(AFX_VECTOR_H__128EAD00_B502_11D5_8DB1_00D00918E99C__INCLUDED_)
