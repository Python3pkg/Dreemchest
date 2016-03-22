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

#ifndef __DC_OpenALSound_H__
#define __DC_OpenALSound_H__

#include "OpenAL.h"
#include "../SoundSource.h"

DC_BEGIN_DREEMCHEST

namespace Sound {

    // ** class OpenALSource
    class OpenALSource : public SoundSource {
    friend class OpenALBuffer;
    public:

                                OpenALSource( void );
        virtual                 ~OpenALSource( void ) DC_DECL_OVERRIDE;

        // ** SoundSource
        virtual void            update( void ) DC_DECL_OVERRIDE;
        virtual void            setBuffer( SoundBufferPtr value ) DC_DECL_OVERRIDE;
        virtual SourceState     state( void ) const DC_DECL_OVERRIDE;
        virtual void            setState( SourceState value ) DC_DECL_OVERRIDE;
        virtual void            setVolume( f32 value ) DC_DECL_OVERRIDE;
        virtual void            setPitch( f32 value ) DC_DECL_OVERRIDE;
        virtual void            setPosition( const Vec3& value ) DC_DECL_OVERRIDE;
        virtual void            setRelative( bool value ) DC_DECL_OVERRIDE;
        virtual void            setReferenceDistance( f32 value ) DC_DECL_OVERRIDE;
        virtual void            setMaximumDistance( f32 value ) DC_DECL_OVERRIDE;
        virtual void            setRolloffFactor( f32 value ) DC_DECL_OVERRIDE;

    private:

        ALuint                  m_id;
        static u32              s_aliveCount;
    };

} // namespace Sound

DC_END_DREEMCHEST

#endif        /*    __DC_OpenALSound_H__    */