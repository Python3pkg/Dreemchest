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

#include "Window.h"

DC_BEGIN_DREEMCHEST

namespace Platform {

//! Platform-specific window constructor.
extern IWindow* createWindow( u32 width, u32 height );

// ** Window::Window
Window::Window( IWindow* impl )
    : m_impl( impl )
    , m_lastUpdateTime(-1)
{
    if( m_impl ) m_impl->setOwner( this );
    else         LogWarning( "window", "%s", "not available on current platform\n" );
}

// ** Window::~Window
Window::~Window( void )
{
    if (m_impl)
    {
        m_impl->close();
    }
    
    delete m_impl;
}

// ** Window::create
Window* Window::create( u32 width, u32 height )
{
#ifdef DC_PLATFORM_ANDROID
    NIMBLE_NOT_IMPLEMENTED
#else
    if( IWindow* impl = createWindow( width, height ) ) {
        return DC_NEW Window( impl );
    }
#endif

    return NULL;
}

// ** Window::width
u32 Window::width( void ) const
{
    if( m_impl ) {
        return m_impl->width();
    }

//    LogWarning( "Window::width : window is not implemented\n" );
    return 0;
}

// ** Window::height
u32 Window::height( void ) const
{
    if( m_impl ) {
        return m_impl->height();
    }

//    LogWarning( "Window::height : window is not implemented\n" );
    return 0;
}

// ** Window::aspectRatio
f32 Window::aspectRatio( void ) const
{
    return f32( width() ) / height();
}

// ** Window::mapCursorToWindow
void Window::mapCursorToWindow( s32& x, s32& y ) const
{
    DC_CHECK_IMPL();
    m_impl->mapCursorToWindow( x, y );
}

// ** Window::caption
String Window::caption( void ) const
{
    if( m_impl == NULL ) {
    //    LogWarning( "Window::caption : window is not implemented\n" );
        return "";
    }

    return m_impl->caption();
}

// ** Window::setCaption
void Window::setCaption( const String& value )
{
    if( m_impl == NULL ) {
    //    LogWarning( "Window::setCaption : window is not implemented\n" );
        return;
    }

    m_impl->setCaption( value );
}

// ** Window::handle
void* Window::handle( void ) const
{
    if( m_impl == NULL ) {
    //    LogWarning( "Window::handle : window is not implemented\n" );
        return NULL;
    }

    return m_impl->handle();
}

// ** Window::notifyUpdate
void Window::notifyUpdate( void )
{
    s32 dt   = 0;
    s32 time = currentTime();
    
    if (m_lastUpdateTime != -1)
    {
        dt = time - m_lastUpdateTime;
    }
    m_lastUpdateTime = time;
    
    notify<Update>(Update( this, dt));
}

// ** Window::notifyMouseUp
void Window::notifyMouseUp( MouseButton button, u32 x, u32 y, int touchId )
{
    notify<TouchEnded>( TouchEnded( this, button, x, y, touchId ) );
}

// ** Window::notifyMouseDown
void Window::notifyMouseDown( MouseButton button, u32 x, u32 y, int touchId )
{
    notify<TouchBegan>( TouchBegan( this, button, x, y, touchId ) );
}

// ** Window::notifyMouseMove
void Window::notifyMouseMove( MouseButton button, u32 sx, u32 sy, u32 ex, u32 ey, int touchId )
{
    notify<TouchMoved>( TouchMoved( this, button, sx, sy, touchId ) );
}

// ** Window::notifyKeyDown
void Window::notifyKeyDown( Key key )
{
#if defined( DC_PLATFORM_KEYBOARD )
    notify<KeyPressed>( KeyPressed( this, key ) );
#endif  /*  #if defined( DC_PLATFORM_KEYBOARD )    */
}

// ** Window::notifyKeyUp
void Window::notifyKeyUp( Key key )
{
#if defined( DC_PLATFORM_KEYBOARD )
    notify<KeyReleased>( KeyReleased( this, key ) );
#endif  /*  #if defined( DC_PLATFORM_KEYBOARD )    */
}

} // namespace Platform

DC_END_DREEMCHEST
