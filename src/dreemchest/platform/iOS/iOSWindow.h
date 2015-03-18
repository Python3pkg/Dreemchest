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

#ifndef __DC_Platform_iOSWindow_H__
#define __DC_Platform_iOSWindow_H__

#include "../Window.h"
#include "UIKitWindow.h"

DC_BEGIN_DREEMCHEST

namespace platform {

    // ** class iOSWindow
    class iOSWindow : public IWindow {
    public:

                            iOSWindow( void );
        virtual             ~iOSWindow( void );

        // ** IWindow
        virtual void        close( void );
        virtual u32         width( void ) const;
        virtual u32         height( void ) const;
        virtual String      caption( void ) const;
        virtual void        setCaption( const String& value );
        virtual void*       handle( void ) const;
        void                setOwner( Window* value );
        Window*             owner( void ) const;

        // ** iOSWindow
        bool                create( void );

    private:

        //! Implementation owner
        Window*             m_owner;

        //! Native window.
        UIKitWindow*        m_window;
    };

} // namespace platform

DC_END_DREEMCHEST

#endif /*   !defined( __DC_Platform_iOSWindow_H__ )   */