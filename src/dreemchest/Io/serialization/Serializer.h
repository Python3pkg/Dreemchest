/**************************************************************************

 The MIT License (MIT)

 Copyright (c) 2015 Dmitry Sovetov

 https://github.com/dmsovetov

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 **************************************************************************/

#ifndef __DC_Io_Serializer_H__
#define __DC_Io_Serializer_H__

#include "Storage.h"
#include "BinarySerializer.h"

DC_BEGIN_DREEMCHEST

namespace Io {
    
    template<typename T>
    class SerializableT;

	//! Base class for a field serializers.
	class Serializer : public RefCounted {
	public:

								//! Constructs field serializer.
								Serializer( const Storage::Key& key )
									: m_key( key ) {}

		virtual					~Serializer( void ) {}

		//! Writes field to a binary storage.
		virtual void			write( BinaryStorage* storage ) = 0;

		//! Reads field from a binary storage.
		virtual void			read( const BinaryStorage* storage ) = 0;

    #if DC_DEPRECATED_FEATURE
		//! Writes field to a binary storage.
		virtual void			write( KeyValueStorage* storage ) = 0;

		//! Reads field from a binary storage.
		virtual void			read( const KeyValueStorage* storage ) = 0;
    #endif

		//! Creates a field serializer with a specified type.
		template<typename T>
		static SerializerPtr	create( const Storage::Key& key, const T& value );

	protected:

		//! Field name.
		Storage::Key			m_key;
	};

	//! Generic type serializer class.
	template<typename T>
	class TypeSerializer : public Serializer {
	public:
				
								//! Constructs a TypeSerializer instance.
								TypeSerializer( const Storage::Key& key, T& value )
									: Serializer( key ), m_value( value ) {}
		virtual					~TypeSerializer( void ) {}

	protected:

		//! Reference to a serializable value.
		T&						m_value;
	};

	//! A POD field serializer.
	template<typename T>
	class PodSerializer : public TypeSerializer<T> {
	public:

								//! Constructs POD serializer instance.
								PodSerializer( const Storage::Key& key, T& value )
									: TypeSerializer<T>( key, value ) {}

		//! Writes value to a binary storage.
		virtual void			write( BinaryStorage* storage );

		//! Reads value from a binary storage.
		virtual void			read( const BinaryStorage* storage );

    #if DC_DEPRECATED_FEATURE
		//! Writes value to a key-value storage.
		virtual void			write( KeyValueStorage* storage );

		//! Reads value from a key-value storage.
		virtual void			read( const KeyValueStorage* storage );
    #endif
	};

	//! Numeric field serializer.
	template<typename T>
	class NumericSerializer : public PodSerializer<T> {
	public:

								//! Constructs NumericSerializer instance.
								NumericSerializer( const Storage::Key& key, T& value )
									: PodSerializer<T>( key, value ) {}

    #if DC_DEPRECATED_FEATURE
		//! Reads numeric value from a key-value storage.
		virtual void			read( const KeyValueStorage* storage );
    #endif
	};

	//! Object serializer type
	template<typename T>
	class ObjectSerializer : public TypeSerializer<T> {
	public:

								//! Constructs object serializer instance.
								ObjectSerializer( const Storage::Key& key, T& value )
									: TypeSerializer<T>( key, value ) {}

		//! Writes object to a binary storage.
		virtual void			write( BinaryStorage* ar ) DC_DECL_OVERRIDE;

		//! Reads object from a binary storage.
		virtual void			read( const BinaryStorage* ar ) DC_DECL_OVERRIDE;

    #if DC_DEPRECATED_FEATURE
		//! Writes object to a key-value storage.
		virtual void			write( KeyValueStorage* storage ) DC_DECL_OVERRIDE;

		//! Reads object from a key-value storage.
		virtual void			read( const KeyValueStorage* storage ) DC_DECL_OVERRIDE;
    #endif
	};

	//! String serializer type.
	class StringSerializer : public TypeSerializer<String> {
	public:

								//! Constructs string serializer instance.
								StringSerializer( const Storage::Key& key, String& value )
									: TypeSerializer( key, value ) {}

		//! Writes string to a binary storage.
		virtual void			write( BinaryStorage* ar ) DC_DECL_OVERRIDE;

		//! Reads string from binary storage.
		virtual void			read( const BinaryStorage* ar ) DC_DECL_OVERRIDE;

    #if DC_DEPRECATED_FEATURE
		//! Writes string to a key-value storage.
		virtual void			write( KeyValueStorage* storage ) DC_DECL_OVERRIDE;

		//! Reads string from a key-value storage.
		virtual void			read( const KeyValueStorage* storage ) DC_DECL_OVERRIDE;
    #endif
	};

	//! Collection serializer type.
	template<typename TCollection, typename TElement>
	struct CollectionSerializer : public TypeSerializer<TCollection>
	{
								//! Constructs CollectionSerializer type.
								CollectionSerializer( const Storage::Key& key, TCollection& value )
									: TypeSerializer<TCollection>( key, value ) {}

		//! Writes collection to a binary storage.
		virtual void			write( BinaryStorage* ar ) DC_DECL_OVERRIDE;

		//! Reads collection from a binary storage.
		virtual void			read( const BinaryStorage* ar ) DC_DECL_OVERRIDE;

    #if DC_DEPRECATED_FEATURE
		//! Writes collection to a key-value storage.
		virtual void			write( KeyValueStorage* storage ) DC_DECL_OVERRIDE;

		//! Reads collection from a key-value storage.
		virtual void			read( const KeyValueStorage* storage ) DC_DECL_OVERRIDE;
    #endif
	};

	//! Collection of serializable objects.
	template<typename TCollection, typename TElement>
	struct ObjectCollectionSerializer : public TypeSerializer<TCollection>
	{
								//! Constructs ObjectCollectionSerializer type.
								ObjectCollectionSerializer( const Storage::Key& key, TCollection& value )
									: TypeSerializer<TCollection>( key, value ) {}

		//! Writes collection to a binary storage.
		virtual void			write( BinaryStorage* ar ) DC_DECL_OVERRIDE;

		//! Reads collection from a binary storage.
		virtual void			read( const BinaryStorage* ar ) DC_DECL_OVERRIDE;

    #if DC_DEPRECATED_FEATURE
		//! Writes collection to a key-value storage.
		virtual void			write( KeyValueStorage* storage ) DC_DECL_OVERRIDE;

		//! Reads collection from a key-value storage.
		virtual void			read( const KeyValueStorage* storage ) DC_DECL_OVERRIDE;
    #endif
	};

	//! Value serialization type.
	template <class T, class Enable = void>
	struct SerializerType
	{
		typedef PodSerializer<T> Type;
	};

	//! Number serializer.
	template <class T>
	struct SerializerType< T, typename std::enable_if<std::is_arithmetic<T>::value>::type >
	{
		typedef NumericSerializer<T> Type;
	};

	//! Object serializer type.
	template <class T>
	struct SerializerType< T, typename std::enable_if<std::is_base_of<SerializableT<T>, T>::value>::type >
	{
		typedef ObjectSerializer<T> Type;
	};

	//! Collection serializer type.
	template <class T>
	struct SerializerType< Array<T> >
	{
		typedef CollectionSerializer< Array<T>, T > Type;
	};

    //! Object collection serializer type.
    template <class T>
    struct SerializerType< List< StrongPtr<T> >, typename std::enable_if<std::is_base_of<Serializable, T>::value>::type >
    {
        typedef ObjectCollectionSerializer< List< StrongPtr<T> >, T > Type;
    };

	//! String serializer type.
	template <>
	struct SerializerType< String >
	{
		typedef StringSerializer Type;
	};

	// ** Serializer::create
	template<typename T>
	StrongPtr<Serializer> Serializer::create( const Storage::Key& key, const T& value )
	{
		return StrongPtr<Serializer>( new typename SerializerType<T>::Type( key, const_cast<T&>( value ) ) );
	}

	// ----------------------------------- PodSerializer ------------------------------------//

	// ** PodSerializer::write
	template<typename T>
	inline void PodSerializer<T>::write( BinaryStorage* storage )
	{
        storage->write( this->m_value );
	}

	// ** PodSerializer::read
	template<typename T>
	inline void PodSerializer<T>::read( const BinaryStorage* storage )
	{
		storage->read( this->m_value );
	}

#if 0
	// ** PodSerializer::write
	template<typename T>
	inline void PodSerializer<T>::write( KeyValueStorage* storage )
	{
		storage->write( this->m_key, this->m_value );
	}

	// ** PodSerializer::read
	template<typename T>
	inline void PodSerializer<T>::read( const KeyValueStorage* storage )
	{
		this->m_value = storage->read( this->m_key ).template as<T>();
	}
#endif

	// --------------------------------- NumericSerializer ----------------------------------//

#if 0
	// ** NumericSerializer::read
	template<typename T>
	inline void NumericSerializer<T>::read( const KeyValueStorage* storage )
	{
		Variant value = storage->read( this->m_key );

		if( value.type() == Variant::NumberValue ) {
			this->m_value = static_cast<T>( value.toDouble() );
		}
	}
#endif

	// -----------------------------------StringSerializer ----------------------------------//

	// ** StringSerializer::write
	inline void StringSerializer::write( BinaryStorage* storage )
	{
		storage->write( m_value.length() );

		if( m_value.length() ) {
			storage->write( m_value.c_str(), m_value.length() );
		}
	}

	// ** StringSerializer::read
	inline void StringSerializer::read( const BinaryStorage* storage )
	{
		s32 length = 0;
		storage->read( length );

		if( length ) {
			m_value.resize( length );
			storage->read( &m_value[0], length );
		}
	}

#if 0
	// ** StringSerializer::write
	inline void StringSerializer::write( KeyValueStorage* storage )
	{
		storage->write( m_key, m_value );
	}

	// ** StringSerializer::read
	inline void StringSerializer::read( const KeyValueStorage* storage )
	{
		m_value = storage->read( m_key ).toString();
	}
#endif

	// -----------------------------------ObjectSerializer ------------------------------------//

	// ** ObjectSerializer::write
	template<typename T>
	inline void ObjectSerializer<T>::write( BinaryStorage* storage )
	{
		this->m_value.write( storage );
	}

	// ** ObjectSerializer::read
	template<typename T>
	inline void ObjectSerializer<T>::read( const BinaryStorage* storage )
	{
		this->m_value.read( storage );
	}

#if 0
	// ** ObjectSerializer::write
	template<typename T>
	inline void ObjectSerializer<T>::write( KeyValueStorage* storage )
	{
		KeyValueStoragePtr object = storage->object( this->m_key );
		DC_BREAK_IF( object == KeyValueStoragePtr() );
		this->m_value.write( object.get() );
	}

	// ** ObjectSerializer::read
	template<typename T>
	inline void ObjectSerializer<T>::read( const KeyValueStorage* storage )
	{
		KeyValueStoragePtr object = storage->object( this->m_key );
		DC_BREAK_IF( object == KeyValueStoragePtr() );
		this->m_value.read( object.get() );
	}
#endif

	// -----------------------------------CollectionSerializer ------------------------------------//

	// ** CollectionSerializer::write
	template<typename TCollection, typename TElement>
	inline void CollectionSerializer<TCollection, TElement>::write( BinaryStorage* storage )
	{
		storage->write( ( u32 )this->m_value.size() );

		for( typename TCollection::iterator i = this->m_value.begin(), end = this->m_value.end(); i != end; ++i ) {
			Serializer::create( NULL, *i )->write( storage );
		}
	}

	// ** CollectionSerializer::read
	template<typename TCollection, typename TElement>
	inline void CollectionSerializer<TCollection, TElement>::read( const BinaryStorage* storage )
	{
		u32 length = 0;

		storage->read( length );
		
		TElement element;
		SerializerPtr serializer = Serializer::create( NULL, element );

		for( u32 i = 0; i < length; i++ ) {
			serializer->read( storage );
			this->m_value.push_back( element );
		}
	}

#if 0
	// ** CollectionSerializer::write
	template<typename TCollection, typename TElement>
	inline void CollectionSerializer<TCollection, TElement>::write( KeyValueStorage* storage )
	{
		KeyValueStoragePtr array = storage->array( this->m_key );
		DC_BREAK_IF( array == KeyValueStoragePtr() );
		
		s32 index = 0;

		for( typename TCollection::iterator i = this->m_value.begin(), end = this->m_value.end(); i != end; ++i ) {
			Serializer::create( index++, *i )->write( array.get() );
		}
	}

	// ** CollectionSerializer::read
	template<typename TCollection, typename TElement>
	inline void CollectionSerializer<TCollection, TElement>::read( const KeyValueStorage* storage )
	{
		KeyValueStoragePtr array = storage->array( this->m_key );
		DC_BREAK_IF( array == KeyValueStoragePtr() );

		for( u32 i = 0; i < array->size(); i++ ) {
			TElement element;
			SerializerPtr serializer = Serializer::create( i, element );
			serializer->read( array.get() );
			this->m_value.push_back( element );
		}
	}
#endif

	// ----------------------------------- ObjectCollectionSerializer ------------------------------------//

	// ** ObjectCollectionSerializer::write
	template<typename TCollection, typename TElement>
	inline void ObjectCollectionSerializer<TCollection, TElement>::write( BinaryStorage* storage )
	{
		storage->write( ( u32 )this->m_value.size() );

		for( typename TCollection::iterator i = this->m_value.begin(), end = this->m_value.end(); i != end; ++i ) {
			BinarySerializer::write( storage, i->get() );
		}
	}

	// ** ObjectCollectionSerializer::read
	template<typename TCollection, typename TElement>
	inline void ObjectCollectionSerializer<TCollection, TElement>::read( const BinaryStorage* storage )
	{
		u32 length = 0;

		storage->read( length );

		for( u32 i = 0; i < length; i++ ) {
			Serializable* data = NULL;
			BinarySerializer::read( storage, &data );

			this->m_value.push_back( castTo<TElement>( data ) );
		}
	}

#if 0
	// ** ObjectCollectionSerializer::write
	template<typename TCollection, typename TElement>
	inline void ObjectCollectionSerializer<TCollection, TElement>::write( KeyValueStorage* storage )
	{
		DC_BREAK;
		KeyValueStoragePtr array = storage->array( this->m_key );
		DC_BREAK_IF( array == KeyValueStoragePtr() );

		s32 index = 0;

		for( typename TCollection::iterator i = this->m_value.begin(), end = this->m_value.end(); i != end; ++i ) {
			TElement*		   data   = i->get();
			KeyValueStoragePtr item   = array->object( index++ );
			KeyValueStoragePtr object = item->object( data->typeName() );
			data->write( object.get() );
		}
	}

	// ** ObjectCollectionSerializer::read
	template<typename TCollection, typename TElement>
	inline void ObjectCollectionSerializer<TCollection, TElement>::read( const KeyValueStorage* storage )
	{
		DC_BREAK;
		//KeyValueStoragePtr array = storage->array( this->m_key );
		//DC_BREAK_IF( array == KeyValueStoragePtr() );

		//for( u32 i = 0; i < array->size(); i++ ) {
		//	TElement element;
		//	SerializerPtr serializer = Serializer::create( i, element );
		//	serializer->read( array.get() );
		//	this->m_value.push_back( element );
		//}
	}
#endif

} // namespace Io

DC_END_DREEMCHEST

#endif	/*	!__DC_Io_Serializer_H__	*/
