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

#ifndef		__DC_Io_Serializable_H__
#define		__DC_Io_Serializable_H__

#include	"../Io.h"

//! Macro definition for serializer fields declaration
#define IoBeginSerializer                                                   \
    virtual io::detail::FieldSerializers fieldSerializers( void ) const {   \
        io::detail::FieldSerializers result;

//! Macro definition for serializer fields declaration with a superclass.
#define IoBeginSerializerSuper( T )											\
    virtual io::detail::FieldSerializers fieldSerializers( void ) const {   \
        io::detail::FieldSerializers result = T::fieldSerializers();

//! Macro definition for adding a serializable field.
#define IoField( field )                            \
    result.push_back( io::detail::createFieldSerializer( #field, field ) );

//! Macro definition for adding a serializable array.
#define IoArray( field )                            \
    result.push_back( io::detail::createArraySerializer( #field, field ) );

//! Macro definition for serializer fields declaration
#define IoEndSerializer                             \
        return result;                              \
    }

//! Macro definition for overriding field serializers
#define IoBeginFieldSerializer( T )                                                 \
    class T##FieldSerializer : public io::detail::FieldSerializer {                 \
    public:                                                                         \
                    T##FieldSerializer( CString name, const T& field )              \
                        : m_name( name ), m_pointer( const_cast<T*>( &field ) ) {}  \
        CString     m_name;                                                         \
        T*          m_pointer;

#define IoWriteField( ... )                                             \
    virtual void write( io::Storage& storage ) const {					\
        storage.pushWrite( m_name );                                    \
        __VA_ARGS__                                                     \
        storage.pop();                                                  \
    }

#define IoReadField( ... )                                              \
    virtual void read( const io::Storage& storage ) {					\
        io::StorageState state( m_name );								\
        storage.pushRead( state );                                      \
        __VA_ARGS__                                                     \
        storage.pop();                                                  \
    }

#define IoEndFieldSerializer( T )                                                                           \
    };                                                                                                      \
    template<>                                                                                              \
    io::detail::FieldSerializerPtr io::detail::createFieldSerializer<T>( CString name, const T& field ) {   \
        return io::detail::FieldSerializerPtr( new T##FieldSerializer( name, field ) );                     \
    }

DC_BEGIN_DREEMCHEST

namespace io {

    namespace detail {

        //! Field serializer
        class FieldSerializer : public RefCounted {
        public:

            virtual         ~FieldSerializer( void ) {}

            //! Writes field data to a storage.
            virtual void    write( Storage& storage ) const = 0;

            //! Reads field data from a storage.
            virtual void    read( const Storage& storage )  = 0;
        };

        //! Serializable field info
        template<typename T>
        class TypedFieldSerializer : public FieldSerializer {
        public:

                            //! Constructs a TypedFieldSerializer instance.
                            TypedFieldSerializer( CString name, const T& field )
                                : m_name( name ), m_pointer( const_cast<T*>( &field ) ) {}

            //! Writes field data to a storage.
            virtual void    write( Storage& storage ) const;

            //! Reads field data from a storage.
            virtual void    read( const Storage& storage );

        protected:

            CString         m_name;     //!< Field name.
            T*              m_pointer;  //!< Field data pointer.
        };

        // ** TypedFieldSerializer::write
        template<typename T>
        void TypedFieldSerializer<T>::write( Storage& storage ) const
        {
            storage.write( m_name, *m_pointer );
        }

        // ** TypedFieldSerializer::read
        template<typename T>
        void TypedFieldSerializer<T>::read( const Storage& storage )
        {
            storage.read( m_name, *m_pointer );
        }

        //! Serializable array info
        template<typename T>
        class TypedArraySerializer : public FieldSerializer {
        public:

                            //! Constructs a TypedArraySerializer instance.
                            TypedArraySerializer( CString name, const Array<T>& field )
                                : m_name( name ), m_pointer( const_cast<Array<T>*>( &field ) ) {}

            //! Writes field data to a storage.
            virtual void    write( Storage& storage ) const;

            //! Reads field data from a storage.
            virtual void    read( const Storage& storage );

        private:

            CString         m_name;     //!< Field name.
            Array<T>*       m_pointer;  //!< Field data pointer.
        };

        // ** TypedArraySerializer::write
        template<typename T>
        void TypedArraySerializer<T>::write( Storage& storage ) const
        {
            storage.write( m_name, *m_pointer );
        }

        // ** TypedArraySerializer::read
        template<typename T>
        void TypedArraySerializer<T>::read( const Storage& storage )
        {
            storage.read( m_name, *m_pointer );
        }

        //! Allocates a TypedFieldSerializer instance.
        template<typename T>
        FieldSerializerPtr createFieldSerializer( CString name, const T& field )
        {
            return FieldSerializerPtr( new TypedFieldSerializer<T>( name, field ) );
        }

        template<typename T>
        FieldSerializerPtr createArraySerializer( CString name, const Array<T>& field )
        {
            return FieldSerializerPtr( new TypedArraySerializer<T>( name, field ) );
        }

    } // namespace detail

    //! Base class for all serializable data structs.
    class Serializable : public RefCounted {
    public:

		ClassEnableCloning( Serializable )
        ClassEnableTypeInfo( Serializable )

        //! Reads data from a storage.
        void                                read( const Storage& storage, CString key = NULL );

        //! Writes data to a storage.
        void                                write( Storage& storage, CString key = NULL ) const;

		//! Writes data to byte buffer.
		ByteBufferPtr						writeToByteBuffer( void ) const;

		//! Reads data from a byte buffer.
		void								readFromByteBuffer( const ByteBufferPtr& buffer );

		//! Reads data from array of bytes.
		void								readFromBytes( const Array<u8>& bytes );

		//! Reads a serializable with a specified type from an array of bytes.
		template<typename T>
		static T readFromBytes( const Array<u8>& bytes )
		{
			T result;
			result.readFromBytes( bytes );
			return result;
		}

    protected:

        //! Returns an array of field serializers.
        virtual detail::FieldSerializers    fieldSerializers( void ) const;
    };

	//! A template class for declaring serializable types.
	template<typename T>
	class SerializableType : public Serializable {
	public:

		//! Returns true if the specified type matches this type.
		virtual bool is( const TypeId& id ) const { return id == TypeInfo<T>::id(); }

		//! Clones this instance.
		virtual Serializable* clone( void ) const { return new T; }

		//! Returns a type id.
		virtual TypeId  typeId( void ) const  { return TypeInfo<T>::id(); }

		//! Returns a type index for this component.
		virtual TypeIdx	typeIndex( void ) const { return TypeIndex<T>::idx(); }

		//! Returns a type name.
		virtual CString typeName( void ) const { return TypeInfo<T>::name(); }

		//! Returns a type index.
		static  TypeIdx typeIdx( void ) { return TypeIndex<T>::idx(); }
	};


} // namespace io

DC_END_DREEMCHEST

#endif		/*	!__DC_Io_Serializable_H__	*/