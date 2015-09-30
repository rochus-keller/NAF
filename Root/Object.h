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

#if !defined(AFX_OBJECT_H__358B9B2B_A90D_4A46_A0DD_B1094451ABC7__INCLUDED_)
#define AFX_OBJECT_H__358B9B2B_A90D_4A46_A0DD_B1094451ABC7__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Root/Resource.h>
#include <Root/Message.h>
#include <Root/SymbolString.h>
#include <Root/Any.h>
#include <QVector>
#include <QSet>
#include <QMap>
#include <QSharedData>

namespace Root
{
	//* Object
	//. Diese Classe enthält die quasiabstrakte Definition des generischen Message-
	//. Handlers. Grundsätzlich könnte jede Methode einer Klasse auch als Message
	//. implementiert werden. Ein Interface wird somit zur Menge aller Messages,
	//. welche von einer abgeleiteten Klasse verstanden werden.

	class Object : public Resource
	{
	public:
		/// Statische und dynamische Attribute
		/// Fields können entweder statisch in einer Klasse eingebaut sein, oder dynamisch gesetzt und gelöscht werden.
		typedef SymbolString Field;
		struct  FieldValues : public QList< QPair<Field,Any> >
		{
			void setFieldValue( Field, const Any& );
			const Any& getFieldValue( Field ) const;
			int indexOf(Field) const;
		};
		enum FieldInfo { UnknownField, ReadOnlyField, ReadWriteField, DynamicField };

		virtual FieldInfo getFieldInfo( Field ) const;

		// Any& getFieldValue( Field ) const;	// Helper, ineffizient
		bool getFieldValue( Field, Any& ) const;	// Helper, ineffizient

		virtual void setFieldValue( Field, const Any& ); 
		virtual void copyDynValuesFrom( const Object* );	// TODO
		virtual FieldValues getFieldValues() const;
		//-

		virtual const char* getClassName() const;
		//. Gebe den Namen dieser Klasse zurück. 
		virtual QByteArray  getInstanceName(bool alphasort = false) const;
		//. Gebe einen eindeutigen String zur Identifikation dieses Objekts zurück.
	protected:
		Object();
		virtual ~Object();
	private:
		FieldValues d_values; // Leer nur ein Pointer gross. QList ist billiger als QSet und QMap
	};

	//* UpdateMessage
	//. Diese Message sendet ein Subject an seine Observer, wenn immer
	//. der Zustand geändert hat. Spezifische Subjects werden diese
	//. Message ableiten, um die Art der Zustandsänderung genauer
	//. zu spezifizieren.

	class UpdateMessage : public Message
	{
	public:
		UpdateMessage( Resource* origin = 0 );
		Resource* getOrigin() const { return d_origin; }
		~UpdateMessage() {}
		void pretendOrigin( Resource* o ) { d_origin = o; }
		// Damit kann ein Mediator die Message unter eigenem Absender weitergeben.
	private:
		Ptr<Resource> d_origin; // Da in vielen Constructors Notification mit this als Origin stattfindet, Ref nicht zulässig.
	};
#define UPDATE_MESSAGE_DEF( name, type ) struct name : public Root::UpdateMessage { name( type* t ):UpdateMessage(t) {} type* sender() const { return static_cast<type*>( getOrigin() ); } };
#define UPDATE_MESSAGE_DEF2( name, type, hint ) struct name : public Root::UpdateMessage { name( type* t, hint h ):UpdateMessage(t),d_hint(h) {} type* sender() const { return static_cast<type*>( getOrigin() ); } hint d_hint; };
#define UPDATE_MESSAGE_DEF3( name, type, hint, value ) struct name : public Root::UpdateMessage { name( type* t, hint h, value v = value() ):UpdateMessage(t),d_hint(h),d_value(v) {} type* sender() const { return static_cast<type*>( getOrigin() ); } hint d_hint; value d_value; };

	//* UpdateDynValue
	//. Wird von Subject gesendet, wenn setDynValue aufgerufen wurde.

	class UpdateDynValue : public UpdateMessage
	{
	public:
		UpdateDynValue( Object* o, Object::Field f ):
			UpdateMessage( o ), d_field( f ) {}
		Object::Field getField() const { return d_field; }
	private:
		Object::Field d_field;
	};

    class SmallObject : public QSharedData
    {
    public:
        void addRef() const { ref.ref(); }
        void release() const { if( !ref.deref() ) delete this; }

        bool getFieldValue( Object::Field, Any& ) const;
		virtual void setFieldValue( Object::Field, const Any& );
		virtual void copyDynValuesFrom( const SmallObject* );
        virtual Object::FieldValues getFieldValues() const { return d_values; }
    protected:
        SmallObject() {}
        virtual ~SmallObject() {}
        SmallObject& operator=( const SmallObject& ) { return *this; }
    private:
		Object::FieldValues d_values;
    };
}

#endif // !defined(AFX_OBJECT_H__358B9B2B_A90D_4A46_A0DD_B1094451ABC7__INCLUDED_)
