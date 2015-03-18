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

#include    "SoundEngine.h"

#include    "../Decoders/WavSoundDecoder.h"

#ifdef HAVE_VORBIS
    #include "../Decoders/OggSoundDecoder.h"
#endif

DC_BEGIN_DREEMCHEST

namespace sound {

// ** SoundEngine::SoundEngine
SoundEngine::SoundEngine( void )
{

}

SoundEngine::~SoundEngine( void )
{

}

// ** SoundEngine::createSource
SoundSource* SoundEngine::createSource( void )
{
    return NULL;
}

// ** SoundEngine::createBuffer
SoundBuffer* SoundEngine::createBuffer( SoundDecoder* decoder, u32 chunks )
{
    return NULL;
}

// ** SoundEngine::createSoundDecoder
SoundDecoder* SoundEngine::createSoundDecoder( SoundFormat format ) const
{
    switch( format ) {
    case SoundFormatUnknown:    log::warn( "SoundEngine::createSoundDecoder : unknown sound format\n" );
                                return NULL;

    case SoundFormatWav:        return DC_NEW WavSoundDecoder;
    case SoundFormatMp3:
                            #ifdef HAVE_MP3
                                return DC_NEW Mp3SoundDecoder;
                            #else
                                log::warn( "SoundEngine::createSoundDecoder : MP3 sound decoder is not supported\n" );
                            #endif
                            break;
    case SoundFormatOgg:
                            #ifdef HAVE_VORBIS
                                return DC_NEW OggSoundDecoder;
                            #else
                                log::warn( "SoundEngine::createSoundDecoder : Vorbis sound decoder is not supported\n" );
                            #endif
                            break;
    }

    return NULL;
}

// ** SoundEngine::createSoundDecoderWithFormat
SoundDecoder* SoundEngine::createSoundDecoderWithFormat( ISoundStream* stream, SoundFormat format )
{
    DC_BREAK_IF( stream == NULL );

    SoundDecoder* soundDecoder = createSoundDecoder( format );

    if( !soundDecoder ) {
        log::error( "OpenAL::createSoundDecoder : failed to open file\n" );
        return NULL;
    }

    if( soundDecoder->open( stream ) ) {
        return soundDecoder;
    }

    log::error( "OpenAL::createSoundDecoder : unknown file format\n" );
    return NULL;
}

// ** SoundEngine::createSoundDecoder
SoundDecoder* SoundEngine::createSoundDecoder( ISoundStream* stream, SoundFormat format )
{
    return createSoundDecoderWithFormat( stream, format );
}

} // namespace sound

DC_END_DREEMCHEST