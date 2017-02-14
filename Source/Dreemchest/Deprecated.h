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

#ifndef DEPRECATED_H
#define DEPRECATED_H

#define DEV_DEPRECATED_SCENE_RENDERER           (0)
#define DEV_DEPRECATED_SCENE_SERIALIZATION      (1)
#define DEV_RENDERER_SOFTWARE_CBUFFERS          (1)
#define DEV_RENDERER_SKIP_DRAW_CALLS            (0)
#define DEV_CSM_BOUNDING_SPHERES                (1)
#define DEV_RENDERER_UNIFORM_CACHING            (1)
#define DEV_RENDERER_PROGRAM_CACHING            (1)
#define DEV_RENDERER_INPUT_LAYOUT_CACHING       (1)
#define DEV_RENDERER_STATE_CACHING              (1)
#define DEV_RENDERER_DEPRECATED_INPUT_LAYOUTS   (0)

DC_BEGIN_DREEMCHEST

    template<class T>
    using StrongPtr = Ptr<T>;

    template<typename T>
    class Hash : public std::map<String64, T> {};

    typedef Kv<String>      KeyValue;
    typedef Array<KeyValue> KeyValueArray;
    typedef Variant            Archive;

typedef Array<Archive> Archives;

DC_END_DREEMCHEST

#include "Base/Preprocessor.h"
#include "Base/Classes.h"

#define DC_NEW new

#ifdef NIMBLE_DEBUG
    #define DC_DEBUG
#endif  //  #ifdef NIMBLE_DEBUG

#endif  /*  !DEPRECATED_H   */
