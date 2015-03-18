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

#ifndef __DC_SoundEngine_H__
#define __DC_SoundEngine_H__

#include    "../Sound.h"

DC_BEGIN_DREEMCHEST

namespace sound {

    // ** class SoundEngine
    //! Hardware sound engine, used for creating hardware sound sources and buffers.
    class SoundEngine {
    public:

                                SoundEngine( void );
        virtual                 ~SoundEngine( void );

        //! Creates a new hardware sound source.
        virtual SoundSource*    createSource( void );

        //! Creates a new hardware sound buffer.
        /*!
         \param decoder Sound decoder used to read PCM sound samples from input stream.
         \param chunks
         */
        virtual SoundBuffer*    createBuffer( SoundDecoder* decoder, u32 chunks );

        //! Creates a sound decoder with a given input stream and file format.
        SoundDecoder*           createSoundDecoder( ISoundStream* stream, SoundFormat format = SoundFormatUnknown );

    private:

        //! Creates a sound decoder instance for a given format.
        SoundDecoder*           createSoundDecoder( SoundFormat format ) const;

        //! Creates a sound decoder with a given input stream and file format.
        SoundDecoder*           createSoundDecoderWithFormat( ISoundStream* stream, SoundFormat format );
    };
    
} // namespace sound
    
DC_END_DREEMCHEST

#endif        /*    __DC_SoundEngine_H__    */