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

#ifndef __DC_Io_BinaryStorage_H__
#define __DC_Io_BinaryStorage_H__

#include "Storage.h"
#include "../streams/Stream.h"

DC_BEGIN_DREEMCHEST

namespace io {

    //! A binary storage interface to use in serialization
    class BinaryStorage : public Storage {
    public:

                        // Constructs BinaryStorage instance.
                        BinaryStorage( const StreamPtr& stream )
                            : m_stream( stream ) {}

                        //! Returns true if this storage is valid.
                        operator bool() const { return m_stream != NULL; }

        void            write( CString key, const String& value );

        void            write( CString key, const u8& value );

        void            write( CString key, const u16& value );

        void            write( CString key, const u32& value );

        void            write( CString key, const s32& value );

        void            write( CString key, const f32& value );

        void            write( CString key, const f64& value );

        void            write( CString key, const Serializable& value );

        void            read( CString key, String& value ) const;

        void            read( CString key, u8& value ) const;

        void            read( CString key, u16& value ) const;

        void            read( CString key, u32& value ) const;

        void            read( CString key, s32& value ) const;

        void            read( CString key, f32& value ) const;

        void            read( CString key, f64& value ) const;

        void            read( CString key, Serializable& value ) const;

        virtual void    startWritingItem( int index );

        virtual void    startWritingArray( CString key, u32 size );

        virtual void    endWritingArray( CString key );

        virtual u32     startReadingArray( CString key ) const;

        virtual void    endReadingArray( CString key ) const;
        
    private:
        
        //! Binary stream.
        StreamPtr       m_stream;
    };

} // namespace io

DC_END_DREEMCHEST

#endif  /*  !defined(__DC_Io_BinaryStorage_H__) */
